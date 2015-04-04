/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @PooyaEimandar on twitter
	Name			 : W_Math.h
	Description		 : The math helper of Wolf
	Comment          :
*/

#pragma once

#include <d2d1_1helper.h>

namespace DirectX
{
	inline bool operator == (const D2D1_COLOR_F& lValue, const D2D1_COLOR_F& rValue)
	{
		return lValue.a == rValue.a && lValue.r == rValue.r && lValue.g == rValue.g && lValue.b == rValue.b;
	}

	inline bool operator != (const D2D1_COLOR_F& lValue, const D2D1_COLOR_F& rValue)
	{
		return !(lValue == rValue);
	}

}

