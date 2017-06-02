/*
    Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website			 : http://WolfSource.io
    Name			 : imgui_imp.h
    Description		 : implementation of imgui in wolf engine
    Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_IMGUI_H__
#define __W_IMGUI_H__

#include <imgui/imgui.h>
#include <w_graphics_device_manager.h>
#include "w_render_pass.h"
#include "w_texture.h"

namespace wolf
{
    namespace graphics
    {
        class w_imgui_pimp;
        class w_imgui
        {
        public:

            static W_EXP HRESULT load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
                _In_ HWND pHWND,
                _In_ const float& pWidth,
                _In_ const float& pHeight,
                _In_ VkRenderPass& pRenderPass,
                _In_ wolf::graphics::w_texture* pImageTexture);

            static W_EXP LRESULT on_msg_proc(
                _In_ const HWND pHWND,
                _In_ const UINT pMessage,
                _In_ const WPARAM pWParam,
                _In_ const LPARAM pLParam);

            static W_EXP HRESULT update_buffers(_In_ wolf::graphics::w_render_pass& pRenderPass);
            static W_EXP void new_frame(_In_ float pDeltaTime, _In_ const std::function<void(void)>& pMakeGuiWork);
            static W_EXP void render(_In_ VkCommandBuffer pCommandBuffer);
            static W_EXP ULONG release();

#pragma region Getters

            static W_EXP UINT get_width();
            static W_EXP UINT get_height();

#pragma endregion

#pragma region Setters

#ifdef __WIN32
            //static W_EXP void set_HWND();
#endif 
            static W_EXP void set_width(_In_ const UINT& pWidth);
            static W_EXP void set_height(_In_ const UINT& pHeight);

#pragma endregion


        private:
            static bool                                        _is_released;
            static w_imgui_pimp*                               _pimp;
        };
    }
}

#endif
