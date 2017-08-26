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
#include <w_std.h>
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

    //rgba(0-255)
    w_color(unsigned char pRGBA) :
        r(pRGBA),
        g(pRGBA),
        b(pRGBA),
        a(pRGBA)
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

    static w_color to_color(_In_ const std::array<float, 4>& pValue)
    {
        return w_color(
            static_cast<UINT>(pValue[0] * 255.0f),
            static_cast<UINT>(pValue[1] * 255.0f),
            static_cast<UINT>(pValue[2] * 255.0f),
            static_cast<UINT>(pValue[3] * 255.0f));
    }

    static w_color ALICE_BLUE()             { return w_color::from_hex(0xFFF0F8FF); }
    static w_color ANTIQUE_WHITE()          { return w_color::from_hex(0xFFFAEBD7); }
    static w_color AQUA()                   { return w_color::from_hex(0xFF00FFFF); }
    static w_color AQUAMARINE()             { return w_color::from_hex(0xFF7FFFD4); }
    static w_color AZURE()                  { return w_color::from_hex(0xFFF0FFFF); }
    static w_color BEIGE()                  { return w_color::from_hex(0xFFF5F5DC); }
    static w_color BISQUE()                 { return w_color::from_hex(0xFFFFE4C4); }
    static w_color BLACK()                  { return w_color::from_hex(0xFF000000); }
    static w_color BLANCHED_ALMOND()        { return w_color::from_hex(0xFFFFEBCD); }
    static w_color BLUE()                   { return w_color::from_hex(0xFF0000FF); }
    static w_color BLUE_VIOLET()            { return w_color::from_hex(0xFF8A2BE2); }
    static w_color BROWN()                  { return w_color::from_hex(0xFFA52A2A); }
    static w_color BURLY_WOOD()             { return w_color::from_hex(0xFFDEB887); }
    static w_color CADET_BLUE()             { return w_color::from_hex(0xFF5F9EA0); }
    static w_color CHARTREUSE()             { return w_color::from_hex(0xFF7FFF00); }
    static w_color CHOCOLATE()              { return w_color::from_hex(0xFFD2691E); }
    static w_color CORAL()                  { return w_color::from_hex(0xFFFF7F50); }
    static w_color CORNFLOWER_BLUE()        { return w_color::from_hex(0xFF6495ED); }
    static w_color CORNSILK()               { return w_color::from_hex(0xFFFFF8DC); }
    static w_color CRIMSON()                { return w_color::from_hex(0xFFDC143C); }
    static w_color CYAN()                   { return w_color::from_hex(0xFF00FFFF); }
    static w_color DARK_BLUE()              { return w_color::from_hex(0xFF00008B); }
    static w_color DARK_CYAN()              { return w_color::from_hex(0xFF008B8B); }
    static w_color DARK_GOLDENROD()         { return w_color::from_hex(0xFFB8860B); }
    static w_color DARK_GRAY()              { return w_color::from_hex(0xFFA9A9A9); }
    static w_color DARK_GREEN()             { return w_color::from_hex(0xFF006400); }
    static w_color DARK_KHAKI()             { return w_color::from_hex(0xFFBDB76B); }
    static w_color DARK_MAGENTA()           { return w_color::from_hex(0xFF8B008B); }
    static w_color DARK_OLIVE_GREEN()       { return w_color::from_hex(0xFF556B2F); }
    static w_color DARK_ORANGE()            { return w_color::from_hex(0xFFFF8C00); }
    static w_color DARK_ORCHID()            { return w_color::from_hex(0xFF9932CC); }
    static w_color DARK_RED()               { return w_color::from_hex(0xFF8B0000); }
    static w_color DARK_SALMON()            { return w_color::from_hex(0xFFE9967A); }
    static w_color DARK_SEA_GREEN()         { return w_color::from_hex(0xFF8FBC8F); }
    static w_color DARK_SLATE_BLUE()        { return w_color::from_hex(0xFF483D8B); }
    static w_color DARK_SLATE_GRAY()        { return w_color::from_hex(0xFF2F4F4F); }
    static w_color DARK_TURQUOISE()         { return w_color::from_hex(0xFF00CED1); }
    static w_color DARK_VIOLET()            { return w_color::from_hex(0xFF9400D3); }
    static w_color DEEP_PINK()              { return w_color::from_hex(0xFFFF1493); }
    static w_color DEEP_SKY_BLUE()          { return w_color::from_hex(0xFF00BFFF); }
    static w_color DIM_GRAY()               { return w_color::from_hex(0xFF696969); }
    static w_color DODGER_BLUE()            { return w_color::from_hex(0xFF1E90FF); }
    static w_color FIREBRICK()              { return w_color::from_hex(0xFFB22222); }
    static w_color FLORAL_WHITE()           { return w_color::from_hex(0xFFFFFAF0); }
    static w_color FOREST_GREEN()           { return w_color::from_hex(0xFF228B22); }
    static w_color FUCHSIA()                { return w_color::from_hex(0xFFFF00FF); }
    static w_color GAINSBORO()              { return w_color::from_hex(0xFFDCDCDC); }
    static w_color GHOST_WHITE()            { return w_color::from_hex(0xFFF8F8FF); }
    static w_color GOLD()                   { return w_color::from_hex(0xFFFFD700); }
    static w_color GOLDENROD()              { return w_color::from_hex(0xFFDAA520); }
    static w_color GRAY()                   { return w_color::from_hex(0xFF808080); }
    static w_color GREEN()                  { return w_color::from_hex(0xFF008000); }
    static w_color GREEN_YELLOW()           { return w_color::from_hex(0xFFADFF2F); }
    static w_color HONEYDEW()               { return w_color::from_hex(0xFFF0FFF0); }
    static w_color HOT_PINK()               { return w_color::from_hex(0xFFFF69B4); }
    static w_color INDIAN_RED()             { return w_color::from_hex(0xFFCD5C5C); }
    static w_color INDIGO()                 { return w_color::from_hex(0xFF4B0082); }
    static w_color IVORY()                  { return w_color::from_hex(0xFFFFFFF0); }
    static w_color KHAKI()                  { return w_color::from_hex(0xFFF0E68C); }
    static w_color LAVENDER()               { return w_color::from_hex(0xFFE6E6FA); }
    static w_color LAVENDER_BLUSH()         { return w_color::from_hex(0xFFFFF0F5); }
    static w_color LAWN_GREEN()             { return w_color::from_hex(0xFF7CFC00); }
    static w_color LEMON_CHIFFON()          { return w_color::from_hex(0xFFFFFACD); }
    static w_color LIGHT_BLUE()             { return w_color::from_hex(0xFFADD8E6); }
    static w_color LIGHT_CORAL()            { return w_color::from_hex(0xFFF08080); }
    static w_color LIGHT_CYAN()             { return w_color::from_hex(0xFFE0FFFF); }
    static w_color LIGHT_GOLDENROD_YELLOW() { return w_color::from_hex(0xFFFAFAD2); }
    static w_color LIGHT_GREEN()            { return w_color::from_hex(0xFF90EE90); }
    static w_color LIGHT_GRAY()             { return w_color::from_hex(0xFFD3D3D3); }
    static w_color LIGHT_PINK()             { return w_color::from_hex(0xFFFFB6C1); }
    static w_color LIGHT_SALMON()           { return w_color::from_hex(0xFFFFA07A); }
    static w_color LIGHT_SEA_GREEN()        { return w_color::from_hex(0xFF20B2AA); }
    static w_color LIGHT_SKY_BLUE()         { return w_color::from_hex(0xFF87CEFA); }

    //TODO remove and convert these to static functions
	enum COLORS_HEX
	{
//		LIGHT_SLATE_GRAY = 0xFF778899,
//		LIGHT_STEEL_BLUE = 0xFFB0C4DE,
//		LIGHT_YELLOW = 0xFFFFFFE0,
//		LIME = 0xFF00FF00,
//		LIME_GREEN = 0xFF32CD32,
//		LINEN = 0xFFFAF0E6,
//		MAGENTA = 0xFFFF00FF,
//		MAROON = 0xFF800000,
//		MEDIUM_AQUAMARINE = 0xFF66CDAA,
//		MEDIUM_BLUE = 0xFF0000CD,
//		MEDIUM_ORCHID = 0xFFBA55D3,
//		MEDIUM_PURPLE = 0xFF9370DB,
//		MEDIUM_SEA_GREEN = 0xFF3CB371,
//		MEDIUM_SLATE_BLUE = 0xFF7B68EE,
//		MEDIUM_SPRING_GREEN = 0xFF00FA9A,
//		MEDIUM_TURQUOISE = 0xFF48D1CC,
//		MEDIUM_VIOLET_RED = 0xFFC71585,
//		MIDNIGHT_BLUE = 0xFF191970,
//		MINT_CREAM = 0xFFF5FFFA,
//		MISTY_ROSE = 0xFFFFE4E1,
//		MOCCASIN = 0xFFFFE4B5,
//		NAVAJO_WHITE = 0xFFFFDEAD,
//		NAVY = 0xFF000080,
//		OLD_LACE = 0xFFFDF5E6,
//		OLIVE = 0xFF808000,
//		OLIVE_DRAB = 0xFF6B8E23,
//		ORANGE = 0xFFFFA500,
//		ORANGE_RED = 0xFFFF4500,
//		ORCHID = 0xFFDA70D6,
//		PALE_GOLDENROD = 0xFFEEE8AA,
//		PALE_GREEN = 0xFF98FB98,
//		PALE_TURQUOISE = 0xFFAFEEEE,
//		PALE_VIOLET_RED = 0xFFDB7093,
//		PAPAYA_WHIP = 0xFFFFEFD5,
//		PEACH_PUFF = 0xFFFFDAB9,
//		PERU = 0xFFCD853F,
//		PINK = 0xFFFFC0CB,
//		PLUM = 0xFFDDA0DD,
//		POWDER_BLUE = 0xFFB0E0E6,
//		PURPLE = 0xFF800080,
//		RED = 0xFFFF0000,
//		ROSY_BROWN = 0xFFBC8F8F,
//		ROYAL_BLUE = 0xFF4169E1,
//		SADDLE_BROWN = 0xFF8B4513,
//		SALMON = 0xFFFA8072,
//		SANDY_BROWN = 0xFFF4A460,
//		SEA_GREEN = 0xFF2E8B57,
//		SEA_SHELL = 0xFFFFF5EE,
//		SIENNA = 0xFFA0522D,
//		SILVER = 0xFFC0C0C0,
//		SKY_BLUE = 0xFF87CEEB,
//		SLATE_BLUE = 0xFF6A5ACD,
//		SLATE_GRAY = 0xFF708090,
//		SNOW = 0xFFFFFAFA,
//		SPRING_GREEN = 0xFF00FF7F,
//		STEEL_BLUE = 0xFF4682B4,
//		TAN = 0xFFD2B48C,
//		TEAL = 0xFF008080,
//		THISTLE = 0xFFD8BFD8,
//		TOMATO = 0xFFFF6347,
//		TRANSPARENT_ = 0x00000000,
//		TURQUOISE = 0xFF40E0D0,
//		VIOLET = 0xFFEE82EE,
//		WHEAT = 0xFFF5DEB3,
//		WHITE = 0xFFFFFFFF,
//		WHITE_SMOKE = 0xFFF5F5F5,
//		YELLOW = 0xFFFFFF00,
//		YELLOW_GREEN = 0xFF9ACD32,
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
