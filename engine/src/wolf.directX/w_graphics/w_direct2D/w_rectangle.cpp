#include "w_directX_pch.h"
#include "w_rectangle.h"

using namespace D2D1;
using namespace wolf::graphics;
using namespace wolf::graphics::direct2D::shapes;

w_rectangle::w_rectangle(const std::shared_ptr<w_graphics_device>& pGDevice,
	float pLeft, float pTop, float pWidth, float pHeight, float pRadiusX, float pRadiusY) : _gDevice(pGDevice), _strokeWidth(1.0f),
	_updateColor(false), _updateBorderColor(false)
{
	_super::set_class_name(typeid(this).name());

	this->_color.r = 0.274f; this->_color.g = 0.274f; this->_color.b = 0.274f; this->_color.a = 1;
	this->_borderColor.r = 1; this->_borderColor.g = 1; this->_borderColor.b = 1; this->_borderColor.a = 1;

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
	if (this->_brush == nullptr || this->_updateColor)
	{
		auto hr = this->_gDevice->context_2D->CreateSolidColorBrush(this->_color, &this->_brush);
		if (FAILED(hr))
		{
			V(hr, L"Create solid color brush for background", this->name, false, true);
			return hr;
		}
		this->_updateColor = false;
	}

	if (this->_borderBrush == nullptr || this->_updateBorderColor)
	{
		auto hr = this->_gDevice->context_2D->CreateSolidColorBrush(this->_borderColor, &this->_borderBrush);
		if (FAILED(hr))
		{
			V(hr, L"Create solid color brush for border", this->name, false, true);
			return hr;
		}
		this->_updateBorderColor = false;
	}

	if (this->_rectangle.radiusX == 0 && this->_rectangle.radiusY == 0)
	{
		this->_gDevice->context_2D->DrawRectangle(&this->_rectangle.rect, this->_borderBrush.Get(), this->_strokeWidth, 0);
		this->_gDevice->context_2D->FillRectangle(&this->_rectangle.rect, this->_brush.Get());
	}
	else
	{
		this->_gDevice->context_2D->DrawRoundedRectangle(&this->_rectangle, this->_borderBrush.Get(), this->_strokeWidth, 0);
		this->_gDevice->context_2D->FillRoundedRectangle(&this->_rectangle, this->_brush.Get());
	}

	return S_OK;
}

ULONG w_rectangle::release()
{
	if (_super::is_released()) return 0;
	
	COMPTR_RELEASE(this->_brush);
	COMPTR_RELEASE(this->_borderBrush);

	this->_gDevice = nullptr;

	return _super::release();
}

#pragma region Getters

ColorF w_rectangle::get_color() const
{
	return ColorF(this->_color.r, this->_color.g, this->_color.b, this->_color.a);
}

ColorF w_rectangle::get_borderColor() const
{
	return ColorF(this->_borderColor.r, this->_borderColor.g, this->_borderColor.b, this->_borderColor.a);
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

float w_rectangle::get_radiusX() const
{
	return this->_rectangle.radiusX;
}

float w_rectangle::get_radiusY() const
{
	return this->_rectangle.radiusY;
}

#pragma endregion

#pragma region Setters

void w_rectangle::set_color(D2D1::ColorF pColor)
{
	this->_color.r = pColor.r; this->_color.g = pColor.g; this->_color.b = pColor.b; this->_color.a = pColor.a;
	this->_updateColor = true;
}

void w_rectangle::set_borderColor(D2D1::ColorF pColor)
{
	this->_borderColor.r = pColor.r; this->_borderColor.g = pColor.g; this->_borderColor.b = pColor.b; this->_borderColor.a = pColor.a;
	this->_updateBorderColor = true;
}

void w_rectangle::set_geormetry(float pLeft, float pTop, float pWidth, float pHeight, float pRadiusX, float pRadiusY)
{
	this->_rectangle.radiusX = pRadiusX;
	this->_rectangle.radiusY = pRadiusY;
	this->_rectangle.rect.left = pLeft;
	this->_rectangle.rect.top = pTop;
	this->_rectangle.rect.right = this->_rectangle.rect.left + pWidth;
	this->_rectangle.rect.bottom = this->_rectangle.rect.top + pHeight;
}

#pragma endregion