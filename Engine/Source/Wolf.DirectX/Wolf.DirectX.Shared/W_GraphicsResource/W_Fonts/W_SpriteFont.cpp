#include "Wolf.DirectX.Shared.PCH.h"
#include "W_SpriteFont.h"

using namespace std;
using namespace D2D1;
using namespace Wolf::Graphics;

W_SpriteFont::W_SpriteFont(const shared_ptr<W_GraphicsDevice>& pGDevice, W_SpriteFontStyle pFontStyle) : gDevice(pGDevice), fontStyle(pFontStyle)
{
	this->Name = typeid(this).name();
}

W_SpriteFont::~W_SpriteFont()
{
	Release();
}

HRESULT W_SpriteFont::Load()
{
	HRESULT hr = S_FALSE;

	hr = gDevice->writeFactory->CreateTextFormat(
		fontStyle.NAME == nullptr ? L"Arial" : fontStyle.NAME,
		nullptr,
		fontStyle.WEIGHT,
		fontStyle.STYLE,
		fontStyle.STRETCH,
		fontStyle.SIZE,
		fontStyle.LOCAL,
		&this->font);
	OnFailed(hr, "Create text format", this->Name, false, true);

	hr = this->font->SetTextAlignment(this->fontStyle.TEXT_ALIGNMENT);
	OnFailed(hr, "Setting text alignment", this->Name, false, true);

	hr = this->font->SetParagraphAlignment(this->fontStyle.PARAGRAPH_ALIGNMENT);
	OnFailed(hr, "Setting paragraph alignment", this->Name, false, true);

	if (this->brush == nullptr)
	{
		hr = SetColor(ColorF(0.2f, 0.78039f, 1));
	}

	return hr;
}

HRESULT W_SpriteFont::SetColor(ColorF color)
{
	auto hr = this->gDevice->context2D->CreateSolidColorBrush(color, this->brush.GetAddressOf());
	OnFailed(hr, "Setting color of font", this->Name, false, true);
	return hr;
}


ULONG W_SpriteFont::Release()
{
	if (IsReleased()) return 0;
	
	COM_RELEASE(this->brush);
	COM_RELEASE(this->font);
	COM_RELEASE(this->textLayout);

	this->gDevice = nullptr;

	return W_Object::Release();
}