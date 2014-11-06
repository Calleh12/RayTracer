#include "ComputeShader.h"
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib") 
#include <string>


ComputeShader::ComputeShader() : m_pDevice(NULL), m_pDeviceContext(NULL){}
ComputeShader::~ComputeShader()
{
	SAFE_RELEASE(m_pDevice);
}

bool ComputeShader::init(LPCWSTR* p_pShaderFile, char* p_pBlobFileAppendix, char* p_pFunctionName, 
			D3D10_SHADER_MACRO* p_pDefines, ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext)
{
	HRESULT hr = S_OK;
	m_pDevice = p_pDevice;
	m_pDeviceContext = p_pDeviceContext;

	ID3DBlob* pCompiledShader = NULL;
	ID3DBlob* pErrorBlob = NULL;
	
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL0;
#else
	dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL0;
#endif

	hr = D3DCompileFromFile(*p_pShaderFile, p_pDefines, NULL, p_pFunctionName, "cs_5_0", 
		dwShaderFlags, NULL, &pCompiledShader, &pErrorBlob);

	if (pErrorBlob)
	{
		std::string error = (char*)pErrorBlob->GetBufferPointer();
		OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
	}

	if(hr == S_OK)
	{
		/*
		ID3D11ShaderReflection* pReflector = NULL;
		hr = D3DReflect( pCompiledShader->GetBufferPointer(), 
			pCompiledShader->GetBufferSize(), IID_ID3D11ShaderReflection, 
			(void**) &pReflector);
		*/
		if(hr == S_OK)
		{
			hr = m_pDevice->CreateComputeShader(pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(), NULL, &m_pShader);
		}
	}

	SAFE_RELEASE(pErrorBlob);
	SAFE_RELEASE(pCompiledShader);

    return (hr == S_OK);
}

void ComputeShader::set()
{
	m_pDeviceContext->CSSetShader( m_pShader, NULL, 0 );
}

void ComputeShader::unset()
{
	m_pDeviceContext->CSSetShader( NULL, NULL, 0 );
}