#include "parameters.hlsli"

float4 main(VS_OUT pIN) : SV_Target
{
	return pIN.color;
}