#pragma once

#include "ComputeHeader.h"
#include "ComputeBuffer.h"

class ComputeTexture
{
private:
	ComputeTexture(const ComputeBuffer& cb) {}

	ID3D11Texture2D*			m_pResource;
	ID3D11ShaderResourceView*	m_pResourceView;
	ID3D11UnorderedAccessView*	m_pUnorderedAccessView;
	ID3D11Texture2D*			m_pStaging;
	ID3D11DeviceContext*        m_pDeviceContext;
public:
	explicit ComputeTexture();
	~ComputeTexture();

	void release();

	template<class T> T* map();
	void unmap();

	ID3D11Texture2D*			getResource();
	ID3D11ShaderResourceView*	getResourceView();
	ID3D11UnorderedAccessView*	getUnorderedAccessView();
	ID3D11Texture2D*			getStaging();
	void						copyToStaging();

	friend class ComputeWrap;
};

