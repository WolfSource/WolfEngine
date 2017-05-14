/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_renderable_model.h
	Description		 : Render wolf::content_pipeline::w_model using this class
	Comment          :
*/

#ifndef __W_RENDERABLE_MODEL_H__
#define __W_RENDERABLE_MODEL_H__

#include "w_graphics_device_manager.h"
#include <vector>
#include <glm/glm.hpp>
#include <w_graphics/w_mesh.h>
#include <w_graphics/w_uniform.h>
#include <w_graphics/w_shader.h>
#include <w_model.h>
#include <w_camera.h>

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
    namespace framework
    {
        class w_renderable_model : public wolf::graphics::w_mesh
        {
        public:

            W_EXP w_renderable_model(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
                wolf::content_pipeline::w_model* pModelData = nullptr,
                _In_ const bool pZup = false);
            W_EXP virtual ~w_renderable_model();

            W_EXP HRESULT load(_In_ const wolf::graphics::w_render_pass* pRenderPass, _In_ const std::string& pPipelineCacheName);
            W_EXP HRESULT update(_In_ const glm::mat4 pViewProjection);
            W_EXP void render(_In_ const VkCommandBuffer& pCommandBuffer);

            W_EXP virtual ULONG release() override;

#pragma region Getters

            UINT get_instances_count() const
            {
                return static_cast<UINT>(this->_model_content->get_instnaces_count());
            }

#pragma endregion

#pragma region Setters
            void set_z_up(_In_ bool pZUp)
            {
                this->_z_up = pZUp;
            }
#pragma endregion

        private:

            HRESULT _create_instances_buffer(_In_ const void* const pInstanceData, _In_ const UINT pInstanceSize);
            
            typedef	wolf::system::w_object                              _super;

            std::shared_ptr<wolf::graphics::w_graphics_device>			_gDevice;
            wolf::content_pipeline::w_model*							_model_content;
            std::vector<wolf::graphics::w_mesh*>                        _meshes;
            wolf::graphics::w_buffer                                    _instances_buffer;
            bool                                                        _z_up;
        };
    }
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif
