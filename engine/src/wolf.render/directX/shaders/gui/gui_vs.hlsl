#include "parameters.hlsli"

VS_OUT main( VS_IN pIn)
{
	VS_OUT _out;

    _out.pos = float4(pIn.pos, 1.0f );
    _out.color = pIn.color;
    _out.uv = pIn.uv;

    return _out;
}

