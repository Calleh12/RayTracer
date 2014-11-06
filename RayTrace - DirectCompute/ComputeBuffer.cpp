#include "ComputeBuffer.h"

ComputeBuffer::ComputeBuffer()
{
	m_pResource = NULL;
	m_pResourceView = NULL;
	m_pUnorderedAccessView = NULL;
	m_pStaging = NULL;
}

ComputeBuffer::~ComputeBuffer()
{
	release();
}

void ComputeBuffer::release()
{
	SAFE_RELEASE(m_pResource);
	SAFE_RELEASE(m_pResourceView);
	SAFE_RELEASE(m_pUnorderedAccessView);
	SAFE_RELEASE(m_pStaging);
}

template<class T> T* ComputeBuffer::map()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource; 
	T* p = NULL;
	if(SUCCEEDED(m_pDeviceContext->Map( m_pStaging, 0, D3D11_MAP_READ, 0, &mappedResource )))
		p = (T*)mappedResource.pData;

	return p;
}

void ComputeBuffer::unmap()
{
	m_pDeviceContext->Unmap(m_pStaging, 0);
}

ID3D11Buffer* ComputeBuffer::getResource()
{
	return m_pResource;
}

ID3D11ShaderResourceView* ComputeBuffer::getResourceView()
{
	return m_pResourceView;
}

ID3D11UnorderedAccessView* ComputeBuffer::getUnorderedAccessView()
{
	return m_pUnorderedAccessView;
}

ID3D11Buffer* ComputeBuffer::getStaging()
{
	return m_pStaging;
}

void ComputeBuffer::copyToStaging()
{
	m_pDeviceContext->CopyResource( m_pStaging, m_pResource);
}

