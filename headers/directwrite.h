#ifndef _DIRECTWRITE_H_
#define _DIRECTWRITE_H_

// Includes
#include <dwrite.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <wincodec.h>
#include <string.h>
#include <intsafe.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <new>
#include <vector>

#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class DWrite {
public:
	DWrite();
	DWrite(const DWrite& other);
	~DWrite();

	HRESULT Initialize(HWND);
	void Shutdown();

	bool RenderText(const WCHAR*);

	HWND GetHwnd();

private:
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	HRESULT DrawD2DContent(const WCHAR*);
	HRESULT Draw(const WCHAR*);

	void OnResize(UINT, UINT);

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

private:
	IDWriteFactory* m_DWritefactory;
	IDWriteTextFormat* m_textFormat;

	ID2D1Factory* m_D2Dfactory;
	ID2D1HwndRenderTarget* m_renderTarget;
	ID2D1SolidColorBrush* m_blackBrush;

	HWND m_hwnd;

	float m_dpiX, m_dpiY;
};

#endif