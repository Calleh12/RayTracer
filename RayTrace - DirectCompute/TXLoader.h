#pragma once

#include "ModelStructs.h"

#include <fstream>
#include <sstream>

#include <string>

using namespace std;

class TXLoader
{
public:
	TXLoader(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext);
	~TXLoader();
	Model		loadModelFromTX(string p_TXFileName, float p_startIndex);
private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

	//ID3D11ShaderResourceView*		createTexture(string p_textureFileName);
	string				m_TXFileName;
	vector<XMFLOAT3>	m_positions;
	vector<XMFLOAT2>	m_texCoords;
	vector<XMFLOAT3>	m_normals;
	vector<XMFLOAT3>	m_tangents;

	Model				m_model;
};