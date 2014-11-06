#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	//Constructor and Destructors
	Camera();
	~Camera();

	void	setTarget(const XMFLOAT4 &p_target);
	void	view(const XMFLOAT4 &p_eye,
			 const XMFLOAT4 &p_target,
			 const XMFLOAT4 &p_up);
	void	perspective(float p_fovx, float p_aspect, float p_znear, float p_zfar);
	void	rotate(float p_pitch, float p_yaw, float p_roll);
	void	update(float p_gameTime);

	void	rotateX(float p_angle);
	void	rotateY(float p_angle);

	//Getter methods.

	const XMVECTOR		getPosition();
	XMMATRIX			getViewMatrix();
	const XMVECTOR		getLookAt();
	XMMATRIX			getProjectionMatrix();
	const XMVECTOR		getTarget();
	XMVECTOR			getRightVector();
	XMVECTOR			getUpVector();

private:
	static const float DEFAULT_FOVX;
	static const float DEFAULT_ZNEAR;
	static const float DEFAULT_ZFAR;
	static const XMFLOAT4 WORLD_XAXIS;
	static const XMFLOAT4 WORLD_YAXIS;
	static const XMFLOAT4 WORLD_ZAXIS;

	float m_pitch, m_yaw, m_roll;
	float m_fovx;
	float m_znear;
	float m_zfar;
	XMFLOAT4 m_eye;
	XMFLOAT4 m_target;
	XMFLOAT4 m_offset;
	XMFLOAT4 m_xAxis;
	XMFLOAT4 m_yAxis;
	XMFLOAT4 m_zAxis;
	XMFLOAT4X4 m_viewMatrix;
	XMFLOAT4X4 m_projMatrix;
	//D3DXQUATERNION m_orientation;
	XMFLOAT4 m_rotate;
};

#endif