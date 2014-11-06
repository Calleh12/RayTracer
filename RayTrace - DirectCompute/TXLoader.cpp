#include "TXLoader.h"

TXLoader::TXLoader(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext)
{
	m_pDevice = p_pDevice;
	m_pDeviceContext = p_pDeviceContext;
}

TXLoader::~TXLoader(){}

Model TXLoader::loadModelFromTX( string p_TXFileName, float p_startIndex )
{
	Model model;
	Material material;
	fstream objFile(p_TXFileName);

	if(objFile)
	{
		string strMaterialFilename;
		string line;
		string prefix;

		while(objFile.eof() == false)
		{
			prefix = "NULL"; //leave nothing from the previous iteration
			stringstream lineStream;

			getline(objFile, line);
			lineStream << line;
			lineStream >> prefix;

			if(prefix == "mtllib")
			{
			//	lineStream >> strMaterialFilename;
			//	model.m_material = loadMaterialFromMTL(strMaterialFilename);
			}
			else if(prefix == "NormalMap:")
			{
					string materialTextureName;
					lineStream >> materialTextureName;
					wstring name (materialTextureName.begin(), materialTextureName.end());
					D3DTexture* texture = new D3DTexture(m_pDevice, m_pDeviceContext);
					texture->createTexture(&name, NULL, true);
					material.setNormalTexture(texture);
					model.m_material = material;
			}
			else if(prefix == "DiffuseMap:")
			{
					string materialTextureName;
					lineStream >> materialTextureName;
					wstring name (materialTextureName.begin(), materialTextureName.end());
					D3DTexture* texture = new D3DTexture(m_pDevice, m_pDeviceContext);
					texture->createTexture(&name, NULL, true);
					material.setTexture(texture);
					model.m_material = material;
			}
			else if(prefix == "v")
			{
				XMFLOAT3 pos;
				lineStream >> pos.x >> pos.y >> pos.z;
				m_positions.push_back(pos);
			}
			else if(prefix == "uv")
			{
				XMFLOAT2 uv;
				lineStream >> uv.x >> uv.y;
				uv.y = 1 - uv.y; 
				m_texCoords.push_back(uv);
			}
			else if (prefix == "n")
			{
				XMFLOAT3 normal;
				lineStream >> normal.x >> normal.y >> normal.z;
				m_normals.push_back(normal);
			}
			else if (prefix == "t")
			{
				XMFLOAT3 tangent;
				lineStream >> tangent.x >> tangent.y >> tangent.z;
				m_tangents.push_back(tangent);
			}
			else if(prefix == "f")
			{
				XMFLOAT3 tempPos;
				TriangleMesh tempTriangle;
				char tmp;

				int indexPos	= 0;
				int texPos		= 0;
				int normPos		= 0;
				int tangoPuss	= 0;

				for(int i=0; i<3; i++)
				{
					lineStream >> indexPos >> tmp >> tangoPuss >>  tmp >> normPos >> tmp >> texPos;

					tempPos = m_positions[ indexPos];

					tempTriangle.trig.corners[i] = XMFLOAT4(tempPos.x, tempPos.y, tempPos.z, 1.0f);
					tempTriangle.uv[i]			 = m_texCoords[ texPos ];
					tempTriangle.normal[i]			 = m_normals[ normPos ];
					tempTriangle.tangent[i]		 = m_tangents[ tangoPuss ];

					//DirectX::XMStoreFloat3(&tempTriangle.binormal, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&tempTriangle.tangent),DirectX::XMLoadFloat3(&tempTriangle.normal)));
				}
				tempTriangle.trig.corners[0].w = p_startIndex;
				p_startIndex++;
				model.m_triangles.push_back(tempTriangle);
			}
		}
	}

	return model;
}