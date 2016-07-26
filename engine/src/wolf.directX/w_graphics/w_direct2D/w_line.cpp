#include "w_directX_pch.h"
#include "w_line.h"

using namespace D2D1;
using namespace wolf::graphics;
using namespace wolf::graphics::direct2D::shapes;

w_line::w_line(const std::shared_ptr<w_graphics_device>& pGDevice, D2D1_POINT_2F pStartPoint, D2D1_POINT_2F pStopPoint) : _gDevice(pGDevice), _startPoint(pStartPoint), _stopPoint(pStopPoint),
_strokeWidth(1.0f), _updateColor(false), _updateBorderColor(false)
{
	_super::set_class_name(typeid(this).name());

	this->_color.r = 0.274f; this->_color.g = 0.274f; this->_color.b = 0.274f; this->_color.a = 1;
	this->_borderColor.r = 1; this->_borderColor.g = 1; this->_borderColor.b = 1; this->_borderColor.a = 1;
}

w_line::~w_line()
{
	release();
}

HRESULT w_line::draw()
{
	if (this->_brush == nullptr || this->_updateColor)
	{
		auto _hr = this->_gDevice->context_2D->CreateSolidColorBrush(this->_color, &this->_brush);
		if (FAILED(_hr))
		{
			V(_hr, L"Create solid color brush for background", this->name, false, true);
			return _hr;
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

	this->_gDevice->context_2D->DrawLine(this->_startPoint, this->_stopPoint, this->_borderBrush.Get(), this->_strokeWidth, 0);

	return S_OK;
}

ULONG w_line::release()
{
	if (_super::is_released()) return 0;
	
	COMPTR_RELEASE(this->_brush);
	COMPTR_RELEASE(this->_borderBrush);

	this->_gDevice = nullptr;

	return _super::release();
}

#pragma region Getters

D2D1_POINT_2F w_line::get_startPoint() const
{
	return this->_startPoint;
}

D2D1_POINT_2F w_line::get_stopPoint() const
{
	return this->_stopPoint;
}

#pragma endregion

#pragma region Setters

void w_line::set_color(D2D1::ColorF pColor)
{
	this->_color.r = pColor.r; this->_color.g = pColor.g; this->_color.b = pColor.b; this->_color.a = pColor.a;
	this->_updateColor = true;
}

void w_line::set_borderColor(D2D1::ColorF pColor)
{
	this->_borderColor.r = pColor.r; this->_borderColor.g = pColor.g; this->_borderColor.b = pColor.b; this->_borderColor.a = pColor.a;
	this->_updateBorderColor = true;
}

void w_line::set_geormetry(D2D1_POINT_2F pStartPoint, D2D1_POINT_2F pStopPoint)
{
	this->_startPoint = pStartPoint;
	this->_stopPoint = pStopPoint;
}

#pragma endregion