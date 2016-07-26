/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
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
			struct c_node : c_obj
			{
				c_node* find_instanced_geomaetry_node();
				c_node* find_instanced_geomaetry_node(std::string pGeometryName);
				c_node* find_instanced_node_name();
				ULONG release();

				bool					proceeded = false;
				glm::vec3				translate;
				glm::vec3				scale;
				glm::vec3				rotation;
				glm::mat4x4				transform;
				std::string				instanced_node_name;
				std::string				instanced_geometry_name;
				std::string				instanced_camera_name;
				std::vector<c_node*>	nodes;

			private:
				bool					is_released = false;
			};
		}
	}
}

#endif