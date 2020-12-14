#include "parameters.hlsli"

PS_In main( VS_In pIn )
{
	PS_In _out = (PS_In)0;

	_out.pos = mul(float4(pIn.pos, 1.0), wvp);
	_out.uv = float2(pIn.uv.x * uv_scale.x, pIn.uv.y * uv_scale.y);

	return _out;
}