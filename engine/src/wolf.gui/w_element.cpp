#include "w_gui_pch.h"
#include "w_element.h"
#include <glm_extention.h>

using namespace wolf::gui;

#pragma region w_blend_color

void w_blend_color::initialize(_In_ const DWORD pDefaultColor, _In_ const DWORD pDisabledColor, _In_ const DWORD pHiddenColor)
{
	for (size_t i = 0; i < MAX_CONTROL_STATES; ++i)
	{
		this->color_states[i] = pDefaultColor;
	}

	this->color_states[W_GUI_STATE_DISABLED] = pDisabledColor;
	this->color_states[W_GUI_STATE_HIDDEN] = pHiddenColor;
	set_current(pHiddenColor);
}

void w_blend_color::blend(_In_ const UINT pStateIndex, _In_ const float pElapsedTime, _In_ const float pFrameRate)
{
    auto _dest_color = glm::to_vec4(this->color_states[pStateIndex]);
    auto _src_color = glm::to_vec4(this->current_color_state);
    auto _lerp = glm::lerp(_src_color, _dest_color, 1.0f - powf(pFrameRate, 30 * pElapsedTime));

    this->current_color_state = w_color::to_color(_lerp);
}

void w_blend_color::set_current(_In_ const DWORD pColor)
{
	this->current_color_state = w_color::to_color(glm::to_vec4(pColor));
}

#pragma endregion

void w_element::set_texture(_In_ const UINT pTexture, 
    _In_ const w_rectangle* pRectangleOfTexture, 
    _In_ const DWORD pDefaultTextureColor)
{
    this->index_texture = pTexture;

    if (pRectangleOfTexture)
    {
        this->rect_texture = *pRectangleOfTexture;
    }
    else
    {
        std::memset(&this->rect_texture, 0, sizeof(this->rect_texture));
    }
    this->texture_color.initialize(pDefaultTextureColor);
}

void w_element::refresh()
{
    this->texture_color.set_current(this->texture_color.color_states[W_GUI_STATE_HIDDEN]);
}


