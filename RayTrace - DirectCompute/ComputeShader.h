#pragma once

#include "ComputeHeader.h"

class ComputeShader
{
private:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
	ID3D11ComputeShader*	m_pShader;

	explicit	ComputeShader();
	bool		init(LPCWSTR* p_pShaderFile, char* p_pBlobFileAppendix, char* p_pFunctionName, 
				D3D10_SHADER_MACRO* p_pDefines, ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext);
public:
	~ComputeShader();
	void	set();
	void	unset();
	
	friend class ComputeWrap;
};

