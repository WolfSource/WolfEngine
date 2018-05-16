#include "w_cpipeline_pch.h"
#include "w_assimp.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

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

w_cpipeline_scene* w_assimp::load(_In_z_ const std::wstring& pAssetPath)
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
        
        auto _w_scene = new w_cpipeline_scene();
        
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
//
//        }
        
        //add meshes
        std::vector<w_cpipeline_mesh*> _model_meshes;
        if (_scene->HasMeshes())
        {
            for (size_t i = 0; i < _scene->mNumMeshes; ++i)
            {
                auto _a_mesh = _scene->mMeshes[i];
                if (_a_mesh)
                {
                    auto _w_mesh = new w_cpipeline_mesh();
                    //create vertices
                    for (size_t j = 0; j < _a_mesh->mNumVertices; ++j)
                    {
                        w_vertex_struct _w_vertex;
                        
                        auto _pos = _a_mesh->mVertices[j];
                        auto _normal = _a_mesh->mNormals[j];
                        auto _uv = _a_mesh->mTextureCoords[0][j];
                        
                        _w_vertex.position[0] = _pos.x;
                        _w_vertex.position[1] = -_pos.y;//vulkan uses a right-handed
                        _w_vertex.position[2] = _pos.z;
                        
                        _w_vertex.normal[0] = _normal.x;
                        _w_vertex.normal[1] = _normal.y;
                        _w_vertex.normal[2] = _normal.z;
                        
                        _w_vertex.uv[0] = _uv.x;
                        _w_vertex.uv[1] = _uv.y;
                        
                        //add vertex
                        _w_mesh->vertices.push_back(_w_vertex);
                    }
                    
                    // generate indices
                    for (size_t f = 0; f < _a_mesh->mNumFaces; ++f)
                    {
                        for (size_t k = 0; k < 3; ++k)
                        {
                            _w_mesh->indices.push_back(_a_mesh->mFaces[f].mIndices[k]);
                        }
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

