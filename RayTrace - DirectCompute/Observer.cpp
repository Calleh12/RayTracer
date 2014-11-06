#include "Observer.h"
#include "Direct3D.h"

Observer::Observer(Direct3D* p_pGame)
{
	m_pGame = p_pGame;
}

int Observer::getIndex()
{
	return m_index;
}

bool Observer::compair( Observer* p_pObserver )
{
	return m_index == p_pObserver->getIndex(); 
}

void Observer::broadcastLeftClick( XMFLOAT2 p_mousePosition )
{
	m_pGame->leftMouseClick( p_mousePosition );
}

void Observer::broadcastRightClick( XMFLOAT2 p_mousePosition )
{
	m_pGame->rightMouseClick( p_mousePosition );
}

void Observer::broadcastMousePos( XMFLOAT2 p_mousePosition )
{
	m_pGame->mouseMove( p_mousePosition );
}

void Observer::broadcastKeyPress( USHORT p_key )
{ 
	m_pGame->keyEvent(p_key);
}