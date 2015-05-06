#include "Wolf.DirectX.Shared.PCH.h"
#include "W_Ellipse.h"

using namespace D2D1;
using namespace Wolf::Graphics;
using namespace Wolf::Graphics::Direct2D::Shapes;

W_Ellipse::W_Ellipse(const std::shared_ptr<W_GraphicsDevice>& pGDevice, 
	float pCenterX, float pCenterY, float pRadiusX, float pRadiusY) : strokeWidth(1.0f),
updateColor(false), updateBorderColor(false)
{
	this->Name = typeid(this).name();
	this->gDevice = pGDevice;
	this->color.r = 0.274f; this->color.g = 0.274f; this->color.b = 0.274f; this->color.a = 1;
	this->borderColor.r = 1; this->borderColor.g = 1; this->borderColor.b = 1; this->borderColor.a = 1;

	//Initialize eclipse
	this->ellipse.point.x = pCenterX;
	this->ellipse.point.y = pCenterY;
	this->ellipse.radiusX = pRadiusX;
	this->ellipse.radiusY = pRadiusY;
}

W_Ellipse::~W_Ellipse()
{
	Release();
}

HRESULT W_Ellipse::Draw()
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

	this->gDevice->context2D->DrawEllipse(&this->ellipse, this->borderBrush.Get(), this->strokeWidth, 0);
	this->gDevice->context2D->FillEllipse(&this->ellipse, this->brush.Get());

	return S_OK;
}

ULONG W_Ellipse::Release()
{
	if (IsReleased()) return 0;

	COM_RELEASE(this->brush);
	COM_RELEASE(this->borderBrush);

	this->gDevice = nullptr;


	return W_Object::Release();
}

#pragma region Getters

ColorF W_Ellipse::GetColor() const
{
	return ColorF(this->color.r, this->color.g, this->color.b, this->color.a);
}

ColorF W_Ellipse::GetBorderColor() const
{
	return ColorF(this->borderColor.r, this->borderColor.g, this->borderColor.b, this->borderColor.a);
}

D2D1_POINT_2F W_Ellipse::GetRadius() const
{
	D2D1_POINT_2F r;

	r.x = this->ellipse.radiusX;
	r.y = this->ellipse.radiusY;

	return r;
}

D2D1_POINT_2F W_Ellipse::GetCenter() const
{
	return this->ellipse.point;
}

#pragma endregion

#pragma region Setters

void W_Ellipse::SetColor(D2D1::ColorF pColor)
{
	this->color.r = pColor.r; this->color.g = pColor.g; this->color.b = pColor.b; this->color.a = pColor.a;
	this->updateColor = true;
}

void W_Ellipse::SetBorderColor(D2D1::ColorF pColor)
{
	this->borderColor.r = pColor.r; this->borderColor.g = pColor.g; this->borderColor.b = pColor.b; this->borderColor.a = pColor.a;
	this->updateBorderColor = true;
}

void W_Ellipse::SetGeormetry(float pCenterX, float pCenterY, float pRadiusX, float pRadiusY)
{
	this->ellipse.point.x = pCenterX;
	this->ellipse.point.y = pCenterY;
	this->ellipse.radiusX = pRadiusX;
	this->ellipse.radiusY = pRadiusY;
}

#pragma endregion