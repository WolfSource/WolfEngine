/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : w_texture_py.h
    Description      : The python exporter for w_texture class
    Comment          :
 */

#ifdef __PYTHON__

#ifndef __W_TEXTURE_PY_H__
#define __W_TEXTURE_PY_H__

namespace pywolf
{
    static void w_texture_py_export()
    {
        using namespace boost::python;
        using namespace wolf::graphics;
        
        //define w_texture_buffer_type enum
        enum_<w_texture_buffer_type>("w_texture_buffer_type")
            .value("W_TEXTURE_COLOR_BUFFER", w_texture_buffer_type::W_TEXTURE_COLOR_BUFFER)
            .value("W_TEXTURE_DEPTH_BUFFER", w_texture_buffer_type::W_TEXTURE_DEPTH_BUFFER)
            .value("W_TEXTURE_STENCIL_BUFFER", w_texture_buffer_type::W_TEXTURE_STENCIL_BUFFER)
            .export_values()
            ;
        
        //define w_texture_view_type enum
        enum_<w_texture_view_type>("w_texture_buffer_type")
            .value("W_TEXTURE_VIEW_TYPE_1D", w_texture_view_type::W_TEXTURE_VIEW_TYPE_1D)
            .value("W_TEXTURE_VIEW_TYPE_2D", w_texture_view_type::W_TEXTURE_VIEW_TYPE_2D)
            .value("W_TEXTURE_VIEW_TYPE_3D", w_texture_view_type::W_TEXTURE_VIEW_TYPE_3D)
            .value("W_TEXTURE_VIEW_TYPE_CUBE", w_texture_view_type::W_TEXTURE_VIEW_TYPE_CUBE)
            .value("W_TEXTURE_VIEW_TYPE_1D_ARRAY", w_texture_view_type::W_TEXTURE_VIEW_TYPE_1D_ARRAY)
            .value("W_TEXTURE_VIEW_TYPE_2D_ARRAY", w_texture_view_type::W_TEXTURE_VIEW_TYPE_2D_ARRAY)
            .value("W_TEXTURE_VIEW_TYPE_CUBE_ARRAY", w_texture_view_type::W_TEXTURE_VIEW_TYPE_CUBE_ARRAY)
            .export_values()
        ;
        
        //define w_sampler_type enum
        enum_<w_sampler_type>("w_sampler_type")
            .value("NO_MIPMAP_AND_NO_ANISOTROPY", w_sampler_type::NO_MIPMAP_AND_NO_ANISOTROPY)
            .value("NO_MIPMAP_AND_ANISOTROPY", w_sampler_type::NO_MIPMAP_AND_ANISOTROPY)
            .value("MIPMAP_AND_NO_ANISOTROPY", w_sampler_type::MIPMAP_AND_NO_ANISOTROPY)
            .value("MIPMAP_AND_ANISOTROPY", w_sampler_type::MIPMAP_AND_ANISOTROPY)
            .export_values()
        ;
        
        //export w_texture class
//        class_<w_texture>("w_texture", init<>())
//            .def_readwrite("debug_gpu", &w_graphics_device_manager_configs::debug_gpu, "debug_gpu")
//            .def_readwrite("off_screen_mode", &w_graphics_device_manager_configs::off_screen_mode, "off_screen_mode")
//            ;

    }
}

#endif//__W_TEXTURE_PY_H__

#endif//__PYTHON__
