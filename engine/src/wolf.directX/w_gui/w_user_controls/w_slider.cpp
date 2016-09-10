#include "w_directX_pch.h"
#include "w_slider.h"
#include "w_gui/w_widgets_resource_manager.h"

using namespace wolf::gui;

w_slider::w_slider(_In_opt_ w_widget* pParent) : _super(pParent),
_min(0),
_max(100),
_value(50),
_pressed(false),
_slider_main_color(RGBA_TO_HEX_COLOR(255, 255, 255, 150)),
_slider_main_mouse_over_color(w_color::YELLOW),
_slider_main_pressed_color(w_color::WHITE),
_slider_main_focused_color(w_color::YELLOW),
_slider_main_disabled_color(RGBA_TO_HEX_COLOR(255, 255, 255, 70)),
_slider_button_color(RGBA_TO_HEX_COLOR(255, 255, 255, 150)),
_slider_button_mouse_over_color(w_color::YELLOW),
_slider_button_pressed_color(w_color::WHITE),
_slider_button_focused_color(w_color::YELLOW),
_slider_button_disabled_color(RGBA_TO_HEX_COLOR(255, 255, 255, 70))
{
	_super::type = W_GUI_CONTROL_SLIDER;
	_super::parent_widget = pParent;
	
}

w_slider::~w_slider()
{
}

bool w_slider::contains_point(_In_ const POINT& pPoint)
{
	return PtInRect(&(_super::boundingBox), pPoint) || PtInRect(&this->_rectangle_button, pPoint);
}

bool w_slider::can_have_focus()
{
	return _super::visible && _super::enabled;
}

void w_slider::update_rects()
{
	_super::update_rects();

	this->_rectangle_button = _super::boundingBox;
	this->_rectangle_button.right = this->_rectangle_button.left + (this->_rectangle_button.bottom - this->_rectangle_button.top);
	OffsetRect(&this->_rectangle_button, -(this->_rectangle_button.right - this->_rectangle_button.left) / 2, 0);

	this->_button_x = (int) ((this->_value - _min) * (float) (_super::boundingBox.right - _super::boundingBox.left) / (this->_max - this->_min));
	OffsetRect(&this->_rectangle_button, this->_button_x, 0);
}

int w_slider::value_from_pos(_In_ int pX)
{
	float fValuePerPixel = (float) (this->_max - this->_min) / (_super::boundingBox.right - _super::boundingBox.left);
	return (int) (0.5f + this->_min + fValuePerPixel * (pX - _super::boundingBox.left));
}

_Use_decl_annotations_
bool w_slider::handle_keyboard(UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	W_UNUSED(pLParam);

	if (!_super::enabled || !_super::visible) return false;

	switch (pMsg)
	{
	case WM_KEYDOWN:
	{
		switch (pWParam)
		{
		case VK_HOME:
			set_value_internal(this->_min, true);
			return true;

		case VK_END:
			set_value_internal(this->_max, true);
			return true;

		case VK_LEFT:
		case VK_DOWN:
			set_value_internal(this->_value - 1, true);
			return true;

		case VK_RIGHT:
		case VK_UP:
			set_value_internal(this->_value + 1, true);
			return true;

		case VK_NEXT:
			set_value_internal(this->_value - (10 > (this->_max - this->_min) / 10 ? 10 : (this->_max - this->_min) / 10),
				true);
			return true;

		case VK_PRIOR:
			set_value_internal(this->_value + (10 > (this->_max - this->_min) / 10 ? 10 : (this->_max - this->_min) / 10),
				true);
			return true;
		}
		break;
	}
	}

	return false;
}

_Use_decl_annotations_
bool w_slider::handle_mouse(UINT pMsg, const POINT& pPoint, WPARAM pWParam, LPARAM pLParam)
{
	W_UNUSED(pLParam);

	if (!_super::enabled || !_super::visible) return false;

	switch (pMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	{
		if (PtInRect(&this->_rectangle_button, pPoint))
		{
			// Pressed while inside the control
			this->_pressed = true;
			if (this->parent_widget)
			{
				auto _hwnd = this->parent_widget->get_hwnd();
				SetCapture(_hwnd);
			}

			this->_drag_x = pPoint.x;
			//m_nDragY = pt.y;
			this->_drag_offset = this->_button_x - this->_drag_x;

			//m_nDragValue = m_nValue;

			if (!_super::has_focus)
			{
				_super::parent_widget->request_focus(this);
			}

			return true;
		}

		if (PtInRect(&(_super::boundingBox), pPoint))
		{
			this->_drag_x = pPoint.x;
			this->_drag_offset = 0;
			this->_pressed = true;

			if (!_super::has_focus)
			{
				_super::parent_widget->request_focus(this);
			}

			if (pPoint.x > this->_button_x + _super::x)
			{
				set_value_internal(this->_value + 1, true);
				return true;
			}

			if (pPoint.x < this->_button_x + _super::x)
			{
				set_value_internal(this->_value - 1, true);
				return true;
			}
		}

		break;
	}

	case WM_LBUTTONUP:
	{
		if (this->_pressed)
		{
			this->_pressed = false;
			ReleaseCapture();
			_super::parent_widget->send_event(W_EVENT_SLIDER_VALUE_CHANGED_UP, true, this);

			return true;
		}

		break;
	}

	case WM_MOUSEMOVE:
	{
		if (this->_pressed)
		{
			set_value_internal(value_from_pos(_super::x + pPoint.x + this->_drag_offset), true);
			return true;
		}

		break;
	}

	case WM_MOUSEWHEEL:
	{
		int nScrollAmount = int((short) HIWORD(pWParam)) / WHEEL_DELTA;
		set_value_internal(this->_value - nScrollAmount, true);
		return true;
	}
	};

	return false;
}

void w_slider::set_range(_In_ int pMin, _In_ int pMax)
{
	this->_min = pMin;
	this->_max = pMax;

	set_value_internal(this->_value, false);
}

void w_slider::set_value_internal(_In_ int pValue, _In_ bool pFromInput)
{
	// Clamp to range
	pValue = std::max(this->_min, pValue);
	pValue = std::min(this->_max, pValue);

	if (pValue == this->_value) return;

	this->_value = pValue;
	update_rects();
	

	_super::parent_widget->send_event(W_EVENT_SLIDER_VALUE_CHANGED, pFromInput, this);
}

void w_slider::render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (!_super::visible) return;

	int _offsetX = 0;
	int _offsetY = 0;

	W_GUI_CONTROL_STATE _state = W_GUI_STATE_NORMAL;

	if (!_super::visible)
	{
		_state = W_GUI_STATE_HIDDEN;
	}
	else if (!_super::enabled)
	{
		_state = W_GUI_STATE_DISABLED;
	}
	else if (this->_pressed)
	{
		_state = W_GUI_STATE_PRESSED;

		_offsetX = 1;
		_offsetY = 2;
	}
	else if (_super::mouse_over)
	{
		_state = W_GUI_STATE_MOUSEOVER;

		_offsetX = -1;
		_offsetY = -2;
	}
	else if (_super::has_focus)
	{
		_state = W_GUI_STATE_FOCUS;
	}

	float fBlendRate = (_state == W_GUI_STATE_PRESSED) ? 0.0f : 0.8f;

	auto _element = this->elements[0];

	//Set colors for main slider
	_element->texture_color.color_states[W_GUI_STATE_NORMAL] = this->_slider_main_color;
	_element->texture_color.color_states[W_GUI_STATE_MOUSEOVER] = this->_slider_main_mouse_over_color;
	_element->texture_color.color_states[W_GUI_STATE_PRESSED] = this->_slider_main_pressed_color;
	_element->texture_color.color_states[W_GUI_STATE_FOCUS] = this->_slider_main_focused_color;
	_element->texture_color.color_states[W_GUI_STATE_DISABLED] = this->_slider_main_disabled_color;

	// Blend current color
	_element->texture_color.blend(_state, pElapsedTime, fBlendRate);
	_super::parent_widget->draw_sprite(_element, &(_super::boundingBox), W_GUI_FAR_BUTTON_DEPTH);

	_element = this->elements[1];

	//Set colors for button slider
	_element->texture_color.color_states[W_GUI_STATE_NORMAL] = this->_slider_button_color;
	_element->texture_color.color_states[W_GUI_STATE_MOUSEOVER] = this->_slider_button_mouse_over_color;
	_element->texture_color.color_states[W_GUI_STATE_PRESSED] = this->_slider_button_pressed_color;
	_element->texture_color.color_states[W_GUI_STATE_FOCUS] = this->_slider_button_focused_color;
	_element->texture_color.color_states[W_GUI_STATE_DISABLED] = this->_slider_button_disabled_color;

	// Blend current color
	_element->texture_color.blend(_state, pElapsedTime, fBlendRate);
	_super::parent_widget->draw_sprite(_element, &this->_rectangle_button, W_GUI_NEAR_BUTTON_DEPTH);
}
