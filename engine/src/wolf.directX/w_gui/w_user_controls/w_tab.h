/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_tab.h
	Description		 : The tab control contains of multiple widgets.

	Comment          :
*/

#ifndef __W_TAB__
#define __W_TAB__

#include "w_control.h"
#include "w_button.h"
#include <tbb/atomic.h>

namespace wolf
{
	namespace gui
	{
		class w_tab : public w_control
		{
		public:
			w_tab(_In_opt_ w_widget* pParent);
			~w_tab();

			virtual HRESULT on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice) override;
			
			virtual bool	handle_keyboard(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual bool	handle_mouse(_In_ UINT pMsg, _In_ const POINT& pPoint, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual void	on_hot_key() override;
			virtual bool	contains_point(_In_ const POINT& pt) override;
			virtual bool	can_have_focus() override;
			virtual void	render(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;

			DX_EXP void		add_tab_item(_In_z_ const std::wstring& pText,
							_In_z_ const std::wstring& pIconPath = L"",
							_In_ bool pRelativePath = true,
							_In_ int pTextOffsetX = -27, _In_ int pTextOffsetY = 10,
							_In_ int pIconOffsetX = 10, _In_ int pIconOffsetY = 10,
							_In_ float pIconScaleX = 1, _In_ float pIconScaleY = 1,
							_In_ UINT pHotkey = 0,
							_In_ bool pIsDefault = false);

			DX_EXP void		move_right();
			DX_EXP void		move_left();

			DX_EXP void		remove_all_tabs();

#pragma region Getter

			UINT	get_button_width() const								{ return this->button_width; }
			UINT	get_button_height() const								{ return this->button_height; }
			int		get_selected_tab() const								{ return this->selected_tab; }

			w_color	get_button_normal_color() const							{ return this->button_normal_color; }
			w_color	get_button_pressed_color() const						{ return this->button_pressed_color; }
			w_color	get_button_mouse_over_color() const						{ return this->button_mouse_over_color; }
			w_color	get_button_focused_color() const						{ return this->button_focused_color; }
			w_color	get_button_disabled_color() const						{ return this->button_disabled_color; }
			
			w_color	get_text_normal_color() const							{ return this->text_color; }
			w_color	get_text_pressed_color() const							{ return this->text_pressed_color; }
			w_color	get_text_mouse_over_color() const						{ return this->text_mouse_over_color; }
			w_color	get_text_focused_color() const							{ return this->text_focused_color; }
			w_color	get_text_disabled_color() const							{ return this->text_disabled_color; }

#pragma endregion

#pragma region Setter

			void set_button_size(UINT pWidth, UINT pHeight)					{ this->button_width = pWidth; this->button_height = pHeight; }
			void set_button_text_offset(LONG pX, LONG pY)					{ this->arrow_button_text_offset.x = pX; this->arrow_button_text_offset.y = pY; }
			void set_selected_tab(int pValue)								{ this->selected_tab = pValue; }

			void set_button_color(w_color pValue)							{ this->button_normal_color = pValue; }
			void set_button_pressed_color(w_color pValue)					{ this->button_pressed_color = pValue; }
			void set_button_mouse_over_color(w_color pValue)				{ this->button_mouse_over_color = pValue; }
			void set_button_focused_color(w_color pValue)					{ this->button_focused_color = pValue; }
			void set_button_disabled_color(w_color pValue)					{ this->button_disabled_color = pValue; }

			void set_text_normal_color(w_color pValue)						{ this->text_color = pValue; }
			void set_text_pressed_color(w_color pValue)						{ this->text_pressed_color = pValue; }
			void set_text_mouse_over_color(w_color pValue)					{ this->text_mouse_over_color = pValue; }
			void set_text_focused_color(w_color pValue)						{ this->text_focused_color = pValue; }
			void set_text_disabled_color(w_color pValue)					{ this->text_disabled_color = pValue; }

#pragma endregion

		protected:
			
			std::vector<w_button*>		buttons;
			UINT						button_width;
			UINT						button_height;
			int							selected_tab;
			w_color						button_normal_color;
			w_color						button_pressed_color;
			w_color						button_mouse_over_color;
			w_color						button_focused_color;
			w_color						button_disabled_color;

			w_color						text_color;
			w_color						text_mouse_over_color;
			w_color						text_pressed_color;
			w_color						text_focused_color;
			w_color						text_disabled_color;

			POINT						arrow_button_text_offset;

		private:
			typedef w_control			_super;

			w_button*					_pre_button;
			w_button*					_nxt_button;

			tbb::atomic<bool>			_first_time_initializing;
		};
	}
}

#endif
