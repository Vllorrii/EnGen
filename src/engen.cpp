// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     engen.cpp
//   Author:
//     Floris van Riel
//   Description:
//     The main class for the game engine
//     Initializes the graphical windows
//     Initializes engine components
//     Runs the main game loop
//     Catches Windows messages
//
// An AP product (c) 2016
// ------------------------------------ //

#include "engen.h"

EnGen::EnGen()
{
	m_Graphics = 0;
}

EnGen::EnGen(const EnGen& other)
{
}

EnGen::~EnGen()
{
}

bool EnGen::Initialize() {
	bool result;

	result = InitializeWindows();
	if (!result) {
		return false;
	}

	m_Graphics = new Graphics;
	if (!m_Graphics) {
		return false;
	}

	result = m_Graphics->Initialize(width, height, m_hwnd, m_hinstance);
	if (!result) {
		return false;
	}

	g_hwnd = m_hwnd;

	return true;
}

bool EnGen::InitializeWindows() {
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	DWORD wndStyle;
	int x, y;

	className = L"WindowClass";

	// Initialize the width and height to zero
	width = 0;
	height = 0;

	// Set the class name to be used globally

	// Create the application handle of this class
	AppHandle = this;

	// Create the instance for this application
	m_hinstance = GetModuleHandle(NULL);

	// Fill out the window class description
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_MYICON));
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = className;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Try to register the window class
	if (!RegisterClassEx(&wc)) {
		MessageBox(0, L"An error occured while initializing window class", L"Error", MB_OK);
		return false;
	}

	// Set the dimensions to the user's screen
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	// Use the correct dimensions for fullscreen or not
	if (FULL_SCREEN) {
		// When we go fullscreen, fill out the appropriate screen settings
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)width;
		dmScreenSettings.dmPelsHeight = (unsigned long)height;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display to fullscreen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		x = y = 0;

		// Set the window style flags
		wndStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
	} else {
		width = 800;
		height = 600;
		x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		wndStyle = WS_OVERLAPPEDWINDOW;
	}

	// Create the window handle from the filled out descriptions
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, className, L"EnGen", wndStyle, x, y, width, height, 0, 0, m_hinstance, 0);

	// Return false if the creation failed
	if (!m_hwnd) {
		MessageBox(0, L"An error occured while creating the window", L"Error", MB_OK);
		return false;
	}

	// Show the window to the user
	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Disable the cursor in our window
	ShowCursor(false);

	return true;
}

void EnGen::Shutdown() {
	// Shutdown pointers
	if (m_Graphics) {
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}
	
	// Re-enable the cursor just in case
	ShowCursor(true);

	// First exit fullscreen mode if we are in it
	if (FULL_SCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	// Destroy our window and the handle
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Destroy our window class and the instance
	UnregisterClass(className, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to the application
	AppHandle = NULL;

	return;
}

void EnGen::Run() {
	MSG msg;
	bool done;

	// Prepare a memory allocation for the message
	ZeroMemory(&msg, sizeof(MSG));

	// This variable determines whether the application is done or not
	done = false;

	// Reset the timer
	m_time.Reset();

	// The game loop
	while (!done) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) {
			done = true;
		} else {
			// Tick the timer
			m_time.Tick();

			// Process any game stuff from here
			done = m_Graphics->Render(GS_CURRENT, m_time.DeltaTime());
		}
	}

	return;
}

LRESULT CALLBACK EnGen::MsgHandler(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	// Space to add custom event handlers
	// Returns the default message for now
	return DefWindowProc(hwnd, umsg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) {
	switch (umsg) {
		// Check if user wants to close the window
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		case WM_CHAR:
			OnCmdKeyPress(wParam, hwnd);
			break;

		case WM_CREATE:
			GS_CURRENT = GS_INTRO;
			break;

		case NGM_CHANGE_STATE: {
			GS_CURRENT = (GAMESTATE)wParam;
			break;
		}

		default:
			// Pass message through our message handler
			return AppHandle->MsgHandler(hwnd, umsg, wParam, lParam);
	}

	return 0;
}