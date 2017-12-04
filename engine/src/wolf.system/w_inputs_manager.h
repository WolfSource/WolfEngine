/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_input.h
	Description		 : User input
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_INPUT_H__
#define __W_INPUT_H__

#ifdef __WIN32
#include <Windows.h>
#else
#include "w_point.h"
#endif

#include "w_system_export.h"
#include <set>

#ifdef __GNUC__
#pragma GCC visibility push(default) //The classes/structs below are exported
#endif

namespace wolf
{
    namespace system
    {
        class w_inputs_manager
        {
        public:
            w_inputs_manager();
            ~w_inputs_manager();

            struct w_mouse
            {
                bool                     left_button_pressed;
                bool                     left_button_released;
                bool                     middle_button_pressed;
                bool                     middle_button_released;
                bool                     right_button_pressed;
                bool                     right_button_released;
                float                    wheel;
                unsigned short           pos_x;
                unsigned short           pos_y;
                unsigned short           last_pos_x;
                unsigned short           last_pos_y;
            } mouse;

            struct w_keyboard
            {
                std::set<int>               keys_pressed;
                std::set<int>               keys_released;
                std::vector<unsigned short> inputed_chars;
            } keyboard;

			//reset all buffers
            WSYS_EXP void reset();
			//reset only keyboard buffers
			WSYS_EXP void reset_keyboard_buffers();
			//reset only mouse buffers
			WSYS_EXP void reset_mouse_buffers();

            WSYS_EXP HRESULT update(
#ifdef __WIN32
                _In_ const HWND pHWND,
                _In_ const UINT pMessage,
                _In_ const WPARAM pWParam,
                _In_ const LPARAM pLParam
#else
                _In_ bool pMouseLeftButtonDown,
                _In_ bool pMouseLeftButtonUp,
                _In_ bool pMouseRightButtonDown,
                _In_ bool pMouseRightButtonUp,
                _In_ bool pMouseMiddleButtonDown,
                _In_ bool pMouseMiddleButtonUp,
                _In_ float pMouseWheel,
                _In_ w_point_f* pMouseMove,
                _In_ unsigned short pKeyDown,
                _In_ unsigned short pKeyUp
#endif
                                    );

            WSYS_EXP std::vector<bool> is_keys_pressed(_In_ std::initializer_list<int> pKeys);
            WSYS_EXP std::vector<bool> is_keys_released(_In_ std::initializer_list<int> pKeys);
        };

    }
}
#ifdef __GNUC__
#pragma GCC visibility pop
#endif


#endif //__W_INPUT_H__
