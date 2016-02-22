// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     graphics.cpp
//   Author:
//     Floris van Riel
//   Description:
//     Hub for various graphics processing
//
// An AP product (c) 2016
// ------------------------------------ //

// Includes
#include "graphics.h"

Graphics::Graphics() {
	m_Direct3D = 0;
	m_Scenario = 0;
	m_Camera = 0;
}

Graphics::Graphics(const Graphics& other) {
}

Graphics::~Graphics() {
}

bool Graphics::Initialize(int width, int height, HWND hwnd) {
	bool result;

	// Initialize the Direct3D components
	m_Direct3D = new Direct3D;
	if (!m_Direct3D) {
		return false;
	}

	result = m_Direct3D->Initialize(width, height, hwnd, VSYNC_ENABLED, FULL_SCREEN);
	if (!result) {
		return false;
	}

	// Initialize the scenario
	m_Scenario = new Scenario;
	if (!m_Scenario) {
		return false;
	}

	result = m_Scenario->Initialize(m_Direct3D->GetDevice());
	if (!result) {
		return false;
	}

	// Initialize the camera
	m_Camera = new Camera;
	if (!m_Camera) {
		return false;
	}
	m_Camera->LookAt(XMFLOAT3(0.0f, 0.0f, -10.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_Camera->SetLens(0.25f * XM_PI, (float)width / (float)height, 1.0f, 1000.0f);

	return true;
}

void Graphics::Shutdown() {
	// Release the Direct3D pointer
	if (m_Direct3D) {
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	// Release the scenario pointer
	if (m_Scenario) {
		m_Scenario->Shutdown();
		delete m_Scenario;
		m_Scenario = 0;
	}

	// Release the camera pointer
	if (m_Camera) {
		delete m_Camera;
		m_Camera = 0;
	}
}

bool Graphics::Render(GAMESTATE state, float dt) {
	if (m_currentState != state) {
		SetState(state);
	}

	// Render graphics according to game state
	if (m_currentState == GS_INGAME) {
		XMFLOAT4X4 view; XMStoreFloat4x4(&view, m_Camera->GetViewMatrix());
		XMFLOAT4X4 proj; XMStoreFloat4x4(&proj, m_Camera->GetProjectionMatrix());
		m_Scenario->Update(m_Direct3D->GetDeviceContext(), dt, m_Direct3D->GetWorldMatrix(), view, proj);

		// Temporary Windows input detection
		if (GetAsyncKeyState('W') & 0x8000) m_Camera->Walk(10.0f * dt);
		if (GetAsyncKeyState('S') & 0x8000) m_Camera->Walk(-10.0f * dt);
		if (GetAsyncKeyState('A') & 0x8000) m_Camera->Strafe(-10.0f * dt);
		if (GetAsyncKeyState('D') & 0x8000) m_Camera->Strafe(10.0f * dt);

		m_Camera->UpdateViewMatrix();
		
		m_Direct3D->Present();
	}

	return true;
}

void Graphics::SetState(GAMESTATE state) {
	m_currentState = state;

	// Handle the code for initialization and shutdown of states
}