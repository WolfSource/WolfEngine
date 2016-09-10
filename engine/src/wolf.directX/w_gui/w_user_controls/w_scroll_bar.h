/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_scroll_bar.h
	Description		 : The scroll bar user control class.This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_SCROLL_BAR_H__
#define __W_SCROLL_BAR_H__

#include "w_control.h"

// Minimum scroll bar thumb size
#define SCROLLBAR_MIN_THUMB_SIZE	 8

// Delay and repeat period when clicking on the scroll bar arrows
#define SCROLLBAR_ARROW_CLICK_DELAY  0.33
#define SCROLLBAR_ARROW_CLICK_REPEAT 0.05

namespace wolf
{
	namespace gui
	{
		class w_scroll_bar : public w_control
		{
		public:
			w_scroll_bar(_In_opt_ w_widget* pParent = nullptr);
			virtual ~w_scroll_bar();

			virtual bool handle_keyboard(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual bool handle_mouse(_In_ UINT pMsg, _In_ const POINT& pPoint, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual bool on_msg_proc(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;

			virtual void render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float fElapsedTime) override;
			virtual void update_rects() override;


#pragma region Getters

			int			get_track_pos() const								{ return this->position; }
			int			get_page_size() const								{ return this->page_size; }

			w_color		get_scroll_color() const							{ return w_color::from_hex(this->_scroll_color); }
			w_color		get_scroll_background_color() const					{ return w_color::from_hex(this->_scroll_background_color); }
			w_color		get_scroll_disabled_color() const					{ return w_color::from_hex(this->_scroll_disabled_color); }
			
#pragma endregion

#pragma region Setters

			void		set_track_range(_In_ int pStart, _In_ int pEnd);
			void		set_track_pos(_In_ int pPosition);
			void		set_page_size(_In_ int pPageSize);

			void		set_scroll_color(w_color pColor)					{ this->_scroll_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			void		set_scroll_background_color(w_color pColor)			{ this->_scroll_background_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			void		set_scroll_disabled_color(w_color pColor)			{ this->_scroll_disabled_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			
#pragma endregion

			//Scroll by delta items (plus or minus).A positive value scrolls down, while a negative value scrolls up.
			void scroll(_In_ int pDelta);
			//Ensure that item index is displayed, scroll if necessary
			void show_item(_In_ int pIndex);  

		protected:
			// ARROWSTATE indicates the state of the arrow buttons.
			// CLEAR            No arrow is down.
			// CLICKED_UP       Up arrow is clicked.
			// CLICKED_DOWN     Down arrow is clicked.
			// HELD_UP          Up arrow is held down for sustained period.
			// HELD_DOWN        Down arrow is held down for sustained period.
			enum arrow_state
			{
				CLEAR,
				CLICKED_UP,
				CLICKED_DOWN,
				HELD_UP,
				HELD_DOWN
			};

			//Compute the dimension of the scroll thumb
			void            update_thumb_rect();
			//Clips position at boundaries. Ensures it stays within legal range.
			void            cap();  

			bool show_thumb;
			bool drag;
			RECT up_button_rect;
			RECT down_button_rect;
			RECT track_rect;
			RECT thumb_rect;
			int position;  // Position of the first displayed item
			int page_size;  // How many items are displayable in one page
			int start;     // First item
			int end;       // The index after the last item
			POINT last_mouse;// Last mouse position
			arrow_state arrow; // State of the arrows
			double arrow_timeStamp;  // Timestamp of last arrow event.

		private:
			typedef w_control _super;

			DWORD _scroll_color;
			DWORD _scroll_background_color;
			DWORD _scroll_disabled_color;
		};

	}
}

#endif