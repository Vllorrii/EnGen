// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     graphics.h
//   Author:
//     Floris van Riel
//   Description:
//     Initializes individual graphics processing components
//     Hosts a hub for DirectX drawing
//
// An AP product (c) 2016
// ------------------------------------ //

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

// Constants
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;

// Includes
#include <Windows.h>
#include "direct3d.h"
#include "directinput.h"
#include "xaudio.h"
#include "scenario.h"
#include "camera.h"
#include "video.h"
#include "cmdline.h"
#define WIN32_LEAN_AND_MEAN

// Graphical components (numerical values)
#define GC_DIRECT3D 0x01
#define GC_SCENARIO 0x02
#define GC_CAMERA   0x04
#define GC_INPUT    0x08
#define GC_AUDIO    0x10
#define GC_VIDEO    0x20
#define GC_CMDLINE  0x40

// The class declaration
class Graphics {
public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	// Initializes and shuts down the necessary graphics components
	bool Initialize(int, int, HWND, HINSTANCE);
	void Shutdown();

	// Render the scene
	bool Render(GAMESTATE, float);

private:
	// Set the current state
	bool SetState(GAMESTATE);

	// Initializes selected components and deactivates inactive ones (excludes specified for shutdown)
	bool InitializeComponents(const int, const int);

private:
	// Pointers to the graphics components
	Direct3D* m_Direct3D;
	Scenario* m_Scenario;
	Camera* m_Camera;
	Input* m_Input;
	Audio* m_Audio;
	Video* m_Video;
	CommandLine* m_CmdLine;

	// The game state
	GAMESTATE m_state;

	// The main window handle
	HWND m_hwnd;
	HINSTANCE m_hinstance;

	// The mouse location
	int m_prevMouseX, m_prevMouseY;

	// Screen dimensions
	int m_screenWidth, m_screenHeight;
};

#endif