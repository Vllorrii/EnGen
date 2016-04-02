// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     scenario.cpp
//   Author:
//     Floris van Riel
//   Description:
//     Hub for various models and animations
//     Sets up all buffers
//
// An AP product (c) 2016
// ------------------------------------ //

// Includes
#include "scenario.h"

Scenario::Scenario() {
	m_models = 0;
	m_colorShader = 0;
	m_scenarioIndex = 0;

	XMMATRIX I = XMMatrixIdentity();
	m_worldMatrix = I;
	m_viewMatrix = I;
	m_projectMatrix = I;
}

Scenario::Scenario(const Scenario& other) {
}

Scenario::~Scenario() {
}

bool Scenario::Initialize(ID3D11Device* device) {
	bool result;

	// Create an instance of the model class
	m_models = new Model;
	if (!m_models) {
		return false;
	}

	// Initialize the model buffers
	result = m_models->InitializeBuffers(device);
	if (!result) {
		return false;
	}

	// Create an instance of the color shader class
	m_colorShader = new ColorShader;
	if (!m_colorShader) {
		return false;
	}

	// Initialize the color shader for drawing
	result = m_colorShader->Initialize(device);
	if (!result) {
		return false;
	}

	return true;
}

void Scenario::Shutdown() {
	// Release the model class
	if (m_models) {
		m_models->ShutdownBuffers();
		m_models = 0;
	}

	// Release the color shader class
	if (m_colorShader) {
		m_colorShader->Shutdown();
		m_colorShader = 0;
	}
	return;
}

void Scenario::Update(ID3D11DeviceContext* deviceContext, float dt, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {
	// Update animation stuff
	
	m_models->RenderBuffers(deviceContext);

	m_colorShader->Render(deviceContext, m_models->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);

	return;
}