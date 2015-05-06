#include "Wolf.DirectX.Shared.PCH.h"
#include "W_Geometry.h"

using namespace D2D1;
using namespace Microsoft::WRL;
using namespace Wolf::Graphics;
using namespace Wolf::Graphics::Direct2D::Shapes;

#define MISSING_BEGIN_CALL "Begin must be called successfully before a method can be called."
#define MISSING_END_CALL   "Begin cannot be called again until End has been successfully called."

W_Geometry::W_Geometry(const std::shared_ptr<W_GraphicsDevice>& pGDevice) : gDevice(pGDevice), strokeWidth(1.0f), 
updateColor(false), updateBorderColor(false), state(ENDED)
{
	this->Name = typeid(this).name();
	this->color.r = 0.274f; this->color.g = 0.274f; this->color.b = 0.274f; this->color.a = 1;
	this->borderColor.r = 1; this->borderColor.g = 1; this->borderColor.b = 1; this->borderColor.a = 1;

	//Create geometry path
	auto hr = this->gDevice->factory2D->CreatePathGeometry(&this->pathGeometry);
	if (FAILED(hr))
	{
		OnFailed(S_FALSE, "creating path geometry", this->Name);
		return;
	}
}

W_Geometry::~W_Geometry()
{
	Release();
}

void W_Geometry::Begin()
{
	if (this->state != ENDED) throw std::exception(MISSING_END_CALL);
	this->state = STARTED;

	//open geometry sink of geometry path and fill data
	auto hr = this->pathGeometry->Open(&sink);
	if (FAILED(hr))
	{
		OnFailed(hr, "opening sink for adding figure", this->Name, false, true);
		return;
	}
}

void W_Geometry::AddLines(_In_ const D2D1_POINT_2F& pStartPoint, 
	_In_ const std::vector<D2D1_POINT_2F> pPoints, 
	D2D1_FILL_MODE pFillMode, 
	D2D1_FIGURE_BEGIN pFigureBegin, 
	D2D1_FIGURE_END pFigureEnd)
{

	sink->SetFillMode(pFillMode);
	sink->BeginFigure(pStartPoint, pFigureBegin);
	{
		sink->AddLines(pPoints.data(), pPoints.size());
	}
	sink->EndFigure(pFigureEnd);
}

void W_Geometry::AddBeziers(_In_ const D2D1_POINT_2F& pStartPoint,
	_In_ const std::vector<std::array<D2D1_POINT_2F, 3>> pPoints,
	D2D1_FILL_MODE pFillMode, 
	D2D1_FIGURE_BEGIN pFigureBegin, 
	D2D1_FIGURE_END pFigureEnd)
{
	sink->SetFillMode(pFillMode);
	sink->BeginFigure(pStartPoint, pFigureBegin);
	{
		for (auto points : pPoints)
		{
			sink->AddBezier(D2D1::BezierSegment(points[0], points[1], points[2]));
		}
	}
	sink->EndFigure(pFigureEnd);
}

void W_Geometry::AddArc(_In_ const D2D1_POINT_2F& pStartPoint, 
	_In_ const D2D1_POINT_2F& pPoint,
	_In_ const D2D1_SIZE_F& pSize,
	float pRotationAngle,
	D2D1_SWEEP_DIRECTION sweepDirection,
	D2D1_ARC_SIZE arcSize,
	D2D1_FILL_MODE pFillMode, 
	D2D1_FIGURE_BEGIN pFigureBegin, 
	D2D1_FIGURE_END pFigureEnd)
{
	sink->SetFillMode(pFillMode);
	sink->BeginFigure(pStartPoint, pFigureBegin);
	{
		sink->AddArc(D2D1::ArcSegment(pPoint, pSize, pRotationAngle, sweepDirection, arcSize));
	}
	sink->EndFigure(pFigureEnd);
}

void W_Geometry::End()
{
	if (this->state != STARTED) throw std::exception(MISSING_BEGIN_CALL);

	auto hr = sink->Close();
	if (FAILED(hr))
	{
		OnFailed(hr, "closing sink for geometry path", this->Name, false, true);
		return;
	}

	this->state = ENDED;
}

HRESULT W_Geometry::Draw()
{
	if (this->brush == nullptr || this->updateColor)
	{
		auto hr = this->gDevice->context2D->CreateSolidColorBrush(this->color, &this->brush);
		if (FAILED(hr))
		{
			OnFailed(hr, "Create solid color brush for background", this->Name, false, true);
			return hr;
		}
		this->updateColor = false;
	}

	if (this->borderBrush == nullptr || this->updateBorderColor)
	{
		auto hr = this->gDevice->context2D->CreateSolidColorBrush(this->borderColor, &this->borderBrush);
		if (FAILED(hr))
		{
			OnFailed(hr, "Create solid color brush for border", this->Name, false, true);
			return hr;
		}
		this->updateBorderColor = false;
	}

	this->gDevice->context2D->DrawGeometry(this->pathGeometry.Get(), this->borderBrush.Get(), this->strokeWidth, 0);
	this->gDevice->context2D->FillGeometry(this->pathGeometry.Get(), this->brush.Get());
}

ULONG W_Geometry::Release()
{
	if (IsReleased()) return 0;

	COM_RELEASE(this->pathGeometry);
	COM_RELEASE(this->brush);
	COM_RELEASE(this->borderBrush);
	COM_RELEASE(this->sink);

	this->gDevice = nullptr;
	
	return W_Object::Release();
}

#pragma region Getters

ColorF W_Geometry::GetColor() const
{
	return ColorF(this->color.r, this->color.g, this->color.b, this->color.a);
}

ColorF W_Geometry::GetBorderColor() const
{
	return ColorF(this->borderColor.r, this->borderColor.g, this->borderColor.b, this->borderColor.a);
}

#pragma endregion

#pragma region Setters

void W_Geometry::SetColor(ColorF pColor)
{
	this->color.r = pColor.r; this->color.g = pColor.g; this->color.b = pColor.b; this->color.a = pColor.a;
	this->updateColor = true;
}

void W_Geometry::SetBorderColor(ColorF pColor)
{
	this->borderColor.r = pColor.r; this->borderColor.g = pColor.g; this->borderColor.b = pColor.b; this->borderColor.a = pColor.a;
	this->updateBorderColor = true;
}

#pragma endregion