/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_checkBox.h
	Description		 : The checkBox user control class.This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_CHECKBOX_H__
#define __W_CHECKBOX_H__

#include "w_button.h"

#define W_GUI_EVENT_CHECKBOX_CHANGED   0x0401

namespace wolf
{
	namespace gui
	{
		class w_checkBox : public w_button
		{
		public:
			w_checkBox(_In_opt_ w_widget* pParent = nullptr);
			virtual ~w_checkBox();

			virtual bool handle_keyboard(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual bool handle_mouse(_In_ UINT pMsg, _In_ const POINT& pPoint, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual void on_hotKey() override;

			virtual bool contains_point(_In_ const POINT& pt) override;
			virtual void update_rects() override;

			virtual void render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float fElapsedTime) override;

			bool get_checked() const											{ return this->checked; }
			void set_checked(_In_ bool bChecked)								{ set_checked_internal(bChecked, false); }

#pragma region Getters

			W_COLOR get_checkBox_color() const									{ return DirectX::DWORD_COLOR_TO_W_COLOR(this->_checkBox_normal_color); }
			W_COLOR get_checkBox_pressed_color() const							{ return DirectX::DWORD_COLOR_TO_W_COLOR(this->_checkBox_pressed_color); }
			W_COLOR get_checkBox_mouse_over_color() const						{ return DirectX::DWORD_COLOR_TO_W_COLOR(this->_checkBox_mouse_over_color); }
			W_COLOR get_checkBox_focused_color() const							{ return DirectX::DWORD_COLOR_TO_W_COLOR(this->_checkBox_focused_color); }

#pragma endregion

#pragma region Setters

			void set_checkBox_color(W_COLOR pColor)								{ this->_checkBox_normal_color = RGBA_TO_DWORD_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			void set_checkBox_pressed_color(W_COLOR pColor)						{ this->_checkBox_pressed_color = RGBA_TO_DWORD_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			void set_checkBox_mouse_over_color(W_COLOR pColor)					{ this->_checkBox_mouse_over_color = RGBA_TO_DWORD_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			void set_checkBox_focused_color(W_COLOR pColor)						{ this->_checkBox_focused_color = RGBA_TO_DWORD_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }

#pragma endregion

		protected:
			virtual void set_checked_internal(_In_ bool bChecked, _In_ bool bFromInput);

			bool checked;
			RECT rect_button;
			RECT rect_text;

		private:
			typedef w_button _super;

			DWORD _checkBox_normal_color;
			DWORD _checkBox_pressed_color;
			DWORD _checkBox_mouse_over_color;
			DWORD _checkBox_focused_color;

		};
	}
}

#endif
