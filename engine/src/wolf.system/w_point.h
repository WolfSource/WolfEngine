/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_point.h
	Description		 : Global color structure
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_POINT_H__
#define __W_POINT_H__

#include "w_std.h"

#ifdef __GNUC__
#pragma GCC visibility push(default) //The classes/structs below are exported
#endif

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

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

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
#endif // __W_POINT_H__
