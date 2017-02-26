#include "w_directX_pch.h"
#include "w_element.h"

using namespace DirectX;
using namespace wolf::gui;

#pragma region w_blendColor

_Use_decl_annotations_
void w_blend_color::initialize(_In_ DWORD pDefaultColor, _In_ DWORD pDisabledColor, _In_ DWORD pHiddenColor)
{
	for (size_t i = 0; i < MAX_CONTROL_STATES; ++i)
	{
		this->color_states[i] = pDefaultColor;
	}

	this->color_states[W_GUI_STATE_DISABLED] = pDisabledColor;
	this->color_states[W_GUI_STATE_HIDDEN] = pHiddenColor;
	set_current(pHiddenColor);
}

_Use_decl_annotations_
void w_blend_color::blend(_In_ UINT pStateIndex, _In_ float pElapsedTime, _In_ float pFrameRate)
{
	XMFLOAT4 destColor = DWORD_COLOR_TO_XMFLOAT4(this->color_states[pStateIndex]);
	XMVECTOR clr1 = XMLoadFloat4(&destColor);
	XMVECTOR clr = XMLoadFloat4(&this->current_color_state);
	clr = XMVectorLerp(clr, clr1, 1.0f - powf(pFrameRate, 30 * pElapsedTime));
	XMStoreFloat4(&this->current_color_state, clr);
}

void w_blend_color::set_current(DWORD color)
{
	this->current_color_state = DWORD_COLOR_TO_XMFLOAT4(color);
}

#pragma endregion

_Use_decl_annotations_
void w_element::set_texture(UINT pTexture, RECT* pRrcTexture, DWORD pDefaultTextureColor)
{
	this->index_texture = pTexture;

	if (pRrcTexture)
	{
		this->rect_texture = *pRrcTexture;
	}
	else
	{
		SetRectEmpty(&this->rect_texture);
	}
	this->texture_color.initialize(pDefaultTextureColor);
}

_Use_decl_annotations_
void w_element::set_font(UINT pFont, DWORD pDefaultFontColor, DWORD pTextFormat)
{
	this->index_font = pFont;
	this->draw_text_format = pTextFormat;
	this->font_color.initialize(pDefaultFontColor);
}

void w_element::refresh()
{
	this->texture_color.set_current(this->texture_color.color_states[W_GUI_STATE_HIDDEN]);
	this->font_color.set_current(this->font_color.color_states[W_GUI_STATE_HIDDEN]);
}


