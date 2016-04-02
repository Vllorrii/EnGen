// Includes
#include "directwrite.h"

DWrite::DWrite() {
	m_hwnd = 0;

	m_D2Dfactory = 0;
	m_renderTarget = 0;
	m_blackBrush = 0;

	m_DWritefactory = 0;
	m_textFormat = 0;
}

DWrite::DWrite(const DWrite& other) {
}

DWrite::~DWrite() {
}

HRESULT DWrite::Initialize(HWND hwndParent) {
	WNDCLASSEX wcex;
	RECT rc;
	float width, height;
	HRESULT result;

	HDC screen = GetDC(0);
	m_dpiX = GetDeviceCaps(screen, LOGPIXELSX) / 96.0f;
	m_dpiY = GetDeviceCaps(screen, LOGPIXELSY) / 96.0f;
	ReleaseDC(0, screen);

	GetClientRect(hwndParent, &rc);
	width = (float)(rc.right - rc.left);
	height = (float)(rc.bottom - rc.top);

	result = S_OK;

	ATOM atom;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = DWrite::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = L"DWrite";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	atom = RegisterClassEx(&wcex);

	result = atom ? S_OK : E_FAIL;

	m_hwnd = CreateWindow(L"DWrite", L"", WS_CHILD, 0, 0, (int)(width / m_dpiX), int(height / m_dpiY), hwndParent, NULL, HINST_THISCOMPONENT, this);

	if (SUCCEEDED(result)) {
		result = m_hwnd ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(result)) {
		result = CreateDeviceIndependentResources();
	}

	if (SUCCEEDED(result)) {
		ShowWindow(m_hwnd, SW_SHOWNORMAL);
		UpdateWindow(m_hwnd);
	}

	if (SUCCEEDED(result)) {
		DrawD2DContent(L"Test");
	}

	return result;
}

void DWrite::Shutdown() {
	if (m_D2Dfactory) {
		m_D2Dfactory->Release();
		m_D2Dfactory = 0;
	}

	if (m_renderTarget) {
		m_renderTarget->Release();
		m_renderTarget = 0;
	}

	if (m_blackBrush) {
		m_blackBrush->Release();
		m_blackBrush = 0;
	}

	if (m_DWritefactory) {
		m_DWritefactory->Release();
		m_DWritefactory = 0;
	}

	if (m_textFormat) {
		m_textFormat->Release();
		m_textFormat = 0;
	}

	DestroyWindow(m_hwnd);
	UnregisterClass(L"DWrite", HINST_THISCOMPONENT);

	return;
}

HRESULT DWrite::CreateDeviceIndependentResources() {
	HRESULT result;

	result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_D2Dfactory);

	if (SUCCEEDED(result)) {
		result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_DWritefactory));
	}

	if (SUCCEEDED(result)) {
		result = m_DWritefactory->CreateTextFormat(L"Consolas", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"en-us", &m_textFormat);
	}

	if (SUCCEEDED(result)) {
		result = m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	}

	if (SUCCEEDED(result)) {
		result = m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}

	return result;
}

HRESULT DWrite::CreateDeviceResources() {
	RECT rc;
	D2D1_SIZE_U size;
	HRESULT result;

	result = S_OK;
	GetClientRect(m_hwnd, &rc);
	size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	if (!m_renderTarget) {
		result = m_D2Dfactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_renderTarget);

		if (SUCCEEDED(result)) {
			result = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGray), &m_blackBrush);
		}
	}

	return result;
}

HRESULT DWrite::Draw(const WCHAR* text) {
	RECT rc;
	D2D1_RECT_F layoutRect;
	UINT32 size;

	size = (UINT32)wcslen(text);

	GetClientRect(m_hwnd, &rc);

	layoutRect = D2D1::RectF(
		static_cast<FLOAT>(rc.top) / m_dpiY,
		static_cast<FLOAT>(rc.left) / m_dpiX,
		static_cast<FLOAT>(rc.right - rc.left) / m_dpiX,
		static_cast<FLOAT>(rc.bottom - rc.top) / m_dpiY);

	m_renderTarget->DrawTextW(text, size, m_textFormat, layoutRect, m_blackBrush);

	return S_OK;
}

HRESULT DWrite::DrawD2DContent(const WCHAR* text) {
	HRESULT result;

	result = CreateDeviceResources();

	if (!(m_renderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED)) {
		m_renderTarget->BeginDraw();

		m_renderTarget->SetTransform(D2D1::IdentityMatrix());

		m_renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

		result = Draw(text);

		if (SUCCEEDED(result)) {
			result = m_renderTarget->EndDraw();
		}
	}

	if (FAILED(result)) {
		Shutdown();
	}

	return result;
}

void DWrite::OnResize(UINT width, UINT height) {
	if (m_renderTarget) {
		D2D1_SIZE_U size;
		size.width = width;
		size.height = height;
		m_renderTarget->Resize(size);
	}
}

bool DWrite::RenderText(const WCHAR* text) {
	PAINTSTRUCT ps;
	HRESULT result;

	BeginPaint(m_hwnd, &ps);

	result = DrawD2DContent(text);
	
	EndPaint(m_hwnd, &ps);

	if (FAILED(result)) {
		return false;
	}

	return true;
}

HWND DWrite::GetHwnd() {
	return m_hwnd;
}

LRESULT CALLBACK DWrite::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_CREATE) {
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DWrite* dwrite = (DWrite*)pcs->lpCreateParams;

		::SetWindowLongPtr(hwnd, GWLP_USERDATA, PtrToUlong(dwrite));

		return 1;
	}

	DWrite* dwrite = reinterpret_cast<DWrite*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));

	if (dwrite) {
		switch (message) {
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				dwrite->OnResize(width, height);
			} return 0;

			case WM_DESTROY:
			{
				// PostQuitMessage(0);
			} return 1;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}