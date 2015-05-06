#include "Wolf.DirectX.Shared.PCH.h"
#include "W_Line.h"

using namespace D2D1;
using namespace Wolf::Graphics;
using namespace Wolf::Graphics::Direct2D::Shapes;

W_Line::W_Line(const std::shared_ptr<W_GraphicsDevice>& pGDevice, D2D1_POINT_2F pStartPoint, D2D1_POINT_2F pStopPoint) : gDevice(pGDevice), startPoint(pStartPoint), stopPoint(pStopPoint),
strokeWidth(1.0f), updateColor(false), updateBorderColor(false)
{
	this->Name = typeid(this).name();
	this->color.r = 0.274f; this->color.g = 0.274f; this->color.b = 0.274f; this->color.a = 1;
	this->borderColor.r = 1; this->borderColor.g = 1; this->borderColor.b = 1; this->borderColor.a = 1;
}

W_Line::~W_Line()
{
	Release();
}

HRESULT W_Line::Draw()
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

	this->gDevice->context2D->DrawLine(this->startPoint, this->stopPoint, this->borderBrush.Get(), this->strokeWidth, 0);

	return S_OK;
}

ULONG W_Line::Release()
{
	if (IsReleased()) return 0;
	
	COM_RELEASE(this->brush);
	COM_RELEASE(this->borderBrush);

	this->gDevice = nullptr;

	return W_Object::Release();
}

#pragma region Getters

D2D1_POINT_2F W_Line::GetStartPoint() const
{
	return this->startPoint;
}

D2D1_POINT_2F W_Line::GetStopPoint() const
{
	return this->stopPoint;
}

#pragma endregion

#pragma region Setters

void W_Line::SetColor(D2D1::ColorF pColor)
{
	this->color.r = pColor.r; this->color.g = pColor.g; this->color.b = pColor.b; this->color.a = pColor.a;
	this->updateColor = true;
}

void W_Line::SetBorderColor(D2D1::ColorF pColor)
{
	this->borderColor.r = pColor.r; this->borderColor.g = pColor.g; this->borderColor.b = pColor.b; this->borderColor.a = pColor.a;
	this->updateBorderColor = true;
}

void W_Line::SetGeormetry(D2D1_POINT_2F pStartPoint, D2D1_POINT_2F pStopPoint)
{
	this->startPoint = pStartPoint;
	this->stopPoint = pStopPoint;
}

#pragma endregion