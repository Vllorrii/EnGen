// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     engen.h
//   Author:
//     Floris van Riel
//   Description:
//     Header for the EnGen main class
//
// An AP product (c) 2016
// ------------------------------------ //

// Define initialization
#ifndef _ENGEN_H_
#define _ENGEN_H_

// Speed up the build process
#define WIN32_LEAN_AND_MEAN

// Includes
#include <Windows.h>
#include "graphics.h"
#include "resource.h"
#include "time.h"

// Header for EnGen main class
class EnGen {
public:
	// Constructors and destructors
	EnGen();
	EnGen(const EnGen&);
	~EnGen();

	// Handler for the message loop
	LRESULT CALLBACK MsgHandler(HWND, UINT, WPARAM, LPARAM);

	// Initialization of the graphical windows and other objects
	bool InitializeWindows();
	bool Initialize();

	// Delete all pointers and objects used
	void Shutdown();

	// Runs the game engine
	void Run();

private:
	// Private handles for the instance and the window
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	// Client dimensions
	int width, height;

	// Class name
	LPCWSTR className;

	// Graphics instance
	Graphics* m_Graphics;

	// The main game timer
	Time m_time;
};

// Window procedure prototype
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global event methods
LRESULT OnCreateWindow(HWND);
void OnPlayerEvent(HWND, WPARAM);
void OnPaint(HWND);
void OnResize(WORD, WORD);

// Static pointer to application handle
static EnGen* AppHandle = 0;

#endif