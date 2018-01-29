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
#include "w_std.h"
#include <cstdlib>
#include <string>
#include <sstream>
#include <array>
#include "python_exporter/w_boost_python_helper.h"

struct w_color
{
    uint8_t r, g, b, a;

    w_color() :
        r(0),
        g(0),
        b(0),
        a(0)
    {
    }

    //r(0-255),g(0-255),b(0-255),a(0-255)
    w_color(uint8_t pR, uint8_t pG, uint8_t pB, uint8_t pA) :
        r(pR),
        g(pG),
        b(pB),
        a(pA)
    {
    }

    //rgba(0-255)
    w_color(uint8_t pRGBA) :
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

        uint8_t _r = 0, _g = 0, _b = 0, _a = 0;
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

    //convert HEX to w_color
    static w_color from_hex(unsigned long pValue)
    {
        //Convert ARGB(unsigned long ) to RGBA(in the range of 0..255)
        return w_color(
            ((pValue >> 16) & 0xFF),//R
            ((pValue >> 8) & 0xFF),//G
            (pValue & 0xFF),//B
            ((pValue >> 24) & 0xFF));//A
    }

    //convert RGBA to HEX
    static unsigned int to_hex(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        return ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
    }
    //convert array of float(elements have value between 0.0f and 1.0f) to w_color
    static w_color to_color(_In_ const std::array<float, 4>& pValue)
    {
        return w_color(
            static_cast<uint8_t>(pValue[0] * 255.0f),
            static_cast<uint8_t>(pValue[1] * 255.0f),
            static_cast<uint8_t>(pValue[2] * 255.0f),
            static_cast<uint8_t>(pValue[3] * 255.0f));
    }

    static w_color ALICE_BLUE() { return w_color::from_hex(0xFFF0F8FF); }
    static w_color ANTIQUE_WHITE() { return w_color::from_hex(0xFFFAEBD7); }
    static w_color AQUA() { return w_color::from_hex(0xFF00FFFF); }
    static w_color AQUAMARINE() { return w_color::from_hex(0xFF7FFFD4); }
    static w_color AZURE() { return w_color::from_hex(0xFFF0FFFF); }
    static w_color BEIGE() { return w_color::from_hex(0xFFF5F5DC); }
    static w_color BISQUE() { return w_color::from_hex(0xFFFFE4C4); }
    static w_color BLACK() { return w_color::from_hex(0xFF000000); }
    static w_color BLANCHED_ALMOND() { return w_color::from_hex(0xFFFFEBCD); }
    static w_color BLUE() { return w_color::from_hex(0xFF0000FF); }
    static w_color BLUE_VIOLET() { return w_color::from_hex(0xFF8A2BE2); }
    static w_color BROWN() { return w_color::from_hex(0xFFA52A2A); }
    static w_color BURLY_WOOD() { return w_color::from_hex(0xFFDEB887); }
    static w_color CADET_BLUE() { return w_color::from_hex(0xFF5F9EA0); }
    static w_color CHARTREUSE() { return w_color::from_hex(0xFF7FFF00); }
    static w_color CHOCOLATE() { return w_color::from_hex(0xFFD2691E); }
    static w_color CORAL() { return w_color::from_hex(0xFFFF7F50); }
    static w_color CORNFLOWER_BLUE() { return w_color::from_hex(0xFF6495ED); }
    static w_color CORNSILK() { return w_color::from_hex(0xFFFFF8DC); }
    static w_color CRIMSON() { return w_color::from_hex(0xFFDC143C); }
    static w_color CYAN() { return w_color::from_hex(0xFF00FFFF); }
    static w_color DARK_BLUE() { return w_color::from_hex(0xFF00008B); }
    static w_color DARK_CYAN() { return w_color::from_hex(0xFF008B8B); }
    static w_color DARK_GOLDENROD() { return w_color::from_hex(0xFFB8860B); }
    static w_color DARK_GRAY() { return w_color::from_hex(0xFFA9A9A9); }
    static w_color DARK_GREEN() { return w_color::from_hex(0xFF006400); }
    static w_color DARK_KHAKI() { return w_color::from_hex(0xFFBDB76B); }
    static w_color DARK_MAGENTA() { return w_color::from_hex(0xFF8B008B); }
    static w_color DARK_OLIVE_GREEN() { return w_color::from_hex(0xFF556B2F); }
    static w_color DARK_ORANGE() { return w_color::from_hex(0xFFFF8C00); }
    static w_color DARK_ORCHID() { return w_color::from_hex(0xFF9932CC); }
    static w_color DARK_RED() { return w_color::from_hex(0xFF8B0000); }
    static w_color DARK_SALMON() { return w_color::from_hex(0xFFE9967A); }
    static w_color DARK_SEA_GREEN() { return w_color::from_hex(0xFF8FBC8F); }
    static w_color DARK_SLATE_BLUE() { return w_color::from_hex(0xFF483D8B); }
    static w_color DARK_SLATE_GRAY() { return w_color::from_hex(0xFF2F4F4F); }
    static w_color DARK_TURQUOISE() { return w_color::from_hex(0xFF00CED1); }
    static w_color DARK_VIOLET() { return w_color::from_hex(0xFF9400D3); }
    static w_color DEEP_PINK() { return w_color::from_hex(0xFFFF1493); }
    static w_color DEEP_SKY_BLUE() { return w_color::from_hex(0xFF00BFFF); }
    static w_color DIM_GRAY() { return w_color::from_hex(0xFF696969); }
    static w_color DODGER_BLUE() { return w_color::from_hex(0xFF1E90FF); }
    static w_color FIREBRICK() { return w_color::from_hex(0xFFB22222); }
    static w_color FLORAL_WHITE() { return w_color::from_hex(0xFFFFFAF0); }
    static w_color FOREST_GREEN() { return w_color::from_hex(0xFF228B22); }
    static w_color FUCHSIA() { return w_color::from_hex(0xFFFF00FF); }
    static w_color GAINSBORO() { return w_color::from_hex(0xFFDCDCDC); }
    static w_color GHOST_WHITE() { return w_color::from_hex(0xFFF8F8FF); }
    static w_color GOLD() { return w_color::from_hex(0xFFFFD700); }
    static w_color GOLDENROD() { return w_color::from_hex(0xFFDAA520); }
    static w_color GRAY() { return w_color::from_hex(0xFF808080); }
    static w_color GREEN() { return w_color::from_hex(0xFF008000); }
    static w_color GREEN_YELLOW() { return w_color::from_hex(0xFFADFF2F); }
    static w_color HONEYDEW() { return w_color::from_hex(0xFFF0FFF0); }
    static w_color HOT_PINK() { return w_color::from_hex(0xFFFF69B4); }
    static w_color INDIAN_RED() { return w_color::from_hex(0xFFCD5C5C); }
    static w_color INDIGO() { return w_color::from_hex(0xFF4B0082); }
    static w_color IVORY() { return w_color::from_hex(0xFFFFFFF0); }
    static w_color KHAKI() { return w_color::from_hex(0xFFF0E68C); }
    static w_color LAVENDER() { return w_color::from_hex(0xFFE6E6FA); }
    static w_color LAVENDER_BLUSH() { return w_color::from_hex(0xFFFFF0F5); }
    static w_color LAWN_GREEN() { return w_color::from_hex(0xFF7CFC00); }
    static w_color LEMON_CHIFFON() { return w_color::from_hex(0xFFFFFACD); }
    static w_color LIGHT_BLUE() { return w_color::from_hex(0xFFADD8E6); }
    static w_color LIGHT_CORAL() { return w_color::from_hex(0xFFF08080); }
    static w_color LIGHT_CYAN() { return w_color::from_hex(0xFFE0FFFF); }
    static w_color LIGHT_GOLDENROD_YELLOW() { return w_color::from_hex(0xFFFAFAD2); }
    static w_color LIGHT_GREEN() { return w_color::from_hex(0xFF90EE90); }
    static w_color LIGHT_GRAY() { return w_color::from_hex(0xFFD3D3D3); }
    static w_color LIGHT_PINK() { return w_color::from_hex(0xFFFFB6C1); }
    static w_color LIGHT_SALMON() { return w_color::from_hex(0xFFFFA07A); }
    static w_color LIGHT_SEA_GREEN() { return w_color::from_hex(0xFF20B2AA); }
    static w_color LIGHT_SKY_BLUE() { return w_color::from_hex(0xFF87CEFA); }
    static w_color LIGHT_SLATE_GRAY() { return w_color::from_hex(0xFF778899); }
    static w_color LIGHT_STEEL_BLUE() { return w_color::from_hex(0xFFB0C4DE); }
    static w_color LIGHT_YELLOW() { return w_color::from_hex(0xFFFFFFE0); }
    static w_color LIME() { return w_color::from_hex(0xFF00FF00); }
    static w_color LIME_GREEN() { return w_color::from_hex(0xFF32CD32); }
    static w_color LINEN() { return w_color::from_hex(0xFFFAF0E6); }
    static w_color MAGENTA() { return w_color::from_hex(0xFFFF00FF); }
    static w_color MAROON() { return w_color::from_hex(0xFF800000); }
    static w_color MEDIUM_AQUAMARINE() { return w_color::from_hex(0xFF66CDAA); }
    static w_color MEDIUM_BLUE() { return w_color::from_hex(0xFF0000CD); }
    static w_color MEDIUM_ORCHID() { return w_color::from_hex(0xFFBA55D3); }
    static w_color MEDIUM_PURPLE() { return w_color::from_hex(0xFF9370DB); }
    static w_color MEDIUM_SEA_GREEN() { return w_color::from_hex(0xFF3CB371); }
    static w_color MEDIUM_SLATE_BLUE() { return w_color::from_hex(0xFF7B68EE); }
    static w_color MEDIUM_SPRING_GREEN() { return w_color::from_hex(0xFF00FA9A); }
    static w_color MEDIUM_TURQUOISE() { return w_color::from_hex(0xFF48D1CC); }
    static w_color MEDIUM_VIOLET_RED() { return w_color::from_hex(0xFFC71585); }
    static w_color MIDNIGHT_BLUE() { return w_color::from_hex(0xFF191970); }
    static w_color MINT_CREAM() { return w_color::from_hex(0xFFF5FFFA); }
    static w_color MISTY_ROSE() { return w_color::from_hex(0xFFFFE4E1); }
    static w_color MOCCASIN() { return w_color::from_hex(0xFFFFE4B5); }
    static w_color NAVAJO_WHITE() { return w_color::from_hex(0xFFFFDEAD); }
    static w_color NAVY() { return w_color::from_hex(0xFF000080); }
    static w_color OLD_LACE() { return w_color::from_hex(0xFFFDF5E6); }
    static w_color OLIVE() { return w_color::from_hex(0xFF808000); }
    static w_color OLIVE_DRAB() { return w_color::from_hex(0xFF6B8E23); }
    static w_color ORANGE() { return w_color::from_hex(0xFFFFA500); }
    static w_color ORANGE_RED() { return w_color::from_hex(0xFFFF4500); }
    static w_color ORCHID() { return w_color::from_hex(0xFFDA70D6); }
    static w_color PALE_GOLDENROD() { return w_color::from_hex(0xFFEEE8AA); }
    static w_color PALE_GREEN() { return w_color::from_hex(0xFF98FB98); }
    static w_color PALE_TURQUOISE() { return w_color::from_hex(0xFFAFEEEE); }
    static w_color PALE_VIOLET_RED() { return w_color::from_hex(0xFFDB7093); }
    static w_color PAPAYA_WHIP() { return w_color::from_hex(0xFFFFEFD5); }
    static w_color PEACH_PUFF() { return w_color::from_hex(0xFFFFDAB9); }
    static w_color PERU() { return w_color::from_hex(0xFFCD853F); }
    static w_color PINK() { return w_color::from_hex(0xFFFFC0CB); }
    static w_color PLUM() { return w_color::from_hex(0xFFDDA0DD); }
    static w_color POWDER_BLUE() { return w_color::from_hex(0xFFB0E0E6); }
    static w_color PURPLE() { return w_color::from_hex(0xFF800080); }
    static w_color RED() { return w_color::from_hex(0xFFFF0000); }
    static w_color ROSY_BROWN() { return w_color::from_hex(0xFFBC8F8F); }
    static w_color ROYAL_BLUE() { return w_color::from_hex(0xFF4169E1); }
    static w_color SADDLE_BROWN() { return w_color::from_hex(0xFF8B4513); }
    static w_color SALMON() { return w_color::from_hex(0xFFFA8072); }
    static w_color SANDY_BROWN() { return w_color::from_hex(0xFFF4A460); }
    static w_color SEA_GREEN() { return w_color::from_hex(0xFF2E8B57); }
    static w_color SEA_SHELL() { return w_color::from_hex(0xFFFFF5EE); }
    static w_color SIENNA() { return w_color::from_hex(0xFFA0522D); }
    static w_color SILVER() { return w_color::from_hex(0xFFC0C0C0); }
    static w_color SKY_BLUE() { return w_color::from_hex(0xFF87CEEB); }
    static w_color SLATE_BLUE() { return w_color::from_hex(0xFF6A5ACD); }
    static w_color SLATE_GRAY() { return w_color::from_hex(0xFF708090); }
    static w_color SNOW() { return w_color::from_hex(0xFFFFFAFA); }
    static w_color SPRING_GREEN() { return w_color::from_hex(0xFF00FF7F); }
    static w_color STEEL_BLUE() { return w_color::from_hex(0xFF4682B4); }
    static w_color TAN() { return w_color::from_hex(0xFFD2B48C); }
    static w_color TEAL() { return w_color::from_hex(0xFF008080); }
    static w_color THISTLE() { return w_color::from_hex(0xFFD8BFD8); }
    static w_color TOMATO() { return w_color::from_hex(0xFFFF6347); }
    static w_color TRANSPARENT_() { return w_color::from_hex(0x00000000); }
    static w_color TURQUOISE() { return w_color::from_hex(0xFF40E0D0); }
    static w_color VIOLET() { return w_color::from_hex(0xFFEE82EE); }
    static w_color WHEAT() { return w_color::from_hex(0xFFF5DEB3); }
    static w_color WHITE() { return w_color::from_hex(0xFFFFFFFF); }
    static w_color WHITE_SMOKE() { return w_color::from_hex(0xFFF5F5F5); }
    static w_color YELLOW() { return w_color::from_hex(0xFFFFFF00); }
    static w_color YELLOW_GREEN() { return w_color::from_hex(0xFF9ACD32); }

#ifdef __PYTHON__

    void py_from_string(const char* pValue)
    {
        auto _c = w_color::from_string(pValue);
        this->r = _c.r;
        this->g = _c.g;
        this->b = _c.b;
        this->a = _c.a;
    }

    void py_from_hex(unsigned long pValue)
    {
        auto _c = w_color::from_hex(pValue);
        this->r = _c.r;
        this->g = _c.g;
        this->b = _c.b;
        this->a = _c.a;
    }

    unsigned int py_to_hex(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        return w_color::to_hex(r, g, b, a);
    }

    void py_to_color(_In_ boost::python::tuple pValue)
    {
        float _r, _g, _b, _a;
        w_boost_extract_tuple(_r, _g, _b, _a, pValue);
        this->r = static_cast<uint8_t>(_r * 255.0f);
        this->g = static_cast<uint8_t>(_g * 255.0f);
        this->b = static_cast<uint8_t>(_b * 255.0f);
        this->a = static_cast<uint8_t>(_a * 255.0f);
    }

    void py_ALICE_BLUE() { py_from_hex(0xFFF0F8FF); }
    void py_ANTIQUE_WHITE() { py_from_hex(0xFFFAEBD7); }
    void py_AQUA() { py_from_hex(0xFF00FFFF); }
    void py_AQUAMARINE() { py_from_hex(0xFF7FFFD4); }
    void py_AZURE() { py_from_hex(0xFFF0FFFF); }
    void py_BEIGE() { py_from_hex(0xFFF5F5DC); }
    void py_BISQUE() { py_from_hex(0xFFFFE4C4); }
    void py_BLACK() { py_from_hex(0xFF000000); }
    void py_BLANCHED_ALMOND() { py_from_hex(0xFFFFEBCD); }
    void py_BLUE() { py_from_hex(0xFF0000FF); }
    void py_BLUE_VIOLET() { py_from_hex(0xFF8A2BE2); }
    void py_BROWN() { py_from_hex(0xFFA52A2A); }
    void py_BURLY_WOOD() { py_from_hex(0xFFDEB887); }
    void py_CADET_BLUE() { py_from_hex(0xFF5F9EA0); }
    void py_CHARTREUSE() { py_from_hex(0xFF7FFF00); }
    void py_CHOCOLATE() { py_from_hex(0xFFD2691E); }
    void py_CORAL() { py_from_hex(0xFFFF7F50); }
    void py_CORNFLOWER_BLUE() { py_from_hex(0xFF6495ED); }
    void py_CORNSILK() { py_from_hex(0xFFFFF8DC); }
    void py_CRIMSON() { py_from_hex(0xFFDC143C); }
    void py_CYAN() { py_from_hex(0xFF00FFFF); }
    void py_DARK_BLUE() { py_from_hex(0xFF00008B); }
    void py_DARK_CYAN() { py_from_hex(0xFF008B8B); }
    void py_DARK_GOLDENROD() { py_from_hex(0xFFB8860B); }
    void py_DARK_GRAY() { py_from_hex(0xFFA9A9A9); }
    void py_DARK_GREEN() { py_from_hex(0xFF006400); }
    void py_DARK_KHAKI() { py_from_hex(0xFFBDB76B); }
    void py_DARK_MAGENTA() { py_from_hex(0xFF8B008B); }
    void py_DARK_OLIVE_GREEN() { py_from_hex(0xFF556B2F); }
    void py_DARK_ORANGE() { py_from_hex(0xFFFF8C00); }
    void py_DARK_ORCHID() { py_from_hex(0xFF9932CC); }
    void py_DARK_RED() { py_from_hex(0xFF8B0000); }
    void py_DARK_SALMON() { py_from_hex(0xFFE9967A); }
    void py_DARK_SEA_GREEN() { py_from_hex(0xFF8FBC8F); }
    void py_DARK_SLATE_BLUE() { py_from_hex(0xFF483D8B); }
    void py_DARK_SLATE_GRAY() { py_from_hex(0xFF2F4F4F); }
    void py_DARK_TURQUOISE() { py_from_hex(0xFF00CED1); }
    void py_DARK_VIOLET() { py_from_hex(0xFF9400D3); }
    void py_DEEP_PINK() { py_from_hex(0xFFFF1493); }
    void py_DEEP_SKY_BLUE() { py_from_hex(0xFF00BFFF); }
    void py_DIM_GRAY() { py_from_hex(0xFF696969); }
    void py_DODGER_BLUE() { py_from_hex(0xFF1E90FF); }
    void py_FIREBRICK() { py_from_hex(0xFFB22222); }
    void py_FLORAL_WHITE() { py_from_hex(0xFFFFFAF0); }
    void py_FOREST_GREEN() { py_from_hex(0xFF228B22); }
    void py_FUCHSIA() { py_from_hex(0xFFFF00FF); }
    void py_GAINSBORO() { py_from_hex(0xFFDCDCDC); }
    void py_GHOST_WHITE() { py_from_hex(0xFFF8F8FF); }
    void py_GOLD() { py_from_hex(0xFFFFD700); }
    void py_GOLDENROD() { py_from_hex(0xFFDAA520); }
    void py_GRAY() { py_from_hex(0xFF808080); }
    void py_GREEN() { py_from_hex(0xFF008000); }
    void py_GREEN_YELLOW() { py_from_hex(0xFFADFF2F); }
    void py_HONEYDEW() { py_from_hex(0xFFF0FFF0); }
    void py_HOT_PINK() { py_from_hex(0xFFFF69B4); }
    void py_INDIAN_RED() { py_from_hex(0xFFCD5C5C); }
    void py_INDIGO() { py_from_hex(0xFF4B0082); }
    void py_IVORY() { py_from_hex(0xFFFFFFF0); }
    void py_KHAKI() { py_from_hex(0xFFF0E68C); }
    void py_LAVENDER() { py_from_hex(0xFFE6E6FA); }
    void py_LAVENDER_BLUSH() { py_from_hex(0xFFFFF0F5); }
    void py_LAWN_GREEN() { py_from_hex(0xFF7CFC00); }
    void py_LEMON_CHIFFON() { py_from_hex(0xFFFFFACD); }
    void py_LIGHT_BLUE() { py_from_hex(0xFFADD8E6); }
    void py_LIGHT_CORAL() { py_from_hex(0xFFF08080); }
    void py_LIGHT_CYAN() { py_from_hex(0xFFE0FFFF); }
    void py_LIGHT_GOLDENROD_YELLOW() { py_from_hex(0xFFFAFAD2); }
    void py_LIGHT_GREEN() { py_from_hex(0xFF90EE90); }
    void py_LIGHT_GRAY() { py_from_hex(0xFFD3D3D3); }
    void py_LIGHT_PINK() { py_from_hex(0xFFFFB6C1); }
    void py_LIGHT_SALMON() { py_from_hex(0xFFFFA07A); }
    void py_LIGHT_SEA_GREEN() { py_from_hex(0xFF20B2AA); }
    void py_LIGHT_SKY_BLUE() { py_from_hex(0xFF87CEFA); }
    void py_LIGHT_SLATE_GRAY() { py_from_hex(0xFF778899); }
    void py_LIGHT_STEEL_BLUE() { py_from_hex(0xFFB0C4DE); }
    void py_LIGHT_YELLOW() { py_from_hex(0xFFFFFFE0); }
    void py_LIME() { py_from_hex(0xFF00FF00); }
    void py_LIME_GREEN() { py_from_hex(0xFF32CD32); }
    void py_LINEN() { py_from_hex(0xFFFAF0E6); }
    void py_MAGENTA() { py_from_hex(0xFFFF00FF); }
    void py_MAROON() { py_from_hex(0xFF800000); }
    void py_MEDIUM_AQUAMARINE() { py_from_hex(0xFF66CDAA); }
    void py_MEDIUM_BLUE() { py_from_hex(0xFF0000CD); }
    void py_MEDIUM_ORCHID() { py_from_hex(0xFFBA55D3); }
    void py_MEDIUM_PURPLE() { py_from_hex(0xFF9370DB); }
    void py_MEDIUM_SEA_GREEN() { py_from_hex(0xFF3CB371); }
    void py_MEDIUM_SLATE_BLUE() { py_from_hex(0xFF7B68EE); }
    void py_MEDIUM_SPRING_GREEN() { py_from_hex(0xFF00FA9A); }
    void py_MEDIUM_TURQUOISE() { py_from_hex(0xFF48D1CC); }
    void py_MEDIUM_VIOLET_RED() { py_from_hex(0xFFC71585); }
    void py_MIDNIGHT_BLUE() { py_from_hex(0xFF191970); }
    void py_MINT_CREAM() { py_from_hex(0xFFF5FFFA); }
    void py_MISTY_ROSE() { py_from_hex(0xFFFFE4E1); }
    void py_MOCCASIN() { py_from_hex(0xFFFFE4B5); }
    void py_NAVAJO_WHITE() { py_from_hex(0xFFFFDEAD); }
    void py_NAVY() { py_from_hex(0xFF000080); }
    void py_OLD_LACE() { py_from_hex(0xFFFDF5E6); }
    void py_OLIVE() { py_from_hex(0xFF808000); }
    void py_OLIVE_DRAB() { py_from_hex(0xFF6B8E23); }
    void py_ORANGE() { py_from_hex(0xFFFFA500); }
    void py_ORANGE_RED() { py_from_hex(0xFFFF4500); }
    void py_ORCHID() { py_from_hex(0xFFDA70D6); }
    void py_PALE_GOLDENROD() { py_from_hex(0xFFEEE8AA); }
    void py_PALE_GREEN() { py_from_hex(0xFF98FB98); }
    void py_PALE_TURQUOISE() { py_from_hex(0xFFAFEEEE); }
    void py_PALE_VIOLET_RED() { py_from_hex(0xFFDB7093); }
    void py_PAPAYA_WHIP() { py_from_hex(0xFFFFEFD5); }
    void py_PEACH_PUFF() { py_from_hex(0xFFFFDAB9); }
    void py_PERU() { py_from_hex(0xFFCD853F); }
    void py_PINK() { py_from_hex(0xFFFFC0CB); }
    void py_PLUM() { py_from_hex(0xFFDDA0DD); }
    void py_POWDER_BLUE() { py_from_hex(0xFFB0E0E6); }
    void py_PURPLE() { py_from_hex(0xFF800080); }
    void py_RED() { py_from_hex(0xFFFF0000); }
    void py_ROSY_BROWN() { py_from_hex(0xFFBC8F8F); }
    void py_ROYAL_BLUE() { py_from_hex(0xFF4169E1); }
    void py_SADDLE_BROWN() { py_from_hex(0xFF8B4513); }
    void py_SALMON() { py_from_hex(0xFFFA8072); }
    void py_SANDY_BROWN() { py_from_hex(0xFFF4A460); }
    void py_SEA_GREEN() { py_from_hex(0xFF2E8B57); }
    void py_SEA_SHELL() { py_from_hex(0xFFFFF5EE); }
    void py_SIENNA() { py_from_hex(0xFFA0522D); }
    void py_SILVER() { py_from_hex(0xFFC0C0C0); }
    void py_SKY_BLUE() { py_from_hex(0xFF87CEEB); }
    void py_SLATE_BLUE() { py_from_hex(0xFF6A5ACD); }
    void py_SLATE_GRAY() { py_from_hex(0xFF708090); }
    void py_SNOW() { py_from_hex(0xFFFFFAFA); }
    void py_SPRING_GREEN() { py_from_hex(0xFF00FF7F); }
    void py_STEEL_BLUE() { py_from_hex(0xFF4682B4); }
    void py_TAN() { py_from_hex(0xFFD2B48C); }
    void py_TEAL() { py_from_hex(0xFF008080); }
    void py_THISTLE() { py_from_hex(0xFFD8BFD8); }
    void py_TOMATO() { py_from_hex(0xFFFF6347); }
    void py_TRANSPARENT_() { py_from_hex(0x00000000); }
    void py_TURQUOISE() { py_from_hex(0xFF40E0D0); }
    void py_VIOLET() { py_from_hex(0xFFEE82EE); }
    void py_WHEAT() { py_from_hex(0xFFF5DEB3); }
    void py_WHITE() { py_from_hex(0xFFFFFFFF); }
    void py_WHITE_SMOKE() { py_from_hex(0xFFF5F5F5); }
    void py_YELLOW() { py_from_hex(0xFFFFFF00); }
    void py_YELLOW_GREEN() { py_from_hex(0xFF9ACD32); }

#endif

};

inline bool operator == (const w_color& lValue, const w_color& rValue)
{
    return lValue.a == rValue.a && lValue.r == rValue.r && lValue.g == rValue.g && lValue.b == rValue.b;
}

inline bool operator != (const w_color& lValue, const w_color& rValue)
{
    return !(lValue == rValue);
}

#include "python_exporter/w_color_py.h"

#endif // __W_COLOR_H__
