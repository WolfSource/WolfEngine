#include "w_directX_pch.h"
#include "w_scroll_bar.h"
#include "w_gui/w_widgets_resource_manager.h"
#include <w_time_span.h>

using namespace wolf::gui;

w_scroll_bar::w_scroll_bar(_In_opt_ w_widget* pParent) : _super(pParent),
_scroll_color(RGBA_TO_HEX_COLOR(200, 200, 200, 255)),
_scroll_background_color(RGBA_TO_HEX_COLOR(255, 255, 255, 200)),
_scroll_disabled_color(RGBA_TO_HEX_COLOR(255, 255, 255, 70))
{
	_super::type = W_GUI_CONTROL_SCROLLBAR;
	_super::parent_widget = pParent;

	this->show_thumb = true;
	this->drag = false;

	SetRect(&this->up_button_rect, 0, 0, 0, 0);
	SetRect(&this->down_button_rect, 0, 0, 0, 0);
	SetRect(&this->track_rect, 0, 0, 0, 0);
	SetRect(&this->thumb_rect, 0, 0, 0, 0);

	this->position = 0;
	this->page_size = 1;
	this->start = 0;
	this->end = 1;
	this->arrow = CLEAR;
	this->arrow_timeStamp = 0.0;
}

w_scroll_bar::~w_scroll_bar()
{
}

void w_scroll_bar::update_rects()
{
	_super::update_rects();

	// Make the buttons square
	SetRect(&this->up_button_rect,
		_super::boundingBox.left, 
		_super::boundingBox.top,
		_super::boundingBox.right,
		_super::boundingBox.top + /*RectWidth*/(_super::boundingBox.right - _super::boundingBox.left));

	SetRect(&this->down_button_rect, 
		_super::boundingBox.left,
		_super::boundingBox.bottom - /*RectWidth*/(_super::boundingBox.right - _super::boundingBox.left),
		_super::boundingBox.right,
		_super::boundingBox.bottom);

	SetRect(&this->track_rect, 
		this->up_button_rect.left, 
		this->up_button_rect.bottom,
		this->down_button_rect.right, 
		this->down_button_rect.top);

	thumb_rect.left = this->up_button_rect.left;
	thumb_rect.right = this->up_button_rect.right;

	update_thumb_rect();
}

void w_scroll_bar::update_thumb_rect()
{
	if (this->end - this->start > this->page_size)
	{
		auto _a = (this->track_rect.bottom - this->track_rect.top) * this->page_size / (this->end - this->start);
		auto _b = SCROLLBAR_MIN_THUMB_SIZE;

		int _thumb_height = std::max(_a , (long)_b);

		int _max_position = this->end - this->start - this->page_size;
		thumb_rect.top = this->track_rect.top + (this->position - this->start) * ((this->track_rect.bottom - this->track_rect.top) - _thumb_height) / _max_position;
		thumb_rect.bottom = thumb_rect.top + _thumb_height;
		this->show_thumb = true;
	}
	else
	{
		// No content to scroll
		thumb_rect.bottom = thumb_rect.top;
		this->show_thumb = false;
	}
}

void w_scroll_bar::scroll(_In_ int nDelta)
{
	// Perform scroll
	this->position += nDelta;

	// Cap position
	cap();

	// Update thumb position
	update_thumb_rect();
}

void w_scroll_bar::show_item(_In_ int nIndex)
{
	// Cap the index

	if (nIndex < 0)
	{
		nIndex = 0;
	}

	if (nIndex >= this->end)
	{
		nIndex = this->end - 1;
	}

	// Adjust position
	if (this->position > nIndex)
	{
		this->position = nIndex;
	}
	else if (this->position + this->page_size <= nIndex)
	{
		this->position = nIndex - this->page_size + 1;
	}
	update_thumb_rect();
}

_Use_decl_annotations_
bool w_scroll_bar::handle_keyboard(UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	W_UNUSED(pMsg);
	W_UNUSED(pWParam);
	W_UNUSED(pLParam);
	return false;
}

_Use_decl_annotations_
bool w_scroll_bar::handle_mouse(UINT pMsg, const POINT& pPoint, WPARAM pWParam, LPARAM pLParam)
{
	W_UNUSED(pWParam);
	W_UNUSED(pLParam);

	static int ThumbOffsetY;

	this->last_mouse = pPoint;

	switch (pMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	{
		// Check for click on up button

		if (PtInRect(&this->up_button_rect, pPoint))
		{
			if (this->parent_widget)
			{
				auto _hwnd = this->parent_widget->get_hwnd();
				SetCapture(_hwnd);
			}
			
			if (this->position > this->start)
			{
				--this->position;
			}
			update_thumb_rect();
			this->arrow = CLICKED_UP;
			this->arrow_timeStamp = wolf::system::w_time_span::now().get_total_seconds();
			return true;
		}

		// Check for click on down button

		if (PtInRect(&this->down_button_rect, pPoint))
		{
			if (this->parent_widget)
			{
				auto _hwnd = this->parent_widget->get_hwnd();
				SetCapture(_hwnd);
			}

			if (this->position + this->page_size <= this->end)
			{
				++this->position;
			}
			update_thumb_rect();
			this->arrow = CLICKED_DOWN;
			this->arrow_timeStamp = wolf::system::w_time_span::now().get_total_seconds();

			return true;
		}

		// Check for click on thumb

		if (PtInRect(&this->thumb_rect, pPoint))
		{
			if (this->parent_widget)
			{
				auto _hwnd = this->parent_widget->get_hwnd();
				SetCapture(_hwnd);
			}

			this->drag = true;
			ThumbOffsetY = pPoint.y - this->thumb_rect.top;
			return true;
		}

		// Check for click on track
		if (this->thumb_rect.left <= pPoint.x && this->thumb_rect.right > pPoint.x)
		{
			if (this->parent_widget)
			{
				auto _hwnd = this->parent_widget->get_hwnd();
				SetCapture(_hwnd);
			}

			if (this->thumb_rect.top > pPoint.y && this->track_rect.top <= pPoint.y)
			{
				scroll(-(this->page_size - 1));
				return true;
			}
			else if (this->thumb_rect.bottom <= pPoint.y && this->track_rect.bottom > pPoint.y)
			{
				scroll(this->page_size - 1);
				return true;
			}
		}
		break;
	}

	case WM_LBUTTONUP:
	{
		this->drag = false;
		ReleaseCapture();
		update_thumb_rect();
		this->arrow = CLEAR;
		break;
	}

	case WM_MOUSEMOVE:
	{
		if (this->drag)
		{
			this->thumb_rect.bottom += pPoint.y - ThumbOffsetY - this->thumb_rect.top;
			this->thumb_rect.top = pPoint.y - ThumbOffsetY;
			if (this->thumb_rect.top < this->track_rect.top)
			{
				OffsetRect(&this->thumb_rect, 0, this->track_rect.top - this->thumb_rect.top);
			}
			else if (this->thumb_rect.bottom > this->track_rect.bottom)
			{
				OffsetRect(&this->thumb_rect, 0, this->track_rect.bottom - this->thumb_rect.bottom);
			}
			// Compute first item index based on thumb position
			int nMaxFirstItem = this->end - this->start - this->page_size + 1;  // Largest possible index for first item
			int nMaxThumb = /*RectHeight*/(this->track_rect.bottom - this->track_rect.top) - /*RectHeight*/(this->thumb_rect.bottom - this->thumb_rect.top);  // Largest possible thumb position from the top

			this->position = this->start + (this->thumb_rect.top - this->track_rect.top +
				nMaxThumb / (nMaxFirstItem * 2)) * // Shift by half a row to avoid last row covered by only one pixel
				nMaxFirstItem / nMaxThumb;

			return true;
		}
		break;
	}
	}

	return false;
}

_Use_decl_annotations_
bool w_scroll_bar::on_msg_proc(UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	W_UNUSED(pWParam);

	if (WM_CAPTURECHANGED == pMsg)
	{
		if (this->parent_widget)
		{
			auto _hwnd = this->parent_widget->get_hwnd();
			// The application just lost mouse capture. We may not have gotten
			// the WM_MOUSEUP message, so reset m_bDrag here.
			if ((HWND) pLParam != _hwnd)
			{
				this->drag = false;
			}
		}


	}

	return false;
}

void w_scroll_bar::render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (!this->visible) return;

	/*
		Check if the arrow button has been held for a while.
		If so, update the thumb position to simulate repeated
		scroll.
	*/
	if (this->arrow != CLEAR)
	{
		double dCurrTime = wolf::system::w_time_span::now().get_total_seconds();
		if (PtInRect(&this->up_button_rect, this->last_mouse))
		{
			switch (this->arrow)
			{
			case CLICKED_UP:
				if (SCROLLBAR_ARROW_CLICK_DELAY < dCurrTime - this->arrow_timeStamp)
				{
					scroll(-1);
					this->arrow = HELD_UP;
					this->arrow_timeStamp = dCurrTime;
				}
				break;
			case HELD_UP:
				if (SCROLLBAR_ARROW_CLICK_REPEAT < dCurrTime - this->arrow_timeStamp)
				{
					scroll(-1);
					this->arrow_timeStamp = dCurrTime;
				}
				break;
			}
		}
		else if (PtInRect(&this->down_button_rect, this->last_mouse))
		{
			switch (this->arrow)
			{
			case CLICKED_DOWN:
				if (SCROLLBAR_ARROW_CLICK_DELAY < dCurrTime - this->arrow_timeStamp)
				{
					scroll(1);
					this->arrow = HELD_DOWN;
					this->arrow_timeStamp = dCurrTime;
				}
				break;
			case HELD_DOWN:
				if (SCROLLBAR_ARROW_CLICK_REPEAT < dCurrTime - this->arrow_timeStamp)
				{
					scroll(1);
					this->arrow_timeStamp = dCurrTime;
				}
				break;
			}
		}
	}

	W_GUI_CONTROL_STATE _state = W_GUI_STATE_NORMAL;

	if (!this->visible)
	{
		_state = W_GUI_STATE_HIDDEN;
	}
	else if (!this->enabled || !this->show_thumb)
	{
		_state = W_GUI_STATE_DISABLED;
	}
	else if (this->mouse_over)
	{
		_state = W_GUI_STATE_MOUSEOVER;
	}
	else if (this->has_focus)
	{
		_state = W_GUI_STATE_FOCUS;
	}

	float _blend_rate = (_state == W_GUI_STATE_PRESSED) ? 0.0f : 0.8f;

	// Background track layer
	auto _element = this->elements[0];

	_element->texture_color.color_states[W_GUI_STATE_NORMAL] = this->_scroll_background_color;
	_element->texture_color.color_states[W_GUI_STATE_DISABLED] = this->_scroll_disabled_color;

	// Blend current color
	_element->texture_color.blend(_state, pElapsedTime, _blend_rate);
	_super::parent_widget->draw_sprite(_element, &this->track_rect, W_GUI_FAR_BUTTON_DEPTH);

	// Up Arrow
	_element = this->elements[1];

	_element->texture_color.color_states[W_GUI_STATE_NORMAL] = this->_scroll_color;
	_element->texture_color.color_states[W_GUI_STATE_DISABLED] = this->_scroll_disabled_color;

	// Blend current color
	_element->texture_color.blend(_state, pElapsedTime, _blend_rate);
	_super::parent_widget->draw_sprite(_element, &this->up_button_rect, W_GUI_NEAR_BUTTON_DEPTH);

	// Down Arrow
	_element = this->elements[2];

	_element->texture_color.color_states[W_GUI_STATE_NORMAL] = this->_scroll_color;
	_element->texture_color.color_states[W_GUI_STATE_DISABLED] = this->_scroll_disabled_color;

	// Blend current color
	_element->texture_color.blend(_state, pElapsedTime, _blend_rate);
	_super::parent_widget->draw_sprite(_element, &this->down_button_rect, W_GUI_NEAR_BUTTON_DEPTH);

	// Thumb button
	_element = this->elements[3];

	_element->texture_color.color_states[W_GUI_STATE_NORMAL] = this->_scroll_color;
	_element->texture_color.color_states[W_GUI_STATE_DISABLED] = this->_scroll_disabled_color;

	// Blend current color
	_element->texture_color.blend(_state, pElapsedTime, _blend_rate);
	_super::parent_widget->draw_sprite(_element, &this->thumb_rect, W_GUI_NEAR_BUTTON_DEPTH);
}

void w_scroll_bar::cap()
{
	if (this->position < this->start || this->end - this->start <= this->page_size)
	{
		this->position = this->start;
	}
	else if (this->position + this->page_size > this->end)
	{
		this->position = this->end - this->page_size + 1;
	}
}

#pragma region Setters

void w_scroll_bar::set_track_range(_In_ int pStart, _In_ int pEnd)
{
	this->start = pStart;
	this->end = pEnd;
	cap();
	update_thumb_rect();
}

void w_scroll_bar::set_track_pos(_In_ int pPosition)
{
	this->position = pPosition;
	cap();
	update_thumb_rect();
}

void w_scroll_bar::set_page_size(_In_ int pPageSize)
{
	this->page_size = pPageSize;
	cap();
	update_thumb_rect();
}

#pragma endregion