/*
	Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_directX_math.h
	Description		 : The math helper of Wolf
	Comment          :
*/

#ifndef __W_DIRECTX_MATH_H__
#define __W_DIRECTX_MATH_H__

#include <DirectXMath.h>
#include <d2d1_1helper.h>
#include <glm/matrix.hpp>
#include <w_math.h>
#include <w_color.h>

inline bool operator == (_In_ const D2D1_COLOR_F& lValue, _In_ const D2D1_COLOR_F& rValue)
{
	return lValue.a == rValue.a && lValue.r == rValue.r && lValue.g == rValue.g && lValue.b == rValue.b;
}

inline bool operator != (_In_ const D2D1_COLOR_F& lValue, _In_ const D2D1_COLOR_F& rValue)
{
	return !(lValue == rValue);
}

namespace DirectX
{
	inline XMFLOAT4 DWORD_COLOR_TO_XMFLOAT4(_In_ DWORD pC)
	{
		//Convert ARGB(DWORD) to RGBA(in the range of 0..1)
		return DirectX::XMFLOAT4(
			((pC >> 16) & 0xFF) / 255.0f,//R
			((pC >> 8) & 0xFF) / 255.0f,//G
			(pC & 0xFF) / 255.0f,//B
			((pC >> 24) & 0xFF) / 255.0f);//A
	}

	inline XMFLOAT4 W_COLOR_TO_XMFLOAT4(_In_ w_color pC)
	{
		return DirectX::XMFLOAT4(pC.r / 255.0f, pC.g / 255.0f, pC.b / 255.0f, pC.a / 255.0f);
	}

	inline w_color XMFLOAT4_TO_W_COLOR(_In_ XMFLOAT4 pC)
	{
		return w_color(static_cast<UINT>(pC.x * 255.0f), static_cast<UINT>(pC.y * 255.0f), static_cast<UINT>(pC.z * 255.0f), static_cast<UINT>(pC.w * 255.0f));
	}

	inline XMMATRIX XMMatrixFromGLMMatrix(_In_ glm::tmat4x4<float, _In_ glm::precision::highp> const& pValue)
	{
		return XMMATRIX(
			pValue[0][0], pValue[0][1], pValue[0][2], pValue[0][3],
			pValue[1][0], pValue[1][1], pValue[1][2], pValue[1][3],
			pValue[2][0], pValue[2][1], pValue[2][2], pValue[2][3],
			pValue[3][0], pValue[3][1], pValue[3][2], pValue[3][3]);
	}
}

#endif

