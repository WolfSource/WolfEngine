#include "Wolf.DirectX.Shared.PCH.h"
#include "W_Rectangle.h"

using namespace D2D1;
using namespace Wolf::Graphics;
using namespace Wolf::Graphics::Direct2D::Shapes;

W_Rectangle::W_Rectangle(const std::shared_ptr<W_GraphicsDevice>& pGDevice, 
	float pLeft, float pTop, float pWidth, float pHeight, float pRadiusX, float pRadiusY) : gDevice(pGDevice), strokeWidth(1.0f),
	updateColor(false), updateBorderColor(false)
{
	this->Name = typeid(this).name();
	this->color.r = 0.274f; this->color.g = 0.274f; this->color.b = 0.274f; this->color.a = 1;
	this->borderColor.r = 1; this->borderColor.g = 1; this->borderColor.b = 1; this->borderColor.a = 1;

	//Initialize rectangle
	this->rectangle.radiusX = pRadiusX;
	this->rectangle.radiusY = pRadiusY;
	this->rectangle.rect.left = pLeft;
	this->rectangle.rect.top = pTop;
	this->rectangle.rect.right = this->rectangle.rect.left + pWidth;
	this->rectangle.rect.bottom = this->rectangle.rect.top + pHeight;
}


W_Rectangle::~W_Rectangle()
{
	Release();
}

HRESULT W_Rectangle::Draw()
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

	if (this->rectangle.radiusX == 0 && this->rectangle.radiusY == 0)
	{
		this->gDevice->context2D->DrawRectangle(&rectangle.rect, this->borderBrush.Get(), this->strokeWidth, 0);
		this->gDevice->context2D->FillRectangle(&rectangle.rect, this->brush.Get());
	}
	else
	{
		this->gDevice->context2D->DrawRoundedRectangle(&rectangle, this->borderBrush.Get(), this->strokeWidth, 0);
		this->gDevice->context2D->FillRoundedRectangle(&rectangle, this->brush.Get());
	}

	return S_OK;
}

ULONG W_Rectangle::Release()
{
	if (IsReleased()) return 0;
	
	COM_RELEASE(this->brush);
	COM_RELEASE(this->borderBrush);

	this->gDevice = nullptr;


	return W_Object::Release();
}

#pragma region Getters

ColorF W_Rectangle::GetColor() const
{
	return ColorF(this->color.r, this->color.g, this->color.b, this->color.a);
}

ColorF W_Rectangle::GetBorderColor() const
{
	return ColorF(this->borderColor.r, this->borderColor.g, this->borderColor.b, this->borderColor.a);
}

float W_Rectangle::GetLeft() const
{
	return this->rectangle.rect.left;
}

float W_Rectangle::GetTop() const
{
	return this->rectangle.rect.top;
}

float W_Rectangle::GetWidth() const
{
	return this->rectangle.rect.right - this->rectangle.rect.left;
}

float W_Rectangle::GetHeight() const
{
	return this->rectangle.rect.top - this->rectangle.rect.bottom;
}

float W_Rectangle::GetRadiusX() const
{
	return this->rectangle.radiusX;
}

float W_Rectangle::GetRadiusY() const
{
	return this->rectangle.radiusY;
}

#pragma endregion

#pragma region Setters

void W_Rectangle::SetColor(D2D1::ColorF pColor)
{
	this->color.r = pColor.r; this->color.g = pColor.g; this->color.b = pColor.b; this->color.a = pColor.a;
	this->updateColor = true;
}

void W_Rectangle::SetBorderColor(D2D1::ColorF pColor)
{
	this->borderColor.r = pColor.r; this->borderColor.g = pColor.g; this->borderColor.b = pColor.b; this->borderColor.a = pColor.a;
	this->updateBorderColor = true;
}

void W_Rectangle::SetGeormetry(float pLeft, float pTop, float pWidth, float pHeight, float pRadiusX, float pRadiusY)
{
	this->rectangle.radiusX = pRadiusX;
	this->rectangle.radiusY = pRadiusY;
	this->rectangle.rect.left = pLeft;
	this->rectangle.rect.top = pTop;
	this->rectangle.rect.right = this->rectangle.rect.left + pWidth;
	this->rectangle.rect.bottom = this->rectangle.rect.top + pHeight;
}

#pragma endregion