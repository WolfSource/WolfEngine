#include "w_cpipeline_pch.h"
#include "w_assimp.h"

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <amd/amd_tootle.h>

#ifdef __WIN32

#include <simplygon/simplygon.h>
#include <mutex>
#include "wavefront/obj.h"

static std::once_flag    do_init_simplygon_once_over_time;
static std::mutex        simplygon_mutex;

#endif

using namespace assimp;
using namespace wolf;
using namespace wolf::system;
using namespace wolf::content_pipeline;

static void _iterate_node(
	_In_ const aiNode* pRootNode,
	_In_ const aiNode* pNode,
	_In_ const std::vector<w_cpipeline_mesh*>& pModelMeshes,
	_Inout_ w_cpipeline_scene** pScene,
	_Inout_ std::vector<w_cpipeline_model*>& pLODs)
	//_Inout_ std::vector<w_cpipeline_model*>& pCHs)
{
	if (!pRootNode || !pNode) return;

	if (pNode->mNumMeshes)
	{
		auto _scene = *pScene;
		std::vector<w_cpipeline_model*> _models;
		for (size_t i = 0; i < pNode->mNumMeshes; ++i)
		{
			auto _mesh_index = pNode->mMeshes[i];

			aiVector3t<float> _scaling, _rotation, _position;
			pNode->mTransformation.Decompose(_scaling, _rotation, _position);

			_models.clear();
			_scene->get_models_by_id(_mesh_index, _models);
			if (_models.size())
			{
				//if we already created a model, so add an instnace for it
				w_instance_info _instance;
				_instance.name = pNode->mName.C_Str();
				_instance.position[0] = _position.x; _instance.position[1] = _position.y; _instance.position[2] = _position.z;
				_instance.rotation[0] = _rotation.x; _instance.rotation[1] = _rotation.y; _instance.rotation[2] = _rotation.z;
				_instance.scale[0] = _scaling.x; _instance.scale[1] = _scaling.y; _instance.scale[2] = _scaling.z;

				//add to first one
				_models[0]->add_instance(_instance);
			}
			else
			{
				auto _name = pModelMeshes[_mesh_index]->name;

				//set transform
				w_transform_info _transform;
				_transform.position[0] = _position.x; _transform.position[1] = _position.y; _transform.position[2] = _position.z;
				_transform.rotation[0] = _rotation.x; _transform.rotation[1] = _rotation.y; _transform.rotation[2] = _rotation.z;
				_transform.scale[0] = _scaling.x; _transform.scale[1] = _scaling.y; _transform.scale[2] = _scaling.z;

				//could not find in models, we need to create a model
				std::vector<w_cpipeline_mesh*> _meshes = { pModelMeshes[_mesh_index] };

				auto _model = new w_cpipeline_model(_meshes);
				_model->set_name(_name);
				_model->set_id(_mesh_index);
				_model->set_transform(_transform);

				std::transform(_name.begin(), _name.end(), _name.begin(), ::tolower);
				if (_name.find("-lod") != std::string::npos)
				{
					pLODs.push_back(_model);
				}
				else
				{
					_scene->add_model(_model);
				}
			}
		}
	}

	for (size_t i = 0; i < pNode->mNumChildren; ++i)
	{
		_iterate_node(pRootNode, pNode->mChildren[i], pModelMeshes, pScene, pLODs);// , pCHs);
	}
}

#pragma region SIMPLYGON OPTIMIZING

#ifdef __WIN32

static W_RESULT _generate_simpolygon_lod(
	_In_ w_cpipeline_mesh* pMesh)
{
	if (!simplygon::is_initialized || !pMesh) return W_FAILED;

	//now store this mesh a single obj file
	auto _mesh_name = pMesh->name;
	auto _current_dir = wolf::system::io::get_current_directory();
	auto _obj_path = _current_dir + pMesh->name + ".obj";
	content_pipeline::wavefront::obj::write(pMesh->vertices, pMesh->indices, _obj_path);
	
	//Create a original simplygon scene from exported obj
	auto _original_scene = simplygon::iSimplygonSDK->CreateScene();
	if (!_original_scene)
	{
		V(W_FAILED,
			w_log_type::W_ERROR,
			"could not allocate memory of source simplygon object scene for model: {}. trace info: _generate_simpolygon_lod", _mesh_name);
		return W_FAILED;
	}

	auto _src_obj_path_w = wolf::system::convert::string_to_wstring(_obj_path);
	if (simplygon::obj_reader(_src_obj_path_w, _original_scene))
	{
		//release original scene
		_original_scene->Release();
		_original_scene = nullptr;

		V(W_FAILED,
			w_log_type::W_ERROR,
			"simplygon could not read obj {} for generating lod. trace info: _generate_simpolygon_lod", _obj_path);
		return W_FAILED;
	}

	//create output simplygon scene for storing lod
	auto _lod_scene = simplygon::iSimplygonSDK->CreateScene();
	if (!_lod_scene)
	{
		//release simplygon scenes
		_original_scene->Release();
		_original_scene = nullptr;

		V(W_FAILED,
			w_log_type::W_ERROR,
			"could not allocate memory of lod simplygon object scene for model: {}. trace info: _generate_simpolygon_lod",
			_mesh_name);
		return W_FAILED;
	}

	//generate lod
	if (simplygon::generate_lod(_lod_scene, _original_scene))
	{
		//release simplygon scenes
		_original_scene->Release();
		_original_scene = nullptr;

		_lod_scene->Release();
		_lod_scene = nullptr;

		V(W_FAILED,
			w_log_type::W_ERROR,
			"could generate lod for model: {}. trace info: _generate_simpolygon_lod",
			_mesh_name);
		return W_FAILED;
	}
	auto _base_lod_path = _current_dir + "/" + _mesh_name;
	auto _obj_lod_path = _base_lod_path + "_lod.obj";
	auto _mtl_lod_path = _base_lod_path + "_lod.mtl";

	auto _lod_path_w = wolf::system::convert::string_to_wstring(_obj_lod_path);

	//save lod as obj file
	if (simplygon::obj_writer(_lod_path_w, _lod_scene))
	{
		//release simplygon scenes
		_original_scene->Release();
		_original_scene = nullptr;

		_lod_scene->Release();
		_lod_scene = nullptr;

		V(W_FAILED,
			w_log_type::W_ERROR,
			"could write lod obj to file for model: {}. trace info: _generate_simpolygon_lod",
			_mesh_name);
		return W_FAILED;
	}

	//delete mtl file before loading obj file
	if (system::io::delete_file(_mtl_lod_path.c_str()))
	{
		V(W_FAILED, 
			w_log_type::W_WARNING, 
			"could not delete file {}. trace info: _generate_simpolygon_lod", 
			_mtl_lod_path);
	}

	W_RESULT __hr = W_PASSED;

	if (wavefront::obj::read(
		pMesh->lod_1_vertices,
		pMesh->lod_1_indices,
		_obj_lod_path))
	{
		__hr = W_FAILED;
		V(__hr,
			w_log_type::W_WARNING,
			"could not read obj lod file {}. trace info: _generate_simpolygon_lod",
			_obj_lod_path);
	}

	//delete 
	if (system::io::delete_file(_obj_path.c_str()))
	{
		V(W_FAILED, 
			w_log_type::W_WARNING, 
			"could not delete file {}. trace info: _generate_simpolygon_lod", 
			_obj_path);
	}
	if (system::io::delete_file(_obj_lod_path.c_str()))
	{
		V(W_FAILED,
			w_log_type::W_WARNING,
			"could not delete file {}. trace info: _generate_simpolygon_lod",
			_obj_lod_path);
	}

	//clear all resources
	_obj_path.clear();
	_base_lod_path.clear();
	_obj_lod_path.clear();
	_mtl_lod_path.clear();
	_src_obj_path_w.clear();

	//release simplygon scenes
	_original_scene->Release();
	_original_scene = nullptr;

	_lod_scene->Release();
	_lod_scene = nullptr;

	return W_PASSED;
}

#endif

#pragma endregion

w_cpipeline_scene* w_assimp::load(_In_z_ const std::wstring& pAssetPath,
                                  _In_ const bool& pOptimizeMeshUsingAMDTootle
#ifdef __WIN32
                                  ,_In_ const bool& pGenerateLODUsingSimplygon
#endif
)
{
	Assimp::Importer _assimp_importer;

	// Flags for loading the mesh
	static const int _assimp_flags =
		aiProcess_ValidateDataStructure			// perform a full validation of the loader's output
		| aiProcess_Triangulate					// triangulate polygons with more than 3 edges
		| aiProcess_ImproveCacheLocality		// improve the cache locality of the output vertices
		| aiProcess_RemoveRedundantMaterials	// remove redundant materials
		| aiProcess_FindDegenerates				// remove degenerated polygons from the import
		| aiProcess_FindInvalidData				// detect invalid model data, such as invalid normal vectors
		| aiProcess_OptimizeMeshes				// join small meshes, if possible
		| aiProcess_GenSmoothNormals			// generate smooth normals if needed
		| aiProcess_CalcTangentSpace			// calculate tangant and bitangent when normal presented
		| aiProcess_FindInstances				// duplicate meshes 
		| aiProcess_SplitLargeMeshes			// split large, unrenderable meshes into submeshes
		| aiProcess_MakeLeftHanded				// make left hand coordinate system
    ;
    
	//first read 3d model file
	auto _path = wolf::system::convert::wstring_to_string(pAssetPath);
	auto _scene = _assimp_importer.ReadFile(_path.c_str(), _assimp_flags);
	if (_scene)
	{
        if (!_scene->mRootNode || !_scene->mRootNode->mNumChildren)
        {
            delete _scene;
            return nullptr;
        }
        
		//convert scene model
		auto _scene_name = wolf::system::io::get_file_name(_path);
		auto _root_name = std::string(_scene->mRootNode->mName.C_Str());

		//create wolf scene
        auto _w_scene = new w_cpipeline_scene();
        _w_scene->set_name(_scene_name);
		_w_scene->set_root_name(_root_name);

        //load cameras
		if (_scene->HasCameras())
		{
            for (size_t i = 0; i < _scene->mNumCameras; ++i)
            {
                auto _a_camera = _scene->mCameras[i];
                if (_a_camera)
                {
                    //create wolf camera
                    auto _w_camera = new w_camera();
                    _w_camera->set_name(_a_camera->mName.C_Str());
                    _w_camera->set_aspect_ratio(_a_camera->mAspect);
                    _w_camera->set_near_plan(_a_camera->mClipPlaneNear);
                    _w_camera->set_far_plan(_a_camera->mClipPlaneFar);
                    _w_camera->set_position(_a_camera->mPosition.x, _a_camera->mPosition.y, _a_camera->mPosition.z);
                    _w_camera->set_look_at(_a_camera->mLookAt.x, _a_camera->mLookAt.y, _a_camera->mLookAt.z);
                    _w_camera->set_up_vector(_a_camera->mUp.x, _a_camera->mUp.y, _a_camera->mUp.z);
                    _w_camera->set_field_of_view(_a_camera->mHorizontalFOV);
                    
                    _w_scene->add_camera(_w_camera);
                }
            }
		}

		std::vector<std::string> _texture_paths;
		if (_scene->HasMaterials())
		{
			aiString _texture_path;
			const auto _texture_index = 0;
			for (size_t i = 0; i < _scene->mNumMaterials; ++i)
			{
				auto _mat = _scene->mMaterials[i];
				if (!_mat) continue;

				_mat->GetTexture(
					aiTextureType_DIFFUSE,
					_texture_index,
					&_texture_path);
				_texture_paths.push_back(_texture_path.C_Str());
			}
		}

//        if (_scene->HasLight())
//        {
//
//        }
        
        //load model meshes
        std::vector<w_cpipeline_mesh*> _model_meshes;
        
		//std::vector<float> _vertices_positions;//we will use it for amd tootle
		if (_scene->HasMeshes())
		{
			for (size_t i = 0; i < _scene->mNumMeshes; ++i)
			{
				//clear vertex positions
				//_vertices_positions.clear();

				//get each assimp meshe
				auto _a_mesh = _scene->mMeshes[i];
				if (_a_mesh)
				{
					glm::vec3 _min_vertex;
					glm::vec3 _max_vertex;

					//copy assimp mesh information to wolf mesh information
					auto _w_mesh = new w_cpipeline_mesh();
					for (size_t j = 0; j < _a_mesh->mNumVertices; ++j)
					{
						//create wolf vertex structure
						w_vertex_struct _w_vertex;

						//TODO: we need to support skinned model in next version

						//modify vertices for right handed coordinate system
						//if (_coordinate_system == w_coordinate_system::RIGHT_HANDED)
						//{
						//	if (_coordinate_system_up_vector.z == 1)
						//	{
						//		std::swap(_a_mesh->mVertices[j].z, _a_mesh->mVertices[j].y);
						//		_a_mesh->mVertices[j].y *= -1;
						//	}
						//}

						//does not have any vertex
						if (!_a_mesh->mVertices)
						{
							logger.error("{} doesn't have any vertex information", _scene_name);
							break;
						}

						auto _pos = _a_mesh->mVertices[j];
						_w_vertex.position[0] = _pos.x;
						_w_vertex.position[1] = _pos.y;
						_w_vertex.position[2] = _pos.z;

						if (_a_mesh->mNormals)
						{
							//get normal
							auto _normal = _a_mesh->mNormals[j];
							_w_vertex.normal[0] = _normal.x;
							_w_vertex.normal[1] = _normal.y;
							_w_vertex.normal[2] = _normal.z;
						}
						else
						{
							logger.warning("{} does have any vertex information", _scene_name);
						}

						//copy mesh
						if (_a_mesh->mTextureCoords && _a_mesh->mTextureCoords[0])
						{
							//get uv
							auto _uv = _a_mesh->mTextureCoords[0][j];
							_w_vertex.uv[0] = _uv.x;
							_w_vertex.uv[1] = _uv.y;
						}
						else
						{
							logger.warning("{} does have any vertex information", _scene_name);
						}

						//add to vertices
						_w_mesh->vertices.push_back(_w_vertex);

						//if (pOptimizeMeshUsingAMDTootle)
						//{
						//	//store vertex positions
						//	_vertices_positions.push_back(_w_vertex.position[0]);
						//	_vertices_positions.push_back(_w_vertex.position[1]);
						//	_vertices_positions.push_back(_w_vertex.position[2]);
						//}

						//check for minimum and maximum vertices for bounding boxes
						_min_vertex.x = std::min(_w_vertex.position[0], _min_vertex.x);
						_min_vertex.y = std::min(_w_vertex.position[1], _min_vertex.y);
						_min_vertex.z = std::min(_w_vertex.position[2], _min_vertex.z);

						_max_vertex.x = std::max(_w_vertex.position[0], _max_vertex.x);
						_max_vertex.y = std::max(_w_vertex.position[1], _max_vertex.y);
						_max_vertex.z = std::max(_w_vertex.position[2], _max_vertex.z);

						if (_a_mesh->mMaterialIndex >= 0 &&
							_a_mesh->mMaterialIndex < _texture_paths.size())
						{
							_w_mesh->textures_path = _texture_paths[_a_mesh->mMaterialIndex];
						}
					}

					// generate indices
					for (size_t f = 0; f < _a_mesh->mNumFaces; ++f)
					{
						for (size_t k = 0; k < 3; ++k)
						{
							_w_mesh->indices.push_back(_a_mesh->mFaces[f].mIndices[k]);
						}
					}

					//copy min and max to bounding box
					std::memcpy(&_w_mesh->bounding_box.min[0], &_min_vertex[0], 3 * sizeof(float));
					std::memcpy(&_w_mesh->bounding_box.max[0], &_max_vertex[0], 3 * sizeof(float));

					//apply amd tootle to it
					//if (pOptimizeMeshUsingAMDTootle)
					//{
						//amd::tootle::apply(_w_mesh->vertices, _vertices_positions, _w_mesh->indices);
					//}

					//add this mesh to others
					_w_mesh->name = _a_mesh->mName.C_Str();
					_model_meshes.push_back(_w_mesh);
				}
			}
		}

        //finally iterate over all nodes to find models and instances, LODs and CHs
		std::vector<w_cpipeline_model*> _LODs;
		//std::vector<w_cpipeline_model*> _CHs;

		_iterate_node(
			_scene->mRootNode,
			_scene->mRootNode,
			_model_meshes,
			&_w_scene,
			_LODs);
        
		//now we need assign LODs and CH to models, if model does not have LOD, we need to generate it with simpolygon (in Windows)
		std::vector<w_cpipeline_model*> _models;
		_w_scene->get_all_models(_models);

		std::vector<std::string> _splits;
		for (auto _model : _models)
		{
			auto _name = _model->get_name();

			wolf::system::convert::split_string(_name, "_", _splits);
			if (_splits.size() > 0)
			{
				auto _design_name = _splits[0];

				_splits.clear();
				wolf::system::convert::split_string(_design_name, "-", _splits);
				if (_splits.size())
				{
					_design_name = _splits[0];
				}

				int _i = 0;
				_LODs.erase(std::remove_if(_LODs.begin(), _LODs.end(),
					[&](_In_ w_cpipeline_model* pIter)
				{
					auto __name = pIter->get_name();
					if (wolf::system::convert::has_string_start_with(__name, _design_name))
					{
						std::vector<w_cpipeline_mesh*> _meshes;
						_model->get_meshes(_meshes);

						std::vector<w_cpipeline_mesh*> _lod_meshes;
						pIter->get_meshes(_lod_meshes);

						if (_meshes.size() && _lod_meshes.size())
						{
							//resize
							_meshes[0]->lod_1_vertices.resize(_lod_meshes[0]->vertices.size());
							_meshes[0]->lod_1_indices.resize(_lod_meshes[0]->indices.size());
							//copy lod vertices & indices
							_meshes[0]->lod_1_vertices.swap(_lod_meshes[0]->vertices);
							_meshes[0]->lod_1_indices.swap(_lod_meshes[0]->indices);
						}
						return true;
					}

					return false;

				}), _LODs.end());

				//_CHs.erase(std::remove_if(_CHs.begin(), _CHs.end(),
				//	[_design_name, _models, &_index_chs](_In_ w_cpipeline_model* pIter)
				//{
				//	auto __name = pIter->get_name();
				//	if (wolf::system::convert::has_string_start_with(__name, _design_name))
				//	{
				//		_index_chs.push_back(pIter);
				//		return true;
				//	}

				//	return false;

				//}), _CHs.end());

#ifdef __WIN32
				if (pGenerateLODUsingSimplygon)
				{
					//we will create automatic LOD for each mesh
					auto _number_of_meshes = _model->get_meshes_count();
					if (_number_of_meshes)
					{
						//first initialize simpolygon 
						simplygon_mutex.lock();
						{
							std::call_once(do_init_simplygon_once_over_time, []()
							{
								std::wstring _simplygon_sdk;
#ifdef _DEBUG
								//get simplygon SDK from dependencies\\simplygon
								_simplygon_sdk = wolf::system::io::get_current_directoryW() + L"..\\..\\..\\..\\engine\\dependencies\\simplygon\\";
#else
								//make sure copy simplygon sdk to execute directory
								_simplygon_sdk = wolf::system::io::get_current_directoryW();
#endif
								if (simplygon::initialize(_simplygon_sdk) == W_FAILED)
								{
									V(W_FAILED,
										w_log_type::W_ERROR,
										"could not initialize simplygon SDK. trace infor : w_assimp::load");
								}
							});
						}
						simplygon_mutex.unlock();

						std::vector<w_cpipeline_mesh*> _meshes;
						_model->get_meshes(_meshes);
						for (size_t i = 0; i < _number_of_meshes; ++i)
						{
							auto _mesh = _meshes[i];

							//if already has lod then continue
							if (_mesh->lod_1_vertices.size()) continue;

							//do not allow to create LOD for a model with 8 vertices
							if (_mesh->vertices.size() > 50)
							{
								//generate lod for it
								if (_generate_simpolygon_lod(_mesh))
								{
									V(W_FAILED,
										w_log_type::W_ERROR,
										"could not generate LOD for model {}, mesh {}. trace infor : w_assimp::load",
										_name,
										_mesh->name);
									_mesh->lod_1_vertices.clear();
									_mesh->lod_1_indices.clear();
								}
							}
						}
					}
				}
#endif
			}

			_splits.clear();
		}

		_LODs.clear();
		//_CHs.clear();
		_models.clear();

		//if (_bounds.size())
		//{
		//	//sort boundaries
		//	std::sort(_bounds.begin(), _bounds.end(), [](_In_ w_bounding_sphere* a, _In_ w_bounding_sphere* b)
		//	{
		//		return a->radius < b->radius;
		//	});

		//	pScene->add_boundaries(_bounds);
		//	_bounds.clear();
		//}


        return _w_scene;
    }

	return nullptr;
}

