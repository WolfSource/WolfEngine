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
#include <w_point.h>

namespace wolf
{
    namespace graphics
    {
        class w_imgui_pimp;
        class w_imgui
        {
        public:

            struct texture_index
            {
                int         _start_index;
                w_texture*  _texture;

                texture_index()
                {
                    this->_start_index = -1;
                    this->_texture = nullptr;
                }
                texture_index(_In_ int  pStartIndex, _In_ w_texture*  pTexture)
                {
                    this->_start_index = pStartIndex;
                    this->_texture = pTexture;
                }
            };

            static W_EXP HRESULT load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
#ifdef __WIN32
                _In_ HWND pHWND,
#endif
                _In_ const w_point_t& pScreenSize,
                _In_ VkRenderPass& pRenderPass,
                _In_ w_texture* pIconTexture,
                _In_ w_texture** pStagingMediaTexture = nullptr,
                _In_ const char* pFontPath = nullptr,
                _In_ const float& pFontPixelSize = 15.0f);

            static W_EXP HRESULT update_buffers(_In_ wolf::graphics::w_render_pass& pRenderPass);
            static W_EXP void new_frame(_In_ const float& pDeltaTime, _In_ const std::function<void(void)>& pMakeGuiWork);
            static W_EXP void render(_In_ VkCommandBuffer pCommandBuffer);
            static W_EXP ULONG release();

#pragma region Getters

            static W_EXP uint32_t get_width();
            static W_EXP uint32_t get_height();

#pragma endregion

#pragma region Setters

#ifdef __WIN32
            //static W_EXP void set_HWND();
#endif 
            static W_EXP void set_width(_In_ const uint32_t& pWidth);
            static W_EXP void set_height(_In_ const uint32_t& pHeight);

#pragma endregion


        private:
            static bool                                        _is_released;
            static w_imgui_pimp*                               _pimp;
        };
    }
}

#endif
