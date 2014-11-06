#include "Material.h"
#include "ModelStructs.h"

Material::Material()
{}

Material::~Material(){}

MaterialStruct Material::getMaterial()
{
	return m_material;
}

D3DTexture* Material::getTexture()
{
	return m_pTexture;	
}

D3DTexture* Material::getNormalTexture()
{
	return m_pNormalTexture;
}

void Material::setMaterial(MaterialStruct p_mtr)
{
	m_material = p_mtr;
}

void Material::setTexture(D3DTexture* p_pTexture)
{
	m_pTexture = p_pTexture;
}

void Material::setNormalTexture(D3DTexture* p_pNormalTexture)
{
	m_pNormalTexture = p_pNormalTexture;
}