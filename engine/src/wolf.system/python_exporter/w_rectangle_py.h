/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : w_rectangle_py.h
    Description      : The python exporter for w_rectangle structs
    Comment          :
 */

#ifdef __PYTHON__

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_RECTANGLE_PY_H__
#define __W_RECTANGLE_PY_H__

namespace pywolf
{
    static void w_rectangle_py_export()
    {
        using namespace boost::python;
        using namespace wolf::system;
        
        class_<w_rectangle>("w_rectangle", init<>())
            .def_readwrite("left", &w_rectangle::left, "left")
            .def_readwrite("top", &w_rectangle::top, "top")
            .def_readwrite("right", &w_rectangle::right, "right")
            .def_readwrite("bottom", &w_rectangle::bottom, "bottom")
            ;
    }
}

#endif//__W_RECTANGLE_PY_H__

#endif//__PYTHON__