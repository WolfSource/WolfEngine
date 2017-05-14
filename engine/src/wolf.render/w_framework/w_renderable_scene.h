/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_renderable_scene.h
	Description		 : Render wolf::graphics::w_scene
	Comment          :
*/

#ifndef __W_RENDERABLE_SCENE_H__
#define __W_RENDERABLE_SCENE_H__

#include <w_graphics_device_manager.h>
#include <w_scene.h>
#include "w_renderable_model.h"

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
	namespace framework
	{
        class w_renderable_scene : public wolf::system::w_object
        {
        public:
            W_EXP w_renderable_scene(_In_ wolf::content_pipeline::w_scene* pScene);

			W_EXP void load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
                _In_ const wolf::graphics::w_render_pass* pRenderPass,
                _In_ const std::string& pPipelineCacheName);

            W_EXP void update(_In_ const glm::mat4& pViewProjection);
            W_EXP void render(_In_ const VkCommandBuffer& pCommandBuffer);

			W_EXP ULONG release();

#pragma region Getters

			W_EXP void get_first_or_default_camera(_Inout_ wolf::content_pipeline::w_camera** pCamera);
            W_EXP uint32_t get_instances_count(_In_ const size_t pIndex) const;
            
#pragma endregion

        private:
            typedef	 wolf::system::w_object				_super;

            size_t										_model_counts;
            std::vector<w_renderable_model*>			_models;
            wolf::content_pipeline::w_scene*			_scene;
        };
    }
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif
