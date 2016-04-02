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
	m_vertices = 0;
	m_indices = 0;
}

Model::Model(const Model& other) {
}

Model::~Model() {
}

bool Model::InitializeBuffers(ID3D11Device* device) {
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT hresult;
	bool result;

	// Load the model file
	result = LoadModelFromFile("res/test.ngm");
	if (!result) {
		return false;
	}

	// Fill out the vertex buffer description
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Create the vertex buffer data
	vertexData.pSysMem = m_vertices;
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
	indexData.pSysMem = m_indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer
	hresult = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	return true;
}

void Model::ShutdownBuffers() {	
	// Release the model vertex array
	if (m_vertices) {
		delete[] m_vertices;
		m_vertices = 0;
	}

	// Release the index array
	if (m_indices) {
		delete[] m_indices;
		m_indices = 0;
	}
	
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

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext) {
	unsigned int stride;
	unsigned int offset;

	// Set the stride and offset and set the vertex buffer
	stride = sizeof(Vertex);
	offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the primitive topology to triangles
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool Model::LoadModelFromFile(char* filename) {
	FILE* filePtr;
	float* floatArray;
	int value, numElements, i;

	numElements = sizeof(Vertex) / sizeof(float);

	fopen_s(&filePtr, filename, "rb");
	if (!filePtr) {
		return false;
	}

	fread(&value, sizeof(int), 1, filePtr);
	m_vertexCount = value;

	fread(&value, sizeof(int), 1, filePtr);
	m_indexCount = value;

	m_indices = new unsigned int[m_indexCount];
	if (!m_indices) {
		fclose(filePtr);
		return false;
	}
	
	floatArray = new float[m_vertexCount * numElements];
	if (!floatArray) {
		fclose(filePtr);
		return false;
	}

	fread(&floatArray[0], sizeof(float), m_vertexCount * numElements, filePtr);
	fread(&m_indices[0], sizeof(unsigned int), m_indexCount, filePtr);

	fclose(filePtr);

	if (filePtr) {
		filePtr = 0;
	}

	m_vertices = new Vertex[m_vertexCount];
	for (i = 0; i < m_vertexCount; ++i) {
		m_vertices[i].position.x = floatArray[numElements * i + 0];
		m_vertices[i].position.y = floatArray[numElements * i + 1];
		m_vertices[i].position.z = floatArray[numElements * i + 2];

		m_vertices[i].color.x = floatArray[numElements * i + 3];
		m_vertices[i].color.y = floatArray[numElements * i + 4];
		m_vertices[i].color.z = floatArray[numElements * i + 5];
		m_vertices[i].color.w = floatArray[numElements * i + 6];

		m_vertices[i].normal.x = floatArray[numElements * i + 7];
		m_vertices[i].normal.y = floatArray[numElements * i + 8];
		m_vertices[i].normal.z = floatArray[numElements * i + 9];
	}

	return true;
}

int Model::GetIndexCount() {
	return m_indexCount;
}