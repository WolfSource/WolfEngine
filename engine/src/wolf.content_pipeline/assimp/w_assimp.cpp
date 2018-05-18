#include "w_cpipeline_pch.h"
#include "w_assimp.h"

#include <assimp/Importer.hpp>
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

static void _iterate_node(_In_ const aiNode* pNode,
                          _In_ std::vector<w_cpipeline_mesh*>& pModelMeshes,
                          _Inout_ w_cpipeline_scene** pScene)
{
    if(!pNode) return;
    
    if(pNode->mNumMeshes)
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
                //set transform
                w_transform_info _transform;
                _transform.position[0] = _position.x; _transform.position[1] = _position.y; _transform.position[2] = _position.z;
                _transform.rotation[0] = _rotation.x; _transform.rotation[1] = _rotation.y; _transform.rotation[2] = _rotation.z;
                _transform.scale[0] = _scaling.x; _transform.scale[1] = _scaling.y; _transform.scale[2] = _scaling.z;
                
                //could not find in models, we need to create a model
                std::vector<w_cpipeline_mesh*> _meshes = { pModelMeshes[_mesh_index] };
                
                auto _model = new w_cpipeline_model(_meshes);
                if (pNode->mParent)
                {
                    _model->set_name(pNode->mParent->mName.C_Str());
                }
                _model->set_id(_mesh_index);
                _model->set_transform(_transform);
                _scene->add_model(_model);
            }
        }
    }
    
    for (size_t i = 0 ; i < pNode->mNumChildren; ++i)
    {
        _iterate_node(pNode->mChildren[i], pModelMeshes, pScene);
    }
}

#pragma region SIMPLYGON OPTIMIZING

#ifdef __WIN32
static void _generate_simpolygon_lod()
{
//    bool _simplygon_initialized = W_PASSED;
//            simplygon_mutex.lock();
//            {
//                std::call_once(do_init_simplygon_once_over_time, []()
//                {
//                    std::wstring _simplygon_sdk;
//#ifdef _DEBUG
//                    //get simplygon SDK from dependencies\\simplygon
//                    _simplygon_sdk = wolf::system::io::get_current_directoryW() + L"..\\..\\..\\..\\engine\\dependencies\\simplygon\\";
//#else
//                    //make sure copy simplygon sdk to execute directory
//                    _simplygon_sdk = wolf::system::io::get_current_directoryW();
//#endif
//                    if (simplygon::initialize(_simplygon_sdk) == W_FAILED)
//                    {
//                        V(W_FAILED, "initializing simplygon SDK", "w_cpipeline_model::create_model", 3);
//                    }
//                });
//            }
//            simplygon_mutex.unlock();
//
//            if (simplygon::is_initialized)
//            {
//                auto _obj_name = system::convert::string_to_wstring(pGeometry.name);
//                auto _obj_path_w = wolf::system::io::get_current_directoryW() + L"/" + _obj_name + L".obj";
//                auto _obj_path = wolf::system::convert::wstring_to_string(_obj_path_w);
//                auto _obj_remeshed_path_w = wolf::system::io::get_current_directoryW() + L"/" + _obj_name + L"_remeshed.obj";
//                auto _obj_remeshed_mtl_path = wolf::system::io::get_current_directory() + "/" + pGeometry.name + "_remeshed.mtl";
//                auto _obj_remeshed_path = wolf::system::convert::wstring_to_string(_obj_remeshed_path_w);
//
//                wavefront::obj::write(_vertices_data, _indices_data, _obj_path);
//
//                //Create a original simplygon scene from current vertices and indices
//                auto _original_scene = simplygon::iSimplygonSDK->CreateScene();
//                if (_original_scene)
//                {
//                    if (simplygon::obj_reader(_obj_path_w, _original_scene) == W_PASSED)
//                    {
//                        //call simplygon for remeshing this scene and copy results to remesh scene
//                        auto _remesh_scene = simplygon::iSimplygonSDK->CreateScene();
//                        if (_remesh_scene)
//                        {
//                            if (simplygon::remeshing(_remesh_scene, _original_scene) == W_PASSED)
//                            {
//                                if (simplygon::obj_writer(_obj_remeshed_path_w.c_str(), _remesh_scene) == W_PASSED)
//                                {
//                                    //delete mtl file before loading obj file
//                                    system::io::delete_file(_obj_remeshed_mtl_path.c_str());
//                                    if (wavefront::obj::read(
//                                        _vertices_data,
//                                        _indices_data,
//                                        _vertices_positions,
//                                        _obj_remeshed_path) == W_FAILED)
//                                    {
//                                        V(W_FAILED, "error on wavefront::obj::read " + pGeometry.name, _trace_info, 3);
//                                    }
//                                }
//                                else
//                                {
//                                    V(W_FAILED, "error on simplygon::obj_writer " + pGeometry.name, _trace_info, 3);
//                                }
//                            }
//                            else
//                            {
//                                V(W_FAILED, "error on simplygon::remeshing " + pGeometry.name , _trace_info, 3);
//                            }
//                        }
//                        else
//                        {
//                            V(W_FAILED, "could not create remesh simplygon object scene for model: " + pGeometry.name, _trace_info, 3);
//                        }
//                    }
//                    else
//                    {
//                        V(W_FAILED, "error on simplygon::create_obj_scene_from_data for model: " + pGeometry.name, _trace_info, 3);
//                    }
//                }
//                else
//                {
//                    V(W_FAILED, "could not allocate memory for remesh simplygon object scene for model: " + pGeometry.name, _trace_info, 3);
//                }
//
//                system::io::delete_file(_obj_path.c_str());
//                system::io::delete_file(_obj_remeshed_path.c_str());
//
//                _obj_path.clear();
//                _obj_path_w.clear();
//
//                _obj_name.clear();
//                _obj_remeshed_path.clear();
//                _obj_remeshed_path_w.clear();
//                _obj_remeshed_mtl_path.clear();
//            }
//            else
//            {
//                V(W_FAILED, "could not initialize simplygon", _trace_info, 3);
//            }
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
    
	auto _path = wolf::system::convert::wstring_to_string(pAssetPath);
	auto _scene = _assimp_importer.ReadFile(_path.c_str(), _assimp_flags);
	if (_scene)
	{
        if (!_scene->mRootNode || !_scene->mRootNode->mNumChildren)
        {
            delete _scene;
            return nullptr;
        }
        
        
        auto _scene_name = std::string(_scene->mRootNode->mName.C_Str());
        std::transform(_scene_name.begin(), _scene_name.end(), _scene_name.begin(), ::tolower);
        
        w_coordinate_system _coordinate_system = w_coordinate_system::LEFT_HANDED;
        glm::vec3 _coordinate_system_up_vector = glm::vec3(0, 1, 0);
        if (_scene_name == "maxscene")
        {
            _coordinate_system = w_coordinate_system::RIGHT_HANDED;
            _coordinate_system_up_vector.x = 0; _coordinate_system_up_vector.y = 0; _coordinate_system_up_vector.z =1;
        }
        
        auto _w_scene = new w_cpipeline_scene(_coordinate_system, _coordinate_system_up_vector);
        _w_scene->set_name(_scene_name);
        
        //add cameras
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
        
//        if (_scene->HasLight())
//        {
//
//        }
        
        //add meshes
        std::vector<w_cpipeline_mesh*> _model_meshes;
        std::vector<float> _vertices_positions;
        if (_scene->HasMeshes())
        {
            for (size_t i = 0; i < _scene->mNumMeshes; ++i)
            {
                //clear vertex positions
                _vertices_positions.clear();
                
                auto _a_mesh = _scene->mMeshes[i];
                if (_a_mesh)
                {
                    glm::vec3 _min_vertex;
                    glm::vec3 _max_vertex;
                    
                    auto _w_mesh = new w_cpipeline_mesh();
                    //create vertices
                    for (size_t j = 0; j < _a_mesh->mNumVertices; ++j)
                    {
                        w_vertex_struct _w_vertex;
                        
                        auto _pos = _a_mesh->mVertices[j];
                        auto _normal = _a_mesh->mNormals[j];
                        auto _uv = _a_mesh->mTextureCoords[0][j];
                        
                        if(_coordinate_system == w_coordinate_system::RIGHT_HANDED)
                        {
                            _w_vertex.position[0] = _pos.x;
                            if (_coordinate_system_up_vector.z == 1)
                            {
                                _w_vertex.position[1] = -_pos.z;
                                _w_vertex.position[2] = _pos.y;
                            }
                            else
                            {
                                _w_vertex.position[1] = _pos.y;
                                _w_vertex.position[2] = _pos.z;
                            }
                        }
                        else
                        {
                            _w_vertex.position[0] = _pos.x;
                            _w_vertex.position[1] = _pos.y;
                            _w_vertex.position[2] = _pos.z;
                        }
                        
                        _w_vertex.normal[0] = _normal.x;
                        _w_vertex.normal[1] = _normal.y;
                        _w_vertex.normal[2] = _normal.z;
                        
                        _w_vertex.uv[0] = _uv.x;
                        _w_vertex.uv[1] = _uv.y;
                        
                        //add to vertices
                        _w_mesh->vertices.push_back(_w_vertex);
                        
                        if (pOptimizeMeshUsingAMDTootle)
                        {
                            //store vertex positions
                            _vertices_positions.push_back(_w_vertex.position[0]);
                            _vertices_positions.push_back(_w_vertex.position[1]);
                            _vertices_positions.push_back(_w_vertex.position[2]);
                        }
                        
                        //check for minimum and maximum vertices for bounding boxes
                        _min_vertex.x = std::min(_w_vertex.position[0], _min_vertex.x);
                        _min_vertex.y = std::min(_w_vertex.position[1], _min_vertex.y);
                        _min_vertex.z = std::min(_w_vertex.position[2], _min_vertex.z);
                        
                        _max_vertex.x = std::max(_w_vertex.position[0], _max_vertex.x);
                        _max_vertex.y = std::max(_w_vertex.position[1], _max_vertex.y);
                        _max_vertex.z = std::max(_w_vertex.position[2], _max_vertex.z);
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
                    
#ifdef __WIN32
                    if (pGenerateLODUsingSimplygon)
                    {
                        
                    }
#endif
                    
                    //apply amd tootle to it
                    if (pOptimizeMeshUsingAMDTootle)
                    {
                        amd::tootle::apply(_w_mesh->vertices, _vertices_positions, _w_mesh->indices);
                    }
                    
                    _model_meshes.push_back(_w_mesh);
                }
            }
        }

        //finally iterate over all nodes to find models and instances
        _iterate_node(_scene->mRootNode, _model_meshes, &_w_scene);
        
        return _w_scene;
    }

	return nullptr;
}

