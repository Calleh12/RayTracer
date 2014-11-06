#pragma once

#include "D3DTexture.h"
#include "Object.h"

struct MaterialStruct
{
	XMFLOAT4	m_ambient; // w = shininess
	XMFLOAT4	m_diffuse; // w = alpha
	XMFLOAT4	m_specular;// w = reflection
};

class Material
{
public:
	Material();
	~Material();
	MaterialStruct		getMaterial();
	D3DTexture*			getTexture();
	D3DTexture*			getNormalTexture();
	void				setMaterial(MaterialStruct p_mtr);
	void				setTexture(D3DTexture* p_pTexture);
	void				setNormalTexture(D3DTexture* p_pNormalTexture);
private:
	MaterialStruct		m_material;
	D3DTexture*			m_pTexture;
	D3DTexture*			m_pNormalTexture;
};

