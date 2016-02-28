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
#include "shading.h"
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
	void Update(ID3D11DeviceContext*, float, XMMATRIX, XMMATRIX, XMMATRIX);

private:
	Model* m_models;
	ColorShader* m_colorShader;
	int m_scenarioIndex;

	XMMATRIX m_worldMatrix;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectMatrix;
};

#endif