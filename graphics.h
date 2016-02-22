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
#include "state.h"
#include "scenario.h"
#include "camera.h"
#define WIN32_LEAN_AND_MEAN

// The class declaration
class Graphics {
public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	// Initializes and shuts down the necessary graphics components
	bool Initialize(int, int, HWND);
	void Shutdown();

	// Render the scene
	bool Render(GAMESTATE, float);

	// Set the current state
	void SetState(GAMESTATE);

private:
	// Pointers to the graphics components
	Direct3D* m_Direct3D;
	Scenario* m_Scenario;
	Camera* m_Camera;

	// The current game state
	GAMESTATE m_currentState;
};

#endif