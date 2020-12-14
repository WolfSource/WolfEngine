#include "model_commons.hlsli"

model_vs_out main(static_model_vs_in pIn )
{
	model_vs_out _out = (model_vs_out) 0;
	
	_out.pos = mul(float4(pIn.pos, 1.0), wvp);
	//_out.uv = pIn.uv;

	return _out;
}