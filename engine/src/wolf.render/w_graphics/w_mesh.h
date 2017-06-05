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
#include "w_render_pass.h"
#include <glm/mat4x4.hpp>
#include <w_vertex_declaration.h>

namespace wolf
{
	namespace graphics
	{        
        enum w_vertex_declaration
        {
            NOT_DEFINED,
            USER_DEFINED,
            VERTEX_POSITION,
            VERTEX_POSITION_COLOR,
            VERTEX_POSITION_UV,
            VERTEX_POSITION_UV_COLOR,
            VERTEX_POSITION_NORMAL_COLOR,
            VERTEX_POSITION_NORMAL_UV,
            VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL,
            VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL_WEIGHT_INDICES,
        };

        enum w_vertex_attribute : uint32_t
        {
            Float = 4,
            Vec2 = 4 * 2,
            Vec3 = 4 * 3,
            Vec4 = 4 * 4,
        };

        struct w_vertex_binding_attributes
        {
            w_vertex_declaration                                    declaration = w_vertex_declaration::NOT_DEFINED;
            std::map<uint32_t, std::vector<w_vertex_attribute>>     binding_attributes;
        };

        class w_mesh_pimp;
		//Represents a 3D model mesh composed of multiple meshpart objects.
		class w_mesh : public system::w_object
		{
		public:
            W_EXP w_mesh();
			W_EXP virtual ~w_mesh();

			//initialize and load mesh
			W_EXP HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                               _In_ const void* const pVerticesData,
                               _In_ const UINT  pVerticesSize,
                               _In_ const UINT pVerticesCount,
                               _In_ const UINT* const pIndicesData,
                               _In_ const UINT pIndicesCount,
                               _In_ bool pUseDynamicBuffer = false);
            
            //update data of vertices and indices
            W_EXP HRESULT update_dynamic_buffer(
                _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const void* const pVerticesData,
                _In_ const UINT pVerticesSize,
                _In_ const UINT pVerticesCount,
                _In_ const UINT* const pIndicesData,
                _In_ const UINT pIndicesCount);

            W_EXP void render(_In_ const VkCommandBuffer& pCommandBuffer, _In_ const VkBuffer& pInstanceHandle,
                _In_ uint32_t& pInstancesCount);

			//release all resources
			W_EXP virtual ULONG release() override;

#pragma region Getters
            
            W_EXP VkBuffer                                                  get_vertex_buffer_handle() const;
            W_EXP VkBuffer                                                  get_index_buffer_handle() const;
            W_EXP const UINT                                                get_vertices_count() const;
            W_EXP const UINT                                                get_indices_count() const;
            W_EXP w_texture*                                                get_texture() const;
            W_EXP const w_vertex_binding_attributes                         get_vertex_binding_attributes() const;

#pragma endregion

#pragma region Setters
		
            W_EXP void set_texture(_In_ w_texture* pTexture);
            W_EXP void set_vertex_binding_attributes(_In_ const w_vertex_declaration& pVertexBindingAttributes);
            W_EXP void set_vertex_binding_attributes(_In_ const w_vertex_binding_attributes& pVertexBindingAttributes);

#pragma endregion	

		private:
            typedef		system::w_object                        _super;
            w_mesh_pimp*                                        _pimp;              
		};
	}
}

#endif
