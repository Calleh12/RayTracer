#ifndef DIRECT3D_H
#define DIRECT3D_H

#include "d3dApp.h"
#include <DirectXPackedVector.h>
#include <string>

#include "Buffer.h"
#include "Shader.h"

#include "ComputeBuffer.h"
#include "ComputeShader.h"

#include "Camera.h"

#include "Object.h"

#include "WinHID.h"
#include "Observer.h"

#include "ModelLoader.h"

using namespace DirectX;


struct RayCBufferStruct
{
	XMVECTOR camPos;
	XMMATRIX view;
	XMMATRIX proj;
};

struct IntersectionCBufferStruct
{
	Triangle triangle; // Structured buffer for models
	Sphere sphere;
	Box box;
	MaterialStruct mtr;
	//PointLight lights; 
};

struct ColorCBufferStruct
{
	SpotLight	lights[15];
	Triangle	triangle;
	Sphere		sphere;
	Box			box;
};
 
class Direct3D : public D3DApp
{
public:
	Direct3D(HINSTANCE p_hInstance);
	~Direct3D();

	void initApp();
	//void onResize();
	void updateScene(float p_dt);
	void drawScene();

	LRESULT msgProc(UINT p_msg, WPARAM p_wParam, LPARAM p_lParam);

	// ## Input Methods ##
	void		leftMouseClick( XMFLOAT2 p_mousePosition );
	void		rightMouseClick( XMFLOAT2 p_mousePosition );
	void		mouseMove( XMFLOAT2 p_mousePosition );
	void		keyEvent( USHORT p_key );
private:
	Camera*		m_pCamera;

	int			m_dispatchSize;

	//Camera Variables
	XMFLOAT4X4	m_camView;
	XMFLOAT4X4	m_camProjection;
	XMFLOAT4X4	m_world;
	XMFLOAT4X4	m_WVP;
	XMFLOAT4	m_camPosition;
	XMFLOAT4	m_camTarget;
	XMFLOAT4	m_camUp;

	Buffer		m_buffer;
	Shader		m_shader;

	// ## Input ##
	HID*		m_pHID;
	Observer*	m_pObserver;
	float		m_speed;

	// ## Ray tracing ##
	ID3D11SamplerState*			m_pTextureSampler;
	Model						optimus;
	IntersectionCBufferStruct	m_intersectionStruct;
	ColorCBufferStruct			m_colorStruct;
	XMFLOAT4X4					m_rotation;
	float						m_movingAngle;

	float rays;
	float intersection;
	float color;
};
#endif