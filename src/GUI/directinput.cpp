// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     directinput.cpp
//   Author:
//     Floris van Riel
//   Description:
//     Methods for updating the input states
//     and getting keyboard and mouse states
//
// An AP product (c) 2016
// ------------------------------------ //

#include "directinput.h"

Input::Input() {
	m_dInput = 0;
	m_mouse = 0;
	m_keyboard = 0;
}

Input::Input(const Input& other) {
}

Input::~Input() {
}

bool Input::Initialize(HINSTANCE hInstance, HWND hwnd, int width, int height) {
	HRESULT result;

	m_screenWidth = width;
	m_screenHeight = height;

	m_mouseX = 0;
	m_mouseY = 0;

	m_hwnd = hwnd;

	// Create the direct input interface
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_dInput, NULL);
	if (FAILED(result)) {
		return false;
	}

	// Create the mouse interface and initialize it
	result = m_dInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result)) {
		return false;
	}
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) {
		return false;
	}
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}
	result = m_mouse->Acquire();
	if (FAILED(result)) {
		return false;
	}

	// Create the keyboard interface and initialize it
	result = m_dInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result)) {
		return false;
	}
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) {
		return false;
	}
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}
	result = m_keyboard->Acquire();
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void Input::Shutdown() {
	// Release all pointers
	if (m_keyboard) {
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	if (m_mouse) {
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	if (m_dInput) {
		m_dInput->Release();
		m_dInput = 0;
	}

	return;
}

bool Input::Update() {
	HRESULT result;

	// First read the keyboard state
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result)) {
		// Try to reaquire if focus lost
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			m_keyboard->Acquire();
		} else {
			return false;
		}
	}

	// Now read the mouse state
	result = m_mouse->GetDeviceState(sizeof(m_mouseState), (LPVOID)&m_mouseState);
	if (FAILED(result)) {
		// Try to reaquire if focus lost
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			m_mouse->Acquire();
		} else {
			return false;
		}
	}

	// Recalculate the mouse position
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	/*if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }*/

	return true;
}

bool Input::IsKeyPressed(int keyCode) {
	if (m_keyboardState[keyCode] & 0x80) {
		return true;
	}

	return false;
}

void Input::GetMouseLocation(int& x, int& y) {
	x = m_mouseX;
	y = m_mouseY;
	return;
}

bool Input::SetCursorPosition(int x, int y) {
	RECT windowRect;
	BOOL result;
	int posX, posY;

	result = GetWindowRect(m_hwnd, &windowRect);
	if (!result) {
		return false;
	}

	posX = windowRect.left;
	posY = windowRect.top;

	result = SetCursorPos(posX + x, posY + y);
	if (!result) {
		return false;
	}

	return true;
}