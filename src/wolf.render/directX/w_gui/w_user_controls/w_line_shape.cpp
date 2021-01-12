#include "w_directX_pch.h"
#include "w_line_shape.h"
#include "w_gui/w_widgets_resource_manager.h"

using namespace wolf::gui;

w_line_shape::w_line_shape(_In_opt_ w_widget* pParent) :
	_super(pParent),
	color(w_color::from_hex(w_color::WHITE)),
	disabled_color(50, 50, 50, 150),
	_line(nullptr)
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_SHAPE;

	this->start_point.x = 0;
	this->start_point.y = 0;

	this->stop_point.x = 100;
	this->stop_point.y = 100;

	this->stroke_width = 1;
}

w_line_shape::~w_line_shape()
{
	release();
}

HRESULT	w_line_shape::on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
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

	D2D1_POINT_2F _start_point;
	_start_point.x = _x + this->start_point.x;
	_start_point.y = _y + this->start_point.y;

	D2D1_POINT_2F _stop_point;
	_stop_point.x = _x + this->stop_point.x;
	_stop_point.y = _y + this->stop_point.y;

	this->_line = new wolf::graphics::direct2D::shapes::w_line(pGDevice, _start_point, _stop_point);
	this->_line->set_stroke_width(this->stroke_width);

	return S_OK;
}

bool w_line_shape::contains_point(_In_ const POINT& pPoint)
{
	return false;// PtInRect(&(_super::boundingBox), pPoint) != 0;
}

bool w_line_shape::can_have_focus()
{
	return false;
}

void w_line_shape::render(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (!this->visible) return;

	W_GUI_CONTROL_STATE _control_state = W_GUI_STATE_NORMAL;

	if (!_super::enabled)
	{
		_control_state = W_GUI_STATE_DISABLED;
		this->_line->set_color(this->disabled_color);
	}
	else
	{
		this->_line->set_color(this->color);
	}


	if (this->parent_widget)
	{
		this->parent_widget->draw_shape(this->_line);
	}
}

ULONG w_line_shape::release()
{
	if (_super::get_is_released()) return 0;
	
	SAFE_RELEASE(this->_line);

	_super::release();
}

#pragma region Setters

void w_line_shape::set_geormetry(_In_ const float pStartPointX, _In_ const float pStartPointY,
	_In_ const float pStopPointX, _In_ const float pStopPointY)
{
	this->start_point.x = pStartPointX;
	this->start_point.y = pStartPointY;
	this->stop_point.x = pStopPointX;
	this->stop_point.y = pStopPointY;

	if (!this->_line) return;

	int _x = 0, _y = 0;
	if (parent_widget)
	{
		parent_widget->get_position(_x, _y);
		if (parent_widget->get_is_caption_enabled())
		{
			_y += parent_widget->get_caption_height();
		}
	}

	D2D1_POINT_2F _start_point;
	_start_point.x = _x + this->start_point.x;
	_start_point.y = _y + this->start_point.y;

	D2D1_POINT_2F _stop_point;
	_stop_point.x = _x + this->stop_point.x;
	_stop_point.y = _y + this->stop_point.y;
	
	this->_line->set_geormetry(_start_point, _stop_point);
}

void w_line_shape::set_stroke_width(_In_ const float pStrokeWidth)
{
	this->stroke_width = pStrokeWidth;
	if (this->_line)
	{
		this->_line->set_stroke_width(this->stroke_width);
	}
}

#pragma endregion
