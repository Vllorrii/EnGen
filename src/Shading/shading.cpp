// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     shading.cpp
//   Author:
//     Floris van Riel
//   Description:
//     Sets up the rendering pipeline for drawing
//
// An AP product (c) 2016
// ------------------------------------ //

#include "shading.h"

ColorShader::ColorShader() {
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_lightBuffer = 0;
	m_SRV = 0;
}

ColorShader::ColorShader(const ColorShader& other) {
}

ColorShader::~ColorShader() {
}

bool ColorShader::Initialize(ID3D11Device* device) {
	HRESULT result;
	ID3D10Blob* errorMsg;
	ID3D10Blob* vertexShader;
	ID3D10Blob* pixelShader;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc, lightBufferDesc;

	// Set all pointers to zero
	errorMsg = 0;
	vertexShader = 0;
	pixelShader = 0;

	// First try to compile the vertex shader code
	result = D3DCompileFromFile(L"FX/color.vs", NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShader, &errorMsg);
	if (FAILED(result)) {
		// If an error message was created, show it in a message box, else the file does not exist
		if (errorMsg) {
			OutputShaderError(errorMsg, NULL, L"color.vs");
		} else {
			MessageBox(NULL, L"Could not find the vertex shader file", L"Shader error", MB_OK);
		}
	}

	// Now try to compile the pixel shader code
	result = D3DCompileFromFile(L"FX/color.ps", NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShader, &errorMsg);
	if (FAILED(result)) {
		// If an error message was created, show it in a message box, else the file does not exist
		if (errorMsg) {
			OutputShaderError(errorMsg, NULL, L"color.ps");
		} else {
			MessageBox(NULL, L"Could not find the pixel shader file", L"Shader error", MB_OK);
		}
	}

	// Release the error message
	if (errorMsg) {
		errorMsg->Release();
		errorMsg = 0;
	}

	// Create the vertex shader from the compiled buffer
	result = device->CreateVertexShader(vertexShader->GetBufferPointer(), vertexShader->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) {
		return false;
	}

	// Create the pixel shader from the compiled buffer
	result = device->CreatePixelShader(pixelShader->GetBufferPointer(), pixelShader->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) {
		return false;
	}

	// Fill out the polygon layout description, matching the model class and shader
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Store the number of elements in the polygon layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShader->GetBufferPointer(), vertexShader->GetBufferSize(), &m_layout);
	if (FAILED(result)) {
		return false;
	}

	// Release the compiled shader buffers
	vertexShader->Release();
	vertexShader = 0;

	pixelShader->Release();
	pixelShader = 0;

	// Fill out the buffer description for the matrix buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the matrix buffer to update the constant buffer in the shader
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) {
		return false;
	}

	// Fill out the buffer description for the light buffer
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the light buffer to contain the lighting variables
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void ColorShader::Shutdown() {
	// Release the private pointers
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_lightBuffer) {
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}

	if (m_SRV) {
		m_SRV->Release();
		m_SRV = 0;
	}

	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

bool ColorShader::UpdateShader(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix) {
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	unsigned int bufferNumber;

	// Take the transpose of the matrices before sending them to the shader
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projMatrix = XMMatrixTranspose(projMatrix);

	// Map the buffer so the CPU can write to it
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes);
	if (FAILED(result)) {
		return false;
	}

	// Get the data pointer to the matrix buffer
	dataPtr = (MatrixBufferType*)mappedRes.pData;

	// Replace the old matrices with the new ones
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->proj = projMatrix;

	// Unmap the buffer when we are done
	deviceContext->Unmap(m_matrixBuffer, 0);

	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes);
	if (FAILED(result))
	{
		return false;
	}

	// Get the data pointer to the light buffer
	dataPtr2 = (LightBufferType*)mappedRes.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->color = XMFLOAT4(0.984f, 0.973f, 0.898f, 1.0f);
	dataPtr2->direction = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	dataPtr2->padding = 0.0f;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightBuffer, 0);

	// Set the position of the buffer in the shader code
	bufferNumber = 0;

	// Now update the variables in the vertex shader
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// And update the variables in the pixel shader
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}

void ColorShader::OutputShaderError(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* filename) {
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; ++i)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", filename, MB_OK);

	return;
}

bool ColorShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projMatrix) {
	bool result;
	
	// First set the shader paramaters
	result = UpdateShader(deviceContext, worldMatrix, viewMatrix, projMatrix);
	if (!result) {
		return false;
	}

	// Set the input layout
	deviceContext->IASetInputLayout(m_layout);

	// Bind the shader to the correct pipeline stage
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Send the buffers through the pipeline
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return true;
}