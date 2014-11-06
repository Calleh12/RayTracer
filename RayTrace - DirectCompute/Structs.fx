struct Ray
{
	float3 direction;
	float3 origin;
};
struct Sphere
{
	float4	center;
	float4	color;
	float4	radius; // z, w =  pad
	//float	radius;
	//float	radius2;
};
struct Triangle
{
	float4 corners[3];
	//float det;
};
struct PointLight
{
	float4 position; // w = range
	//float range;
	float4 attenuation; // w = pad
	float4 ambient;
	float4 diffuse;
};
struct HitData
{
	float	t;
	float4	color;
	int		objectType;
	// 0 = sphere, 1 = triangle
};