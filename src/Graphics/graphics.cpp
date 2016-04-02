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
	m_Input = 0;
	m_Audio = 0;
	m_Video = 0;
	m_CmdLine = 0;

	m_state = GS_NOTSPECIFIED;

	m_prevMouseX = 0;
	m_prevMouseY = 0;
}

Graphics::Graphics(const Graphics& other) {
}

Graphics::~Graphics() {
}

bool Graphics::Initialize(int width, int height, HWND hwnd, HINSTANCE hInstance) {
	m_screenWidth = width;
	m_screenHeight = height;

	m_hwnd = hwnd;
	m_hinstance = hInstance;

	/* // Initialize the Direct3D components
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
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// Initialize the keyboard and mouse input
	m_Input = new Input;
	if (!m_Input) {
		return false;
	}

	result = m_Input->Initialize(hInstance, hwnd, width, height);
	if (!result) {
		return false;
	}

	// Initialize the audio output
	m_Audio = new Audio;
	if (!m_Audio) {
		return false;
	}

	result = m_Audio->Initialize();
	if (!result) {
		return false;
	}
	result = m_Audio->LoadWavFromFile(L"res/audio01.wav");
	if (!result) {
		return false;
	}
	result = m_Audio->PlayWavFile();
	if (!result) {
		return false;
	}*/

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
		m_Camera = 0;
	}

	// Release the input pointer
	if (m_Input) {
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// Release the audio pointer
	if (m_Audio) {
		m_Audio->Shutdown();
		delete m_Audio;
		m_Audio = 0;
	}

	if (m_Video) {
		m_Video->Shutdown();
		// delete m_Video;
		m_Video = 0;
	}

	if (m_CmdLine) {
		m_CmdLine->Shutdown();
		delete m_CmdLine;
		m_CmdLine = 0;
	}
}

bool Graphics::Render(GAMESTATE state, float dt) {	
	if (m_state != state) {
		if (!SetState(state)) {
			return true;
		}
	}
	
	// Render graphics according to game state
	switch (GS_CURRENT) {
		case GS_INGAME: {
			int mouseX, mouseY;
			mouseX = 0;
			mouseY = 0;

			m_Direct3D->StartScene();

			m_Input->Update();

			if (m_Input->IsKeyPressed(DIK_W)) m_Camera->Walk(10.0f * dt);
			if (m_Input->IsKeyPressed(DIK_A)) m_Camera->Strafe(-10.0f * dt);
			if (m_Input->IsKeyPressed(DIK_S)) m_Camera->Walk(-10.0f * dt);
			if (m_Input->IsKeyPressed(DIK_D)) m_Camera->Strafe(10.0f * dt);
			m_Input->GetMouseLocation(mouseX, mouseY);

			if (m_hwnd == GetActiveWindow()) {
				if (!m_Input->SetCursorPosition(m_screenWidth / 2, m_screenHeight / 2)) {
					return true;
				}
			}

			if (m_Input->IsKeyPressed(DIK_ESCAPE)) {
				SendMessage(m_hwnd, NGM_CHANGE_STATE, (WPARAM)GS_MAINMENU, 0);
			}

			m_Camera->Render();
			m_Camera->Pitch(XMConvertToRadians(0.2f * static_cast<float>(mouseY - m_prevMouseY)));
			m_Camera->RotateY(XMConvertToRadians(0.2f * static_cast<float>(mouseX - m_prevMouseX)));

			m_prevMouseX = mouseX;
			m_prevMouseY = mouseY;

			m_Scenario->Update(m_Direct3D->GetDeviceContext(), dt, m_Direct3D->GetWorldMatrix(), m_Camera->GetViewMatrix(), m_Direct3D->GetProjectionMatrix());

			m_Direct3D->Present();
		} 

		case GS_INTRO: {
			return false;
		}
	}

	return false;
}

bool Graphics::SetState(GAMESTATE state) {
	bool result;

	if (m_state == GS_NOTSPECIFIED && state == GS_INTRO) {
		result = InitializeComponents(GC_VIDEO, 0);
		if (!result) {
			return false;
		}
	}

	if (m_state == GS_INTRO && state == GS_MAINMENU) {
		result = InitializeComponents(GC_CMDLINE, 0);
		if (!result) {
			return false;
		}
	}

	if (m_state == GS_MAINMENU && state == GS_INGAME) {
		result = InitializeComponents(GC_DIRECT3D | GC_SCENARIO | GC_CAMERA | GC_INPUT | GC_AUDIO, 0);
		if (!result) {
			return false;
		}

		
	}
	if (m_state == GS_INGAME && state == GS_MAINMENU) {
		result = InitializeComponents(GC_CMDLINE, 0);
		if (!result) {
			return false;
		}
	}

	GS_CURRENT = state;
	m_state = GS_CURRENT;

	return true;
}

bool Graphics::InitializeComponents(const int components, const int excludeShutdown) {
	bool result;
	
	// -- Shutdowns -------------------------------
	
	if (!(excludeShutdown & GC_DIRECT3D) || components & GC_DIRECT3D) {
		if (m_Direct3D) {
			m_Direct3D->Shutdown();
			delete m_Direct3D;
			m_Direct3D = 0;
		}
	}

	if (!(excludeShutdown & GC_SCENARIO) || components & GC_SCENARIO) {
		if (m_Scenario) {
			m_Scenario->Shutdown();
			delete m_Scenario;
			m_Scenario = 0;
		}
	}

	if (!(excludeShutdown & GC_CAMERA) || components & GC_CAMERA) {
		if (m_Camera) {
			m_Camera = 0;
		}
	}

	if (!(excludeShutdown & GC_INPUT) || components & GC_INPUT) {
		if (m_Input) {
			m_Input->Shutdown();
			delete m_Input;
			m_Input = 0;
		}
	}

	if (!(excludeShutdown & GC_AUDIO) || components & GC_AUDIO) {
		if (m_Audio) {
			m_Audio->Shutdown();
			delete m_Audio;
			m_Audio = 0;
		}
	}

	if (!(excludeShutdown & GC_VIDEO) || components & GC_VIDEO) {
		if (m_Video) {
			m_Video->Shutdown();
			// delete m_Video;
			m_Video = 0;
		}
	}

	if (!(excludeShutdown & GC_CMDLINE) || components & GC_CMDLINE) {
		if (m_CmdLine) {
			m_CmdLine->Shutdown();
			delete m_CmdLine;
			m_CmdLine = 0;
		}
	}
	
	// --------------------------------------------

	// -- Initializations  ------------------------

	if (components & GC_DIRECT3D || components & GC_SCENARIO || components & GC_CAMERA || components & GC_INPUT) {
		m_Direct3D = new Direct3D;
		if (!m_Direct3D) {
			return false;
		}

		result = m_Direct3D->Initialize(m_screenWidth, m_screenHeight, m_hwnd, VSYNC_ENABLED, FULL_SCREEN);
		if (!result) {
			return false;
		}

		m_Scenario = new Scenario;
		if (!m_Scenario) {
			return false;
		}

		result = m_Scenario->Initialize(m_Direct3D->GetDevice());
		if (!result) {

		}

		m_Camera = new Camera;
		if (!m_Camera) {
			return false;
		}
		m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

		m_Input = new Input;
		if (!m_Input) {
			return false;
		}

		result = m_Input->Initialize(m_hinstance, m_hwnd, m_screenWidth, m_screenHeight);
		if (!result) {
			return false;
		}
	}

	if (components & GC_AUDIO) {
		m_Audio = new Audio;
		if (!m_Audio) {
			return false;
		}

		result = m_Audio->Initialize();
		if (!result) {
			return false;
		}
		result = m_Audio->LoadWavFromFile(L"res/audio01.wav");
		if (!result) {
			return false;
		}
		result = m_Audio->PlayWavFile();
		if (!result) {
			return false;
		}
	}

	if (components & GC_VIDEO) {
		m_Video = new Video;
		if (!m_Video) {
			return false;
		}

		result = m_Video->Initialize(m_hwnd);
		if (!result) {
			return false;
		}
	}

	if (components & GC_CMDLINE) {
		m_CmdLine = new CommandLine;
		if (!m_CmdLine) {
			return false;
		}

		result = m_CmdLine->Initialize(m_hwnd);
		if (!result) {
			return false;
		}
	}

	return true;
}