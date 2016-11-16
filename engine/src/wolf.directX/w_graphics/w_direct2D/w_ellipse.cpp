#include "w_directX_pch.h"
#include "w_ellipse.h"

using namespace D2D1;
using namespace wolf::graphics;
using namespace wolf::graphics::direct2D::shapes;

w_ellipse::w_ellipse(const std::shared_ptr<w_graphics_device>& pGDevice,
	float pCenterX, float pCenterY, float pRadiusX, float pRadiusY) : _stroke_width(1.0f), _update_color(false), _update_border_color(false)
{
	_super::set_class_name(typeid(this).name());

	this->_gDevice = pGDevice;
	this->_color.r = 0.274f; this->_color.g = 0.274f; this->_color.b = 0.274f; this->_color.a = 1;
	this->_border_color.r = 1; this->_border_color.g = 1; this->_border_color.b = 1; this->_border_color.a = 1;

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
	if (this->_brush == nullptr || this->_update_color)
	{
		auto hr = this->_gDevice->context_2D->CreateSolidColorBrush(this->_color, &this->_brush);
		if (FAILED(hr))
		{
			V(hr, L"Create solid color brush for background", this->name, 3, false, true);
			return hr;
		}
		this->_update_color = false;
	}

	if (this->_border_brush == nullptr || this->_update_border_color)
	{
		auto hr = this->_gDevice->context_2D->CreateSolidColorBrush(this->_border_color, &this->_border_brush);
		if (FAILED(hr))
		{
			V(hr, L"Create solid color brush for border", this->name, 3, false, true);
			return hr;
		}
		this->_update_border_color = false;
	}

	this->_gDevice->context_2D->DrawEllipse(&this->_ellipse, this->_border_brush.Get(), this->_stroke_width, 0);
	this->_gDevice->context_2D->FillEllipse(&this->_ellipse, this->_brush.Get());

	return S_OK;
}

ULONG w_ellipse::release()
{
	if (_super::is_released()) return 0;

	COMPTR_RELEASE(this->_brush);
	COMPTR_RELEASE(this->_border_brush);

	this->_gDevice = nullptr;

	return _super::release();
}

#pragma region Getters

w_color w_ellipse::get_color() const
{
	return w_color(
		this->_color.r * 255.000f, 
		this->_color.g * 255.000f, 
		this->_color.b * 255.000f, 
		this->_color.a * 255.000f);
}

w_color w_ellipse::get_border_color() const
{
	return w_color(
		this->_border_color.r * 255.000f, 
		this->_border_color.g * 255.000f, 
		this->_border_color.b * 255.000f, 
		this->_border_color.a * 255.000f);
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

void w_ellipse::set_color(_In_ const w_color pColor)
{
	this->_color.r = pColor.r / 255.000f; 
	this->_color.g = pColor.g / 255.000f; 
	this->_color.b = pColor.b / 255.000f; 
	this->_color.a = pColor.a / 255.000f;
	
	this->_update_color = true;
}

void w_ellipse::set_border_color(_In_ const w_color pColor)
{
	this->_border_color.r = pColor.r / 255.000f;
	this->_border_color.g = pColor.g / 255.000f;
	this->_border_color.b = pColor.b / 255.000f;
	this->_border_color.a = pColor.a / 255.000f;

	this->_update_border_color = true;
}

void w_ellipse::set_geormetry(_In_ const float pCenterX, _In_ const float pCenterY, _In_ const float pRadiusX, _In_ const float pRadiusY)
{
	this->_ellipse.point.x = pCenterX;
	this->_ellipse.point.y = pCenterY;
	this->_ellipse.radiusX = pRadiusX;
	this->_ellipse.radiusY = pRadiusY;
}

#pragma endregion