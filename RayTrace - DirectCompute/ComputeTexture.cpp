#include "ComputeTexture.h"

ComputeTexture::ComputeTexture()
{
	m_pResource = NULL;
	m_pResourceView = NULL;
	m_pUnorderedAccessView = NULL;
	m_pStaging = NULL;
}

ComputeTexture::~ComputeTexture()
{
	release();
}

void ComputeTexture::release()
{
	SAFE_RELEASE(m_pResource);
	SAFE_RELEASE(m_pResourceView);
	SAFE_RELEASE(m_pUnorderedAccessView);
	SAFE_RELEASE(m_pStaging);
}

template<class T> T* ComputeTexture::map()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource; 
	T* p = NULL;
	if(SUCCEEDED(m_pDeviceContext->Map( m_pStaging, 0, D3D11_MAP_READ, 0, &mappedResource )))
		p = (T*)mappedResource.pData;

	return p;
}

void ComputeTexture::unmap()
{
	m_pDeviceContext->Unmap(m_pStaging, 0);
}

ID3D11Texture2D* ComputeTexture::getResource()
{
	return m_pResource;
}

ID3D11ShaderResourceView* ComputeTexture::getResourceView()
{
	return m_pResourceView;
}

ID3D11UnorderedAccessView* ComputeTexture::getUnorderedAccessView()
{
	return m_pUnorderedAccessView;
}

ID3D11Texture2D* ComputeTexture::getStaging()
{
	return m_pStaging;
}

void ComputeTexture::copyToStaging()
{
	m_pDeviceContext->CopyResource( m_pStaging, m_pResource);
}