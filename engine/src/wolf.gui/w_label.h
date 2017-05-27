/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_label.h
	Description		 : The static control which shows just text. This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT

	Comment          :
*/

#ifndef __W_LABEL__
#define __W_LABEL__

#include "w_gui_export.h"
#include "w_control.h"
//#include <ft2build.h>
//#include <freetype/freetype.h>

namespace wolf
{
	namespace gui
	{
		class w_label : public w_control
		{
		public:
			WGUI_EXP										w_label(_In_opt_ w_widget* pParent);

			virtual void									render(_In_ const float& pElapsedTime, _In_ std::vector<w_gui_vertex>& pVertexDeclarations) override;
			virtual bool									contains_point(_In_ const w_point& pPoint) override;

			virtual	ULONG									release() override;

#pragma region Getter

			WGUI_EXP const wchar_t*							get_text() const												{ return this->text.c_str(); }
            WGUI_EXP const w_color							get_font_color() const                                          { return this->font_color; }
			WGUI_EXP int									get_original_text_offset_x() const								{ return this->original_text_offset_x; }
			WGUI_EXP int									get_original_text_offset_y() const								{ return this->original_text_offset_y; }
			WGUI_EXP int									get_text_offset_x() const										{ return this->text_offset_x; }
			WGUI_EXP int									get_text_offset_y() const										{ return this->text_offset_y; }
			WGUI_EXP float									get_font_size(int pValue) const									{ return this->font_size; }

			WGUI_EXP const w_color							get_label_color() const											{ return this->label_color; }
			WGUI_EXP const w_color							get_label_mouse_over_color() const								{ return this->label_mouse_over_color; }
			WGUI_EXP const w_color							get_label_pressed_color() const									{ return this->label_pressed_color; }
			WGUI_EXP const w_color							get_label_focused_color() const									{ return this->label_focused_color; }
			WGUI_EXP const w_color							get_label_disabled_color() const								{ return this->label_disabled_color; }

            WGUI_EXP w_color								get_current_color_state() const									{ return this->current_color_state; }
            WGUI_EXP bool									get_force_use_current_color_state(bool pValue) const			{ return this->force_use_current_color_state; }

#pragma endregion

#pragma region Setter

            WGUI_EXP void								    set_text(_In_z_ const std::wstring& pText)                      { this->text = pText; }
			WGUI_EXP void									set_original_text_offset_x(int pX)								{ this->original_text_offset_x = pX; }
			WGUI_EXP void									set_original_text_offset_y(int pY)								{ this->original_text_offset_y = pY; }
			WGUI_EXP void									set_text_offset_x(int pX)										{ this->text_offset_x = pX; }
			WGUI_EXP void									set_text_offset_y(int pY)										{ this->text_offset_y = pY; }

            WGUI_EXP void									set_font_size(float pValue)										{ this->font_size = pValue; }

			WGUI_EXP void									set_label_color(w_color pColor)									{ this->label_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			WGUI_EXP void									set_label_mouse_over_color(w_color pColor)						{ this->label_mouse_over_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			WGUI_EXP void									set_label_pressed_color(w_color pColor)							{ this->label_pressed_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			WGUI_EXP void									set_label_focused_color(w_color pColor)							{ this->label_focused_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			WGUI_EXP void									set_label_disabled_color(w_color pColor)						{ this->label_disabled_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }

			WGUI_EXP void									set_current_color_state(w_color pColor)							{ this->current_color_state = pColor; }
			WGUI_EXP void									set_force_use_current_color_state(bool pValue)					{ this->force_use_current_color_state = pValue; }

#pragma endregion

		protected:
			bool											render_from_child;
			std::wstring									text; 
			int												text_offset_x;
			int												text_offset_y;
			int												original_text_offset_x;
			int												original_text_offset_y;
			float											font_size;
			w_color									        font_color;
			
			bool											force_use_current_color_state;
			w_color											current_color_state;

			w_color											label_color;
			w_color											label_mouse_over_color;
			w_color											label_pressed_color;
			w_color											label_focused_color;
			w_color											label_disabled_color;

            float                                           position_x;
            float                                           position_y;

		private:
			typedef w_control                               _super;
            //FT_Library                                      _freetype_lib;
            //FT_Face                                         _freetype_face;

		};
	}
}

#endif
