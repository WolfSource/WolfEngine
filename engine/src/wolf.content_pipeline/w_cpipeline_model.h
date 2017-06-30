/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_cpipeline_model.h
	Description		 : The class of model used in collada
	Comment          :
*/

#ifndef __W_CPIPELINE_MODEL_H__
#define __W_CPIPELINE_MODEL_H__

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
#include "w_cpipeline_structs.h"

namespace wolf
{
	namespace content_pipeline
	{
		struct w_vertex_index
		{
            UINT		        vertex_index;
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

		//struct c_material : public collada::c_obj
		//{
		//	std::string instance_effect;
		//};

		//struct c_effect : public collada::c_obj
		//{
		//	std::string _initialize_from;
		//};

		//struct w_texture_info : public collada::c_obj
		//{
		//	std::string file_path;
		//	std::string format;
		//	int height;
		//	int width;
		//};
        
		struct w_transform_info
		{
			float	        position[3];
            float	        rotation[3];
            float	        scale[3];
			glm::mat4x4     transform;

            MSGPACK_DEFINE(position, rotation, scale);
		};

        WCP_EXP struct w_bounding_box
        {
            float           min[3];
            float           max[3];
            
            float	        position[3];
            float	        rotation[3];
            
            //used for rendering and masked occulusion culling
            std::vector<float>      vertices;
            void generate_vertices_indices()
            {
                if (vertices.size()) vertices.clear();
                vertices =
                {
                    min[0], min[1], min[2],
                    min[0], max[1], min[2],
                    max[0], max[1], min[2],
                    max[0], max[1], min[2],
                    max[0], min[1], min[2],
                    min[0], min[1], min[2],
                    min[0], min[1], max[2],
                    max[0], min[1], max[2],
                    max[0], max[1], max[2],
                    max[0], max[1], max[2],
                    min[0], max[1], max[2],
                    min[0], min[1], max[2],
                    min[0], min[1], min[2],
                    max[0], min[1], min[2],
                    max[0], min[1], max[2],
                    max[0], min[1], max[2],
                    min[0], min[1], max[2],
                    min[0], min[1], min[2],
                    max[0], min[1], min[2],
                    max[0], max[1], min[2],
                    max[0], max[1], max[2],
                    max[0], max[1], max[2],
                    max[0], min[1], max[2],
                    max[0], min[1], min[2],
                    max[0], max[1], min[2],
                    min[0], max[1], min[2],
                    min[0], max[1], max[2],
                    min[0], max[1], max[2],
                    max[0], max[1], max[2],
                    max[0], max[1], min[2],
                    min[0], max[1], min[2],
                    min[0], min[1], min[2],
                    min[0], min[1], max[2],
                    min[0], min[1], max[2],
                    min[0], max[1], max[2],
                    min[0], max[1], min[2]
                };
            }
            MSGPACK_DEFINE(min, max, position, rotation);
        };

        WCP_EXP struct w_instance_info
        {
            std::string     name;
            float	        position[3];
            float	        rotation[3];
            float	        scale = 1.0f;
            UINT            texture_sampler_index = 0;

            MSGPACK_DEFINE(name, position, rotation, scale, texture_sampler_index);
        };

		class w_cpipeline_model
		{
		public:
			WCP_EXP w_cpipeline_model();
			WCP_EXP virtual ~w_cpipeline_model();

			WCP_EXP struct w_mesh
			{
				//posX, posY, posZ
				std::vector<w_vertex_data>		vertices;
				std::vector<UINT>               indices;
				//c_material*						material;
				//std::vector<c_effect*>			effects;
				std::string		                textures_path;
				w_bounding_box		            bounding_box;

                void release()
                {
                    this->vertices.clear();
                    this->indices.clear();
                }

                MSGPACK_DEFINE(vertices, indices, textures_path, bounding_box);
			};

            WCP_EXP void add_instance(_In_ const w_instance_info pValue);
            WCP_EXP void add_lods(_Inout_ std::vector<w_cpipeline_model*>& pLODs);
            WCP_EXP void add_bounding_boxes(_Inout_ std::vector<w_bounding_box>& pBBs);
			WCP_EXP void update_world();
			WCP_EXP void release();

#pragma region Getters

			WCP_EXP std::string get_name() const									{ return this->_name; }
            WCP_EXP std::string set_instance_geometry_name()                        { return this->_instanced_geo_name; }
			WCP_EXP w_transform_info get_transform() const							{ return this->_transform; }
            WCP_EXP size_t get_instances_count() const                              { return this->_instances_info.size(); }
            WCP_EXP w_instance_info* get_instance_at(_In_ const size_t pIndex);
            WCP_EXP void get_instances(_Inout_ std::vector<w_instance_info>& pInstances);
            WCP_EXP std::string get_instance_geometry_name() const;
            WCP_EXP size_t get_meshes_count();
            WCP_EXP void get_meshes(_Inout_ std::vector<w_mesh*>& pMeshes);
            WCP_EXP void get_lods(_Inout_ std::vector<w_cpipeline_model*>& pLODs);
            /*
                bounding boxe of mesh
            */
            WCP_EXP w_bounding_box* get_mesh_bounding_box(_In_ const size_t& pIndex);
            /*
                bounding boxes which are use for masked occulusion culling.
                These bounding boxes exported from 3D modeling software same as following name: modelname-bb[index]
            */
            WCP_EXP std::vector<w_bounding_box> get_bounding_boxes() const          { return this->_bounding_boxes; }
            WCP_EXP size_t get_lods_count();
            WCP_EXP w_cpipeline_model* get_lod_at(_In_ size_t pIndex);

#pragma endregion

#pragma region Setters

			WCP_EXP void set_name(_In_z_ const std::string& pValue);
            WCP_EXP void set_instance_geometry_name(_In_z_ const std::string& pValue);
			//WCP_EXP void set_materials(std::vector<c_material*>& pValue);
			//WCP_EXP void set_effects(std::vector<c_effect*>& pValue);
			WCP_EXP void set_transform(_In_ const w_transform_info& pValue);

#pragma endregion

			//access to private members of instance model from static method
			//class w_cpipeline_model;
			static w_cpipeline_model* create_model(_In_ collada::c_geometry& pGeometry,
                _In_ collada::c_skin* pSkin,
				_In_ std::vector<collada::c_bone*>& pBones, 
                _In_ std::string pBoneNames [], 
                _In_ std::map<std::string, std::string>& sLibraryMaterials,
                _In_ std::map<std::string, std::string>& sLibraryEffects,
                _In_ std::map<std::string, std::string>& sLibraryImages,
                _In_ const w_transform_info& pTransform,
                _In_ const bool& pOptimizing,
                _In_ const bool& pZUp);

            MSGPACK_DEFINE(_name, _instanced_geo_name, _transform, _instances_info, _lods, _bounding_boxes, _meshes);

		private:

            std::string												_name;
            std::string												_instanced_geo_name;

			//std::vector<c_material*>								_materials;
			//std::vector<c_effect*>									_effects;

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
            w_transform_info										_transform;
            std::vector<w_cpipeline_model>                          _lods;
            std::vector<w_bounding_box>                             _bounding_boxes;
			std::vector<w_instance_info>							_instances_info;

			std::vector<w_mesh>								    	_meshes;

			std::vector<collada::c_bone*>							_skeleton;
			std::vector<std::string>								_bone_names;

          
		};

	}
}

#endif
