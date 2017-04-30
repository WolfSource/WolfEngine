/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_model.h
	Description		 : The class of model used in collada
	Comment          :
*/

#ifndef __W_MODEL_H__
#define __W_MODEL_H__

#include "w_cpipeline_export.h"
#include <w_object.h>
#include <vector>
#include <map>
#include <glm/matrix.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/compatibility.hpp>//for int4
#include "collada/c_node.h"
#include "collada/c_bone.h"
#include "collada/c_skin.h"
#include "collada/c_animation.h"

namespace wolf
{
	namespace content_pipeline
	{
		struct w_vertex_index
		{
			unsigned short		vertex_index;
			std::vector<float>	vertex;
			std::vector<float>	texture;
			std::vector<float>	normal;

			ULONG release()
			{
				if (_is_released) return 0;

				vertex.clear();
				texture.clear();
				normal.clear();

				this->_is_released = true;
				return 1;
			}

		private:
			bool _is_released = false;
		};

		struct c_material : public collada::c_obj
		{
			std::string instance_effect;
		};

		struct c_effect : public collada::c_obj
		{
			std::string _initialize_from;
		};

		struct w_texture_info : public collada::c_obj
		{
			std::string file_path;
			std::string format;
			int height;
			int width;
		};

		struct w_transform_info
		{
			glm::vec3	position;
			glm::vec3	rotation;
			glm::vec3	scale;
			glm::mat4x4 transform;
		};

		struct w_vertex_data
		{
			glm::vec4		position;
			glm::vec3		normal;
			glm::vec4		blend_weight;
			glm::int4		blend_indices;
			glm::vec2		uv;
			glm::vec3		tangent;
			glm::vec3		binormal;
			glm::vec4		color;
			unsigned short	vertex_index;
		};

		class w_model : public wolf::system::w_object
		{
		public:
			WCP_EXP w_model();
			WCP_EXP virtual ~w_model();

			WCP_EXP struct w_bounding_box
			{
				glm::vec3 min;
				glm::vec3 max;
			};
			WCP_EXP struct w_mesh
			{
				//posX, posY, posZ
				std::vector<float>				just_vertices_pos;
				std::vector<w_vertex_data>		vertices;
				std::vector<UINT>               indices;
				c_material*						material;
				std::vector<c_effect*>			effects;
				std::vector<w_texture_info>		texture_infos;
				w_bounding_box					bounding_box;
			};

			WCP_EXP void update_world();
			
#pragma region Getters

			WCP_EXP std::string get_name() const										{ return this->_name; }
			WCP_EXP void get_meshes(_Inout_ std::vector<w_mesh*>& pValue) 			{ pValue = this->_meshes; }
			WCP_EXP w_transform_info get_transform() const							{ return this->_transform; }

#pragma endregion

#pragma region Setters

			WCP_EXP void set_name(const std::string& pValue);
			WCP_EXP void set_materials(std::vector<c_material*>& pValue);
			WCP_EXP void set_effects(std::vector<c_effect*>& pValue);
			WCP_EXP void set_transform(w_transform_info& pValue);

#pragma endregion

			//access to private members of instance model from static method
			//class w_model;
			static w_model* create_model(_In_ collada::c_geometry& pGeometry, _In_ collada::c_skin* pSkin,
				_In_ std::vector<collada::c_bone*>& pBones, _In_ std::string pBoneNames [], _In_ std::vector<c_material*>& pMaterials,
				_In_ std::vector<collada::c_node*>& pNodes, _In_ bool pOptimizing);

		private:
			typedef w_object _super;

			std::string												_name;

			std::vector<c_material*>								_materials;
			std::vector<c_effect*>									_effects;

			glm::mat4x4												_world;

			std::vector<glm::mat4x4>								_m_bind_pos;
			std::vector<glm::vec4>									_v_bind_pos;
			float													_animation_time;
			float													_last_animation_time;
			std::map<std::string, collada::c_animation_container>	_animation_containers;
			float													_frame_overlap;
			bool													_overlapping;
			float													_overlapping_start_time;
			std::vector<collada::c_bone*>							_temp_skeleton;
			std::vector<w_transform_info>							_instanced_transforms;
			w_transform_info										_transform;

			std::vector<w_mesh*>									_meshes;

			std::vector<collada::c_bone*>							_skeleton;
			std::vector<std::string>								_bone_names;
		};

	}
}

#endif
