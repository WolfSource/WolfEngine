/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : py_plane.h
    Description      : The python exporter for w_plane structs
    Comment          :
 */

#ifdef __PYTHON__

#ifndef __PY_PLANE_H__
#define __PY_PLANE_H__

#include <boost/python.hpp>

namespace pyWolf
{
    static void py_plane_export()
    {
		using namespace boost::python;

		//define w_plane enum
		enum_<w_plane>("w_plane")
			.value("XY", w_plane::XY)
			.value("XZ", w_plane::XZ)
			.value("YZ", w_plane::YZ)
			.export_values()
			;
    }
}

#endif//__W_POINT_PY_H__

#endif//__PYTHON__
