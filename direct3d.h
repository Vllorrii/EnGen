// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     direct3d.h
//   Author:
//     Floris van Riel
//   Description:
//     Initializes the Direct3D API and its components
//     Holds the swap chain
//     Holds (back) buffers
//     Holds world/view/projection matrices
//
// An AP product (c) 2016
// ------------------------------------ //

#ifndef _DIRECT3D_H_
#define _DIRECT3D_H_

#define _XM_NO_INTRINSICS_

// Libs
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Includes
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

// The Direct3D API class
class Direct3D {
public:
	Direct3D();
	Direct3D(const Direct3D&);
	~Direct3D();

	// Initializes and shuts down the Direct3D components
	bool Initialize(int, int, HWND, bool, bool);
	void Shutdown();

	// Presents the back buffer to the screen
	void StartScene();
	void Present();

	// Getter methods for the device and device context
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	// Getter methods for the world and projection matrices
	XMMATRIX GetWorldMatrix();
	XMMATRIX GetProjectionMatrix();

private:
	// Necessary Direct3D components
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterizerState;

	// Transformation matrices
	XMMATRIX m_world;
	XMMATRIX m_projection;

	// Private variables
	bool m_vsync_enabled;
	bool m_fullscreen;
};

#endif