// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     video.cpp
//   Author:
//     Floris van Riel
//   Description:
//     Enables video playback in the
//     game window.
//
// An AP product (c) 2016
// ------------------------------------ //

// Includes
#include "video.h"

Video::Video() {
	m_player = 0;
}

Video::Video(const Video& other) {
}

Video::~Video() {
}

bool Video::Initialize(HWND hwndParent) {
	WNDCLASSEX wcex;
	RECT rc;
	HRESULT result;
	ATOM atom;

	g_VideoHandle = this;

	m_hwndParent = hwndParent;
	m_hInstance = (HINSTANCE)GetWindowLong(m_hwndParent, GWL_HINSTANCE);

	result = S_OK;

	GetClientRect(hwndParent, &rc);

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = /*HINST_THISCOMPONENT*/ m_hInstance;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = 0;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = L"Video";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	atom = RegisterClassEx(&wcex);

	result = atom ? S_OK : E_FAIL;

	m_hwndEvent = CreateWindow(L"Video", L"", WS_CHILD, 0, 0, rc.right - rc.left, rc.bottom - rc.top, m_hwndParent, NULL, /*HINST_THISCOMPONENT*/ m_hInstance, 0);

	if (SUCCEEDED(result)) {
		result = m_hwndEvent ? S_OK : E_FAIL;
	}

	if (FAILED(result)) {
		return false;
	}

	return true;
}

void Video::Shutdown() {
	if (m_player) {
		if (m_player->HasVideo()) {
			m_player->Stop();
		}

		m_player->Shutdown();
		m_player->Release();
		m_player = 0;
	}

	g_VideoHandle = 0;

	return;
}

LRESULT CALLBACK Video::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CREATE:
		{
			return g_VideoHandle->OnCreate(hwnd);
		}

		case WM_PAINT:
		{
			g_VideoHandle->OnPaint();
			break;
		}

		case WM_SIZE:
		{
			g_VideoHandle->OnResize(LOWORD(lParam), HIWORD(lParam));
			break;
		}

		case NGM_PLAYER_EVENT:
		{
			g_VideoHandle->OnPlayerEvent(wParam);
			break;
		}

		case NGM_CHANGE_STATE:
		{
			// g_VideoHandle->Shutdown();
			return g_VideoHandle->OnDestroy(wParam);
		}

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT Video::OnCreate(HWND hwndEvent) {
	HRESULT result;

	result = CPlayer::CreateInstance(m_hwndParent, hwndEvent, &m_player);
	if (FAILED(result)) {
		assert(m_player);
		MessageBox(m_hwndParent, L"Could not initialize player object.", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	result = m_player->OpenURL(L"res/video01.avi");
	if (FAILED(result)) {
		MessageBox(m_hwndParent, L"Could not open the file.", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	assert(m_player);

	result = m_player->Play();
	if (FAILED(result)) {
		MessageBox(m_hwndParent, L"Could not play the file", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	return 0;
}

void Video::OnPlayerEvent(WPARAM unkPtr) {
	HRESULT result;

	result = m_player->HandleEvent(unkPtr);
	if (FAILED(result)) {
		MessageBox(m_hwndParent, L"An error occured.", L"Error", MB_OK | MB_ICONERROR);
	}

	assert(m_player);

	return;
}

void Video::OnPaint() {
	PAINTSTRUCT ps;
	HDC hdc;

	hdc = BeginPaint(m_hwndParent, &ps);

	if (m_player && m_player->HasVideo()) {
		m_player->Repaint();
	} else {
		RECT rc;
		GetClientRect(m_hwndParent, &rc);
		FillRect(hdc, &rc, (HBRUSH)COLOR_WINDOW);
	}

	EndPaint(m_hwndParent, &ps);

	return;
}

void Video::OnResize(WORD width, WORD height) {
	if (m_player) {
		m_player->ResizeVideo(width, height);
	}
}

LRESULT Video::OnDestroy(WPARAM state) {
	DestroyWindow(m_hwndEvent);

	return SendMessage(m_hwndParent, NGM_CHANGE_STATE, state, 0);
}