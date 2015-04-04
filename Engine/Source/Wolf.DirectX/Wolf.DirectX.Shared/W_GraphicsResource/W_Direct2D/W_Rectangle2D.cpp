#include "Wolf.DirectX.Shared.PCH.h"
#include "W_Rectangle2D.h"

using namespace D2D1;
using namespace Wolf::Graphics;
using namespace Wolf::Graphics::Direct2D;

W_Rectangle2D::W_Rectangle2D(const std::shared_ptr<W_GraphicsDevice>& pGDevice) : rectangle(RectF(7, 7, 400, 200)), strokeWidth(1.0f),
	updateColor(false), updateBorderColor(false)
{
	this->Name = typeid(this).name();
	this->gDevice = pGDevice;
	this->color.r = 0.274f; this->color.g = 0.274f; this->color.b = 0.274f; this->color.a = 1;
	this->borderColor.r = 1; this->borderColor.g = 1; this->borderColor.b = 1; this->borderColor.a = 1;
}


W_Rectangle2D::~W_Rectangle2D()
{
	Release();
}

HRESULT W_Rectangle2D::Draw()
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

	if (this->border == nullptr || this->updateBorderColor)
	{
		auto hr = this->gDevice->context2D->CreateSolidColorBrush(this->borderColor, &this->border);
		if (FAILED(hr))
		{
			OnFailed(hr, "Create solid color brush for border", this->Name, false, true);
			return hr;
		}
		this->updateBorderColor = false;
	}

	this->gDevice->context2D->DrawRectangle(&rectangle, this->border.Get(), this->strokeWidth, 0);
	this->gDevice->context2D->FillRectangle(&rectangle, this->brush.Get());

	return S_OK;
}

ULONG W_Rectangle2D::Release()
{
	if (IsReleased()) return 0;
	
	COM_RELEASE(this->brush);
	COM_RELEASE(this->border);

	this->gDevice = nullptr;


	return W_Object::Release();
}

#pragma region Getters

ColorF W_Rectangle2D::GetColor()
{
	return ColorF(this->color.r, this->color.g, this->color.b, this->color.a);
}

ColorF W_Rectangle2D::GetBorderColor()
{
	return ColorF(this->borderColor.r, this->borderColor.g, this->borderColor.b, this->borderColor.a);
}

float W_Rectangle2D::GetLeft()
{
	return this->rectangle.left;
}

float W_Rectangle2D::GetTop()
{
	return this->rectangle.top;
}

float W_Rectangle2D::GetWidth()
{
	return this->rectangle.right - this->rectangle.left;
}

float W_Rectangle2D::GetHeight()
{
	return this->rectangle.top - this->rectangle.bottom;
}

#pragma endregion

#pragma region Setters

void W_Rectangle2D::SetColor(D2D1::ColorF pColor)
{
	this->color.r = pColor.r; this->color.g = pColor.g; this->color.b = pColor.b; this->color.a = pColor.a;
	this->updateColor = true;
}

void W_Rectangle2D::SetBorderColor(D2D1::ColorF pColor)
{
	this->borderColor.r = pColor.r; this->borderColor.g = pColor.g; this->borderColor.b = pColor.b; this->borderColor.a = pColor.a;
	this->updateBorderColor = true;
}

void W_Rectangle2D::SetGeormetry(float pLeft, float pTop, float pWidth, float pHeight)
{
	this->rectangle.left = pLeft;
	this->rectangle.top = pTop;
	this->rectangle.right = this->rectangle.left + pWidth;
	this->rectangle.bottom = this->rectangle.top + pHeight;
}

#pragma endregion