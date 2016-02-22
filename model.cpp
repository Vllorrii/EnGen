// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     model.cpp
//   Author:
//     Floris van Riel
//   Description:
//     Sets up buffers to draw your models
//
// An AP product (c) 2016
// ------------------------------------ //

// Includes
#include "model.h"

Model::Model() {
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_vertexCount = 0;
	m_indexCount = 0;

	m_effect = 0;
	m_effectTechnique = 0;
	m_worldViewProj = 0;
}

Model::Model(const Model& other) {
}

Model::~Model() {
}

bool Model::InitializeBuffers(ID3D11Device* device) {
	Vertex* vertices;
	unsigned int* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT hresult;
	bool result;

	vertices = 0;
	indices = 0;

	// For now just creates a simple triangle
	m_vertexCount = 3;
	m_indexCount = 3;

	vertices = new Vertex[m_vertexCount];
	if (!vertices) {
		return false;
	}

	indices = new unsigned int[m_indexCount];
	if (!indices) {
		return false;
	}

	// The vertices of the triangle
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	// The indices of the vertices in each primitive
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	// Fill out the vertex buffer description
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Create the vertex buffer data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer
	hresult = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	// Fill out the index buffer description
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Create the index buffer data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer
	hresult = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	// Delete the data pointers
	delete[] vertices;
	vertices = 0;
	
	delete[] indices;
	indices = 0;

	// Finally initialize the shader
	result = InitializeShader(device);
	if (!result) {
		return false;
	}

	return true;
}

void Model::ShutdownBuffers() {
	// Shutdown effect variables
	ShutdownShader();
	
	// Release the vertex buffer
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	// Release the index buffer
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
}

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix) {
	unsigned int stride;
	unsigned int offset;
	XMFLOAT4X4 worldViewProj;

	// Set the stride and offset and set the vertex buffer
	stride = sizeof(Vertex);
	offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the primitive topology to triangles
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the transformation matrices
	XMMATRIX world = XMLoadFloat4x4(&worldMatrix);
	XMMATRIX view = XMLoadFloat4x4(&viewMatrix);
	XMMATRIX proj = XMLoadFloat4x4(&projectionMatrix);
	XMStoreFloat4x4(&worldViewProj, world * view * proj);

	// Render the shader output
	RenderShader(deviceContext, worldViewProj);

	return;
}

int Model::GetIndexCount() {
	return m_indexCount;
}

bool Model::LoadModelFromFile(char* filename, Vertex* vertices, unsigned int* indices) {

	return true;
}

bool Model::InitializeShader(ID3D11Device* device) {
	int size;
	D3DX11_PASS_DESC passDesc;
	HRESULT result;

	// Load compiled shader from file
	std::ifstream fin("FX/shader.fxo", std::ios::binary);
	if (fin.fail()) return false;
	fin.seekg(0, std::ios_base::end);
	size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);
	fin.read(&compiledShader[0], size);
	fin.close();

	// Create the effect for the effect file
	result = D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, device, &m_effect);
	if (FAILED(result)) {
		return false;
	}

	m_effectTechnique = m_effect->GetTechniqueByName("ColorTech");
	m_worldViewProj = m_effect->GetVariableByName("g_worldViewProj")->AsMatrix();

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	m_effectTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	result = device->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_inputLayout);
	if (FAILED(result)) {
		return false;
	}



	return true;
}

void Model::ShutdownShader() {
	// Release all pointers
	if (m_inputLayout) {
		m_inputLayout->Release();
		m_inputLayout = 0;
	}
	
	if (m_effectTechnique) {
		m_effectTechnique = 0;
	}

	if (m_worldViewProj) {
		m_worldViewProj = 0;
	}

	if (m_effect) {
		m_effect->Release();
		m_effect = 0;
	}
}

void Model::RenderShader(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldViewProjF) {
	unsigned int p;
	XMMATRIX worldViewProj;
	D3DX11_TECHNIQUE_DESC techDesc;

	// Load the world view projection matrix
	worldViewProj = XMLoadFloat4x4(&worldViewProjF);

	// Set the shader variable
	m_worldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	// Set the input layout to the one we created
	deviceContext->IASetInputLayout(m_inputLayout);

	// Get the technique description and draw the objects per pass
	m_effectTechnique->GetDesc(&techDesc);
	for (p = 0; p < techDesc.Passes; ++p) {
		m_effectTechnique->GetPassByIndex(0)->Apply(0, deviceContext);

		deviceContext->DrawIndexed(m_indexCount, 0, 0);
	}
}