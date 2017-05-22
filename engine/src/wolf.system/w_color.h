/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_color.h
	Description		 : Global color structure
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_COLOR_H__
#define __W_COLOR_H__

#include "w_system_export.h"
#include <cstdlib>
#include <string>
#include <sstream>
#include <array>

#ifdef __GNUC__
#pragma GCC visibility push(default) //The classes/structs below are exported
#endif

struct w_color
{
	unsigned char r, g, b, a;

	w_color() :
		r(0),
		g(0),
		b(0),
		a(0)
	{
	}

	//r(0-255),g(0-255),b(0-255),a(0-255)
	w_color(unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA) :
		r(pR),
		g(pG),
		b(pB),
		a(pA)
	{
	}

	//Create w_color from string in the format of "r(0-255),g(0-255),b(0-255),a(0-255)", for example "255,0,0,255"
	static w_color from_string(const char* pValue)
	{
		std::stringstream _sstrream(pValue);

		unsigned char _r = 0, _g = 0, _b = 0, _a = 0;
		int i = 0;
		char _c;
		std::string _number;
		while (_sstrream >> _c)
		{
			_number += _c;
			if (_sstrream.peek() == ',')
			{
				_sstrream.ignore();

				if (i == 0)
				{
					_r = std::atoi(_number.c_str());
				}
				else if (i == 1)
				{
					_g = std::atoi(_number.c_str());
				}
				else if (i == 2)
				{
					_b = std::atoi(_number.c_str());
				}

				i++;
				_number = "";
			}
		}

		if (!_number.empty())
		{
			_a = std::atoi(_number.c_str());
		}

		_sstrream.clear();

		return w_color(_r, _g, _b, _a);
	}

	//convert hex color to w_color
	static w_color from_hex(unsigned long pValue)
	{
		//Convert ARGB(unsigned long ) to RGBA(in the range of 0..255)
		return w_color(
			((pValue >> 16) & 0xFF),//R
			((pValue >> 8) & 0xFF),//G
			(pValue & 0xFF),//B
			((pValue >> 24) & 0xFF));//A
	}

	//convert hex color to w_color
	static unsigned int to_hex(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		return ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
	}

#if defined(__DX12__) || defined(__DX11__)
    static w_color to_color(_In_ const XMFLOAT4& pValue)
    {
        return w_color(
            static_cast<UINT>(pValue.x * 255.0f),
            static_cast<UINT>(pValue.y * 255.0f),
            static_cast<UINT>(pValue.z * 255.0f),
            static_cast<UINT>(pValue.w * 255.0f));
    }
#else
    static w_color to_color(_In_ const std::array<float, 4>& pValue)
    {
        return w_color(
            static_cast<UINT>(pValue[0] * 255.0f),
            static_cast<UINT>(pValue[1] * 255.0f),
            static_cast<UINT>(pValue[2] * 255.0f),
            static_cast<UINT>(pValue[3] * 255.0f));
    }
#endif //defined(__DX12__) || defined(__DX11__)

	enum COLORS_HEX
	{
		ALICE_BLUE = 0xFFF0F8FF,
		ANTIQUE_WHITE = 0xFFFAEBD7,
		AQUA = 0xFF00FFFF,
		AQUAMARINE = 0xFF7FFFD4,
		AZURE = 0xFFF0FFFF,
		BEIGE = 0xFFF5F5DC,
		BISQUE = 0xFFFFE4C4,
		BLACK = 0xFF000000,
		BLANCHED_ALMOND = 0xFFFFEBCD,
		BLUE = 0xFF0000FF,
		BLUE_VIOLET = 0xFF8A2BE2,
		BROWN = 0xFFA52A2A,
		BURLY_WOOD = 0xFFDEB887,
		CADET_BLUE = 0xFF5F9EA0,
		CHARTREUSE = 0xFF7FFF00,
		CHOCOLATE = 0xFFD2691E,
		CORAL = 0xFFFF7F50,
		CORNFLOWER_BLUE = 0xFF6495ED,
		CORNSILK = 0xFFFFF8DC,
		CRIMSON = 0xFFDC143C,
		CYAN = 0xFF00FFFF,
		DARK_BLUE = 0xFF00008B,
		DARK_CYAN = 0xFF008B8B,
		DARK_GOLDENROD = 0xFFB8860B,
		DARK_GRAY = 0xFFA9A9A9,
		DARK_GREEN = 0xFF006400,
		DARK_KHAKI = 0xFFBDB76B,
		DARK_MAGENTA = 0xFF8B008B,
		DARK_OLIVE_GREEN = 0xFF556B2F,
		DARK_ORANGE = 0xFFFF8C00,
		DARK_ORCHID = 0xFF9932CC,
		DARK_RED = 0xFF8B0000,
		DARK_SALMON = 0xFFE9967A,
		DARK_SEA_GREEN = 0xFF8FBC8F,
		DARK_SLATE_BLUE = 0xFF483D8B,
		DARK_SLATE_GRAY = 0xFF2F4F4F,
		DARK_TURQUOISE = 0xFF00CED1,
		DARK_VIOLET = 0xFF9400D3,
		DEEP_PINK = 0xFFFF1493,
		DEEP_SKY_BLUE = 0xFF00BFFF,
		DIM_GRAY = 0xFF696969,
		DODGER_BLUE = 0xFF1E90FF,
		FIREBRICK = 0xFFB22222,
		FLORAL_WHITE = 0xFFFFFAF0,
		FOREST_GREEN = 0xFF228B22,
		FUCHSIA = 0xFFFF00FF,
		GAINSBORO = 0xFFDCDCDC,
		GHOST_WHITE = 0xFFF8F8FF,
		GOLD = 0xFFFFD700,
		GOLDENROD = 0xFFDAA520,
		GRAY = 0xFF808080,
		GREEN = 0xFF008000,
		GREEN_YELLOW = 0xFFADFF2F,
		HONEYDEW = 0xFFF0FFF0,
		HOT_PINK = 0xFFFF69B4,
		INDIAN_RED = 0xFFCD5C5C,
		INDIGO = 0xFF4B0082,
		IVORY = 0xFFFFFFF0,
		KHAKI = 0xFFF0E68C,
		LAVENDER = 0xFFE6E6FA,
		LAVENDER_BLUSH = 0xFFFFF0F5,
		LAWN_GREEN = 0xFF7CFC00,
		LEMON_CHIFFON = 0xFFFFFACD,
		LIGHT_BLUE = 0xFFADD8E6,
		LIGHT_CORAL = 0xFFF08080,
		LIGHT_CYAN = 0xFFE0FFFF,
		LIGHT_GOLDENROD_YELLOW = 0xFFFAFAD2,
		LIGHT_GREEN = 0xFF90EE90,
		LIGHT_GRAY = 0xFFD3D3D3,
		LIGHT_PINK = 0xFFFFB6C1,
		LIGHT_SALMON = 0xFFFFA07A,
		LIGHT_SEA_GREEN = 0xFF20B2AA,
		LIGHT_SKY_BLUE = 0xFF87CEFA,
		LIGHT_SLATE_GRAY = 0xFF778899,
		LIGHT_STEEL_BLUE = 0xFFB0C4DE,
		LIGHT_YELLOW = 0xFFFFFFE0,
		LIME = 0xFF00FF00,
		LIME_GREEN = 0xFF32CD32,
		LINEN = 0xFFFAF0E6,
		MAGENTA = 0xFFFF00FF,
		MAROON = 0xFF800000,
		MEDIUM_AQUAMARINE = 0xFF66CDAA,
		MEDIUM_BLUE = 0xFF0000CD,
		MEDIUM_ORCHID = 0xFFBA55D3,
		MEDIUM_PURPLE = 0xFF9370DB,
		MEDIUM_SEA_GREEN = 0xFF3CB371,
		MEDIUM_SLATE_BLUE = 0xFF7B68EE,
		MEDIUM_SPRING_GREEN = 0xFF00FA9A,
		MEDIUM_TURQUOISE = 0xFF48D1CC,
		MEDIUM_VIOLET_RED = 0xFFC71585,
		MIDNIGHT_BLUE = 0xFF191970,
		MINT_CREAM = 0xFFF5FFFA,
		MISTY_ROSE = 0xFFFFE4E1,
		MOCCASIN = 0xFFFFE4B5,
		NAVAJO_WHITE = 0xFFFFDEAD,
		NAVY = 0xFF000080,
		OLD_LACE = 0xFFFDF5E6,
		OLIVE = 0xFF808000,
		OLIVE_DRAB = 0xFF6B8E23,
		ORANGE = 0xFFFFA500,
		ORANGE_RED = 0xFFFF4500,
		ORCHID = 0xFFDA70D6,
		PALE_GOLDENROD = 0xFFEEE8AA,
		PALE_GREEN = 0xFF98FB98,
		PALE_TURQUOISE = 0xFFAFEEEE,
		PALE_VIOLET_RED = 0xFFDB7093,
		PAPAYA_WHIP = 0xFFFFEFD5,
		PEACH_PUFF = 0xFFFFDAB9,
		PERU = 0xFFCD853F,
		PINK = 0xFFFFC0CB,
		PLUM = 0xFFDDA0DD,
		POWDER_BLUE = 0xFFB0E0E6,
		PURPLE = 0xFF800080,
		RED = 0xFFFF0000,
		ROSY_BROWN = 0xFFBC8F8F,
		ROYAL_BLUE = 0xFF4169E1,
		SADDLE_BROWN = 0xFF8B4513,
		SALMON = 0xFFFA8072,
		SANDY_BROWN = 0xFFF4A460,
		SEA_GREEN = 0xFF2E8B57,
		SEA_SHELL = 0xFFFFF5EE,
		SIENNA = 0xFFA0522D,
		SILVER = 0xFFC0C0C0,
		SKY_BLUE = 0xFF87CEEB,
		SLATE_BLUE = 0xFF6A5ACD,
		SLATE_GRAY = 0xFF708090,
		SNOW = 0xFFFFFAFA,
		SPRING_GREEN = 0xFF00FF7F,
		STEEL_BLUE = 0xFF4682B4,
		TAN = 0xFFD2B48C,
		TEAL = 0xFF008080,
		THISTLE = 0xFFD8BFD8,
		TOMATO = 0xFFFF6347,
		TRANSPARENT_ = 0x00000000,
		TURQUOISE = 0xFF40E0D0,
		VIOLET = 0xFFEE82EE,
		WHEAT = 0xFFF5DEB3,
		WHITE = 0xFFFFFFFF,
		WHITE_SMOKE = 0xFFF5F5F5,
		YELLOW = 0xFFFFFF00,
		YELLOW_GREEN = 0xFF9ACD32,
	};
};

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

inline bool operator == (const w_color& lValue, const w_color& rValue)
{
    return lValue.a == rValue.a && lValue.r == rValue.r && lValue.g == rValue.g && lValue.b == rValue.b;
}

inline bool operator != (const w_color& lValue, const w_color& rValue)
{
    return !(lValue == rValue);
}

#endif // __W_COLOR_H__
