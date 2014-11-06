#pragma once

#include <DirectXMath.h>

using namespace DirectX;

struct Sphere
{
	XMFLOAT4	center; // w = shininess
	XMFLOAT4	color;
	XMFLOAT4	radius; // y = radius^2, z = id, w = flagga AccBuff
	//float		radius;
	//float		radius2;

	Sphere(XMFLOAT4 p_center, XMFLOAT4 p_color, float p_radius, float p_id)
	{
		center	= p_center;
		color	= p_color;
		radius	= XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		radius.x	= p_radius;
		radius.y	= p_radius * p_radius;
		radius.z	= p_id;
		radius.w	= 0.0f; // Flag for ful haxx
	}

	Sphere(){}
};

struct Triangle
{
	XMFLOAT4 corners[3]; //0.w = id, corners[1].w = shininess
	//XMFLOAT2 uv[3];
	//XMFLOAT2 pad;
	//float det;

	Triangle(XMFLOAT4 p_corners[3], float p_id)
	{
		for(int i = 0; i < 3; i++)
		{
			corners[i] = p_corners[i];
		}
		corners[0].w = p_id;
	}

	Triangle(XMFLOAT4 p_corners[3], float p_id, XMFLOAT2 p_uv)
	{
		for(int i = 0; i < 3; i++)
		{
			corners[i] = p_corners[i];
		}
		corners[0].w = p_id;
	}

	Triangle(){}
};

struct TriangleMesh
{
	Triangle trig;
	XMFLOAT2 uv[3];
	XMFLOAT3 normal[3];
	XMFLOAT3 tangent[3];
	//XMFLOAT3 binormal;
	//float	pad;

	TriangleMesh(){}

	TriangleMesh(Triangle p_trig, XMFLOAT2 p_uv[3], XMFLOAT3 p_normal[3], XMFLOAT3 p_tangent[3])
	{
		trig = p_trig;

		for(int i = 0; i < 3; i++)
		{
			uv[i] = p_uv[i];
			normal[i] = p_normal[i];
			tangent[i] = p_tangent[i];
		}

		//DirectX::XMStoreFloat3(&binormal, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&tangent),DirectX::XMLoadFloat3(&normal)));
	}
};

struct Box
{
	Triangle triangles[10]; 

	Box(XMFLOAT4 p_bot, XMFLOAT4 p_top, float p_id, float p_shininess)
	{
		//Left side
		triangles[0].corners[0] = p_bot; // 0
		triangles[0].corners[0].w = p_id;
		triangles[0].corners[1] = XMFLOAT4( p_bot.x, p_bot.y, p_top.z, p_shininess); // 4
		triangles[0].corners[2] = XMFLOAT4( p_bot.x, p_top.y, p_bot.z, 0.0f); // 2
		p_id++;

		triangles[1].corners[0] = XMFLOAT4( p_bot.x, p_top.y, p_bot.z, p_id); // 2
		triangles[1].corners[1] = XMFLOAT4( p_bot.x, p_bot.y, p_top.z, p_shininess); // 4
		triangles[1].corners[2] = XMFLOAT4( p_bot.x, p_top.y, p_top.z, 0.0f); // 6
		p_id++;
		
		//Right side
		triangles[2].corners[0] = p_top; // 7
		triangles[2].corners[0].w = p_id;
		triangles[2].corners[1] = XMFLOAT4( p_top.x, p_bot.y, p_top.z, p_shininess); // 5
		triangles[2].corners[2] = XMFLOAT4( p_top.x, p_bot.y, p_bot.z, 0.0f); // 1
		p_id++;

		triangles[3].corners[0] = XMFLOAT4( p_top.x, p_bot.y, p_bot.z, p_id); // 1
		triangles[3].corners[1] = XMFLOAT4( p_top.x, p_top.y, p_bot.z, p_shininess); // 3
		triangles[3].corners[2] = p_top;
		p_id++;

		//Back side
		triangles[4].corners[0] = XMFLOAT4( p_bot.x, p_bot.y, p_top.z, p_id); // 4
		triangles[4].corners[1] = XMFLOAT4( p_top.x, p_bot.y, p_top.z, p_shininess); // 5
		triangles[4].corners[2] = XMFLOAT4( p_bot.x, p_top.y, p_top.z, 0.0f); // 6
		p_id++;

		triangles[5].corners[0] = XMFLOAT4( p_bot.x, p_top.y, p_top.z, p_id); // 6
		triangles[5].corners[1] = XMFLOAT4( p_top.x, p_bot.y, p_top.z, p_shininess); // 5
		triangles[5].corners[2] = p_top; // 7
		p_id++;

		//Top side
		triangles[6].corners[0] = XMFLOAT4( p_bot.x, p_top.y, p_top.z, p_id); // 6
		triangles[6].corners[1] = XMFLOAT4( p_top.x, p_top.y, p_bot.z, p_shininess); // 3
		triangles[6].corners[2] = XMFLOAT4( p_bot.x, p_top.y, p_bot.z, 0.0f); // 2
		p_id++;

		triangles[7].corners[0] = XMFLOAT4( p_top.x, p_top.y, p_bot.z, p_id); // 3
		triangles[7].corners[1] = XMFLOAT4( p_bot.x, p_top.y, p_top.z, p_shininess); // 6
		triangles[7].corners[2] = p_top; // 7
		p_id++;

		//Bottom side
		triangles[8].corners[0] = p_bot;
		triangles[8].corners[0].w = p_id;
		triangles[8].corners[1] = XMFLOAT4( p_top.x, p_bot.y, p_bot.z, p_shininess); // 1
		triangles[8].corners[2] = XMFLOAT4( p_bot.x, p_bot.y, p_top.z, 0.0f); // 4
		p_id++;

		triangles[9].corners[0] = XMFLOAT4( p_top.x, p_bot.y, p_bot.z, p_id); // 1
		triangles[9].corners[1] = XMFLOAT4( p_top.x, p_bot.y, p_top.z, p_shininess); // 5
		triangles[9].corners[2] = XMFLOAT4( p_bot.x, p_bot.y, p_top.z, 0.0f); // 4
	}

	Box(){}

	//	// Cube Composition
	//	/*		  +----+  6    7	| Refered to as
	//			 /    /|			| indices of the
	//			+----+ |  4    5	| vertices below
	//	2	 3	|    | +			
	//			|    |/				
	//	0	 1	+----+								*/
	//

	//	// Cube Composition
	//	/*		  +----+  4    8	| Refered to as
	//			 /    /|			| indices of the
	//			+----+ |  2    6	| vertices below
	//	3	 7	|    | +			
	//			|    |/				
	//	1	 5	+----+								*/
	//
};

struct PointLight
{
	XMFLOAT4 position; // w = range
	XMFLOAT4 attenuation; // w = pad
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;

	PointLight(XMFLOAT3 p_pos, float p_range, XMFLOAT4 p_att, XMFLOAT4 p_amb, XMFLOAT4 p_diff)
	{
		position = XMFLOAT4(p_pos.x, p_pos.y, p_pos.z, p_range);
		//range = p_range;
		attenuation = p_att;
		ambient = p_amb;
		diffuse = p_diff;
	}

	PointLight(){}
};

struct SpotLight
{
	XMFLOAT4 position; // w = range
	XMFLOAT4 attenuation; // w = pad
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 direction; // w = cutoff

	SpotLight(XMFLOAT3 p_direction, float p_cutoff, XMFLOAT3 p_pos, float p_range, XMFLOAT4 p_att, XMFLOAT4 p_amb, XMFLOAT4 p_diff)
	{
		position = XMFLOAT4(p_pos.x, p_pos.y, p_pos.z, p_range);
		//range = p_range;
		attenuation = p_att;
		ambient = p_amb;
		diffuse = p_diff;
		direction = XMFLOAT4(p_direction.x, p_direction.y, p_direction.z, p_cutoff);
	}

	SpotLight(){}
};

struct Surface
{
	XMFLOAT3 origin;
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	float shininess;
	float power;
	float id;
};

