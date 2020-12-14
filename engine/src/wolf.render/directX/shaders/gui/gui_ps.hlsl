#include "parameters.hlsli"

float4 main(VS_OUT pIN) : SV_Target
{
	return sTexture.Sample(sSampler, pIN.uv) * pIN.color;
}