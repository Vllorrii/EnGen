#include "texture.h"

Texture::Texture() {
	m_filename = L"";
	m_SRV = 0;
	m_samplerState = 0;
}

Texture::Texture(const Texture& other) {
}

Texture::~Texture() {
}

bool Texture::CreateTexture(ID3D11Device* pDevice, LPCWSTR filename, Texture** ppTexture) {
	Texture* pTexture;
	bool result;

	pTexture = 0;
	result = false;

	pTexture = new Texture();
	if (!pTexture) {
		goto done;
	}

	result = pTexture->Initialize(pDevice, filename);
	if (!result) {
		goto done;
	}

	(*ppTexture) = pTexture;

done:
	if (pTexture) {
		pTexture->Shutdown();
		delete pTexture;
		pTexture = 0;
	}

	if (!result) {
		return false;
	}

	return true;
}

bool Texture::CreateTexture(ID3D11Device* pDevice, void* pData, Texture** ppTexture) {
	Texture* pTexture;
	bool result;

	pTexture = 0;
	result = false;

	pTexture = new Texture();
	if (!pTexture) {
		goto done;
	}

	result = pTexture->Initialize(pDevice, pData);
	if (!result) {
		goto done;
	}

	(*ppTexture) = pTexture;

done:
	if (pTexture) {
		pTexture->Shutdown();
		delete pTexture;
		pTexture = 0;
	}

	if (!result) {
		return false;
	}

	return true;
}

bool Texture::Initialize(ID3D11Device* pDevice, LPCWSTR filename) {
	HRESULT hresult;

	hresult = CreateDDSTextureFromFile(pDevice, filename, NULL, &m_SRV);
	if (FAILED(hresult)) {
		return false;
	}

	return true;
}

bool Texture::Initialize(ID3D11Device* pDevice, void* pData) {
	HRESULT hresult;

	hresult = CreateDDSTextureFromMemory(pDevice, static_cast<uint8_t*>(pData), sizeof(pData), NULL, &m_SRV);
	if (FAILED(hresult)) {
		return false;
	}

	return true;
}

void Texture::Shutdown() {
	if (m_samplerState) {
		m_samplerState->Release();
		m_samplerState = 0;
	}

	if (m_SRV) {
		m_SRV->Release();
		m_SRV = 0;
	}

	if (m_filename) {
		delete[] m_filename;
		m_filename = 0;
	}

	return;
}

ID3D11ShaderResourceView* Texture::GetSRV() {
	return m_SRV;
}