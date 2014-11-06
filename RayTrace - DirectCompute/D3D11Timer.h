#include <D3D11.h>

class D3D11Timer
{
	ID3D11Query*	m_pDisjoint;
	ID3D11Query*	m_pStart;
	ID3D11Query*	m_pStop;

	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;

public:
	explicit D3D11Timer(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext);
	~D3D11Timer();

	void start();
	void stop();

	double getTime();
};