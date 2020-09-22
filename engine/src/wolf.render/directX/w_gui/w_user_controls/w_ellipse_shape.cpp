#include "w_directX_pch.h"
#include "w_ellipse_shape.h"
#include "w_gui/w_widgets_resource_manager.h"

using namespace wolf::gui;

w_ellipse_shape::w_ellipse_shape(_In_opt_ w_widget* pParent) :
	_super(pParent),
	fill_color(128, 57, 123, 255),
	mouse_over_color(169, 75, 162, 255),
	border_color(w_color::from_hex(w_color::WHITE)),
	disabled_color(50, 50, 50, 150),
	stroke_width(3),
	_ellipse(nullptr)
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_SHAPE;
}

w_ellipse_shape::~w_ellipse_shape()
{
	release();
}

HRESULT	w_ellipse_shape::on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
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
	this->_ellipse = new wolf::graphics::direct2D::shapes::w_ellipse(pGDevice, _x + this->x, _y + this->y, this->width, this->height);
	this->_ellipse->set_stroke_width(stroke_width);

	return S_OK;
}

bool w_ellipse_shape::contains_point(_In_ const POINT& pPoint)
{
	//check whether point is inside ellipse
	auto _a = pow(pPoint.x - this->x, 2) / pow(this->width, 2);
	auto _b = pow(pPoint.y - this->y, 2) / pow(this->height, 2);

	return (_a + _b) <= 1;
}

bool w_ellipse_shape::can_have_focus()
{
	return false;
}

void w_ellipse_shape::render(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (!this->visible) return;

	W_GUI_CONTROL_STATE _control_state = W_GUI_STATE_NORMAL;

	if (!_super::enabled)
	{
		_control_state = W_GUI_STATE_DISABLED;
		this->_ellipse->set_color(this->disabled_color);
	}
	else if (this->mouse_over)
	{
		_control_state = W_GUI_STATE_MOUSEOVER;
		this->_ellipse->set_color(this->mouse_over_color);
	}
	else
	{
		this->_ellipse->set_color(this->fill_color);
	}

	this->_ellipse->set_border_color(this->border_color);


	if (this->parent_widget)
	{
		this->parent_widget->draw_shape(this->_ellipse);
	}
}

ULONG w_ellipse_shape::release()
{
	if (_super::get_is_released()) return 0;
	
	SAFE_RELEASE(this->_ellipse);

	_super::release();
}

#pragma region Getters

float w_ellipse_shape::get_centerX() const
{
	return this->x;
}

float w_ellipse_shape::get_centerY() const
{
	return this->y;
}

float w_ellipse_shape::get_radiusX() const
{
	return this->width;
}

float w_ellipse_shape::get_radiusY() const
{
	return this->height;
}

#pragma endregion

#pragma region Setters

void w_ellipse_shape::set_geormetry(_In_ const float pCenterX, _In_ const float pCenterY,
	_In_ float const pRadiusX, _In_ const float pRadiusY)
{
	this->x = pCenterX;
	this->y = pCenterY;
	this->width = pRadiusX;
	this->height = pRadiusY;

	if (this->_ellipse)
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
		this->_ellipse->set_geormetry(_x + this->x, _y + this->y, this->width, this->height);
		
		_super::update_rects();
	}
}

void w_ellipse_shape::set_stroke_width(_In_ const float pValue)
{
	this->stroke_width = pValue;
	if (this->_ellipse)
	{
		this->_ellipse->set_stroke_width(pValue);
	}
}

#pragma endregion
