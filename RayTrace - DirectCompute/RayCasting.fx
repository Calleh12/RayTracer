//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
// Copyright (c) Stefan Petersson, 2012
//--------------------------------------------------------------------------------------

#pragma pack_matrix(row_major)
#define THREADS 16
#define WSCREEN 800
#define HSCREEN 1024


struct Ray
{
	float3 direction;
	float3 origin;
	float power;
	float id;
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

cbuffer cb
{
	float4		eyePos; // 4
	float4x4	view; //16
	float4x4	proj; // 16
};

RWStructuredBuffer<Ray> output : register(u0);

[numthreads(THREADS, THREADS, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	// ## Cast primary rays ##
	float width = WSCREEN;
	float height = WSCREEN;

	int id = threadID.y * height + threadID.x;
	//output[threadID.xy] = float4(float3(1,0,1) * (1 - length(threadID.xy - float2(400, 400)) / 400.0f), 1);
	float4 rayPos;

	// Take screen coordinates first to post projective space.
	rayPos = float4((((float)(threadID.x / width) - 0.5f) * 2.0f), (((1.0f - (float)(threadID.y / height)) - 0.5f) * 2.0f), 1.0f, 1.0f);

	// Move it to camera-space.
	rayPos = mul(rayPos, proj);
	float iw = 1.0f / rayPos.w;
	rayPos *= iw;

	// Move it to world-space.
	rayPos = mul(rayPos, view);
	
	Ray rayCharles;
	rayPos -= eyePos;
	rayCharles = createRay( rayPos.xyz, eyePos.xyz );

	rayCharles.direction = float3(normalize( rayCharles.direction.xyz ));
	// ## End primary rays ##

	output[id] = rayCharles;
}