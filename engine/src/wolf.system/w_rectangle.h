/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_rectangle.h
	Description		 : Rectangle structure
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_RECTANGLE_H__
#define __W_RECTANGLE_H__

#include "w_std.h"

#include "python_exporter/w_boost_python_helper.h"

struct w_rectangle
{
    long left;
    long top;
    long right;
    long bottom;
};

inline bool operator == (const w_rectangle& lValue, const w_rectangle& rValue)
{
    return lValue.left == rValue.left && lValue.right == rValue.right && lValue.top == rValue.top && lValue.bottom == rValue.bottom;
}

inline bool operator != (const w_rectangle& lValue, const w_rectangle& rValue)
{
    return !(lValue == rValue);
}

#ifdef __PYTHON__
#include "python_exporter/w_rectangle_py.h"
#endif

#endif // __W_RECTANGLE_H__
