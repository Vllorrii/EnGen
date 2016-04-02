#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#pragma comment(lib, "DirectXTK.lib")

#include <DDSTextureLoader.h>
#include <Windows.h>
using namespace DirectX;

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

class Texture {
private:
	Texture();
	Texture(const Texture& other);
	~Texture();

	bool Initialize(ID3D11Device* pDevice, LPCWSTR filename);
	bool Initialize(ID3D11Device* pDevice, void* pData);

public:
	static bool CreateTexture(ID3D11Device* pDevice, LPCWSTR filename, Texture** ppTexture);
	static bool CreateTexture(ID3D11Device* pDevice, void* pData, Texture** ppTexture);

	void Shutdown();

	ID3D11ShaderResourceView* GetSRV();

private:
	LPCWSTR m_filename;
	ID3D11ShaderResourceView* m_SRV;
	ID3D11SamplerState* m_samplerState;
};

#endif