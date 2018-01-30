/*
    Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website			 : http://WolfSource.io
    Name			 : w_inputs_manager_py.h
    Description		 : The python exporter for w_inputs_manager class
    Comment          :
*/

#ifdef __PYTHON__

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_INPUTS_MANAGER_PY_H__
#define __W_INPUTS_MANAGER_PY_H__

namespace pywolf
{
    static void w_inputs_manager_py_export()
    {
        using namespace boost::python;
        using namespace wolf::system;
        
        //export w_bounding_box class
        class_<w_inputs_manager::w_mouse>("w_mouse", init<>())
            .def_readonly("left_button_pressed", &w_inputs_manager::w_mouse::left_button_pressed, "left button pressed status")
            .def_readonly("left_button_released", &w_inputs_manager::w_mouse::left_button_released, "left button pressed status")
            .def_readonly("middle_button_pressed", &w_inputs_manager::w_mouse::middle_button_pressed, "left button pressed status")
            .def_readonly("middle_button_released", &w_inputs_manager::w_mouse::middle_button_released, "left button pressed status")
            .def_readonly("right_button_pressed", &w_inputs_manager::w_mouse::right_button_pressed, "left button pressed status")
            .def_readonly("right_button_released", &w_inputs_manager::w_mouse::right_button_released, "right button pressed status")
            .def_readonly("wheel", &w_inputs_manager::w_mouse::wheel, "mouse wheel data")
            .def_readonly("pos_x", &w_inputs_manager::w_mouse::pos_x, "mouse X position data")
            .def_readonly("pos_y", &w_inputs_manager::w_mouse::pos_y, "mouse Y position data")
            .def_readonly("last_pos_x", &w_inputs_manager::w_mouse::last_pos_x, "last mouse X position value")
            .def_readonly("last_pos_y", &w_inputs_manager::w_mouse::last_pos_y, "last mouse Y position value")
            ;

        //export w_bounding_box class
        class_<w_inputs_manager::w_keyboard>("w_keyboard", init<>())
            .def("get_keys_pressed", &w_inputs_manager::w_keyboard::py_get_keys_pressed, "get pressed keys")
            .def("get_keys_released", &w_inputs_manager::w_keyboard::py_get_keys_released, "get released keys")
            .def("get_inputed_chars", &w_inputs_manager::w_keyboard::py_get_inputed_chars, "inputed characters")
            ;
    }
}

#endif//__W_INPUTS_MANAGER_PY_H__

#endif//__PYTHON__
