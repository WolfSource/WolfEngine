cbuffer WVP : register(b0)
{
	//Scalar Types for world view projection matrix
	min16float4x4	wvp;
};

cbuffer QUAD_TEXTURE_PARAMS : register(b1)
{
	float2			uv_scale;
	float			opacity;
	float			padding;
};

cbuffer QUAD_COLOR_PARAMS : register(b1)
{
	float4			color;
};

struct VS_In
{
	float3 pos		: POSITION0;
	float3 normal	: NORMAL0;
	float2 uv		: TEXCOORD0;
};

struct PS_In
{
	float4 pos		: SV_POSITION0;
	float2 uv		: TEXCOORD0;
};
