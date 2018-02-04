/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : py_window.h
    Description      : The python exporter for w_window class and relative structs
    Comment          :
 */

#ifdef __PYTHON__

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __PY_WINDOW_H__
#define __PY_WINDOW_H__

namespace pywolf
{
    static void py_window_export()
    {
        using namespace boost::python;
        using namespace wolf::system;
        
        //export w_enumerate_screens class
        class_<w_enumerate_screens>("w_enumerate_screens", init<>())
            .def("get_screens", &w_enumerate_screens::py_screens, "Return screen(s) rectangle(s)")
            ;

        //export w_window_info class
        class_<w_window_info>("w_window_info", init<>())
            .def_readwrite("width", &w_window_info::width, "Width")
            .def_readwrite("height", &w_window_info::height, "Height")
#ifndef __APPLE__
            .def_readwrite("v_sync_enable", &w_window_info::v_sync_enable, "Enable/Disable GPU V-Sync")
#endif
            .def_readwrite("is_full_screen", &w_window_info::is_full_screen, "get is full screen")
            .def_readwrite("swap_chain_format", &w_window_info::swap_chain_format, "Swap chain preferred format")
            .def_readwrite("cpu_access_swap_chain_buffer", &w_window_info::cpu_access_swap_chain_buffer, "Allow CPU access to swap chain of GPU")
            .def("set_win_id", &w_window_info::py_set_win_id, "Set window ID(For windows is type of HWND, for linux is type of xcb_window_t and osx is type of void*)")
            ;
    }
}

#endif//__PY_WINDOW_H__

#endif//__PYTHON__