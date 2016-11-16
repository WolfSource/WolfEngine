/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_skin.h
	Description		 : The class of collada's skin
	Comment          :
*/

#ifndef __C_SKIN_H__
#define __C_SKIN_H__

#include <vector>
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include <glm_extention.h>
#include "c_animation.h"

namespace wolf
{
	namespace content_pipeline
	{
		namespace collada
		{
			struct c_vertex_weight
			{
				std::vector<c_semantic> semantics;
				std::vector<int> vertices_counts;
				std::vector<int> vertices;

				unsigned long release()
				{
					if (this->_is_released) return 0;

					this->semantics.clear();
					this->vertices_counts.clear();
					this->vertices.clear();

					this->_is_released = true;

					return 1;
				}

			private:
				bool _is_released = false;
			};

			struct c_skin
			{
				std::string source = "";
				glm::mat4x4 bind_shape;
				std::vector<c_source*> sources;
				std::vector<c_semantic*> joints;
				c_vertex_weight vertex_weight;

				std::vector<std::string> get_bone_names()
				{
					std::vector<std::string> _result;
					for (auto _source : this->sources)
					{
						if (_source == nullptr ||
							(_source->technique_common.param_name != "JOINT" && _source->technique_common.param_type != "IDREF"))
						{
							continue;
						}

						_result = _source->name_array;
					}
					return _result;
				}

				void set_bone_names(std::string pNames [])
				{
					for (auto _source : this->sources)
					{
						if (_source == nullptr || _source->technique_common.param_name != "JOINT") continue;
						_source->name_array = std::vector<std::string>(pNames, pNames + W_ARRAY_SIZE(pNames));
					}
				}

				std::vector<glm::mat4x4> get_inverse_bind_poses()
				{
					std::vector<glm::mat4x4> _result;
					for (auto _source : this->sources)
					{
						if (_source == nullptr ||
							(_source->technique_common.param_name != "TRANSFORM" && _source->technique_common.param_type != "float4x4"))
						{
							continue;
						}
						_result = glm::to_matrix_array(_source->float_array.data());
					}
					return _result;
				}
			};
		}
	}
}

#endif