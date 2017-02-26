#include "w_directX_pch.h"
#include "w_line.h"

using namespace D2D1;
using namespace wolf::graphics;
using namespace wolf::graphics::direct2D::shapes;

w_line::w_line(const std::shared_ptr<w_graphics_device>& pGDevice, D2D1_POINT_2F pStartPoint, D2D1_POINT_2F pStopPoint) : _gDevice(pGDevice), _start_point(pStartPoint), _stop_point(pStopPoint),
_stroke_width(1.0f), _update_color(false), _update_border_color(false)
{
	_super::set_class_name(typeid(this).name());

	this->_color.r = 1.0f; this->_color.g = 1.0f; this->_color.b = 1.0f; this->_color.a = 1.0f;
}

w_line::~w_line()
{
	release();
}

HRESULT w_line::draw()
{
	if (this->_brush == nullptr || this->_update_color)
	{
		auto _hr = this->_gDevice->context_2D->CreateSolidColorBrush(this->_color, &this->_brush);
		if (FAILED(_hr))
		{
			V(_hr, L"Create solid color brush for background", this->name, false, true);
			return _hr;
		}
		this->_update_color = false;
	}

	this->_gDevice->context_2D->DrawLine(this->_start_point, this->_stop_point, this->_brush.Get(), this->_stroke_width, 0);

	return S_OK;
}

ULONG w_line::release()
{
	if (_super::get_is_released()) return 0;
	
	COMPTR_RELEASE(this->_brush);

	this->_gDevice = nullptr;

	return _super::release();
}

#pragma region Getters

w_color w_line::get_color() const
{
	return w_color(
		this->_color.r * 255.000f,
		this->_color.g * 255.000f,
		this->_color.b * 255.000f,
		this->_color.a * 255.000f);
}

D2D1_POINT_2F w_line::get_start_point() const
{
	return this->_start_point;
}

D2D1_POINT_2F w_line::get_stop_point() const
{
	return this->_stop_point;
}

#pragma endregion

#pragma region Setters

void w_line::set_color(_In_ const w_color pColor)
{
	this->_color.r = pColor.r / 255.000f; 
	this->_color.g = pColor.g / 255.000f; 
	this->_color.b = pColor.b / 255.000f; 
	this->_color.a = pColor.a / 255.000f;
	
	this->_update_color = true;
}

void w_line::set_geormetry(_In_ const D2D1_POINT_2F pStartPoint, _In_ const D2D1_POINT_2F pStopPoint)
{
	this->_start_point = pStartPoint;
	this->_stop_point = pStopPoint;
}

#pragma endregion