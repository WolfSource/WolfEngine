/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_point.h
	Description		 : Global color structure
	Comment          :
*/

#pragma once

#include "w_std.h"
#include "python_exporter/w_boost_python_helper.h"

struct w_point
{
    long x;
    long y;
};

struct w_point_f
{
    float x;
    float y;
};

struct w_point_t
{
    uint32_t x;
    uint32_t y;
};

inline bool operator == (const w_point& lValue, const w_point& rValue)
{
    return lValue.x == rValue.x && lValue.y == rValue.y;
}

inline bool operator != (const w_point& lValue, const w_point& rValue)
{
    return !(lValue == rValue);
}

inline bool operator == (const w_point_f& lValue, const w_point_f& rValue)
{
    return lValue.x == rValue.x && lValue.y == rValue.y;
}

inline bool operator != (const w_point_f& lValue, const w_point_f& rValue)
{
    return !(lValue == rValue);
}

inline bool operator == (const w_point_t& lValue, const w_point_t& rValue)
{
    return lValue.x == rValue.x && lValue.y == rValue.y;
}

inline bool operator != (const w_point_t& lValue, const w_point_t& rValue)
{
    return !(lValue == rValue);
}

#include "python_exporter/py_point.h"
