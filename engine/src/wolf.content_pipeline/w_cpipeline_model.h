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
#include <msgpack.hpp>
#include <map>
#include <glm/matrix.hpp>
#include <glm/gtx/compatibility.hpp>//for int4
#include "collada/c_node.h"
#include "collada/c_bone.h"
#include "collada/c_skin.h"
#include "collada/c_animation.h"
#include "w_vertex_struct.h"
#include "w_bounding.h"
#include "python_exporter/w_boost_python_helper.h"

namespace wolf
{
	namespace content_pipeline
	{
		struct w_vertex_index
		{
            uint32_t		    vertex_index;
			w_vector_float		vertex;
			w_vector_float		uv;
			w_vector_float		normal;

			ULONG release()
			{
				if (_is_released) return 1;

				vertex.clear();
				uv.clear();
				normal.clear();

				this->_is_released = true;
				return 0;
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

#ifdef __PYTHON__

			glm::w_vec3		py_get_position() { return glm::w_vec3(this->position[0], this->position[1], this->position[2]); }
			void			py_set_position(_In_ glm::w_vec3& pValue) { this->position[0] = pValue.get_x(); this->position[1] = pValue.get_y(); this->position[2] = pValue.get_z(); }

			glm::w_vec3		py_get_rotation() { return glm::w_vec3(this->rotation[0], this->rotation[1], this->rotation[2]); }
			void			py_set_rotation(_In_ glm::w_vec3& pValue) { this->rotation[0] = pValue.get_x(); this->rotation[1] = pValue.get_y(); this->rotation[2] = pValue.get_z(); }

			glm::w_vec3		py_get_scale() { return glm::w_vec3(this->scale[0], this->scale[1], this->scale[2]); }
			void			py_set_scale(_In_ glm::w_vec3& pValue) { this->scale[0] = pValue.get_x(); this->scale[1] = pValue.get_y(); this->scale[2] = pValue.get_z(); }

			glm::w_mat4x4	py_get_transform() 
			{
				auto _mat = glm::w_mat4x4();
				
				_mat.set_00(transform[0][0]);
				_mat.set_01(transform[0][1]);
				_mat.set_02(transform[0][2]);
				_mat.set_03(transform[0][3]);

				_mat.set_10(transform[1][0]);
				_mat.set_11(transform[1][1]);
				_mat.set_12(transform[1][2]);
				_mat.set_13(transform[1][3]);

				_mat.set_20(transform[2][0]);
				_mat.set_21(transform[2][1]);
				_mat.set_22(transform[2][2]);
				_mat.set_23(transform[2][3]);

				_mat.set_30(transform[3][0]);
				_mat.set_31(transform[3][1]);
				_mat.set_32(transform[3][2]);
				_mat.set_33(transform[3][3]);

				return _mat;
			}
			void			py_set_transform(_In_ glm::w_mat4x4& pValue)
			{ 
				this->transform[0][0] = pValue.get_00();
				this->transform[0][1] = pValue.get_01(); 
				this->transform[0][2] = pValue.get_02(); 
				this->transform[0][3] = pValue.get_03();

				this->transform[1][0] = pValue.get_10();
				this->transform[1][1] = pValue.get_11();
				this->transform[1][2] = pValue.get_12();
				this->transform[1][3] = pValue.get_13();

				this->transform[2][0] = pValue.get_20();
				this->transform[2][1] = pValue.get_21();
				this->transform[2][2] = pValue.get_22();
				this->transform[2][3] = pValue.get_23();

				this->transform[3][0] = pValue.get_30();
				this->transform[3][1] = pValue.get_31();
				this->transform[3][2] = pValue.get_32();
				this->transform[3][3] = pValue.get_33();

			}

#endif

		};

        WCP_EXP struct w_instance_info
        {
            std::string     name;
            float	        position[3];
            float	        rotation[3];
            float	        scale[3];

            uint32_t        texture_sampler_index = 0;

            MSGPACK_DEFINE(name, position, rotation, scale, texture_sampler_index);

#ifdef __PYTHON__
			glm::w_vec3		py_get_position() { return glm::w_vec3(this->position[0], this->position[1], this->position[2]); }
			void			py_set_position(_In_ glm::w_vec3& pValue) { this->position[0] = pValue.get_x(); this->position[1] = pValue.get_y(); this->position[2] = pValue.get_z(); }

			glm::w_vec3		py_get_rotation() { return glm::w_vec3(this->rotation[0], this->rotation[1], this->rotation[2]); }
			void			py_set_rotation(_In_ glm::w_vec3& pValue) { this->rotation[0] = pValue.get_x(); this->rotation[1] = pValue.get_y(); this->rotation[2] = pValue.get_z(); }
            
            glm::w_vec3     py_get_scale() { return glm::w_vec3(this->scale[0], this->scale[1], this->scale[2]); }
            void            py_set_scale(_In_ glm::w_vec3& pValue) { this->scale[0] = pValue.get_x(); this->scale[1] = pValue.get_y(); this->scale[2] = pValue.get_z(); }
#endif
        };

		WCP_EXP struct w_cpipeline_mesh
		{
			std::string							name;
			//posX, posY, posZ
			std::vector<w_vertex_struct>		vertices;
			w_vector_uint32_t					indices;
			//c_material*						material;
			//std::vector<c_effect*>			effects;
			std::string							textures_path;
			wolf::system::w_bounding_box		bounding_box;

			std::vector<w_vertex_struct>		lod_1_vertices;
			w_vector_uint32_t					lod_1_indices;

			void release()
			{
				this->vertices.clear();
				this->indices.clear();
				this->lod_1_vertices.clear();
				this->lod_1_indices.clear();
			}

			MSGPACK_DEFINE(vertices, indices, textures_path, bounding_box, lod_1_vertices, lod_1_indices);

#ifdef __PYTHON__

			boost::python::list					py_get_vertices()
			{
				boost::python::list _list;
				std_vector_to_boost_list(_list, this->vertices);
				return _list;
			}

			void								py_set_vertices(_In_ boost::python::list pValue)
			{
				this->vertices.clear();
				boost_list_to_std_vector(pValue, this->vertices);
			}
#endif
		};

		class w_cpipeline_model
		{
		public:
            WCP_EXP w_cpipeline_model();
			WCP_EXP w_cpipeline_model(_In_ std::vector<w_cpipeline_mesh*>& pModelMeshes);
			WCP_EXP virtual ~w_cpipeline_model();

            WCP_EXP void add_instance(_In_ const w_instance_info& pValue);
			WCP_EXP void add_lods(_In_ const std::vector<w_cpipeline_mesh*>& pLODs);
			//WCP_EXP void add_convex_hulls(_In_ const std::vector<w_cpipeline_model*>& pCHs);

			WCP_EXP void update_world();
			WCP_EXP void release();

#pragma region Getters

			WCP_EXP std::string get_name() const									{ return this->_name; }
            WCP_EXP int get_id() const                                              { return this->_id; }
            WCP_EXP bool get_is_all_sub_meshes_use_same_texture() const             { return this->_all_sub_meshes_use_same_texture;}
            WCP_EXP std::string get_instance_geometry_name()                        { return this->_instanced_geo_name; }
            //get pointer to transform structure
			WCP_EXP w_transform_info* get_transform()   						    { return &(this->_transform); }
            WCP_EXP size_t get_instances_count() const                              { return this->_instances_info.size(); }
            WCP_EXP w_instance_info* get_instance_at(_In_ const size_t& pIndex);
            WCP_EXP void get_instances(_Inout_ std::vector<w_instance_info>& pInstances);
            WCP_EXP size_t get_meshes_count();
            WCP_EXP void get_meshes(_Inout_ std::vector<w_cpipeline_mesh*>& pMeshes);
            //WCP_EXP void get_lods(_Inout_ std::vector<w_cpipeline_model*>& pLODs);
			//WCP_EXP size_t get_lods_count();
			//WCP_EXP w_cpipeline_mesh* get_lod_at(_In_ size_t& pIndex);
            /*
                convex hulls use for masked occulusion culling.
                These convex hulls exported from 3D modeling software same as following name: modelname-ch[index]
            */
			//WCP_EXP void get_convex_hulls(_Inout_ std::vector<w_cpipeline_model*>& pCHs);
            
			//bounding boxe of mesh
            WCP_EXP wolf::system::w_bounding_box* get_bounding_box(_In_ const size_t& pIndex);
            //WCP_EXP size_t get_convex_hulls_count();
            //WCP_EXP w_cpipeline_model* get_convex_hull_at(_In_ size_t pIndex);

#pragma endregion

#pragma region Setters

			WCP_EXP void set_name(_In_z_ const std::string& pValue);
            WCP_EXP void set_id(_In_z_ const int& pValue);
            WCP_EXP void set_instance_geometry_name(_In_z_ const std::string& pValue);
			//WCP_EXP void set_materials(std::vector<c_material*>& pValue);
			//WCP_EXP void set_effects(std::vector<c_effect*>& pValue);
			WCP_EXP void set_transform(_In_ const w_transform_info& pValue);

#pragma endregion

			//access to private members of instance model from static method
			//class w_cpipeline_model;
//            static w_cpipeline_model* create_model(_In_ collada::c_geometry& pGeometry,
//                _In_ collada::c_skin* pSkin,
//                _In_ std::vector<collada::c_bone*>& pBones, 
//                _In_ std::string pBoneNames [], 
//                _In_ std::map<std::string, std::string>& sLibraryMaterials,
//                _In_ std::map<std::string, std::string>& sLibraryEffects,
//                _In_ std::map<std::string, std::string>& sLibraryImages,
//                _In_ const w_transform_info& pTransform,
//                _In_ const bool& pAMDTootleOptimizing,
//#ifdef __WIN32
//                _In_ const bool& pSimplygonOptimizing,
//#endif
//                _In_ const bool& pZUp,
//                _In_ const bool& pInvertNormal);

            MSGPACK_DEFINE(_name, _instanced_geo_name, _transform, _instances_info, _bounding_box, _meshes);


#ifdef __PYTHON__

			boost::python::list py_get_instances()
			{
				boost::python::list _list;
				std::vector<w_instance_info> _instances;
				get_instances(_instances);
				if (_instances.size())
				{
					std_vector_to_boost_list(_list, _instances);
				}
				return _list;
			}

			boost::python::list py_get_meshes()
			{
				boost::python::list _list;
				std::vector<w_cpipeline_mesh*> _meshes;

				get_meshes(_meshes);
				auto _size = _meshes.size();
				if (_size)
				{
					for (size_t i = 0; i < _size; ++i)
					{
						_list.append(*_meshes[i]);
					}
				}
				return _list;
			}

			boost::python::list py_get_lods()
			{
				boost::python::list _list;
				std::vector<w_cpipeline_model*> _lods;

				get_lods(_lods);
				auto _size = _lods.size();
				if (_size)
				{
					for (size_t i = 0; i < _size; ++i)
					{
						_list.append(*_lods[i]);
					}
				}
				return _list;
			}


			boost::python::list py_get_convex_hulls()
			{
				boost::python::list _list;
				std::vector<w_cpipeline_model*> _chs;

				get_convex_hulls(_chs);
				auto _size = _chs.size();
				if (_size)
				{
					for (size_t i = 0; i < _size; ++i)
					{
						_list.append(*_chs[i]);
					}
				}
				return _list;
			}

			wolf::system::w_bounding_box py_get_bounding_box(_In_ const size_t& pIndex)
			{
				auto _bbox = get_bounding_box(pIndex);
				return *_bbox;
			}
#endif

		private:

            std::string												_name;
            int                                                     _id;
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
            //std::vector<w_cpipeline_model>                          _lods;
            //std::vector<w_cpipeline_model>                          _convex_hulls;
			std::vector<w_instance_info>							_instances_info;
			wolf::system::w_bounding_box                            _bounding_box;

			std::vector<w_cpipeline_mesh>							_meshes;

			std::vector<collada::c_bone*>							_skeleton;
			std::vector<std::string>								_bone_names;
            
            bool                                                    _all_sub_meshes_use_same_texture;

          
		};

	}
}

#include "python_exporter/py_cpipeline_model.h"

#endif
