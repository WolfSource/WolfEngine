/*
    Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website			 : http://WolfSource.io
    Name			 : imgui_imp.h
    Description		 : implementation of imgui(https://github.com/ocornut/imgui) in wolf engine
    Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_IMGUI_H__
#define __W_IMGUI_H__

#include <imgui/imgui.h>
#include <w_graphics_device_manager.h>
#include "w_viewport.h"
#include "w_render_pass.h"
#include "w_texture.h"
#include <w_point.h>

namespace wolf
{
	namespace render
	{
		namespace vulkan
		{
			class w_imgui_pimp;
			class w_imgui
			{
			public:

				//struct texture_index
				//{
				//    int         start_index;
				//    w_texture*  texture;

				//    texture_index()
				//    {
				//        this->start_index = -1;
				//        this->texture = nullptr;
				//    }
				//    texture_index(_In_ int  pStartIndex, _In_ w_texture*  pTexture)
				//    {
				//        this->start_index = pStartIndex;
				//        this->texture = pTexture;
				//    }
				//};

				static W_VK_EXP W_RESULT load(
					_In_ const std::shared_ptr<wolf::render::vulkan::w_graphics_device>& pGDevice,
					_In_ const w_output_presentation_window* pOutputPresentationWindow,
					_In_ const w_viewport& pViewport,
					_In_ const w_viewport_scissor& pViewportScissor,
					_In_ w_texture* pIconTexture,
					_In_ w_texture** pStagingMediaTexture = nullptr,
					_In_ const char* pFontPath = nullptr,
					_In_ const float& pFontPixelSize = 15.0f);

				static W_VK_EXP void new_frame(_In_ const float& pDeltaTime, _In_ const std::function<void(void)>& pGuiWorkFlow);
				static W_VK_EXP void render();
				static W_VK_EXP ULONG release();

#pragma region Getters

				static W_VK_EXP uint32_t get_width();
				static W_VK_EXP uint32_t get_height();
				static W_VK_EXP w_command_buffer get_command_buffer_at(_In_ const uint32_t pFrameIndex);

#pragma endregion

#pragma region Setters

				static W_VK_EXP void set_width(_In_ const uint32_t& pWidth);
				static W_VK_EXP void set_height(_In_ const uint32_t& pHeight);

#pragma endregion

			private:
				static bool                                        _is_released;
				static w_imgui_pimp*                               _pimp;
			};
		}
	}
}

#endif
