struct VS_IN
{
	float3 pos		: POSITION0;
	float4 color	: COLOR0;
	float2 uv		: TEXCOORD0;
};

struct VS_OUT
{
	float4 pos		: SV_POSITION0;
	float4 color	: COLOR0;
	float2 uv		: TEXCOORD0;
};

Texture2D sTexture;
SamplerState sSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};