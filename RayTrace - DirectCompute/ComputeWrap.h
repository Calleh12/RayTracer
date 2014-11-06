#pragma once

#include "ComputeHeader.h"
#include "ComputeShader.h"
#include "ComputeBuffer.h"
#include "ComputeTexture.h"
//#include <DirectXTex.h>
#include "WICTextureLoader.h"

class ComputeWrap
{
private:
	ID3D11Device*               m_pDevice;
	ID3D11DeviceContext*        m_pDeviceContext;

	ID3D11Buffer* createStructuredBuffer(UINT p_uElementSize, UINT p_uCount, bool p_bSRV, bool p_bUAV, VOID* p_pInitData);
	ID3D11Buffer* createRawBuffer(UINT p_uSize, VOID* p_pInitData);
	ID3D11ShaderResourceView* createBufferSRV(ID3D11Buffer* p_pBuffer);
	ID3D11UnorderedAccessView* createBufferUAV(ID3D11Buffer* p_pBuffer);
	ID3D11Buffer* createStagingBuffer(UINT p_uSize);

	//texture functions
	ID3D11Texture2D* createTextureResource(DXGI_FORMAT p_dxFormat,
		UINT p_uWidth, UINT p_uHeight, UINT p_uRowPitch, VOID* p_pInitData);
	//ID3D11Buffer* CreateRawBuffer(UINT uSize, VOID* pInitData);
	ID3D11ShaderResourceView* createTextureSRV(ID3D11Texture2D* p_pTexture);
	ID3D11UnorderedAccessView* createTextureUAV(ID3D11Texture2D* p_pTexture);
	ID3D11Texture2D* createStagingTexture(ID3D11Texture2D* p_pTexture);
	
	void setDebugName(ID3D11DeviceChild* p_object, char* p_debugName);

public:
	ComputeWrap(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext);
	~ComputeWrap();

	ComputeShader* createComputeShader(LPCWSTR* p_pShaderFile, char* p_pBlobFileAppendix, char* p_pFunctionName, D3D10_SHADER_MACRO* p_pDefines);

	ID3D11Buffer* createConstantBuffer(UINT p_uSize, VOID* p_pInitData, char* p_pDebugName = NULL);
	// Constant Buffer sizeof(elementSize)
	ComputeBuffer* createBuffer(COMPUTE_BUFFER_TYPE p_uType, UINT p_uElementSize,
		UINT p_uCount, bool p_bSRV, bool p_bUAV, VOID* p_pInitData, bool p_bCreateStaging = false, char* p_debugName = NULL);

	ComputeTexture* createTexture(DXGI_FORMAT p_dxFormat,	UINT p_uWidth,
	UINT p_uHeight, UINT p_uRowPitch, VOID* p_pInitData, bool p_bCreateStaging = false, char* p_debugName = NULL);

	ComputeTexture* createTexture(wchar_t* p_textureFilename, char* p_debugName = NULL);
};

