/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_gui_vertex.h
	Description		 : Vertex declaration for gui
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_GUI_VERTEX_H__
#define __W_GUI_VERTEX_H__

#include <w_color.h>

namespace wolf
{
    namespace gui
    {
        struct w_gui_vertex
        {
            float position[2];
            float uv[2];
        };

        struct w_gui_element
        {
            w_gui_vertex vertices[4];
            w_color      left_top_color;
            w_color      left_bottom_color;
            w_color      right_top_color;
            w_color      right_bottom_color;
        };
    }
}

#endif //__W_GUI_VERTEX_H__
