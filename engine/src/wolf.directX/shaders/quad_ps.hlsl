#include "parameters.hlsli"

Texture2D Texture0 : register(t0);
Texture2D Texture1 : register(t1);

SamplerState Sampler : register(s0);

float4 main(PS_In pIn) : SV_TARGET
{
	float4 _color_0 = Texture0.Sample(Sampler, pIn.uv);
	float4 _color_1 = Texture1.Sample(Sampler, pIn.uv);

	float4 result = (_color_0 + _color_1);
	return float4(result.rgb, result.a * opacity);
}