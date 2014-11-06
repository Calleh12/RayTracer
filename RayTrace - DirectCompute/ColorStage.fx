//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
// Copyright (c) Stefan Petersson, 2012
//--------------------------------------------------------------------------------------

#pragma pack_matrix(row_major)
#define THREADS 16

#define NoL 15
#define DSCREEN 800

struct Ray
{
	float3 direction;
	float3 origin;
	float power;
	float id;
};
struct PointLight
{
	float4 position; // w = range
	float4 attenuation; // w = pad
	float4 ambient;
	float4 diffuse;
};
struct SpotLight
{
	float4 position; // w = range
	float4 attenuation; // w = pad
	float4 ambient;
	float4 diffuse;
	float4 direction; // w = cutoff
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
struct Sphere
{
	float4	center; // w = shininess
	float4	color;
	float4	radius; // z = id, w = ful haxx reset accBuff
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
};
struct HitData
{
	float	t;
	float4	color;
	int		objectType;
	// 0 = sphere, 1 = triangle
};

Ray createRay(float3 p_direction, float3 p_origin)
{
	Ray ray;
	ray.direction	= p_direction;
	ray.origin		= p_origin;
	ray.power		= 1.0f;
	ray.id			= -1.0f;
	return ray;
}

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
		hitData.t = f * dot(e2, r);
		hitData.color = float4(
			1.0f * (u + v),
			0.5f * (u + v),
			0.1f * (u + v),
			1.0f
			);
		return hitData;
	}
}
// ## INTERSECTION TESTS END ##

cbuffer cb
{

	SpotLight	lights[15];
	Triangle	triangl;
	Sphere		sphere; 
	Triangle	triangls[10];
};

StructuredBuffer<Surface> input : register(t0);
StructuredBuffer<TriangleMesh> optimusP : register(t1);
RWTexture2D<float4> output : register(u0);
RWStructuredBuffer<Ray> bounceRay : register(u1);
RWStructuredBuffer<float4> colorAccu : register(u2);

[numthreads(THREADS, THREADS, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	float screen = DSCREEN;
	Surface surface;
	int id = threadID.y * screen + threadID.x;
	surface = input[id];

	if(sphere.radius.w > 2.0f)
	{
		colorAccu[id] = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	Ray bRay;
	bRay.direction = reflect(normalize(surface.position.xyz - surface.origin.xyz), surface.normal);
	bRay.origin = surface.position;
	bRay.power = surface.power * surface.shininess;
	bRay.id = surface.id;

	// The input surface is not really a surface.
	if (surface.normal.x > 2.0f)
	{
		colorAccu[id] += surface.color * 0.0f;
		output[threadID.xy] = colorAccu[id];

		bRay.power = 0.0f;
		bounceRay[id] = bRay;
		//output[threadID.xy] = bRay.direction;
		//output[threadID.xy] = float4(normalize(surface.origin.xyz - surface.position.xyz ), 1.0f);
		return;
	}
	
	bounceRay[id] = bRay;

	float3 finalColor;
	finalColor = float3(0.0f, 0.0f, 0.0f);

	float4 finalAmbient;
	finalAmbient = surface.color * 0.1f;
	for (int lightIndex = 0; lightIndex < NoL; lightIndex++)
	{
		//Create the vector between light position and surface position
		float3 surfaceToLight;
		surfaceToLight = lights[lightIndex].position.xyz - surface.position;

		//Find the distance between the light pos and pixel pos
		float dist;
		dist = length(surfaceToLight);

		////Create the ambient light
		//float4 finalAmbient;
		//finalAmbient = surface.color * lights[0].ambient;

		//If pixel is too far, return pixel color with ambient light
		if(dist > lights[lightIndex].position.w)
		{
			continue;
		}

		//Turn lightToPixelVec into a unit length vector describing
		//the pixels direction from the lights position
		surfaceToLight /= dist;

		//## Find out if the surface is visible from the light source. ##
		Ray ray;
		// Creating ray from light towards surface.
		ray = createRay( surfaceToLight, surface.position );

		HitData sphHitData;
		sphHitData = createHitData(0);

		if(surface.id != sphere.radius.z)
			sphHitData = intersectSphere(sphere, ray, sphHitData);
	
		HitData triHitData;
		triHitData = createHitData(1);

		for (int i = 0; i < 15; i++)
		{
			if(surface.id != triangls[i].corners[0].w)
				triHitData = intersectTriangle(triangls[i], ray, triHitData);

			if(triHitData.t >= 0.0f && sphHitData.t >= 0.0f)
			{
				if(triHitData.t < sphHitData.t)
				{
					sphHitData.t = triHitData.t;
				}
			}
			else if(triHitData.t >= 0)
			{
				sphHitData = triHitData;
			}
		}

		int optimusLoopSize = optimusP[0].trig.corners[2].w;
		for(int i = 0; i < optimusLoopSize; i++)
		{
			if(input[id].id != optimusP[i].trig.corners[0].w)
				triHitData = intersectTriangle(optimusP[i].trig, ray, triHitData);

			if(triHitData.t >= 0.0f && sphHitData.t >= 0.0f)
			{
				if(triHitData.t < sphHitData.t)
				{
					sphHitData.t = triHitData.t;
				}
			}
			else if(triHitData.t >= 0)
			{
				sphHitData = triHitData;
			}
		}

		if(sphHitData.t < dist && sphHitData.t > 0)
		{
			finalColor += surface.color * 0.0f;
			continue;
		}

		//Phong shasyyasyasys shading 
		//## Surface is visible from light ##

		//Calculate how much light the pixel gets by the angle
		//in which the light strikes the pixels surface

		// ## MOVE LATER ##
		//surfaceToLight = lights[lightIndex].position - surface.position;
		//dist = length(surfaceToLight);
		//surfaceToLight /= dist;
		//
		////If pixel is too far, return pixel color with ambient light
		//if(dist > lights[lightIndex].position.w)
		//{
		//	break;
		//}

		float3 tempColor = float3(0.f, 0.f, 0.f);
		//Create the ambient light
		
		float SpotFactor = dot(normalize(-surfaceToLight), normalize(lights[lightIndex].direction.xyz));

		if(lights[lightIndex].direction.w != 0.f)
		{
			if (SpotFactor > lights[lightIndex].direction.w) 
			{
				float howLighty;
				howLighty = dot(surfaceToLight, surface.normal);

				float3 eyeToSurface;
				eyeToSurface = normalize(surface.position - surface.origin.xyz);
				float3 reflectionVector;
				reflectionVector = reflect(surfaceToLight, surface.normal);

				//If light is striking the front side of the pixel

				if( howLighty > 0.0f)
				{
					// Specular
					tempColor += (0.5f * pow( saturate(dot(reflectionVector, eyeToSurface)), 128.0f ));
				}
		
				// Diffuse
				tempColor += ((howLighty * surface.color.xyz * lights[lightIndex].diffuse.xyz));
		
				//Calculate Light's Falloff factor
				// Attenuation
				tempColor /= lights[lightIndex].attenuation[0] + (lights[lightIndex].attenuation[1] * dist) + 
							(lights[lightIndex].attenuation[2] * (dist*dist));


				tempColor *= (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - lights[lightIndex].direction.w));
			}
			else 
			{
				//return vec4(0,0,0,0);
			}
		}
		else
		{
			float howLighty;
			howLighty = dot(surfaceToLight, surface.normal);

			float3 eyeToSurface;
			eyeToSurface = normalize(surface.position - surface.origin.xyz);
			float3 reflectionVector;
			reflectionVector = reflect(surfaceToLight, surface.normal);

			//If light is striking the front side of the pixel

			if( howLighty > 0.0f)
			{
				// Specular
				tempColor += (0.5f * pow( saturate(dot(reflectionVector, eyeToSurface)), 128.0f ));
			}
		
			// Diffuse
			tempColor += ((howLighty * surface.color.xyz * lights[lightIndex].diffuse.xyz));
		
			//Calculate Light's Falloff factor
			// Attenuation
			tempColor /= lights[lightIndex].attenuation[0] + (lights[lightIndex].attenuation[1] * dist) + 
						(lights[lightIndex].attenuation[2] * (dist*dist));
		}

		//output[threadID.xy] = float4(finalColor, 1.0f);
		//return;

		//make sure the values are between 1 and 0, and add the ambient
		finalColor += tempColor;
	}

	//finalColor = saturate(finalColor + finalAmbient.xyz);

	//output[threadID.xy] = float4(bRay.direction, 1.0f);//float4(finalColor.xyz, surface.color.a);

	colorAccu[id] += ( float4(finalColor.xyz, surface.color.a) * surface.power );

	output[threadID.xy] = colorAccu[id];
	//output[threadID.xy] = float4(bRay.direction, 1.0f);
	//output[threadID.xy] = float4(finalColor.xyz, surface.color.a);
	//output[threadID.xy] = float4(normalize(surface.position.xyz), 1.0f);
}