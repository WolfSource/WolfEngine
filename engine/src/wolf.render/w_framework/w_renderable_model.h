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
#include <w_graphics/w_shader_buffer.h>
#include <w_model.h>

namespace wolf
{
	namespace framework
	{
#pragma pack(push,1)
			struct mvp_shader_buffer
			{
				//view projection (transform) matrix
                glm::mat4x4 view_projection = glm::mat4x4(1);//Identity matrix
			};
#pragma pack(pop)

        class w_renderable_model : public wolf::graphics::w_mesh
			{
			public:

				W_EXP w_renderable_model(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
					wolf::content_pipeline::w_model* pModelData = nullptr);
				W_EXP virtual ~w_renderable_model();
				
				W_EXP HRESULT load();

				W_EXP virtual ULONG release() override;

#pragma region Getters
                
                VkBuffer get_vertex_buffer_handle() const
                {
                    return this->_meshes[0]->get_vertex_buffer_handle();
                }
                
                VkBuffer get_index_buffer_handle() const
                {
                    return this->_meshes[0]->get_index_buffer_handle();
                }
               
                UINT get_vertices_count() const
                {
                    return this->_meshes[0]->get_vertices_count();
                }
                
                UINT get_indices_count() const
                {
                    return this->_meshes[0]->get_indices_count();
                }
                
#pragma endregion
                
#pragma region Setters

                void set_view_projection(const glm::mat4x4 pValue);
				
#pragma endregion

			private:
				typedef	wolf::system::w_object                              _super;
                
				std::shared_ptr<wolf::graphics::w_graphics_device>			_gDevice;
				wolf::content_pipeline::w_model*							_model;
                glm::mat4x4                                                 _view_projection;
				wolf::graphics::w_shader_buffer<mvp_shader_buffer>          _mvp_shader_buffer;
                std::vector<wolf::graphics::w_mesh*>                        _meshes;
			};
    }
}

#endif
