#include "parameters.hlsli"

Texture2D Texture0 : register(t0);

SamplerState Sampler : register(s0);

float4 main(PS_In pIn) : SV_TARGET
{
	float4 _color = Texture0.Sample(Sampler, pIn.uv);
	return float4(_color.rgb, _color.a * opacity);
}