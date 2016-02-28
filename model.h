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
	void RenderBuffers(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

#endif