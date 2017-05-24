#include "w_gui_pch.h"
#include "w_image.h"
#include "w_widget.h"

using namespace wolf::gui;

w_image::w_image(_In_opt_ w_widget* pParent) : _super(pParent),
path(L""),
is_absolute_path(false),
use_default_size(true),
rotation(0, 0),
opacity(1.0f),
top_left(w_color(w_color::from_hex(w_color::WHITE))),
down_left(w_color(w_color::from_hex(w_color::WHITE))),
top_right(w_color(w_color::from_hex(w_color::WHITE))),
down_right(w_color(w_color::from_hex(w_color::WHITE)))
{
    _super::set_class_name(typeid(this).name());
    _super::type = W_GUI_CONTROL_IMAGE;
}

w_image::~w_image()
{
	release();
}

void w_image::render_2d(_In_ const float& pElapsedTime, std::vector<w_gui_vertex_2d>& pVertexDeclarations)
{
    if (!_super::visible || !_super::parent_widget) return;

    //generate vertices
    int _parent_x, _parent_y;
    _super::parent_widget->get_position(_parent_x, _parent_y);
    auto _position = glm::vec2(float(_parent_x + _super::x), float(_parent_y + _super::y));

    auto _parent_widget_width = this->parent_widget->get_background_buffer_width();
    auto _parent_widget_height = this->parent_widget->get_background_buffer_height();

    w_color _corner_colors[4] = { top_left , down_left , top_right , down_right };
    w_control::generate_2d_vertices(
        glm::vec2((float)_parent_widget_width, (float)_parent_widget_height),
        glm::vec2((float)_super::width, (float)_super::height),
        _position,
        _corner_colors,
        pVertexDeclarations);
}

ULONG w_image::release()
{
	if (_super::get_is_released()) return 0;

	
	return _super::release();
}

