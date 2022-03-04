/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_vertex_declaration.h
	Description		 : The enumuration that is represent types of vertex
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_VERTEX_DECLARATION_H__
#define __W_VERTEX_DECLARATION_H__

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/compatibility.hpp>

namespace wolf
{
	namespace content_pipeline
	{
		namespace vertex_declaration_structs
		{
			struct vertex_position
			{
				glm::vec3 position;
			};

			struct vertex_position_color
			{
				glm::vec3 position;
				glm::vec4 color;
			};

			struct vertex_position_uv
			{
				glm::vec3 position;
				glm::vec2 uv;
			};

			struct vertex_position_uv_color
			{
				glm::vec3 position;
				glm::vec2 uv;
                glm::vec4 color;
			};

			struct vertex_position_normal_color
			{
				glm::vec3 position;
				glm::vec3 normal;
				glm::vec4 color;
			};

			struct vertex_position_normal_uv
			{
				glm::vec3 position;
				glm::vec3 normal;
				glm::vec2 uv;
			};

			struct vertex_position_normal_uv_tangent_binormal
			{
				glm::vec3 position;
				glm::vec3 normal;
				glm::vec2 uv;
				glm::vec3 tangent;
				glm::vec3 binormal;
			};

			struct vertex_position_normal_uv_tangent_binormal_weight_indices
			{
				glm::vec3 position;
				glm::vec3 normal;
				glm::vec2 uv;
				glm::vec3 tangent;
				glm::vec3 binormal;
				glm::vec4 blend_weight;
				glm::int4 blend_indices;
			};
		}
	}
}

#endif
