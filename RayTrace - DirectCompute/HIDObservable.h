#ifndef HIDOBS_H
#define HIDOBS_H

#include <Windows.h>
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

class Observer;

class HIDObservable
{
public:
	HIDObservable();
	~HIDObservable();

	void					broadcastLeftClick( XMFLOAT2 p_mousePosition );
	void					broadcastRightClick( XMFLOAT2 p_mousePosition );
	void					broadcastMousePos( XMFLOAT2 p_mousePosition );
	void					broadcastKeyPress( USHORT p_key );
	void					addSubscriber( Observer* p_pObserver );
	void					removeSubscriber( Observer* p_pObserver );

private:
	std::vector<Observer*>	m_subscribers;
};

#endif