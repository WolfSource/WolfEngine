#include "parameters.hlsli"

Texture2D texture_0 : register(t0);
Texture2D texture_1 : register(t1);

SamplerState sampler_0 : register(s0);

float4 main(PS_In pIn) : SV_TARGET
{
	float4 _color_0 = texture_0.Sample(sampler_0, pIn.uv);
	float4 _color_1 = texture_1.Sample(sampler_0, pIn.uv);

	return lerp(_color_0, _color_1, smoothstep(fade_value, fade_value + fade_smooth, pIn.uv.y));
}