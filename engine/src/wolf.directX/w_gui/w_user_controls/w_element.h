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

#include <w_object.h> 
#include <w_math.h>

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
			void initialize(_In_ DWORD defaultColor, _In_ DWORD disabledColor = RGBA_TO_DWORD_COLOR(128, 128, 128, 200), _In_ DWORD hiddenColor = 0);
			void blend(_In_ UINT pStateIndex, _In_ float pElapsedTime, _In_ float pRate = 0.7f);
			void set_current(DWORD pColor);
			// Control color states
			DWORD color_states[MAX_CONTROL_STATES];
			DirectX::XMFLOAT4 current_color_state;
		};

		class w_element
		{
		public:
			void set_texture(_In_ UINT texture, _In_ RECT* prcTexture, _In_ DWORD defaultTextureColor = RGBA_TO_DWORD_COLOR(255, 255, 255, 255));
			void set_font(_In_ UINT font, _In_ DWORD defaultFontColor = RGBA_TO_DWORD_COLOR(255, 255, 255, 255), DWORD textFormat = DT_CENTER | DT_VCENTER);
			void refresh();

			// Index of the texture for this Element 
			UINT index_texture;          
			// Index of the font for this Element
			UINT index_font;             
			// The format argument to DrawText
			DWORD draw_text_format;      
			// Bounding rect of this element on the composite texture
			RECT rect_texture;         
			w_blend_color texture_color;
			w_blend_color font_color;
		};

		struct w_element_holder
		{
			UINT control_type;
			UINT index_element;
			w_element element;
		};
	}
}

#endif
