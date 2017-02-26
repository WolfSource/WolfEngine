/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_slider.h
	Description		 : The slider user control class.This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_SLIDER_H__
#define __W_SLIDER_H__

#include "w_control.h"

#define W_EVENT_SLIDER_VALUE_CHANGED			0x0501
#define	W_EVENT_SLIDER_VALUE_CHANGED_UP			0x0502

namespace wolf
{
	namespace gui
	{
		class w_slider : public w_control
		{
		public:
			DX_EXP w_slider(_In_opt_ w_widget* pParent = nullptr);
			DX_EXP virtual ~w_slider();

			virtual bool contains_point(_In_ const POINT& pPoint) override;
			virtual bool can_have_focus() override;
			virtual bool handle_keyboard(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual bool handle_mouse(_In_ UINT pMsg, _In_ const POINT& pPoint, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;

			virtual void update_rects() override;
			virtual void render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;

#pragma region Getters

			DX_EXP int			get_value() const									{ return _value; }
			DX_EXP void		get_range(_Out_ int& pMin, _Out_ int& pMax) const		{ pMin = _min; pMax = _max; }

			DX_EXP w_color		get_main_color() const								{ return w_color::from_hex(this->_slider_main_color); }
			DX_EXP w_color		get_main_mouse_over_color() const					{ return w_color::from_hex(this->_slider_main_mouse_over_color); }
			DX_EXP w_color		get_main_pressed_color() const						{ return w_color::from_hex(this->_slider_main_pressed_color); }
			DX_EXP w_color		get_main_focused_color() const						{ return w_color::from_hex(this->_slider_main_focused_color); }
			DX_EXP w_color		get_main_disabled_color() const						{ return w_color::from_hex(this->_slider_main_disabled_color); }

			DX_EXP w_color		get_button_color() const							{ return w_color::from_hex(this->_slider_button_color); }
			DX_EXP w_color		get_button_mouse_over_color() const					{ return w_color::from_hex(this->_slider_button_mouse_over_color); }
			DX_EXP w_color		get_button_pressed_color() const					{ return w_color::from_hex(this->_slider_button_pressed_color); }
			DX_EXP w_color		get_button_focused_color() const					{ return w_color::from_hex(this->_slider_button_focused_color); }
			DX_EXP w_color		get_button_disabled_color() const					{ return w_color::from_hex(this->_slider_button_disabled_color); }

#pragma endregion

#pragma region Setters

			DX_EXP void set_value(int pValue)										{ set_value_internal(pValue, false); }
			DX_EXP void set_range(_In_ int pMin, _In_ int pMax);
			
			DX_EXP void set_main_color(w_color pColor)								{ this->_slider_main_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void set_main_mouse_over_color(w_color pColor)					{ this->_slider_main_mouse_over_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void set_main_pressed_color(w_color pColor)						{ this->_slider_main_pressed_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void set_main_focused_color(w_color pColor)						{ this->_slider_main_focused_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void set_main_disabled_color(w_color pColor)						{ this->_slider_main_disabled_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }

			DX_EXP void set_button_color(w_color pColor)							{ this->_slider_button_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void set_button_mouse_over_color(w_color pColor)					{ this->_slider_button_mouse_over_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void set_button_pressed_color(w_color pColor)					{ this->_slider_button_pressed_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void set_button_focused_color(w_color pColor)					{ this->_slider_button_focused_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void set_button_disabled_color(w_color pColor)					{ this->_slider_button_disabled_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }

#pragma endregion

		protected:
			void set_value_internal(_In_ int pValue, _In_ bool pFromInput);
			int value_from_pos(_In_ int pX);

			int _value;

			int _min;
			int _max;

			int _drag_x;      // Mouse position at start of drag
			int _drag_offset; // Drag offset from the center of the button
			int _button_x;

			bool _pressed;
			RECT _rectangle_button;

		private:
			typedef w_control _super;

			DWORD _slider_main_color;
			DWORD _slider_main_mouse_over_color;
			DWORD _slider_main_pressed_color;
			DWORD _slider_main_focused_color;
			DWORD _slider_main_disabled_color;

			DWORD _slider_button_color;
			DWORD _slider_button_mouse_over_color;
			DWORD _slider_button_pressed_color;
			DWORD _slider_button_focused_color;
			DWORD _slider_button_disabled_color;
		};
	}
}

#endif
