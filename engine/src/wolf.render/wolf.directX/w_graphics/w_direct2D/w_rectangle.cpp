#include "w_directX_pch.h"
#include "w_rectangle.h"

using namespace D2D1;
using namespace wolf::graphics;
using namespace wolf::graphics::direct2D::shapes;

w_rectangle::w_rectangle(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ float pLeft, _In_ float pTop, 
	_In_ float pWidth, _In_ float pHeight, 
	_In_ float pRadiusX, _In_ float pRadiusY) : 
	_gDevice(pGDevice), 
	_stroke_width(1.0f),
	_update_color(false), 
	_update_border_color(false)
{
	_super::set_class_name(typeid(this).name());

	this->_color.r = 0.274f; this->_color.g = 0.274f; this->_color.b = 0.274f; this->_color.a = 1;
	this->_border_color.r = 1; this->_border_color.g = 1; this->_border_color.b = 1; this->_border_color.a = 1;

	//Initialize rectangle
	this->_rectangle.radiusX = pRadiusX;
	this->_rectangle.radiusY = pRadiusY;
	this->_rectangle.rect.left = pLeft;
	this->_rectangle.rect.top = pTop;
	this->_rectangle.rect.right = this->_rectangle.rect.left + pWidth;
	this->_rectangle.rect.bottom = this->_rectangle.rect.top + pHeight;
}


w_rectangle::~w_rectangle()
{
	release();
}

HRESULT w_rectangle::draw()
{
	if (this->_brush == nullptr || this->_update_color)
	{
		auto hr = this->_gDevice->context_2D->CreateSolidColorBrush(this->_color, &this->_brush);
		if (FAILED(hr))
		{
			V(hr, L"Create solid color brush for background", this->name, false, true);
			return hr;
		}
		this->_update_color = false;
	}

	if (this->_border_brush == nullptr || this->_update_border_color)
	{
		auto hr = this->_gDevice->context_2D->CreateSolidColorBrush(this->_border_color, &this->_border_brush);
		if (FAILED(hr))
		{
			V(hr, L"Create solid color brush for border", this->name, false, true);
			return hr;
		}
		this->_update_border_color = false;
	}

	if (this->_rectangle.radiusX == 0 && this->_rectangle.radiusY == 0)
	{
		this->_gDevice->context_2D->DrawRectangle(&this->_rectangle.rect, this->_border_brush.Get(), this->_stroke_width, 0);
		this->_gDevice->context_2D->FillRectangle(&this->_rectangle.rect, this->_brush.Get());
	}
	else
	{
		this->_gDevice->context_2D->DrawRoundedRectangle(&this->_rectangle, this->_border_brush.Get(), this->_stroke_width, 0);
		this->_gDevice->context_2D->FillRoundedRectangle(&this->_rectangle, this->_brush.Get());
	}

	return S_OK;
}

ULONG w_rectangle::release()
{
	if (_super::get_is_released()) return 0;
	
	COMPTR_RELEASE(this->_brush);
	COMPTR_RELEASE(this->_border_brush);

	this->_gDevice = nullptr;

	return _super::release();
}

#pragma region Getters

w_color w_rectangle::get_color() const
{
	return w_color(
		this->_color.r * 255.000f,
		this->_color.g * 255.000f,
		this->_color.b * 255.000f,
		this->_color.a * 255.000f);
}

w_color w_rectangle::get_border_color() const
{
	return w_color(
		this->_border_color.r * 255.000f,
		this->_border_color.g * 255.000f,
		this->_border_color.b * 255.000f,
		this->_border_color.a * 255.000f);
}

float w_rectangle::get_left() const
{
	return this->_rectangle.rect.left;
}

float w_rectangle::get_top() const
{
	return this->_rectangle.rect.top;
}

float w_rectangle::get_width() const
{
	return this->_rectangle.rect.right - this->_rectangle.rect.left;
}

float w_rectangle::get_height() const
{
	return this->_rectangle.rect.top - this->_rectangle.rect.bottom;
}

D2D1_POINT_2F w_rectangle::get_rectangle_corner_radius() const
{
	D2D1_POINT_2F _radius;
	_radius.x = this->_rectangle.radiusX;
	_radius.y = this->_rectangle.radiusY;

	return _radius;
}

#pragma endregion

#pragma region Setters

void w_rectangle::set_color(_In_ const w_color pColor)
{
	this->_color.r = pColor.r / 255.000f;
	this->_color.g = pColor.g / 255.000f;
	this->_color.b = pColor.b / 255.000f;
	this->_color.a = pColor.a / 255.000f;

	this->_update_color = true;
}

void w_rectangle::set_border_color(_In_ const w_color pColor)
{
	this->_border_color.r = pColor.r / 255.000f;
	this->_border_color.g = pColor.g / 255.000f;
	this->_border_color.b = pColor.b / 255.000f;
	this->_border_color.a = pColor.a / 255.000f;

	this->_update_border_color = true;
}

void w_rectangle::set_geormetry(_In_ const float pLeft, _In_ const float pTop, 
	_In_ float const pWidth, _In_ const float pHeight, 
	_In_ float const pRadiusX, _In_ const float pRadiusY)
{
	this->_rectangle.radiusX = pRadiusX;
	this->_rectangle.radiusY = pRadiusY;
	this->_rectangle.rect.left = pLeft;
	this->_rectangle.rect.top = pTop;
	this->_rectangle.rect.right = this->_rectangle.rect.left + pWidth;
	this->_rectangle.rect.bottom = this->_rectangle.rect.top + pHeight;
}

#pragma endregion