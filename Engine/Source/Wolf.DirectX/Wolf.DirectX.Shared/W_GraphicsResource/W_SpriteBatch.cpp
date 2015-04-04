#include "Wolf.DirectX.Shared.PCH.h"
#include "W_SpriteBatch.h"
#include <d2d1effects.h>
#include <W_Framework\W_Math.h>

#define MISSING_BEGIN_CALL "Begin must be called successfully before a method can be called."
#define MISSING_END_CALL   "Begin cannot be called again until End has been successfully called."

using namespace Microsoft::WRL;
using namespace D2D1;
using namespace DirectX;
using namespace Wolf::Graphics;
using namespace Wolf::Graphics::Direct2D;

W_SpriteBatch::W_SpriteBatch(const std::shared_ptr<W_GraphicsDevice>& pGdevice) : gDevice(pGdevice), state(ENDED), text(L"WolfEngine")
{
	this->Name = typeid(this).name();

	auto hr = gDevice->factory2D->CreateDrawingStateBlock(this->stateBlock.GetAddressOf());
	OnFailed(hr, "create drawing state block", this->Name, false, true);
}

W_SpriteBatch::~W_SpriteBatch()
{
	Release();
}

void W_SpriteBatch::Load()
{
	auto hr = this->gDevice->context2D->CreateEffect(CLSID_D2D1BitmapSource, &this->bitmapSourceEffect);
	OnFailed(hr, "create 2D effect", this->Name, false, true);
}

void W_SpriteBatch::AddEffect(_In_ REFCLSID pEffectId)
{
	HRESULT hr = S_FALSE;

	if (pEffectId == CLSID_D2D1DirectionalBlur && this->directionalBlurEffect == nullptr)
	{
		//Add directional blur
		hr = this->gDevice->context2D->CreateEffect(CLSID_D2D1DirectionalBlur, &this->directionalBlurEffect);
		OnFailed(hr, "Creating directional blur effect", this->Name, false, true);
	}
	else if (pEffectId == CLSID_D2D1GaussianBlur && this->gaussianBlurEffect == nullptr)
	{
		//Add guassian blur
		hr = this->gDevice->context2D->CreateEffect(CLSID_D2D1GaussianBlur, &this->gaussianBlurEffect);
		OnFailed(hr, "Creating gaussian blur effect", this->Name, false, true);
	}
}

void W_SpriteBatch::SetWICBitmapSource(ComPtr<IWICFormatConverter>  pWicConverter)
{
	auto hr = this->bitmapSourceEffect->SetValue(D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE, pWicConverter.Get());
	OnFailed(hr, "set bitmap source effect", this->Name, false, true);

	this->gaussianBlurEffect->SetInputEffect(0, this->bitmapSourceEffect.Get());
}

void W_SpriteBatch::SetGaussianBlurValue(D2D1_GAUSSIANBLUR_PROP pEffectProperty, float val)
{
	auto hr = this->gaussianBlurEffect.Get()->SetValue(pEffectProperty, val);
	OnFailed(hr, "Set gaussian blur value", this->Name, false, true);
}

void W_SpriteBatch::Begin()
{
	if (this->state != ENDED) throw std::exception(MISSING_END_CALL);
	this->state = STARTED;
	this->gDevice->context2D->SaveDrawingState(this->stateBlock.Get());
	this->gDevice->context2D->BeginDraw();
}

HRESULT W_SpriteBatch::Draw(_In_ W_Rectangle2D* pRectangle2D)
{
	if (this->state != STARTED) throw std::exception(MISSING_BEGIN_CALL);

	if (pRectangle2D == nullptr) return S_FALSE;

	return pRectangle2D->Draw();
}

HRESULT W_SpriteBatch::DrawString(std::wstring pText, _In_ XMFLOAT2* const pPosition, _In_ const W_SpriteFont* pSpriteFont)
{
	return DrawString(text, pPosition, nullptr, pSpriteFont, Matrix3x2F::Identity());
}

HRESULT W_SpriteBatch::DrawString(std::wstring pText, _In_ XMFLOAT2* const pPosition,
	_In_ ID2D1SolidColorBrush* const pBrush, _In_ const W_SpriteFont* pSpriteFont, D2D1::Matrix3x2F pWorld)
{
	if (this->state != STARTED) throw std::exception(MISSING_BEGIN_CALL);

	auto hr = S_FALSE;
		
	if (this->text != pText)
	{
		this->text = pText;
		
		hr = this->gDevice->writeFactory->CreateTextLayout(
			text.c_str(),
			text.size(),
			pSpriteFont->GetFont(),
			4096.0f,
			4096.0f,
			&this->textLayout);
		OnFailed(hr, "Create text layout", this->Name, false, true);
	}

	auto _brush = (pBrush == nullptr ? pSpriteFont->GetBrush() : pBrush);
	this->gDevice->context2D->SetTransform(&pWorld);
	this->gDevice->context2D->DrawTextLayout(Point2F(pPosition->x, pPosition->y),
		this->textLayout.Get(), _brush);
	return hr;
}

void W_SpriteBatch::Draw(XMFLOAT2 Position)
{
	if (this->state != STARTED) throw std::exception(MISSING_BEGIN_CALL);
	this->gDevice->context2D->DrawImage(this->gaussianBlurEffect.Get(), Point2F(Position.x, Position.y));
}

HRESULT W_SpriteBatch::End()
{
	auto hr = this->gDevice->context2D->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
	{
		OnFailed(hr, "End draw of sprite batch", this->Name, false, true);
	}
	this->gDevice->context2D->RestoreDrawingState(this->stateBlock.Get());
	this->state = ENDED;
	return hr;
}

ULONG W_SpriteBatch::Release()
{
	if (this->IsReleased()) return 0;

	this->gDevice = nullptr;
	text.clear();
	COM_RELEASE(bitmapSourceEffect);
	COM_RELEASE(textLayout);
	COM_RELEASE(stateBlock);
	COM_RELEASE(directionalBlurEffect);
	COM_RELEASE(gaussianBlurEffect);

	return W_Object::Release();
}
