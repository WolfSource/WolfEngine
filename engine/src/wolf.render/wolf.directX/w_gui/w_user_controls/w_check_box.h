/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_check_box.h
	Description		 : The checkBox user control class.This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_CHECK_BOX_H__
#define __W_CHECK_BOX_H__

#include "w_button.h"

#define W_GUI_EVENT_CHECKBOX_CHANGED   0x0401

namespace wolf
{
	namespace gui
	{
		class w_check_box : public w_button
		{
		public:
			DX_EXP w_check_box(_In_opt_ w_widget* pParent = nullptr);
			DX_EXP virtual ~w_check_box();

			virtual bool handle_keyboard(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual bool handle_mouse(_In_ UINT pMsg, _In_ const POINT& pPoint, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual void on_hot_key() override;

			virtual bool contains_point(_In_ const POINT& pt) override;
			virtual void update_rects() override;

			virtual void render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float fElapsedTime) override;


#pragma region Getters

			DX_EXP bool get_checked() const { return this->checked; }
			DX_EXP w_color get_check_box_color() const								{ return w_color::from_hex(this->_check_box_color); }
			DX_EXP w_color get_check_box_pressed_color() const						{ return w_color::from_hex(this->_check_box_pressed_color); }
			DX_EXP w_color get_check_box_mouse_over_color() const					{ return w_color::from_hex(this->_check_box_mouse_over_color); }
			DX_EXP w_color get_check_box_focused_color() const						{ return w_color::from_hex(this->_check_box_focused_color); }
			DX_EXP w_color get_check_box_disabled_color() const						{ return w_color::from_hex(this->_check_box_disabled_color); }

#pragma endregion

#pragma region Setters

			DX_EXP void set_checked(_In_ bool bChecked)								{ set_checked_internal(bChecked, false); }
			DX_EXP void set_check_box_color(w_color pValue)							{ this->_check_box_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void set_check_box_pressed_color(w_color pValue)					{ this->_check_box_pressed_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void set_check_box_mouse_over_color(w_color pValue)				{ this->_check_box_mouse_over_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void set_check_box_focused_color(w_color pValue)					{ this->_check_box_focused_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void set_check_box_disabled_color(w_color pValue)				{ this->_check_box_disabled_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }

#pragma endregion

		protected:
			virtual void set_checked_internal(_In_ bool bChecked, _In_ bool bFromInput);

			bool checked;
			RECT rect_button;
			RECT rect_text;

		private:
			typedef w_button _super;

			DWORD _check_box_color;
			DWORD _check_box_pressed_color;
			DWORD _check_box_mouse_over_color;
			DWORD _check_box_focused_color;
			DWORD _check_box_disabled_color;

		};
	}
}

#endif
