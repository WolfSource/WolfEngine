#include "w_gui_pch.h"
#include "w_label.h"
#include "w_widget.h"

using namespace wolf::gui;

w_label::w_label(_In_ w_widget* pParent) : _super(pParent),
force_use_current_color_state(false),
original_text_offset_x(0),
original_text_offset_y(0),
render_from_child(false),
label_color(w_color::WHITE),
label_mouse_over_color(w_color::BLACK),
label_pressed_color(w_color::BLACK),
label_focused_color(w_color::BLACK),
label_disabled_color(RGBA_TO_HEX_COLOR(50, 50, 50, 150))
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_LABEL;

	std::memset(&this->text, 0, sizeof(this->text));

	for (auto it = _super::elements.begin(); it != _super::elements.end(); ++it)
	{
		auto _element = *it;
		SAFE_DELETE(_element);
	}

	this->text_offset_x = this->original_text_offset_x;
	this->text_offset_y = this->original_text_offset_y;

	_super::elements.clear();
}

void w_label::render(_In_ const float pElapsedTime)
{
	if (_super::visible == false) return;

	//position
	int _x, _y;
	_super::parent_widget->get_position(_x, _y);
	//auto _pos = DirectX::XMFLOAT2(float(_x + _super::x +  this->text_offset_x), float(_y + _super::y +  this->text_offset_y));

	////color
	//D2D1_COLOR_F _font_color;
	//if (force_use_current_color_state)
	//{
	//	_font_color.r = static_cast<float>(this->current_color_state.r);
	//	_font_color.g = static_cast<float>(this->current_color_state.g);
	//	_font_color.b = static_cast<float>(this->current_color_state.b);
	//	_font_color.a = static_cast<float>(this->current_color_state.a);
	//}
	//else
	//{
	//	W_GUI_CONTROL_STATE _state = W_GUI_STATE_NORMAL;

	//	if (_super::enabled == false)
	//	{
	//		_state = W_GUI_STATE_DISABLED;
	//	}

	//	//blend color
	//	auto _element = _super::elements[0];

	//	_element->font_color.color_states[W_GUI_STATE_NORMAL] = this->label_color;
	//	_element->font_color.color_states[W_GUI_STATE_MOUSEOVER] = this->label_mouse_over_color;
	//	_element->font_color.color_states[W_GUI_STATE_PRESSED] = this->label_pressed_color;
	//	_element->font_color.color_states[W_GUI_STATE_FOCUS] = this->label_focused_color;
	//	_element->font_color.color_states[W_GUI_STATE_DISABLED] = this->label_disabled_color;
	//	
	//	//if this method, called by child, such as w_button or etc, so allow blend on mouse over
	//	if (!this->render_from_child)
	//	{
	//		_element->font_color.blend(_state, pElapsedTime);
	//	}

	//	_font_color.r = _element->font_color.current_color_state.x;
	//	_font_color.g = _element->font_color.current_color_state.y;
	//	_font_color.b = _element->font_color.current_color_state.z;
	//	_font_color.a = _element->font_color.current_color_state.w;
	//}

	////if opacity is zero, skip draw text
	//if (_font_color.a == 0) return;

	////if color is changed, then create new direct2D brush
	//if (this->font_color != _font_color)
	//{
	//	this->font_color = _font_color;

	//	if (this->brush != nullptr)
	//	{
	//		COMPTR_RELEASE(this->brush);
	//	}
	//	auto _hr = pGDevice->context_2D->CreateSolidColorBrush(this->font_color, this->brush.GetAddressOf());
	//	V(_hr, L"Setting color of font", this->name, 2, false, true);
	//}

	//_super::parent_widget->draw_text(this->text, _pos, this->brush.Get());
}

ULONG w_label::release()
{
	if (_super::get_is_released()) return 0;
	
	text.clear();

	return _super::release();
}

bool w_label::contains_point(_In_ const w_point& pPoint)
{
	W_UNUSED(pPoint);
	return false;
}

