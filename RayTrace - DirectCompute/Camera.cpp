#include "camera.h"

const float Camera::DEFAULT_FOVX = 80.f;
const float Camera::DEFAULT_ZFAR = 1000.0f;
const float Camera::DEFAULT_ZNEAR = 1.0f;

const XMFLOAT4 Camera::WORLD_XAXIS = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
const XMFLOAT4 Camera::WORLD_YAXIS = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
const XMFLOAT4 Camera::WORLD_ZAXIS = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);


Camera::Camera()
{
	m_pitch = 0.0f;
	m_yaw = 0.0f;
	m_roll = 0.0f;

	m_fovx = DEFAULT_FOVX;
	m_znear = DEFAULT_ZNEAR;
	m_zfar = DEFAULT_ZFAR;

	m_eye = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_target = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_offset = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	m_xAxis = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	m_yAxis = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	m_zAxis = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projMatrix, XMMatrixIdentity());

	m_rotate = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	//D3DXQuaternionIdentity(&m_orientation);
}

Camera::~Camera(){};

void Camera::setTarget(const XMFLOAT4 &p_target)
{
	m_target = p_target;
}

void Camera::view(const XMFLOAT4 &p_eye,
			 const XMFLOAT4 &p_target,
			 const XMFLOAT4 &p_up)
{
	m_eye = p_eye;
	m_target = p_target;

	//The offset vector is the vector from the taget to the eye pos.
	//This means that the negative z axis is the following.

	XMStoreFloat4(&m_zAxis, XMLoadFloat4(&p_target) - XMLoadFloat4(&p_eye));
	//m_zAxis = XMVector3Transform(m_zAxis, XMMatrixRotationQuaternion(m_rotate));
	//m_zAxis =  XMVector3Transform( m_zAxis, XMMatrixRotationX( m_yaw ) );
	XMStoreFloat4(&m_offset, -XMLoadFloat4(&m_zAxis));
	XMStoreFloat4(&m_zAxis, XMVector4Normalize(XMLoadFloat4(&m_zAxis)));

	XMStoreFloat4(&m_xAxis, XMVector3Cross( XMLoadFloat4(&p_up), XMLoadFloat4(&m_zAxis) ));
	//m_xAxis =  XMVector3Transform( m_xAxis, XMMatrixRotationX( m_yaw ) );
	//m_xAxis = XMVector3Transform(m_xAxis, XMMatrixRotationQuaternion(m_rotate));
	XMStoreFloat4(&m_xAxis, XMVector4Normalize( XMLoadFloat4(&m_xAxis) ));
	
	XMStoreFloat4(&m_yAxis, XMVector3Cross( XMLoadFloat4(&m_zAxis), XMLoadFloat4(&m_xAxis) ));
	XMStoreFloat4(&m_yAxis, XMVector4Normalize( XMLoadFloat4(&m_yAxis) ));

	rotateX(m_pitch);
	rotateY(m_yaw);

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());

	//Building View Matrix
	float dot;
	dot = -XMVector3Dot(XMLoadFloat4(&m_xAxis), XMLoadFloat4(&m_eye)).m128_f32[0];

	m_viewMatrix._11 = m_xAxis.x;
	m_viewMatrix._21 = m_xAxis.y;
	m_viewMatrix._31 = m_xAxis.z;
	m_viewMatrix._41 = dot;

	m_viewMatrix._12 = m_yAxis.x;
	m_viewMatrix._22 = m_yAxis.y;
	m_viewMatrix._32 = m_yAxis.z;
	dot = -XMVector3Dot(XMLoadFloat4(&m_yAxis), XMLoadFloat4(&m_eye)).m128_f32[0];
	m_viewMatrix._42 = dot;

	m_viewMatrix._13 = m_zAxis.x;
	m_viewMatrix._23 = m_zAxis.y;
	m_viewMatrix._33 = m_zAxis.z;
	dot = -XMVector3Dot(XMLoadFloat4(&m_zAxis), XMLoadFloat4(&m_eye)).m128_f32[0];
	m_viewMatrix._43 = dot;

	XMStoreFloat4x4(&m_viewMatrix, XMLoadFloat4x4(&m_viewMatrix) * XMMatrixRotationQuaternion(XMLoadFloat4(&m_rotate)));

	//m_viewMatrix = XMMatrixLookAtLH( p_eye, p_target, p_up );
}

void Camera::perspective(float p_fovx, float p_aspect, float p_znear, float p_zfar)
{
	float e = 1.0f / tanf(p_fovx / 2.0f);
	float aspectInv = 1.0f / p_aspect;
	float fovy = 2.0f * atanf(aspectInv / e);
	float xScale = 1.0f / tanf(0.5f * fovy);
	float yScale = xScale / aspectInv;

	m_projMatrix._11 = xScale;
	m_projMatrix._21 =	0.0f;
	m_projMatrix._31 =	0.0f;
	m_projMatrix._41 =	0.0f;

	m_projMatrix._12 = 0.0f; 
	m_projMatrix._22 = yScale; 
	m_projMatrix._32 = 0.0f; 
	m_projMatrix._42 = 0.0f;

	m_projMatrix._13 = 0.0f;
	m_projMatrix._23 = 0.0f;
	m_projMatrix._33 = p_zfar / (p_zfar - p_znear);
	m_projMatrix._43 = -p_znear * p_zfar / (p_zfar - p_znear);

	m_projMatrix._14 = 0.0f;
	m_projMatrix._24 = 0.0f;
	m_projMatrix._34 = 1.0f;
	m_projMatrix._44 = 0.0f;

    m_fovx = p_fovx;
    m_znear = p_znear;
    m_zfar = p_zfar;

	
	//m_camProjection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, (float)m_ClientWidth/m_ClientHeight, 1.0f, 1000.0f);
	//m_projMatrix = XMMatrixPerspectiveFovLH( p_fovx, p_aspect, 1.0f, 300.0f);
}

void Camera::rotate(float p_pitch, float p_yaw, float p_roll)
{
	// Longitude and Latitude degrees represents the maximum
	// number of degrees of rotation per second.
	m_pitch += p_pitch;
	if(m_pitch > 3.14f * 0.5f)
		m_pitch = 3.14f * 0.5f;
	else if (m_pitch < -3.14f * 0.5f)
		m_pitch = -3.14f * 0.5f;

	m_yaw	+= p_yaw;
	m_roll	+= p_roll;
	//m_rotate = XMQuaternionRotationRollPitchYaw(m_pitch, m_yaw, m_roll);
}

void Camera::update(float gameTime)
{
	/*float longitudeElapsed = m_longitudeDegrees;
	float latitudeElapsed = m_latitudeDegrees;*/

	XMVECTOR rot;
	XMVECTOR rotInverse;

	rot = XMQuaternionRotationRollPitchYaw(m_pitch, m_yaw, m_roll );
	//rotInverse = XMQuaternionConjugate(rot);

	XMVECTOR offsetVector = XMLoadFloat4(&m_offset);
	offsetVector.m128_f32[3] = 0.0f;

	XMVECTOR result = /*rotInverse */ offsetVector * rot;

	XMVECTOR transformedOffsetVector = result;
	XMVECTOR newCameraPosition = transformedOffsetVector + XMLoadFloat4(&m_target);

	//Rebuild view matrix
	XMFLOAT4 temp;
	XMStoreFloat4(&temp, newCameraPosition);
	view(temp, m_target, WORLD_YAXIS);
}

void Camera::rotateX(float p_angle)
{
	XMMATRIX rotate;
	rotate = XMMatrixRotationAxis(XMLoadFloat4(&m_xAxis), p_angle);

	XMStoreFloat4(&m_yAxis, XMVector3TransformNormal(XMLoadFloat4(&m_yAxis), rotate));
	XMStoreFloat4(&m_zAxis, XMVector3TransformNormal(XMLoadFloat4(&m_zAxis), rotate));
}

void Camera::rotateY(float p_angle)
{
	XMMATRIX rotate;
	rotate = XMMatrixRotationY(p_angle);

	XMStoreFloat4(&m_xAxis, XMVector3TransformNormal(XMLoadFloat4(&m_xAxis), rotate));
	XMStoreFloat4(&m_yAxis, XMVector3TransformNormal(XMLoadFloat4(&m_yAxis), rotate));
	XMStoreFloat4(&m_zAxis, XMVector3TransformNormal(XMLoadFloat4(&m_zAxis), rotate));
}

//Get Functions

const XMVECTOR Camera::getPosition()
{
	return XMLoadFloat4(&m_eye);
}

const XMVECTOR Camera::getLookAt()
{
	return XMLoadFloat4(&m_zAxis);
}

XMMATRIX Camera::getViewMatrix()
{
	return XMLoadFloat4x4(&m_viewMatrix);
}

XMMATRIX Camera::getProjectionMatrix()
{
	return XMLoadFloat4x4(&m_projMatrix);
}

const XMVECTOR Camera::getTarget()
{
	return XMLoadFloat4(&m_target);
}

XMVECTOR Camera::getRightVector()
{
	return XMLoadFloat4(&m_xAxis);
}

XMVECTOR Camera::getUpVector()
{
	return XMLoadFloat4(&WORLD_YAXIS);
}