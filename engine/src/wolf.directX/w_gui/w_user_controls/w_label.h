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

#include "w_control.h"
#include "w_graphics/w_sprite_batch.h"

namespace wolf
{
	namespace gui
	{
		class w_label : public w_control
		{
		public:
			DX_EXP											w_label(_In_opt_ w_widget* pParent);

			virtual void									render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;
			virtual bool									contains_point(_In_ const POINT& pPoint) override;

			virtual	ULONG									release() override;

#pragma region Getter

			DX_EXP HRESULT									get_text_copy(_Out_writes_(bufferCount) std::wstring pText, UINT pBufferCount) const;
			DX_EXP const wchar_t*							get_text() const												{ return this->text.c_str(); }
			DX_EXP int										get_original_text_offset_x() const								{ return this->original_text_offset_x; }
			DX_EXP int										get_original_text_offset_y() const								{ return this->original_text_offset_y; }
			DX_EXP int										get_text_offset_x() const										{ return this->text_offset_x; }
			DX_EXP int										get_text_offset_y() const										{ return this->text_offset_y; }
			DX_EXP float									get_font_size(int pValue) const									{ return this->font_size; }

			DX_EXP w_color									get_label_color() const											{ return w_color::from_hex(this->label_color); }
			DX_EXP w_color									get_label_mouse_over_color() const								{ return w_color::from_hex(this->label_mouse_over_color); }
			DX_EXP w_color									get_label_pressed_color() const									{ return w_color::from_hex(this->label_pressed_color); }
			DX_EXP w_color									get_label_focused_color() const									{ return w_color::from_hex(this->label_focused_color); }
			DX_EXP w_color									get_label_disabled_color() const								{ return w_color::from_hex(this->label_disabled_color); }

			DX_EXP w_color									get_current_color_state() const									{ return this->current_color_state; }
			DX_EXP bool										get_force_use_current_color_state(bool pValue) const			{ return this->force_use_current_color_state; }

#pragma endregion

#pragma region Setter

			DX_EXP HRESULT									set_text(_In_z_ std::wstring pText);
			DX_EXP void										set_original_text_offset_x(int pX)								{ this->original_text_offset_x = pX; }
			DX_EXP void										set_original_text_offset_y(int pY)								{ this->original_text_offset_y = pY; }
			DX_EXP void										set_text_offset_x(int pX)										{ this->text_offset_x = pX; }
			DX_EXP void										set_text_offset_y(int pY)										{ this->text_offset_y = pY; }

			DX_EXP void										set_font_size(float pValue)										{ this->font_size = pValue; }

			DX_EXP void										set_label_color(w_color pColor)									{ this->label_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void										set_label_mouse_over_color(w_color pColor)						{ this->label_mouse_over_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void										set_label_pressed_color(w_color pColor)							{ this->label_pressed_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void										set_label_focused_color(w_color pColor)							{ this->label_focused_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void										set_label_disabled_color(w_color pColor)						{ this->label_disabled_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }

			DX_EXP void										set_current_color_state(w_color pColor)							{ this->current_color_state = pColor; }
			DX_EXP void										set_force_use_current_color_state(bool pValue)					{ this->force_use_current_color_state = pValue; }

#pragma endregion

		protected:
			bool											render_from_child;
			std::wstring									text;      // Window text  
			int												text_offset_x;
			int												text_offset_y;
			int												original_text_offset_x;
			int												original_text_offset_y;
			float											font_size;
			D2D1_COLOR_F									font_color;
			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	brush;

			bool											force_use_current_color_state;
			w_color											current_color_state;

			DWORD											label_color;
			DWORD											label_mouse_over_color;
			DWORD											label_pressed_color;
			DWORD											label_focused_color;
			DWORD											label_disabled_color;

		private:
			typedef w_control _super;
		};
	}
}

#endif
