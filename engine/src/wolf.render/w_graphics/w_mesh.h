/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_mesh.h
	Description		 : Render a mesh
	Comment          : 
*/
#ifndef __W_MESH_H__
#define __W_MESH_H__

#include "w_graphics_device_manager.h"
#include "w_texture.h"
#include "w_shader.h"
#include "w_render_pass.h"
#include <glm/mat4x4.hpp>

namespace wolf
{
	namespace graphics
	{        
        class w_mesh_pimp;
		//Represents a 3D model mesh composed of multiple meshpart objects.
		class w_mesh : public system::w_object
		{
		public:
            W_EXP w_mesh();
			W_EXP virtual ~w_mesh();

			//initialize mesh
			W_EXP HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                               _In_ const void* const pVerticesData,
                               _In_ const UINT pVerticesCount,
                               _In_ const UINT pVerticesSize,
                               _In_ const UINT* const pIndicesData,
                               _In_ const UINT pIndicesCount,
                               _In_ w_shader* pShader,
                               _In_ const w_render_pass* pRenderPass,
                               _In_ const std::string& pPipelineCacheName = "",
                               _In_ const bool pZUp = false,
                               _In_ bool pStaging = true);
            
            W_EXP void render(_In_ const VkCommandBuffer& pCommandBuffer, _In_ const VkBuffer& pInstanceHandle,
                _In_ uint32_t& pInstancesCount);

			//release all resources
			W_EXP virtual ULONG release() override;

#pragma region Getters
            
            W_EXP VkBuffer get_vertex_buffer_handle() const;
            W_EXP VkBuffer get_index_buffer_handle() const;
            W_EXP const UINT get_vertices_count() const;
            W_EXP const UINT get_indices_count() const;
            W_EXP w_shader* get_shader() const ;
            W_EXP w_texture* get_texture() const;

#pragma endregion

#pragma region Setters
		
            W_EXP void set_shader(_In_ w_shader* pShader);
            W_EXP void set_texture(_In_ w_texture* pTexture);

#pragma endregion
			
		private:
            typedef		system::w_object                        _super;
            w_mesh_pimp*                                        _pimp;              
		};
	}
}

#endif
