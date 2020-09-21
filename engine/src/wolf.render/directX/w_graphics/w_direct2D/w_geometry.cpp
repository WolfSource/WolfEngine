#include "w_directX_pch.h"
#include "w_geometry.h"
//#include "w_geometry_sink_read_data.h"

using namespace D2D1;
using namespace Microsoft::WRL;
using namespace wolf::graphics;
using namespace wolf::graphics::direct2D::shapes;

#define MISSING_BEGIN_CALL "Begin must be called successfully before a method can be called."
#define MISSING_END_CALL   "Begin cannot be called again until End has been successfully called."

w_geometry::w_geometry(const std::shared_ptr<w_graphics_device>& pGDevice) : 
	_gDevice(pGDevice), 
	_stroke_width(1.0f),
	_fill_geometry(true),
	_update_color(false), 
	_update_border_color(false), 
	_state(ENDED)
{
	_super::set_class_name(typeid(this).name());

	this->_color.r = 0.274f; this->_color.g = 0.274f; this->_color.b = 0.274f; this->_color.a = 1;
	this->_border_color.r = 1; this->_border_color.g = 1; this->_border_color.b = 1; this->_border_color.a = 1;

	//Create geometry path
	auto _hr = this->_gDevice->factory_2D->CreatePathGeometry(&this->_path_geometry);
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
	auto _hr = this->_path_geometry->Open(&_sink);
	if (FAILED(_hr))
	{
		V(_hr, L"opening sink for adding figure", this->name, false, true);
		return;
	}
}

HRESULT w_geometry::set_style(D2D1_STROKE_STYLE_PROPERTIES pStyle)
{
	auto _hr = this->_gDevice->factory_2D->CreateStrokeStyle(pStyle, nullptr, 0, &this->_style);
	if (FAILED(_hr))
	{
		V(_hr, L"opening style for geomwtry", this->name, false, true);
		return S_FALSE;
	}
	return S_OK;
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

//auto _data = new wolf::graphics::direct2D::w_geometry_sink_read_data();
HRESULT w_geometry::draw()
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
	
	this->_gDevice->context_2D->DrawGeometry(this->_path_geometry.Get(), this->_border_brush.Get(), this->_stroke_width, this->_style.Get());

	if (this->_fill_geometry)
	{
		this->_gDevice->context_2D->FillGeometry(this->_path_geometry.Get(), this->_brush.Get());
	}

	//auto _hr = this->_path_geometry->Simplify(
	//	D2D1_GEOMETRY_SIMPLIFICATION_OPTION_CUBICS_AND_LINES, // This causes any curves to be flattened into line segments.
	//	NULL, // world transform
	//	_data
	//);
	//if (SUCCEEDED(_hr))
	//{
	//	_data->Close();
	//}

	return S_OK;
}

ULONG w_geometry::release()
{
	if (_super::get_is_released()) return 0;

	COMPTR_RELEASE(this->_style);
	COMPTR_RELEASE(this->_sink);
	COMPTR_RELEASE(this->_path_geometry);
	COMPTR_RELEASE(this->_brush);
	COMPTR_RELEASE(this->_border_brush);
	
	this->_gDevice = nullptr;
	
	return w_object::release();
}

#pragma region Getters

w_color w_geometry::get_color() const
{
	return w_color(
		this->_color.r * 255.000f, 
		this->_color.g * 255.000f, 
		this->_color.b * 255.000f, 
		this->_color.a * 255.000f);
}

w_color w_geometry::get_border_color() const
{
	return w_color(
		this->_border_color.r * 255.000f, 
		this->_border_color.g * 255.000f, 
		this->_border_color.b * 255.000f, 
		this->_border_color.a * 255.000f);
}

#pragma endregion

#pragma region Setters

void w_geometry::set_color(w_color pColor)
{
	this->_color.r = pColor.r / 255.000f; 
	this->_color.g = pColor.g / 255.000f; 
	this->_color.b = pColor.b / 255.000f; 
	this->_color.a = pColor.a / 255.000f;
	
	this->_update_color = true;
}

void w_geometry::set_border_color(w_color pColor)
{
	this->_border_color.r = pColor.r / 255.000f; 
	this->_border_color.g = pColor.g / 255.000f; 
	this->_border_color.b = pColor.b / 255.000f; 
	this->_border_color.a = pColor.a / 255.000f;
	
	this->_update_border_color = true;
}

#pragma endregion