#include "w_directX_pch.h"
#include "w_check_box.h"
#include "w_gui/w_widgets_resource_manager.h"

using namespace wolf::gui;

w_check_box::w_check_box(_In_opt_ w_widget* pParent) : _super(pParent),
_check_box_normal_color(w_color::WHITE),
_check_box_pressed_color(RGBA_TO_HEX_COLOR(255, 255, 255, 200)),
_check_box_mouse_over_color(w_color::YELLOW),
_check_box_focused_color(w_color::YELLOW),
_check_box_disabled_color(RGBA_TO_HEX_COLOR(50, 50, 50, 150))
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_CHECKBOX;
	_super::parent_widget = pParent;
	//we are rendering label from checkBox
	_super::render_from_child = true;
	//Set the offset of label
	_super::original_text_offset_x = -56;
	_super::original_text_offset_y = -6;

	this->checked = false;
}

w_check_box::~w_check_box()
{

}

void w_check_box::on_hot_key()
{
	if (_super::parent_widget->get_is_keyboard_input_enabled())
	{
		_super::parent_widget->request_focus(this);
	}
	set_checked_internal(!this->checked, true);
}

_Use_decl_annotations_
bool w_check_box::handle_keyboard(UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	W_UNUSED(pLParam);

	if (!_super::enabled || !_super::visible) return false;

	switch (pMsg)
	{
	case WM_KEYDOWN:
	{
		switch (pWParam)
		{
		case VK_SPACE:
			_super::pressed = true;
			return true;
		}
	}

	case WM_KEYUP:
	{
		switch (pWParam)
		{
		case VK_SPACE:
			if (_super::pressed == true)
			{
				_super::pressed = false;
				set_checked_internal(!this->checked, true);
			}
			return true;
		}
	}
	}
	return false;
}

_Use_decl_annotations_
bool w_check_box::handle_mouse(UINT pMsg, const POINT& pPoint, WPARAM pWParam, LPARAM pLParam)
{
	W_UNUSED(pWParam);
	W_UNUSED(pLParam);

	if (!this->enabled || !this->visible) return false;

	switch (pMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	{
		if (contains_point(pPoint))
		{
			// Pressed while inside the control
			_super::pressed = true;

			auto _hwnd = this->parent_widget->get_hwnd();
			if (_hwnd)
			{
				SetCapture(_hwnd);
			}
			
			if (!_super::has_focus)
			{
				_super::parent_widget->request_focus(this);
			}
			return true;
		}

		break;
	}

	case WM_LBUTTONUP:
	{
		if (_super::pressed)
		{
			_super::pressed = false;
			ReleaseCapture();

			// Button click
			if (contains_point(pPoint))
			{
				set_checked_internal(!this->checked, true);
			}
			return true;
		}

		break;
	}
	};

	return false;
}

_Use_decl_annotations_
void w_check_box::set_checked_internal(bool pChecked, bool pFromInput)
{
	this->checked = pChecked;

	_super::parent_widget->send_event(W_GUI_EVENT_CHECKBOX_CHANGED, pFromInput, this);
}

bool w_check_box::contains_point(_In_ const POINT& pPoint)
{
	return (PtInRect(&(_super::boundingBox), pPoint) || PtInRect(&this->rect_button, pPoint));
}

void w_check_box::update_rects()
{
	w_button::update_rects();

	this->rect_button = _super::boundingBox;

	this->rect_button.right = this->rect_button.left + /*height of rectangle*/(this->rect_button.bottom - this->rect_button.top);

	this->rect_text = _super::boundingBox;
	this->rect_text.left += (int)(1.25f * /*width of rectangle*/(this->rect_button.right - this->rect_button.left));
}

void w_check_box::render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (this->visible == false) return;
	W_GUI_CONTROL_STATE _state = W_GUI_STATE_NORMAL;

	if (this->visible == false)
	{
		_state = W_GUI_STATE_HIDDEN;
	}
	else if (this->enabled == false)
	{
		_state = W_GUI_STATE_DISABLED;
	}
	else if (_super::pressed)
	{
		_state = W_GUI_STATE_PRESSED;
	}
	else if (_super::mouse_over)
	{
		_state = W_GUI_STATE_MOUSEOVER;
	}
	else if (_super::has_focus)
	{
		_state = W_GUI_STATE_FOCUS;
	}
	auto _element = _super::elements[0];

	float _blendRate = (_state == W_GUI_STATE_PRESSED) ? 0.0f : 0.8f;

	//Set colors for box
	_element->texture_color.color_states[W_GUI_STATE_NORMAL] = this->_check_box_normal_color;
	_element->texture_color.color_states[W_GUI_STATE_PRESSED] = this->_check_box_pressed_color;
	_element->texture_color.color_states[W_GUI_STATE_MOUSEOVER] = this->_check_box_mouse_over_color;
	_element->texture_color.color_states[W_GUI_STATE_FOCUS] = this->_check_box_focused_color;
	_element->texture_color.color_states[W_GUI_STATE_DISABLED] = this->_check_box_disabled_color;

	//Set colors for text
	_element->font_color.color_states[W_GUI_STATE_NORMAL] = this->_label_color;
	_element->font_color.color_states[W_GUI_STATE_PRESSED] = this->_label_pressed_color;
	_element->font_color.color_states[W_GUI_STATE_MOUSEOVER] = this->_label_mouse_over_color;
	_element->font_color.color_states[W_GUI_STATE_FOCUS] = this->_label_focused_color;
	_element->font_color.color_states[W_GUI_STATE_DISABLED] = this->_label_disabled_color;

	//Apply blend
	_element->texture_color.blend(_state, pElapsedTime, _blendRate);
	_element->font_color.blend(_state, pElapsedTime, _blendRate);

	_super::parent_widget->draw_sprite(_element, &this->rect_button, W_GUI_NEAR_BUTTON_DEPTH);
	
	if (!this->checked)
	{
		_state = W_GUI_STATE_HIDDEN;
	}
	_element = this->elements[1];

	_element->texture_color.blend(_state, pElapsedTime, _blendRate);
	_super::parent_widget->draw_sprite(_element, &this->rect_button, W_GUI_FAR_BUTTON_DEPTH);

	//Draw label
	w_label::text_offset_x = _super::width / 2 + w_label::original_text_offset_x;
	w_label::text_offset_y = _super::height / 2 + w_label::original_text_offset_y;
	w_label::render(pGDevice, pElapsedTime);

}