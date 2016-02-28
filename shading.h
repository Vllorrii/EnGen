// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     shading.h
//   Author:
//     Floris van Riel
//   Description:
//     Classes and methods for setting up
//     the rendering pipeline for drawing
//
// An AP product (c) 2016
// ------------------------------------ //

#ifndef SHADING_H_
#define SHADING_H_

#define _XM_NO_INTRINSICS_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
using namespace DirectX;

class ColorShader {
public:
	ColorShader();
	ColorShader(const ColorShader& other);
	~ColorShader();

	// Methods for initialization and shutdown
	bool Initialize(ID3D11Device*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);

private:
	// Update the shader to work with the current matrices
	bool UpdateShader(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);

	// A structure containing the necessary matrices
	struct MatrixBufferType {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
	};

private:
	// Private pointers to shaders, input layout, matrix buffer and sampler state
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_samplerState;
};

#endif