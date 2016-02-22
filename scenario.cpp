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
	m_scenarioIndex = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_worldMatrix, I);
	XMStoreFloat4x4(&m_viewMatrix, I);
	XMStoreFloat4x4(&m_projectMatrix, I);
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

	return true;
}

void Scenario::Shutdown() {
	// Release the model class
	if (m_models) {
		m_models->ShutdownBuffers();
		m_models = 0;
	}

	return;
}

void Scenario::Update(ID3D11DeviceContext* deviceContext, float dt, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix) {
	// Update animation stuff
	
	m_models->RenderBuffers(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	return;
}