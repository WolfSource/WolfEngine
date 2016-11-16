#include "w_directX_pch.h"
#include "w_rounded_rectangle_shape.h"
#include "w_gui/w_widgets_resource_manager.h"

using namespace wolf::gui;

w_rounded_rectangle_shape::w_rounded_rectangle_shape(_In_opt_ w_widget* pParent) :
	_super(pParent),
	fill_color(128, 57, 123, 255),
	mouse_over_color(169,75,162,255),
	border_color(w_color::from_hex(w_color::WHITE)),
	disabled_color(50, 50, 50, 150),
	stroke_width(1),
	_rectangle(nullptr)
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_SHAPE;
}

w_rounded_rectangle_shape::~w_rounded_rectangle_shape()
{
	release();
}

HRESULT	w_rounded_rectangle_shape::on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
	int _x = 0, _y = 0;
	if (parent_widget)
	{
		parent_widget->get_position(_x, _y);
		if (parent_widget->get_is_caption_enabled())
		{
			_y += parent_widget->get_caption_height();
		}
	}
	this->_rectangle = new wolf::graphics::direct2D::shapes::w_rectangle(pGDevice, _x + this->x, _y + this->y, this->width, this->height);
	this->_rectangle->set_stroke_width(this->stroke_width);

	return S_OK;
}

bool w_rounded_rectangle_shape::contains_point(_In_ const POINT& pPoint)
{
	return PtInRect(&(_super::bounding_box), pPoint) != 0;
}

bool w_rounded_rectangle_shape::can_have_focus()
{
	return false;
}

void w_rounded_rectangle_shape::render(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (!this->visible) return;

	W_GUI_CONTROL_STATE _control_state = W_GUI_STATE_NORMAL;

	if (!_super::enabled)
	{
		_control_state = W_GUI_STATE_DISABLED;
		this->_rectangle->set_color(this->disabled_color);
	}
	else if (this->mouse_over)
	{
		_control_state = W_GUI_STATE_MOUSEOVER;
		this->_rectangle->set_color(this->mouse_over_color);
	}
	else
	{
		this->_rectangle->set_color(this->fill_color);
	}

	this->_rectangle->set_border_color(this->border_color);


	if (this->parent_widget)
	{
		this->parent_widget->draw_shape(this->_rectangle);
	}
}

ULONG w_rounded_rectangle_shape::release()
{
	if (_super::is_released()) return 0;
	
	SAFE_RELEASE(this->_rectangle);

	_super::release();
}

#pragma region Setters

void w_rounded_rectangle_shape::set_geormetry(_In_ const float pLeft, _In_ const float pTop,
	_In_ float const pWidth, _In_ const float pHeight,
	_In_ float const pRadiusX, _In_ const float pRadiusY)
{
	this->x = pLeft;
	this->y = pTop;
	this->width = pWidth;
	this->height = pHeight;
	this->corner_radius.x = pRadiusX;
	this->corner_radius.y = pRadiusY;

	if (this->_rectangle)
	{
		int _x = 0, _y = 0;
		if (parent_widget)
		{
			parent_widget->get_position(_x, _y);
			if (parent_widget->get_is_caption_enabled())
			{
				_y += parent_widget->get_caption_height();
			}
		}
		this->_rectangle->set_geormetry(_x + this->x, _y + this->y, this->width, this->height, pRadiusX, pRadiusY);
		_super::update_rects();
	}
}


void w_rounded_rectangle_shape::set_stroke_width(_In_ const float pValue)
{
	this->stroke_width = pValue;
	if (this->_rectangle)
	{
		this->_rectangle->set_stroke_width(pValue);
	}
}

#pragma endregion
