/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_node.h
	Description		 : The class of collada's node
	Comment          :
*/

#ifndef __C_NODE_H__
#define __C_NODE_H__

#include <vector>
#include "c_obj.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

namespace wolf
{
	namespace content_pipeline
	{
		namespace collada
		{
            enum c_node_type { NONE, MESH, LIGHT, CAMERA };
			struct c_node : c_obj
			{
				c_node* find_instanced_geomaetry_node();
				c_node* find_instanced_geomaetry_node(std::string pGeometryName);
				//c_node* find_instanced_node_name();
				ULONG release();

                int                     type = c_node_type::NONE;
				bool					proceeded = false;
				glm::vec3				translate = glm::vec3(0.0f);
				glm::vec4				rotation = glm::vec4(0.0f);
				glm::vec3				scale = glm::vec3(1.0f);
				glm::mat4				transform;
				std::string				instanced_geometry_name = "";
                std::string				instanced_material_symbol_name = "";
                std::string				instanced_material_target_name = "";
				std::string				instanced_camera_name = "";
				std::vector<c_node*>	child_nodes;

			private:
				bool					is_released = false;
			};
		}
	}
}

#endif
