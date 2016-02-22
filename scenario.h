// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     scenario.h
//   Author:
//     Floris van Riel
//   Description:
//     Manages drawing the current scenario
//
// An AP product (c) 2016
// ------------------------------------ //

#ifndef _SCENARIO_H_
#define _SCENARIO_H_

#define _XM_NO_INTRINSICS_

// Includes
#include "model.h"
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

// The scenario class
class Scenario {
public:
	Scenario();
	Scenario(const Scenario& other);
	~Scenario();

	// Initialize and shut down
	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Update(ID3D11DeviceContext*, float, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4);

private:
	Model* m_models;
	int m_scenarioIndex;

	XMFLOAT4X4 m_worldMatrix;
	XMFLOAT4X4 m_viewMatrix;
	XMFLOAT4X4 m_projectMatrix;
};

#endif