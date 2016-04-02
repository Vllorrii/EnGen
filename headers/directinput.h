// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     directinput.h
//   Author:
//     Floris van Riel
//   Description:
//     Handles mouse and keyboard events
//
// An AP product (c) 2016
// ------------------------------------ //

#ifndef _DIRECTINPUT_H_
#define _DIRECTINPUT_H_

#define DIRECTINPUT_VERSION 0x0800

// Includes
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>

class Input {
public:
	Input();
	Input(const Input&);
	~Input();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Update();

	bool IsKeyPressed(int);
	void GetMouseLocation(int&, int&);

	bool SetCursorPosition(int, int);

private:
	IDirectInput8* m_dInput;
	IDirectInputDevice8* m_mouse;
	IDirectInputDevice8* m_keyboard;
	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;
	HWND m_hwnd;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};

#endif