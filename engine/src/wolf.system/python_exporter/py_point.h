/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : py_point.h
    Description      : The python exporter for w_point structs
    Comment          :
 */

#ifdef __PYTHON__

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __PY_POINT_H__
#define __PY_POINT_H__

namespace pywolf
{
    static void py_point_export()
    {
        using namespace boost::python;
        using namespace wolf::system;
        
        class_<w_point>("w_point", init<>())
            .def_readwrite("x", &w_point::x, "x")
            .def_readwrite("y", &w_point::y, "y")
            ;

        class_<w_point_f>("w_point_f", init<>())
            .def_readwrite("x", &w_point_f::x, "x")
            .def_readwrite("y", &w_point_f::y, "y")
            ;

        class_<w_point_t>("w_point_t", init<>())
            .def_readwrite("x", &w_point_t::x, "x")
            .def_readwrite("y", &w_point_t::y, "y")
            ;
    }
}

#endif//__W_POINT_PY_H__

#endif//__PYTHON__
