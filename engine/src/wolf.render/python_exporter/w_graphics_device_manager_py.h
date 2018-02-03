/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : w_graphics_device_manager_py.h
    Description      : The python exporter for w_graphics_device_manager class and structs
    Comment          :
 */

#ifdef __PYTHON__

#ifndef __W_GRAPHICS_DEVICE_MANAGER_PY_H__
#define __W_GRAPHICS_DEVICE_MANAGER_PY_H__

namespace pywolf
{
    //it will be declared in w_graphics_device_manager.cpp
    static std::vector<std::shared_ptr<wolf::graphics::w_graphics_device>>  py_graphics_devices;

    static void w_graphics_device_manager_py_export()
    {
        using namespace boost::python;
        using namespace wolf::graphics;
        
        //export w_graphics_device_manager_configs class
        class_<w_graphics_device_manager_configs>("w_graphics_device_manager_configs", init<>())
            .def_readwrite("debug_gpu", &w_graphics_device_manager_configs::debug_gpu, "debug_gpu")
            .def_readwrite("off_screen_mode", &w_graphics_device_manager_configs::off_screen_mode, "off_screen_mode")
            ;

        //export w_viewport class
        class_<w_viewport>("w_viewport", init<>())
            .def_readwrite("x", &w_viewport::x, "x")
            .def_readwrite("y", &w_viewport::y, "y")
            .def_readwrite("width", &w_viewport::width, "width")
            .def_readwrite("height", &w_viewport::height, "height")
            .def_readwrite("minDepth", &w_viewport::minDepth, "minDepth")
            .def_readwrite("maxDepth", &w_viewport::maxDepth, "maxDepth")
            ;

        //export w_viewport_scissor class
        class_<w_viewport_scissor>("w_viewport_scissor", init<>())
            .add_property("offset", &w_viewport_scissor::py_get_offset, &w_viewport_scissor::py_set_offset, "offset")
            .add_property("extent", &w_viewport_scissor::py_get_extent, &w_viewport_scissor::py_set_extent, "extent")
            ;

    }
}

#endif//__W_GAME_PY_H__

#endif//__PYTHON__
