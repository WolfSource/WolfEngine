/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_cpipeline_structs.h
	Description		 : The structures
	Comment          :
*/

#ifndef __W_VERTEX_STRUCT_H__
#define __W_VERTEX_STRUCT_H__

#if __cplusplus <= 201402L
#include <msgpack.hpp>
#endif
#include <vector>

namespace wolf
{
    namespace content_pipeline
    {
        struct w_vertex_struct
        {
            float		    position[3];
            float		    normal[3];
            float		    uv[2];
            float		    blend_weight[4];
            int	            blend_indices[4];
            float		    tangent[3];
            float		    binormal[3];
            float		    color[4];
            uint32_t	    vertex_index;

#if __cplusplus <= 201402L
            MSGPACK_DEFINE(position, normal, uv, vertex_index);
#endif

#ifdef __PYTHON__
			glm::w_vec3		py_get_position() { return glm::w_vec3(this->position[0], this->position[1], this->position[2]); }
			void			py_set_position(_In_ glm::w_vec3& pValue) { this->position[0] = pValue.get_x(); this->position[1] = pValue.get_y(); this->position[2] = pValue.get_z(); }

			glm::w_vec3		py_get_normal() { return glm::w_vec3(this->normal[0], this->normal[1], this->normal[2]); }
			void			py_set_normal(_In_ glm::w_vec3& pValue) { this->normal[0] = pValue.get_x(); this->normal[1] = pValue.get_y(); this->normal[2] = pValue.get_z(); }

			glm::w_vec2		py_get_uv() { return glm::w_vec2(this->uv[0], this->uv[1]); }
			void			py_set_uv(_In_ glm::w_vec2& pValue) { this->uv[0] = pValue.get_x(); this->uv[1] = pValue.get_y(); }

			glm::w_vec4		py_get_blend_weight() { return glm::w_vec4(this->blend_weight[0], this->blend_weight[1], this->blend_weight[2], this->blend_weight[3]); }
			void			py_set_blend_weight(_In_ glm::w_vec4& pValue)
			{
				this->blend_weight[0] = pValue.get_x();
				this->blend_weight[1] = pValue.get_y();
				this->blend_weight[2] = pValue.get_z();
				this->blend_weight[3] = pValue.get_w();
			}

			glm::w_vec4		py_get_blend_indices() { return glm::w_vec4(this->blend_indices[0], this->blend_indices[1], this->blend_indices[2], this->blend_indices[3]); }
			void			py_set_blend_indices(_In_ glm::w_vec4& pValue)
			{
				this->blend_indices[0] = pValue.get_x();
				this->blend_indices[1] = pValue.get_y();
				this->blend_indices[2] = pValue.get_z();
				this->blend_indices[3] = pValue.get_w();
			}

			glm::w_vec3		py_get_tangent() { return glm::w_vec3(this->tangent[0], this->tangent[1], this->tangent[2]); }
			void			py_set_tangent(_In_ glm::w_vec3& pValue) { this->tangent[0] = pValue.get_x(); this->tangent[1] = pValue.get_y(); this->tangent[2] = pValue.get_z(); }

			glm::w_vec3		py_get_binormal() { return glm::w_vec3(this->binormal[0], this->binormal[1], this->binormal[2]); }
			void			py_set_binormal(_In_ glm::w_vec3& pValue) { this->binormal[0] = pValue.get_x(); this->binormal[1] = pValue.get_y(); this->binormal[2] = pValue.get_z(); }

			glm::w_vec4		py_get_color() { return glm::w_vec4(this->color[0], this->color[1], this->color[2], this->color[3]); }
			void			py_set_color(_In_ glm::w_vec4& pValue) {this->color[0] = pValue.get_x(); this->color[1] = pValue.get_y(); this->color[2] = pValue.get_z(); this->color[3] = pValue.get_w();}

#endif
		};
    }
}

#endif
