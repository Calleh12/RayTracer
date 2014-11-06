#ifndef STRUCTS_
#define STRUCTS_

#include "Object.h"
#include <DirectXMath.h>
#include <string>
#include <vector>
#include "Material.h"

using namespace DirectX;

struct Model
{
	std::vector<TriangleMesh>	m_triangles;
	Material					m_material; // kan bli vector senare f�r supporta fler �n en mtr p� mesh
};

#endif