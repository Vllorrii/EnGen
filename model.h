// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     model.h
//   Author:
//     Floris van Riel
//   Description:
//     Contains model data and animations
//
// An AP product (c) 2016
// ------------------------------------ //

#ifndef _MODEL_H_
#define _MODEL_H_

#define _XM_NO_INTRINSICS_

// Includes
#include <d3d11.h>
#include <d3dx11effect.h>
#include <fstream>
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;

class Model {
public:
	Model();
	Model(const Model& other);
	~Model();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4);

	int GetIndexCount();

private:
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	bool LoadModelFromFile(char*, Vertex*, unsigned int*);

	bool InitializeShader(ID3D11Device*);
	void ShutdownShader();
	void RenderShader(ID3D11DeviceContext*, XMFLOAT4X4);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	ID3DX11Effect* m_effect;
	ID3DX11EffectTechnique* m_effectTechnique;
	ID3DX11EffectMatrixVariable* m_worldViewProj;

	ID3D11InputLayout* m_inputLayout;
};

#endif