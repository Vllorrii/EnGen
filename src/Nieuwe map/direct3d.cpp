// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     direct3d.cpp
//   Author:
//     Floris van Riel
//   Description:
//     Initializes the Direct3D components
//     Renders the buffers to the swap chain
//
// An AP product (c) 2016
// ------------------------------------ //

#include "direct3d.h"

Direct3D::Direct3D() {
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterizerState = 0;
}

Direct3D::Direct3D(const Direct3D& other) {
}

Direct3D::~Direct3D() {
}

bool Direct3D::Initialize(int width, int height, HWND hwnd, bool vsync, bool fullscreen) {
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	DXGI_MODE_DESC* displayModeList;
	D3D_FEATURE_LEVEL featureLevel;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ID3D11Texture2D* backBuffer;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;

	// Set vsync and fullscreen
	m_vsync_enabled = vsync;
	m_fullscreen = fullscreen;

	// Get the refresh rate of the system
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)) { return false; }
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) { return false; }
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) { return false; }
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)) { return false; }
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) { return false; }
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) { return false; }
	for (i = 0; i < numModes; ++i) {
		if (displayModeList[i].Width = (unsigned int)width) {
			if (displayModeList[i].Height = (unsigned int)height) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}
	delete[] displayModeList;
	displayModeList = 0;
	adapterOutput->Release();
	adapterOutput = 0;
	adapter->Release();
	adapter = 0;
	factory->Release();
	factory = 0;

	// Fill out the swap chain description
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (m_vsync_enabled) {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	} else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Flags = 0;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = m_fullscreen ? false : true;

	// Create the Direct3D swap chain, device and device context
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, &featureLevel, &m_deviceContext);
	if (FAILED(result)) { return false; }

	// Create the render target view from the back buffer
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(result)) { return false; }
	result = m_device->CreateRenderTargetView(backBuffer, 0, &m_renderTargetView);
	if (FAILED(result)) { return false; }
	backBuffer->Release(); // We no longer need this pointer
	backBuffer = 0;

	// Fill out the depth stencil buffer description
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.MiscFlags = 0;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.Width = width;

	// Create the depth stencil buffer
	result = m_device->CreateTexture2D(&depthBufferDesc, 0, &m_depthStencilBuffer);
	if (FAILED(result)) { return false; }

	// Fill out the depth stencil description
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// First create the depth stencil state and bind it to the output merger state
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result)) { return false; }
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// Fill out the depth stencil view description
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Now create the depth stencil view
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result)) { return false; }

	// Set the render target
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Fill out the rasterizer description
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterizerState);
	if (FAILED(result)) { return false; }

	// Set the rasterizer state
	m_deviceContext->RSSetState(m_rasterizerState);

	// Create the viewport (relative to the client area)
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	m_deviceContext->RSSetViewports(1, &viewport);

	// Setup the world matrix
	m_world = XMMatrixIdentity();

	// Setup the projection matrix
	m_projection = XMMatrixPerspectiveFovLH(XM_PI / 4.0f, (float)width / (float)height, 1.0f, 1000.0f);

	return true;
}

void Direct3D::Shutdown() {
	// Release all pointers if they exist
	if (m_swapChain) {
		m_swapChain->SetFullscreenState(false, NULL);
	}
	if (m_rasterizerState) {
		m_rasterizerState->Release();
		m_rasterizerState = 0;
	}
	if (m_depthStencilView) {
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}
	if (m_depthStencilState) {
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}
	if (m_depthStencilBuffer) {
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}
	if (m_renderTargetView) {
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}
	if (m_deviceContext) {
		m_deviceContext->Release();
		m_deviceContext = 0;
	}
	if (m_device) {
		m_device->Release();
		m_device = 0;
	}
	if (m_swapChain) {
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

void Direct3D::StartScene() {
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	// Clear the render target view to black
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth stencil view
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Direct3D::Present() {
	m_swapChain->Present(m_vsync_enabled ? 1 : 0, 0);

	return;
}

ID3D11Device* Direct3D::GetDevice() {
	return m_device;
}

ID3D11DeviceContext* Direct3D::GetDeviceContext() {
	return m_deviceContext;
}

XMMATRIX Direct3D::GetWorldMatrix() {
	return m_world;
}

XMMATRIX Direct3D::GetProjectionMatrix() {
	return m_projection;
}