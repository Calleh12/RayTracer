#ifndef OBSERVER_H
#define OBSERVER_H

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

class Direct3D;

class Observer
{
public:
	Observer(Direct3D* p_pGame);
	
	void		broadcastLeftClick( XMFLOAT2 p_mousePosition );
	void		broadcastRightClick( XMFLOAT2 p_mousePosition );
	void		broadcastMousePos( XMFLOAT2 p_mousePosition );
	void		broadcastKeyPress( USHORT p_key );

	int			getIndex();
	bool		compair( Observer* p_pObserver );
private:
	int			m_index;
protected:
	Direct3D*	m_pGame;
};

#endif