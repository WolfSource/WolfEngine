/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_element.h 
	Description		 : The base element class for gui of Wolf. This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_ELEMENT__
#define __W_ELEMENT__

#include <w_color.h>
#include <w_rectangle.h>
#include <glm_extention.h>

#define MAX_CONTROL_STATES 6

namespace wolf
{
    namespace gui
    {
        enum W_GUI_CONTROL_STATE
        {
            W_GUI_STATE_NORMAL = 0,
            W_GUI_STATE_DISABLED,
            W_GUI_STATE_HIDDEN,
            W_GUI_STATE_FOCUS,
            W_GUI_STATE_MOUSEOVER,
            W_GUI_STATE_PRESSED,
        };

        struct w_blend_color
        {
            void initialize(
                _In_ const w_color& default_color,
                _In_ const w_color& disabled_color = w_color(128, 128, 128, 200), 
                _In_ const w_color& hidden_Color = w_color(0));
            
            void blend(
                _In_ const UINT pStateIndex, 
                _In_ const float pElapsedTime, 
                _In_ const float pRate = 0.7f);
            
            void set_current(_In_ const w_color& pColor);

            // Control color states
            w_color color_states[MAX_CONTROL_STATES];
            w_color current_color_state;
        };

        class w_element
        {
        public:
            void set_texture(_In_ const UINT pTexture, _In_ const w_rectangle* pRectangleTexture,
                _In_ const DWORD pDefaultTextureColor = RGBA_TO_HEX_COLOR(255, 255, 255, 255));
            void refresh();
            // Index of the texture for this Element 
            UINT            index_texture;
            w_rectangle     rect_texture;
            w_blend_color   texture_color;
            w_blend_color   font_color;
        };

        struct w_element_holder
        {
            UINT        control_type;
            UINT        index_element;
            w_element   element;
        };
    }
}

#endif
