#pragma once

#include "ComputeHeader.h"

enum COMPUTE_BUFFER_TYPE
{
	STRUCTURED_BUFFER,
	RAW_BUFFER,
	CONSTANT_BUFFER
};

class ComputeBuffer
{
private:
	ComputeBuffer(const ComputeBuffer& cb) {}

	ID3D11Buffer*				m_pResource;
	ID3D11ShaderResourceView*	m_pResourceView;
	ID3D11UnorderedAccessView*	m_pUnorderedAccessView;
	ID3D11Buffer*				m_pStaging;

	ID3D11DeviceContext*        m_pDeviceContext;
public:
	explicit ComputeBuffer();
	~ComputeBuffer();

	void release();

	template<class T> T* map();
	void unmap();

	ID3D11Buffer*				getResource();
	ID3D11ShaderResourceView*	getResourceView();
	ID3D11UnorderedAccessView*	getUnorderedAccessView();
	ID3D11Buffer*				getStaging();
	void						copyToStaging();

	friend class ComputeWrap;

};

