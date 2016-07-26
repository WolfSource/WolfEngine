#include "w_directX_pch.h"
#include "w_geometry.h"

using namespace D2D1;
using namespace Microsoft::WRL;
using namespace wolf::graphics;
using namespace wolf::graphics::direct2D::shapes;

#define MISSING_BEGIN_CALL "Begin must be called successfully before a method can be called."
#define MISSING_END_CALL   "Begin cannot be called again until End has been successfully called."

w_geometry::w_geometry(const std::shared_ptr<w_graphics_device>& pGDevice) : _gDevice(pGDevice), _strokeWidth(1.0f),
_updateColor(false), _updateBorderColor(false), _state(ENDED)
{
	_super::set_class_name(typeid(this).name());

	this->_color.r = 0.274f; this->_color.g = 0.274f; this->_color.b = 0.274f; this->_color.a = 1;
	this->_borderColor.r = 1; this->_borderColor.g = 1; this->_borderColor.b = 1; this->_borderColor.a = 1;

	//Create geometry path
	auto _hr = this->_gDevice->factory_2D->CreatePathGeometry(&this->_pathGeometry);
	if (FAILED(_hr))
	{
		V(S_FALSE, L"creating path geometry", this->name);
		return;
	}
}

w_geometry::~w_geometry()
{
	release();
}

void w_geometry::begin()
{
	if (this->_state != ENDED) throw std::exception(MISSING_END_CALL);
	this->_state = STARTED;

	//open geometry sink of geometry path and fill data
	auto _hr = this->_pathGeometry->Open(&_sink);
	if (FAILED(_hr))
	{
		V(_hr, L"opening sink for adding figure", this->name, false, true);
		return;
	}
}

void w_geometry::add_lines(_In_ const D2D1_POINT_2F& pStartPoint,
	_In_ const std::vector<D2D1_POINT_2F> pPoints, 
	D2D1_FILL_MODE pFillMode, 
	D2D1_FIGURE_BEGIN pFigureBegin, 
	D2D1_FIGURE_END pFigureEnd)
{

	_sink->SetFillMode(pFillMode);
	_sink->BeginFigure(pStartPoint, pFigureBegin);
	{
		_sink->AddLines(pPoints.data(), pPoints.size());
	}
	_sink->EndFigure(pFigureEnd);
}

void w_geometry::add_beziers(_In_ const D2D1_POINT_2F& pStartPoint,
	_In_ const std::vector<std::array<D2D1_POINT_2F, 3>> pPoints,
	D2D1_FILL_MODE pFillMode, 
	D2D1_FIGURE_BEGIN pFigureBegin, 
	D2D1_FIGURE_END pFigureEnd)
{
	_sink->SetFillMode(pFillMode);
	_sink->BeginFigure(pStartPoint, pFigureBegin);
	{
		for (auto points : pPoints)
		{
			_sink->AddBezier(D2D1::BezierSegment(points[0], points[1], points[2]));
		}
	}
	_sink->EndFigure(pFigureEnd);
}

void w_geometry::add_arc(_In_ const D2D1_POINT_2F& pStartPoint, 
	_In_ const D2D1_POINT_2F& pPoint,
	_In_ const D2D1_SIZE_F& pSize,
	float pRotationAngle,
	D2D1_SWEEP_DIRECTION sweepDirection,
	D2D1_ARC_SIZE arcSize,
	D2D1_FILL_MODE pFillMode, 
	D2D1_FIGURE_BEGIN pFigureBegin, 
	D2D1_FIGURE_END pFigureEnd)
{
	_sink->SetFillMode(pFillMode);
	_sink->BeginFigure(pStartPoint, pFigureBegin);
	{
		_sink->AddArc(D2D1::ArcSegment(pPoint, pSize, pRotationAngle, sweepDirection, arcSize));
	}
	_sink->EndFigure(pFigureEnd);
}

void w_geometry::end()
{
	if (this->_state != STARTED) throw std::exception(MISSING_BEGIN_CALL);

	auto hr = _sink->Close();
	if (FAILED(hr))
	{
		V(hr, L"closing sink for geometry path", this->name, false, true);
		return;
	}

	this->_state = ENDED;
}

HRESULT w_geometry::draw()
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

	this->_gDevice->context_2D->DrawGeometry(this->_pathGeometry.Get(), this->_borderBrush.Get(), this->_strokeWidth, 0);
	this->_gDevice->context_2D->FillGeometry(this->_pathGeometry.Get(), this->_brush.Get());

	return S_OK;
}

ULONG w_geometry::release()
{
	if (_super::is_released()) return 0;

	COMPTR_RELEASE(this->_pathGeometry);
	COMPTR_RELEASE(this->_brush);
	COMPTR_RELEASE(this->_borderBrush);
	COMPTR_RELEASE(this->_sink);

	this->_gDevice = nullptr;
	
	return w_object::release();
}

#pragma region Getters

ColorF w_geometry::get_color() const
{
	return ColorF(this->_color.r, this->_color.g, this->_color.b, this->_color.a);
}

ColorF w_geometry::get_borderColor() const
{
	return ColorF(this->_borderColor.r, this->_borderColor.g, this->_borderColor.b, this->_borderColor.a);
}

#pragma endregion

#pragma region Setters

void w_geometry::set_color(ColorF pColor)
{
	this->_color.r = pColor.r; this->_color.g = pColor.g; this->_color.b = pColor.b; this->_color.a = pColor.a;
	this->_updateColor = true;
}

void w_geometry::set_borderColor(ColorF pColor)
{
	this->_borderColor.r = pColor.r; this->_borderColor.g = pColor.g; this->_borderColor.b = pColor.b; this->_borderColor.a = pColor.a;
	this->_updateBorderColor = true;
}

#pragma endregion