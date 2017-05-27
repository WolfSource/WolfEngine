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

void w_label::render(_In_ const float& pElapsedTime, _In_ std::vector<w_gui_vertex>& pVertexDeclarations)
{
    if (_super::visible == false) return;

    //position
    int _x, _y;
    _super::parent_widget->get_position(_x, _y);

    this->position_x = float(_x + _super::x + this->text_offset_x);
    this->position_y = float(_y + _super::y + this->text_offset_y);

    //color
    if (force_use_current_color_state)
    {
        this->font_color = this->current_color_state;
    }
    else
    {
        W_GUI_CONTROL_STATE _state = W_GUI_STATE_NORMAL;
        if (_super::enabled == false)
        {
            _state = W_GUI_STATE_DISABLED;
        }

        //blend color
        auto _element = _super::elements[0];

        _element->font_color.color_states[W_GUI_STATE_NORMAL] = this->label_color;
        _element->font_color.color_states[W_GUI_STATE_MOUSEOVER] = this->label_mouse_over_color;
        _element->font_color.color_states[W_GUI_STATE_PRESSED] = this->label_pressed_color;
        _element->font_color.color_states[W_GUI_STATE_FOCUS] = this->label_focused_color;
        _element->font_color.color_states[W_GUI_STATE_DISABLED] = this->label_disabled_color;

        //if this method, called by child, such as w_button or etc, so allow blend on mouse over
        if (!this->render_from_child)
        {
            _element->font_color.blend(_state, pElapsedTime);
        }

        this->font_color = _element->font_color.current_color_state;
    }

    //TODO: create all vertices for text
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

