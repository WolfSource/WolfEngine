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

namespace wolf
{
    namespace gui
    {
        struct w_gui_vertex_2d
        {
            float position[2];
            float color[4];
            float uv[2];
        };
    }
}

#endif //__W_GUI_VERTEX_H__
