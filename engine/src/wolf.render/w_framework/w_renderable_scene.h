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
			W_EXP void load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);

			W_EXP ULONG release();

#pragma region Getters

			W_EXP void get_first_or_default_camera(_Inout_ wolf::content_pipeline::w_camera** pCamera);
            
            W_EXP VkBuffer get_vertex_buffer_handle(_In_ const size_t pIndex) const;
            W_EXP VkBuffer get_index_buffer_handle(_In_ const size_t pIndex) const;
            W_EXP UINT get_vertices_count(_In_ const size_t pIndex) const;
            W_EXP UINT get_indices_count(_In_ const size_t pIndex) const;
            W_EXP VkBuffer get_instance_buffer_handle(_In_ const size_t pIndex) const;
            W_EXP uint32_t get_instances_count(_In_ const size_t pIndex) const;
            
#pragma endregion

#pragma region Setters

            W_EXP void set_view_projection(_In_ const glm::mat4x4 pValue);

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
