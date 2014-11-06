#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

#ifndef MOUSE_H
#define	MOUSE_H

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

class Mouse
{
public:
	Mouse();
	Mouse( HWND p_hwnd );
	~Mouse();
	
	bool			click( RAWINPUT* p_pRaw, USHORT  p_flag );
	POINT			getPosition();
	XMFLOAT2		mouseMove( RAWINPUT* p_pRaw );
	
private:
	int				m_x;
	int				m_y;
	HWND			m_hwnd;
};

#endif