#include "ModelLoader.h"

ModelLoader::ModelLoader(ID3D11Device* p_pDevice, ID3D11DeviceContext* p_pDeviceContext)
{
	m_pDevice = p_pDevice;
	m_pDeviceContext = p_pDeviceContext;
}

ModelLoader::~ModelLoader(){}

Material ModelLoader::loadMaterialFromMTL( string p_materialFileName )
{
	Material material;
	MaterialStruct mtrStruct;
	fstream objFile(p_materialFileName);

	if(objFile)
	{
		string line;
		string prefix;

		while(objFile.eof() == false)
		{
			prefix = "NULL"; //leave nothing from the previous iteration
			stringstream lineStream;

			getline(objFile, line);
			lineStream << line;
			lineStream >> prefix;

			if(prefix == "map_Kd")
			{
				string materialTextureName;
				lineStream >> materialTextureName;
				wstring name (materialTextureName.begin(), materialTextureName.end());
				D3DTexture* texture = new D3DTexture(m_pDevice, m_pDeviceContext);
				texture->createTexture(&name, NULL, false);
				material.setTexture(texture);
				//m_material->m_textureResource = createTexture(materialTextureName);
			}
			else if(prefix == "Ns")
			{
				int nShininess;
				lineStream >> nShininess;
				mtrStruct.m_ambient.w = nShininess;
			}
			else if(prefix == "d" || prefix == "Tr" )
			{
				lineStream >> mtrStruct.m_diffuse.w;
			}
			else if(prefix == "Ks")
			{
				float r, g, b;
				lineStream >> r >> g >> b;
				mtrStruct.m_specular.x = r; 
				mtrStruct.m_specular.y = g;
				mtrStruct.m_specular.z = b;
			}
			else if(prefix == "Kd")
			{
				float r, g, b;
				lineStream >> r >> g >> b;
				mtrStruct.m_diffuse.x = r; 
				mtrStruct.m_diffuse.y = g;
				mtrStruct.m_diffuse.z = b;
			}	
			else if(prefix == "Ka")
			{
				float r, g, b;
				lineStream >> r >> g >> b;
				mtrStruct.m_ambient.x = r; 
				mtrStruct.m_ambient.y = g;
				mtrStruct.m_ambient.z = b;
			}
		}
	}
	
	material.setMaterial(mtrStruct);
	return material;
}

Model ModelLoader::loadModelFromOBJ( string p_OBJFileName, float p_startIndex )
{
	Model model;
	fstream objFile(p_OBJFileName);

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
				lineStream >> strMaterialFilename;
				model.m_material = loadMaterialFromMTL(strMaterialFilename);
			}
			else if(prefix == "v")
			{
				XMFLOAT3 pos;
				lineStream >> pos.x >> pos.y >> pos.z;
				m_positions.push_back(pos);
			}
			else if(prefix == "vt")
			{
				XMFLOAT2 uv;
				lineStream >> uv.x >> uv.y;
				uv.y = 1 - uv.y; 
				m_texCoords.push_back(uv);
			}
			else if (prefix == "vn")
			{
				XMFLOAT3 normal;
				lineStream >> normal.x >> normal.y >> normal.z;
				m_normals.push_back(normal);
			}
			else if(prefix == "f")
			{
				XMFLOAT3 tempPos;
				TriangleMesh tempTriangle;
				char tmp;

				int indexPos	= 0;
				int texPos		= 0;
				int normPos		= 0;

				for(int i=0; i<3; i++)
				{
					lineStream >> indexPos >> tmp >> texPos >>  tmp >> normPos;

					tempPos = m_positions[ indexPos - 1];

					tempTriangle.trig.corners[i] = XMFLOAT4(tempPos.x, tempPos.y, tempPos.z, 1.0f);
					tempTriangle.uv[i]			 = m_texCoords[ texPos-1 ];
					tempTriangle.normal[i]			 = m_normals[ normPos-1 ];
				}
				tempTriangle.trig.corners[0].w = p_startIndex;
				p_startIndex++;
				model.m_triangles.push_back(tempTriangle);
			}
		}
	}

	return model;
}






