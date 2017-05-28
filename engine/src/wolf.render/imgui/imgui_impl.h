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

#ifndef __IMGUI_IMPL_H__
#define __IMGUI_IMPL_H__

#include <w_graphics_device_manager.h>
#include <w_graphics/w_render_pass.h>

class imgui_pimp;
class imgui_imp : public wolf::system::w_object
{
public:
    W_EXP imgui_imp();
    W_EXP~imgui_imp();

    W_EXP HRESULT load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
        _In_ const float& pWidth, 
        _In_ const float& pHeight, 
        _In_ VkRenderPass& pRenderPass);

    W_EXP HRESULT update_buffers(_In_ wolf::graphics::w_render_pass& pRenderPass);
    W_EXP void new_frame();
    W_EXP void render(_In_ VkCommandBuffer pCommandBuffer, _In_ float pDeltaTimeTicks);
    W_EXP ULONG release() override;

    private:
        typedef wolf::system::w_object              _super;
        imgui_pimp*                                 _pimp;
};

#endif
