#include "w_directX_pch.h"
#include "w_button.h"
#include "w_gui/w_widgets_resource_manager.h"

using namespace wolf::gui;
using namespace wolf::graphics;

w_button::w_button(_In_opt_ w_widget* pParent) : _super(pParent),
pressed(false),
icon_offset_x(0),
icon_offset_y(0),
icon_scale_x(1.0f),
icon_scale_y(1.0f),
effect_min_value(0.0f),
effect_max_value(1.0f),
button_normal_color(w_color::GRAY),
button_pressed_color(RGBA_TO_HEX_COLOR(255, 255, 255, 200)),
button_mouse_over_color(w_color::WHITE),
button_focused_color(RGBA_TO_HEX_COLOR(255, 255, 255, 200)),
button_disabled_color(RGBA_TO_HEX_COLOR(50, 50, 50, 150))
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_BUTTON;
	_super::hot_key = 0;
	_super::parent_widget = pParent;
	//we are rendering label from button
	_super::render_from_child = true;
	//Set the offset of label
	_super::original_text_offset_x = -10;
	_super::original_text_offset_y = -10;
}

w_button::~w_button()
{
}

_Use_decl_annotations_
HRESULT w_button::on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
	if (!this->icon_path.empty())
	{
		this->icon = std::make_unique<w_image>(_super::parent_widget);
		this->icon->set_path(this->icon_path.c_str());
	}

	return S_OK;
}

_Use_decl_annotations_
bool w_button::handle_keyboard(UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	UNREFERENCED_PARAMETER(pLParam);

	if (!_super::enabled || !this->visible) return false;

	switch (pMsg)
	{
		case WM_KEYDOWN:
		{
			switch (pWParam)
			{
			case VK_SPACE:
				this->pressed = true;
				return true;
			}
		}
		case WM_KEYUP:
		{
			switch (pWParam)
			{
			case VK_SPACE:
				if (this->pressed == true)
				{
					this->pressed = false;
					(_super::parent_widget)->send_event(W_GUI_EVENT_BUTTON_CLICKED, true, this);
				}
				return true;
			}
		}
	}
	return false;
}

_Use_decl_annotations_
bool w_button::handle_mouse(UINT pMsg, const POINT& pPoint, WPARAM pWParam, LPARAM pLParam)
{
	UNREFERENCED_PARAMETER(pWParam);
	UNREFERENCED_PARAMETER(pLParam);

	if (!_super::enabled || !this->visible) return false;

	switch (pMsg)
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			if (contains_point(pPoint))
			{
				// Pressed while inside the control
				this->pressed = true;
				if (parent_widget)
				{
					auto _hwnd = parent_widget->get_hwnd();
					SetCapture(_hwnd);
				}

				if (!this->has_focus)
				{
					(_super::parent_widget)->request_focus(this);
				}

				return true;
			}
			break;
		}
		case WM_LBUTTONUP:
		{
			if (this->pressed)
			{
				this->pressed = false;
				ReleaseCapture();

				if (!_super::parent_widget->keyboard_input)
				{
					_super::parent_widget->clear_focus();
				}

				// Button click
				if (contains_point(pPoint))
				{
					(_super::parent_widget)->send_event(W_GUI_EVENT_BUTTON_CLICKED, true, this);
				}

				return true;
			}

			break;
		}
	};

	return false;
}

void w_button::on_hot_key()
{
	if (_super::parent_widget->get_is_keyboard_input_enabled())
	{
		_super::parent_widget->request_focus(this);
	}
	_super::parent_widget->send_event(W_GUI_EVENT_BUTTON_CLICKED, true, this);
}

bool w_button::contains_point(_In_ const POINT& pt)
{
	return PtInRect(&(_super::boundingBox), pt) != 0;
}

bool w_button::can_have_focus()
{
	return (this->visible && _super::enabled);
}

void w_button::render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (this->visible == false) return;

	int _offsetX = 0;
	int _offsetY = 0;

	W_GUI_CONTROL_STATE _control_state = W_GUI_STATE_NORMAL;

	if (!this->visible)
	{
		_control_state = W_GUI_STATE_HIDDEN;
	}
	else if (!_super::enabled)
	{
		_control_state = W_GUI_STATE_DISABLED;
	}
	else if (this->pressed)
	{
		_control_state = W_GUI_STATE_PRESSED;

		_offsetX = 1;
		_offsetY = 2;
	}
	else if (this->mouse_over)
	{
		_control_state = W_GUI_STATE_MOUSEOVER;

		_offsetX = -1;
		_offsetY = -2;
	}
	else if (this->has_focus)
	{
		_control_state = W_GUI_STATE_FOCUS;
	}

	auto _width_over_2 = (_super::width / 2);
	auto _height_over_2 = (_super::height / 2);

	_super::text_offset_x = _width_over_2 + _super::original_text_offset_x + _offsetX;
	_super::text_offset_y = _height_over_2 + _super::original_text_offset_y + _offsetY;

	// Background fill layer
	auto _element = _super::elements[0];

	//Set the colors of button
	_element->texture_color.color_states[W_GUI_STATE_NORMAL] = this->button_normal_color;
	_element->texture_color.color_states[W_GUI_STATE_MOUSEOVER] = this->button_mouse_over_color;
	_element->texture_color.color_states[W_GUI_STATE_PRESSED] = this->button_pressed_color;
	_element->texture_color.color_states[W_GUI_STATE_FOCUS] = this->button_focused_color;
	_element->texture_color.color_states[W_GUI_STATE_DISABLED] = this->button_disabled_color;

	//Set the colors of font
	_element->font_color.color_states[W_GUI_STATE_NORMAL] = _super::_label_color;
	_element->font_color.color_states[W_GUI_STATE_MOUSEOVER] = _super::_label_mouse_over_color;
	_element->font_color.color_states[W_GUI_STATE_PRESSED] = _super::_label_pressed_color;
	_element->font_color.color_states[W_GUI_STATE_FOCUS] = _super::_label_focused_color;
	_element->font_color.color_states[W_GUI_STATE_DISABLED] = _super::_label_disabled_color;

	float _blend_rate = (_control_state == W_GUI_STATE_PRESSED) ? 0.0f : 0.8f;

	RECT _rect = _super::boundingBox;
	OffsetRect(&_rect, _offsetX, _offsetY);

	//Blend current color of texture
	_element->texture_color.blend(_control_state, pElapsedTime, _blend_rate);
	//Blend current color of font
	_element->font_color.blend(_control_state, pElapsedTime, _blend_rate);

	_super::parent_widget->draw_sprite(_element, &_rect, W_GUI_FAR_BUTTON_DEPTH);

	//Main button
	_element = this->elements[1];

	//Blend current color of texture
	_element->texture_color.blend(_control_state, pElapsedTime, _blend_rate);
	//Blend current color of font
	//_element->font_color.blend(_control_state, pElapsedTime, _blend_rate);

	_super::parent_widget->draw_sprite(_element, &_rect, W_GUI_NEAR_BUTTON_DEPTH);

	//Draw label
	_super::render(pGDevice, pElapsedTime);

	//Draw icon
	if (this->icon)
	{
		UINT _x, _y;
		_super::parent_widget->get_position(_x, _y);
		this->icon->set_position(
			_super::x + this->icon_offset_x + _offsetX,
			_super::y + this->icon_offset_y + _offsetY);
		this->icon->set_scale(DirectX::XMFLOAT2(this->icon_scale_x, this->icon_scale_y));
		this->icon->set_guasian_effect_value(_super::text_offset_x == -11 ? this->effect_max_value : this->effect_min_value);
		this->icon->render(pGDevice, pElapsedTime);
	}
}
