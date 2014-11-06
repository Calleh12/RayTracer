//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
// Copyright (c) Stefan Petersson, 2012
//--------------------------------------------------------------------------------------

#pragma pack_matrix(row_major)
#define THREADS 16
#define DSCREEN 800

struct Ray
{
	float3 direction;
	float3 origin;
	float power;
	float id;
};
struct Sphere
{
	float4	center; // w = shininess
	float4	color;
	float4	radius; // z = id, w =  flagga AccBuff
};
struct Triangle
{
	float4 corners[3]; // corners[0].w = id, corners[1].w = shininess
};
struct TriangleMesh
{
	Triangle trig;
	float2 uv[3];
	float3 normal[3];
	float3 tangent[3];
	//float3 binormal;
	//float  pad;
};
struct HitData
{
	float	t;
	float4	color;
	float4	normal;
	int		objectType;
	// 0 = sphere, 1 = triangle
};
struct Surface
{
	float3 origin;
	float3 position;
	float3 normal;
	float4 color;
	float shininess;
	float power;
	float id;
};

Surface createSurface()
{
	Surface sur;
	sur.origin		= float3(0.0f, 0.0f, 0.0f);
	sur.position	= float3(0.0f, 0.0f, 0.0f);
	sur.normal		= float3(100.0f, 0.0f, 0.0f);
	sur.color		= float4(0.0f, 0.0f, 0.0f, 1.0f);
	sur.shininess	= 1.0f; 
	sur.power		= 0.0f;
	sur.id			= -1.0f;
	return sur; // >:(
}
HitData	createHitData(int p_objectType)
{
	HitData hitData;
	hitData.t		= -1.0f;
	hitData.color	= float4(0.0f, 0.0f, 0.0f, 1.0f);
	hitData.normal	= float4(0.0f, 0.0f, 0.0f, 0.0f);
	hitData.objectType = p_objectType;
	return hitData;
}

// ## INTERSECTION TESTS ##
HitData intersectSphere(Sphere sphere, Ray ray, HitData hitData)
{
	float3	l; //the length between ray origin and sphere center.
	float	s; //the projection of the length onto the ray.
	float	l2; //the sq-length.

	l = sphere.center - ray.origin;
	s = dot(l, ray.direction);
	l2 = dot(l, l);

	if (s < 0 && l2 > sphere.radius.y)
	{
		//check if the sphere is behind the camera and if the camera origin
		//in that case is not between the sphere origin and the shell.
		hitData.t = -1.0f;
		return hitData;
	}

	float m2 = l2 - s*s;

	if (m2 > sphere.radius.y)
	{
		//imaginary number in the sqrt of the equation
		hitData.t = -1.0f;
		return hitData;
	}

	float q = sqrt(sphere.radius.y - m2);

	if(l2 > sphere.radius.y)
	{
		// if the camera is not inside the sphere we want the smallest root.
		hitData.t = s - q;
	}
	else
	{
		// the camera is inside the sphere and we want the bigger root.
		hitData.t = s + q;
	}
	hitData.color = sphere.color;
	return hitData;
}
HitData intersectTriangle(Triangle tri, Ray ray, HitData hitData)
{
	//first calculate two vectors to build the triangle
	float3 e1 = tri.corners[1] - tri.corners[0];
	float3 e2 = tri.corners[2] - tri.corners[0];

	//tri.edges[0] = e1;
	//tri.edges[1] = e2;

	//calculation of the determinant in two steps
	float3 q = cross(ray.direction, e2);
	float a = dot(e1, q);

	if (a > -0.00001f && a < 0.00001f)
	{
		//avoid determinants close to zero.
		hitData.t = -1.0f;
		return hitData;
	}

	float f = 1 / a;
	float3 s = ray.origin - tri.corners[0];
	float u = f * dot(s, q);
	if (u < 0.0f)
	{
		//in barycentric coordinates, negative values indicate that the ray is outside of the object
		hitData.t = -1.0f;
		return hitData;
	}

	float3 r = cross(s, e1);
	float v = f * dot(ray.direction, r);
	if (v < 0.0f || (v + u) > 1.0f)
	{
		//same as for u. Also v + u can't be bigger than 1. That's also outside.
		hitData.t = -1.0f;
		return hitData;
	}
	else
	{
		//otherwise, return. * (u + v) gives a shadow feeling since it's a small value close to the origin and big far away.
		hitData.t = f * dot(e2, r); // Knas blir 0

		hitData.color = float4(
			0.8f,
			0.2f,
			0.0f,
			1.0f
			);
		return hitData;
	}
}

Texture2D optimusT : register(t2);
Texture2D optimusN : register(t3);
//SamplerState m_sampler : register(s0);
HitData intersectTriangleMesh(TriangleMesh tri, Ray ray, HitData hitData)
{
	//first calculate two vectors to build the triangle
	float3 e1 = tri.trig.corners[1] - tri.trig.corners[0];
	float3 e2 = tri.trig.corners[2] - tri.trig.corners[0];

	//tri.edges[0] = e1;
	//tri.edges[1] = e2;

	//calculation of the determinant in two steps
	float3 q = cross(ray.direction, e2);
	float a = dot(e1, q);

	if (a > -0.00001f && a < 0.00001f)
	{
		//avoid determinants close to zero.
		hitData.t = -1.0f;
		return hitData;
	}

	float f = 1 / a;
	float3 s = ray.origin - tri.trig.corners[0];
	float u = f * dot(s, q);
	if (u < 0.0f)
	{
		//in barycentric coordinates, negative values indicate that the ray is outside of the object
		hitData.t = -1.0f;
		return hitData;
	}

	float3 r = cross(s, e1);
	float v = f * dot(ray.direction, r);
	if (v < 0.0f || (v + u) > 1.0f)
	{
		//same as for u. Also v + u can't be bigger than 1. That's also outside.
		hitData.t = -1.0f;
		return hitData;
	}
	else
	{
		//otherwise, return. * (u + v) gives a shadow feeling since it's a small value close to the origin and big far away.
		hitData.t = f * dot(e2, r); // Knas blir 0

		//## LÄGG TILL KOD FÖR TEXTURE MAPPING HÄR! ##

		float w = 1 - u - v;
		float2 uv = tri.uv[0] * w + tri.uv[1] * u + tri.uv[2] * v;
		uv.x *= 512;
		uv.y *= 512;
		hitData.color = optimusT[uv];

		// Normal map magic
		float3 bumpMap = normalize(optimusN[uv].xyz);
		float3 normal = normalize(tri.normal[0] * w + tri.normal[1] * u + tri.normal[2] * v);
		float3 tangent = normalize(tri.tangent[0] * w + tri.tangent[1] * u + tri.tangent[2] * v);
		float3 binormal = cross(tangent, normal);
		normal = bumpMap.x * tangent + bumpMap.y * binormal + bumpMap.z * normal;
		hitData.normal	= float4(normal, 0.0f);

		//hitData.color = optimusT.Sample(m_sampler, uv);
		return hitData;
	}
}
// ## INTERSECTION TESTS END ##

struct MaterialStruct
{
	float4	m_ambient; // w = shininess
	float4	m_diffuse; // w = alpha
	float4	m_specular;// w = reflection
};

cbuffer cb
{
	Triangle	triangl;
	Sphere		sphere; 
	Triangle	triangls[10];
	MaterialStruct material;
};

StructuredBuffer<Ray> input : register(t0);
StructuredBuffer<TriangleMesh> optimusP : register(t1);
RWStructuredBuffer<Surface> output : register(u0);

[numthreads(THREADS, THREADS, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	// ## Collision ##
	Surface surface;
	surface = createSurface();
	float screen = DSCREEN;
	int id = threadID.y * screen + threadID.x;
	surface.origin = input[id].origin;
	HitData hitData = createHitData(0);

	if(input[id].id != sphere.radius.z)
		hitData = intersectSphere(sphere, input[id], hitData);

	HitData tempHitData = createHitData(1);

	int index;
	index = -1;
	for(int i = 0; i < 10; i++)
	{
		if(input[id].id != triangls[i].corners[0].w)
		{
			tempHitData = intersectTriangle(triangls[i], input[id], tempHitData);

			if(hitData.t >= 0 && tempHitData.t >= 0)
			{
				if(tempHitData.t < hitData.t)
				{
					hitData = tempHitData;
					index = i;
				}
			}
			else if(tempHitData.t >= 0)
			{
				hitData = tempHitData;
				index = i;
			}
			else if(hitData.t >= 0){}
			else 
				hitData.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	int optimusIndex;
	optimusIndex = -1;

	int optimusLoopSize = optimusP[0].trig.corners[2].w;
	for(int i = 0; i < optimusLoopSize; i++)
	{
		if(input[id].id != optimusP[i].trig.corners[0].w)
		{
			tempHitData = intersectTriangleMesh(optimusP[i], input[id], tempHitData);

			if(hitData.t >= 0 && tempHitData.t >= 0)
			{
				if(tempHitData.t < hitData.t)
				{
					hitData = tempHitData;
					optimusIndex = i;
					hitData.objectType = 2;
				}
			}
			else if(tempHitData.t >= 0)
			{
				hitData = tempHitData;
				optimusIndex = i;
				hitData.objectType = 2;
			}
			else if(hitData.t >= 0){}
			else 
				hitData.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	if (hitData.t <= -1)
	{
		output[id]		= surface;
		return;
	}

	surface.position = mul(input[id].direction, hitData.t);
	surface.position += input[id].origin;

	if(hitData.objectType == 0) // sphere
	{
		surface.normal = surface.position - sphere.center;
		surface.id = sphere.radius.z;
		surface.shininess = sphere.center.w;
	}
	else if ( hitData.objectType == 1) // Triangle
	{
		surface.normal = float4(cross(triangls[index].corners[2] - triangls[index].corners[0], triangls[index].corners[1] - triangls[index].corners[0]), 0.0f);
		surface.id = triangls[index].corners[0].w;
		surface.shininess = triangls[index].corners[1].w;
	}
	else
	{
		//surface.normal = float4(cross(optimusP[optimusIndex].corners[2] - optimusP[optimusIndex].corners[0], optimusP[optimusIndex].corners[1] - optimusP[optimusIndex].corners[0]), 0.0f);
		//surface.normal	= optimusP[optimusIndex].normal;
		surface.normal	= hitData.normal;
		surface.id = optimusP[optimusIndex].trig.corners[0].w;
		surface.shininess = material.m_ambient.w;
		//surface.color = optimusT.Sample(m_sampler, optimusP[optimusIndex].uv[0]);
	}

	surface.power = input[id].power;

	surface.normal = normalize(surface.normal);
	surface.color = hitData.color;

	output[id] = surface;
	// ## End collision ##
}