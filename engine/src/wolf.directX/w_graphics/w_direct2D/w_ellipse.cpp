#include "w_directX_pch.h"
#include "w_ellipse.h"

using namespace D2D1;
using namespace wolf::graphics;
using namespace wolf::graphics::direct2D::shapes;

w_ellipse::w_ellipse(const std::shared_ptr<w_graphics_device>& pGDevice,
	float pCenterX, float pCenterY, float pRadiusX, float pRadiusY) : _strokeWidth(1.0f), _updateColor(false), _updateBorderColor(false)
{
	_super::set_class_name(typeid(this).name());

	this->_gDevice = pGDevice;
	this->_color.r = 0.274f; this->_color.g = 0.274f; this->_color.b = 0.274f; this->_color.a = 1;
	this->_borderColor.r = 1; this->_borderColor.g = 1; this->_borderColor.b = 1; this->_borderColor.a = 1;

	//Initialize eclipse
	this->_ellipse.point.x = pCenterX;
	this->_ellipse.point.y = pCenterY;
	this->_ellipse.radiusX = pRadiusX;
	this->_ellipse.radiusY = pRadiusY;
}

w_ellipse::~w_ellipse()
{
	release();
}

HRESULT w_ellipse::draw()
{
	if (this->_brush == nullptr || this->_updateColor)
	{
		auto hr = this->_gDevice->context_2D->CreateSolidColorBrush(this->_color, &this->_brush);
		if (FAILED(hr))
		{
			V(hr, L"Create solid color brush for background", this->name, 3, false, true);
			return hr;
		}
		this->_updateColor = false;
	}

	if (this->_borderBrush == nullptr || this->_updateBorderColor)
	{
		auto hr = this->_gDevice->context_2D->CreateSolidColorBrush(this->_borderColor, &this->_borderBrush);
		if (FAILED(hr))
		{
			V(hr, L"Create solid color brush for border", this->name, 3, false, true);
			return hr;
		}
		this->_updateBorderColor = false;
	}

	this->_gDevice->context_2D->DrawEllipse(&this->_ellipse, this->_borderBrush.Get(), this->_strokeWidth, 0);
	this->_gDevice->context_2D->FillEllipse(&this->_ellipse, this->_brush.Get());

	return S_OK;
}

ULONG w_ellipse::release()
{
	if (_super::is_released()) return 0;

	COMPTR_RELEASE(this->_brush);
	COMPTR_RELEASE(this->_borderBrush);

	this->_gDevice = nullptr;

	return _super::release();
}

#pragma region Getters

ColorF w_ellipse::get_color() const
{
	return ColorF(this->_color.r, this->_color.g, this->_color.b, this->_color.a);
}

ColorF w_ellipse::get_borderColor() const
{
	return ColorF(this->_borderColor.r, this->_borderColor.g, this->_borderColor.b, this->_borderColor.a);
}

D2D1_POINT_2F w_ellipse::get_radius() const
{
	D2D1_POINT_2F r;

	r.x = this->_ellipse.radiusX;
	r.y = this->_ellipse.radiusY;

	return r;
}

D2D1_POINT_2F w_ellipse::get_center() const
{
	return this->_ellipse.point;
}

#pragma endregion

#pragma region Setters

void w_ellipse::set_color(D2D1::ColorF pColor)
{
	this->_color.r = pColor.r; this->_color.g = pColor.g; this->_color.b = pColor.b; this->_color.a = pColor.a;
	this->_updateColor = true;
}

void w_ellipse::set_borderColor(D2D1::ColorF pColor)
{
	this->_borderColor.r = pColor.r; this->_borderColor.g = pColor.g; this->_borderColor.b = pColor.b; this->_borderColor.a = pColor.a;
	this->_updateBorderColor = true;
}

void w_ellipse::set_geormetry(float pCenterX, float pCenterY, float pRadiusX, float pRadiusY)
{
	this->_ellipse.point.x = pCenterX;
	this->_ellipse.point.y = pCenterY;
	this->_ellipse.radiusX = pRadiusX;
	this->_ellipse.radiusY = pRadiusY;
}

#pragma endregion