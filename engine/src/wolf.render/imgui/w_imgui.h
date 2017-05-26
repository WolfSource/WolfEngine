/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_buffer
	Description		 : buffer
	Comment          : 
*/

#ifndef __W_IMGUI_H__
#define __W_IMGUI_H__

#include "w_graphics_device_manager.h"
#include <glm/vec2.hpp>

namespace imgui
{
    class w_imgui_pimp;
    class w_imgui : public  wolf::system::w_object
    {
    public:
        w_imgui();
        ~w_imgui();

        void load(_In_ const float pWidth, _In_ const float pHeight);

        void update_buffers();

        void render(_In_ VkCommandBuffer pCommandBuffer);

        ULONG release() override;

        struct w_imgui_push_const_block
        {
            glm::vec2 scale;
            glm::vec2 translate;
        } _push_const_block;

    private:
        typedef wolf::system::w_object      _super;
        w_imgui_pimp*                       _pimp;
    };
}

#endif
