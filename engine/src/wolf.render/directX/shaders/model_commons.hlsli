struct static_model_vs_in
{
	float3	pos				: POSITION0;
	float3	normal			: NORMAL0;
	float2	uv				: TEXCOORD0;
	float3	tangent			: TANGENT0;
	float3	binormal		: BINORMAL0;
};

struct skinned_model_vs_in
{
	float3	pos				: POSITION0;
	float3	normal			: NORMAL0;
	float2	uv				: TEXCOORD0;
	float3	tangent			: TANGENT0;
	float3	binormal		: BINORMAL0;
	float4	blend_weight	: BLENDWEIGHT0;
	int4	blend_indices	: BLENDINDICES0;
};

struct model_vs_out
{
	float4 pos				: SV_POSITION0;
	float2 uv				: TEXCOORD0;
	float3 normal			: NORMAL0;
};

cbuffer model_cbuffer		: register(b0)
{
	//scalar Types for world view projection matrix
	min16float4x4	wvp;
};