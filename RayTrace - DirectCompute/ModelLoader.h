#ifndef MODELLOADER_
#define MODELLOADER_

#include "ModelStructs.h"

#include <fstream>
#include <sstream>

#include <string>


using namespace std;

class ModelLoader
{
public:
	ModelLoader(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext);
	~ModelLoader();
	Material	loadMaterialFromMTL(string p_materialFileName);
	Model		loadModelFromOBJ(string p_OBJFileName, float p_startIndex);
private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

	//ID3D11ShaderResourceView*		createTexture(string p_textureFileName);
	string				m_OBJFileName;
	vector<XMFLOAT3>	m_positions;
	vector<XMFLOAT2>	m_texCoords;
	vector<XMFLOAT3>	m_normals;

	Model				m_model;
};
#endif