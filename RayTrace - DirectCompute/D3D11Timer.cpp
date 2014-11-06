#include "D3D11Timer.h"

D3D11Timer::D3D11Timer(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext)
	: m_pDevice(p_pDevice), m_pDeviceContext(p_pDeviceContext)
{
	//ugly, should not be done in constructor :-)
    D3D11_QUERY_DESC desc;
    desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
    desc.MiscFlags = 0;
	m_pDevice->CreateQuery(&desc, &m_pDisjoint);

	desc.Query = D3D11_QUERY_TIMESTAMP;
	m_pDevice->CreateQuery(&desc, &m_pStart);
	m_pDevice->CreateQuery(&desc, &m_pStop);
}

D3D11Timer::~D3D11Timer()
{
	if(m_pStart)		m_pStart->Release();
	if(m_pStop)			m_pStart->Release();
	if(m_pDisjoint)		m_pStart->Release();
}

void D3D11Timer::start()
{
	m_pDeviceContext->Begin(m_pDisjoint);
 
	m_pDeviceContext->End(m_pStart);
}

void D3D11Timer::stop()
{
    m_pDeviceContext->End(m_pStop);

    m_pDeviceContext->End(m_pDisjoint);
}

double D3D11Timer::getTime()
{
	UINT64 startTime = 0;
	while(m_pDeviceContext->GetData(m_pStart, &startTime, sizeof(startTime), 0) != S_OK);

	UINT64 endTime = 0;
	while(m_pDeviceContext->GetData(m_pStop, &endTime, sizeof(endTime), 0) != S_OK);

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
	while(m_pDeviceContext->GetData(m_pDisjoint, &disjointData, sizeof(disjointData), 0) != S_OK);

	double time = -1.0f;
	if(disjointData.Disjoint == FALSE)
	{
		UINT64 delta = endTime - startTime;
		double frequency = static_cast<double>(disjointData.Frequency);
		time = (delta / frequency) * 1000.0f;
	}

	return time;
}