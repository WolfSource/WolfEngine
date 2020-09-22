#include "w_cpipeline_pch.h"
#include "w_cpipeline_model.h"

#include "amd/amd_tootle.h"
#include "simplygon/simplygon.h"

#include <mutex>
#include "wavefront/obj.h"

using namespace std;
using namespace wolf::system;
using namespace wolf::content_pipeline;
using namespace wolf::content_pipeline::collada;

static std::once_flag	do_init_simplygon_once_over_time;
static std::mutex		simplygon_mutex;

w_cpipeline_model::w_cpipeline_model() : _id(-1)
{
}

w_cpipeline_model::w_cpipeline_model(_In_ std::vector<w_cpipeline_mesh*>& pModelMeshes) :
	 _id(-1)
{
    for (auto _mesh : pModelMeshes)
    {
        this->_meshes.push_back(*_mesh);
    }
}

w_cpipeline_model::~w_cpipeline_model()
{
}

//w_cpipeline_model* w_cpipeline_model::create_model(
//    _In_ c_geometry& pGeometry, 
//    _In_ c_skin* pSkin,
//    _In_ std::vector<c_bone*>& pBones, 
//    _In_ string pBoneNames [], 
//    _In_ std::map<std::string, std::string>& sLibraryMaterials,
//    _In_ std::map<std::string, std::string>& sLibraryEffects,
//    _In_ std::map<std::string, std::string>& sLibraryImages,
//    _In_ const w_transform_info& pTransform,
//    _In_ const bool& pAMDTootleOptimizing,
//#ifdef __WIN32
//    _In_ const bool& pSimplygonOptimizing,
//#endif
//    _In_ const bool& pZUp,
//    _In_ const bool& pInvertNormal)
//{
//    const std::string _trace_info = "w_cpipeline_model::create_model";
//
//    auto _model = new w_cpipeline_model();
//    _model->set_transform(pTransform);
//    _model->update_world();
//
//    //used to find out weather all sub meshes used from same texture or not
//    _model->_all_sub_meshes_use_same_texture = true;
//    std::set<std::string> _texture_paths_temps;
//    
//    for (auto _triangle : pGeometry.triangles)
//    {
//        //find indices
//        auto _pos_index = -1;
//        auto _nor_index = -1;
//        auto _tex_index = -1;
//        auto _map_index = -1;
//        auto _ind = 0;
//
//        //find semantic resource index
//        for (auto _sem : _triangle->semantics)
//        {
//            for (int i = 0; i < pGeometry.sources.size(); ++i)
//            {
//                if (pGeometry.sources[i] && pGeometry.sources[i]->c_id != _sem->source) continue;
//
//                if (_pos_index == -1 && _sem->semantic == "POSITION")
//                {
//                    _pos_index = i;
//                }
//                else if (_nor_index == -1 && _sem->semantic == "NORMAL")
//                {
//                    _nor_index = i;
//                }
//                else if (_tex_index == -1 && _sem->semantic == "TEXCOORD")
//                {
//                    _tex_index = i;
//                }
//                else if (_map_index == -1 && _sem->semantic == "map")
//                {
//                    _map_index = i;
//                }
//                _ind++;
//            }
//        }
//
//
//        //auto _joint_index = 0;
//        //auto _weight_index = 0;
//        //auto _weight_src = -1;
//        if (pSkin != nullptr)
//        {
//            ////find transform and joint source index
//            //for (size_t i = 0; i < pSkin->source.size(); ++i)
//            //{
//            //    auto _common = pSkin->sources[i]->technique_common;
//            //    if (_common.param_name == "TRANSFORM")
//            //    {
//            //    }
//            //    else if(_common.param_name == "JOINT")
//            //    {
//            //        _model->_bone_names.swap(pSkin->sources[i]->name_array);
//            //    }
//            //    if (_common.param_name == "NULL")
//            //    {
//            //        //if (_common.accessor_source.Contains("Joints"))
//            //        //{
//            //        //    m.BoneNames = s.Sources[i].NameArray.Array;
//            //        //}
//            //    }
//            //}
//            //if (s.VertexWeight != null)
//            //{
//            //    for (var i = 0; i < s.VertexWeight.Inputs.Count; i++)
//            //    {
//            //        switch (s.VertexWeight.Inputs[i].Semantic)
//            //        {
//            //        case "JOINT":
//            //            jointIndex = i;
//            //            break;
//            //        case "WEIGHT":
//            //            weightIndex = i;
//            //            break;
//            //        }
//            //    }
//            //}
//            ////find vertex weight values index in skin sources
//            //for (var i = 0; i < s.Sources.Count; i++)
//            //{
//            //    if (s.Sources[i].TechniqueCommon.ParamName == "WEIGHT" ||
//            //        s.Sources[i].TechniqueCommon.AccessorSource.Contains("Weights"))
//            //        weightSrc = i;
//            //}
//        }
//
//        //sort bone names
//        for (auto _bone : pBones)
//        {
//            for (auto i = 0; i < _bone->flat_bones.size(); ++i)
//            {
//                if (_bone->flat_bones[i] && _bone->flat_bones[i]->c_name != "null")
//                {
//                    _model->_bone_names.push_back(_bone->flat_bones[i]->c_name);
//                }
//            }
//        }
//
//#pragma region read vertices & indices
//
//        //auto _ind2 = 0;
//        //auto _ind3 = 0;
//        auto _pos_source = pGeometry.sources[_pos_index];
//        auto _tex_source = _tex_index != -1 ? pGeometry.sources[_tex_index] : nullptr;
//        auto _nor_source = _nor_index != -1 ? pGeometry.sources[_nor_index] : nullptr;
//
//        // uint32_t _faces = 0;
//        //if (_ind)
//        //{
//        //    _faces = (static_cast<uint32_t>(_pos_source->float_array.size()) / _ind) - 2;
//        //}
//
//        std::vector<w_vertex_struct> _vertices_data;
//        std::vector<float> _vertices_positions;
//        std::vector<uint32_t> _indices_data;// (_faces * 3 * 2, -1);
//
//        //read vertices and indices
//        w_vertex_struct _vertex;
//        glm::vec3 _min_vertex;
//        glm::vec3 _max_vertex;
//
//        uint32_t _vertex_index = 0, _texcoord_index = 0, _normal_index = 0;
//        std::vector<float> _pos;
//        std::vector<float> _tex;
//        std::vector<float> _nor;
//
//        bool _append_vertex;
//        for (int i = 0; i < _triangle->indices.size(); i += _ind)
//        {
//            _append_vertex = false;
//
//            if (_pos_index != -1)
//            {
//                _vertex_index = _triangle->indices[i];
//                auto _start_index = _vertex_index * _pos_source->stride;
//                //resize destination vector
//                _pos.resize(_pos_source->stride);
//
//                auto _from = _pos_source->float_array.begin() + _start_index;
//                auto _to = _pos_source->float_array.begin() + _start_index + _pos_source->stride;
//                std::copy(_from, _to, _pos.begin());
//                
//                _append_vertex = true;
//            }
//            if (_tex_index != -1)
//            {
//                _texcoord_index = _triangle->indices[i + _tex_index];
//                auto _start_index = _texcoord_index * _tex_source->stride;
//                //resize destination vector
//                _tex.resize(_tex_source->stride);
//
//                auto _from = _tex_source->float_array.begin() + _start_index;
//                auto _to = _tex_source->float_array.begin() + _start_index + _tex_source->stride;
//                std::copy(_from, _to, _tex.begin());
//            }
//            if (_nor_index != -1)
//            {
//                _normal_index = _triangle->indices[i + _nor_index];
//                auto _start_index = _normal_index * _nor_source->stride;
//                //resize destination vector
//                _nor.resize(_nor_source->stride);
//
//                auto _from = _nor_source->float_array.begin() + _start_index;
//                auto _to = _nor_source->float_array.begin() + _start_index + _nor_source->stride;
//                std::copy(_from, _to, _nor.begin());
//            }
//
//            if (!_append_vertex) continue;
//            
//
//            if (pZUp)
//            {
//                std::swap(_pos[1], _pos[2]);
//                //_pos[1] *= -1;
//                
//                std::swap(_nor[1], _nor[2]);
//                _nor[2] = 1 - _nor[2];
//            }
//
//            if (pInvertNormal)
//            {
//                _nor[0] = 1 - _nor[0];
//                _nor[1] = 1 - _nor[1];
//                _nor[2] = 1 - _nor[2];
//            }
//
//            //check for minimum and maximum vertices for bounding boxes
//            _min_vertex.x = std::min(_pos[0], _min_vertex.x);
//            _min_vertex.y = std::min(_pos[1], _min_vertex.y);
//            _min_vertex.z = std::min(_pos[2], _min_vertex.z);
//
//            _max_vertex.x = std::max(_pos[0], _max_vertex.x);
//            _max_vertex.y = std::max(_pos[1], _max_vertex.y);
//            _max_vertex.z = std::max(_pos[2], _max_vertex.z);
//
//            if (_pos.size()) std::memcpy(&_vertex.position[0], _pos.data(), 3 * sizeof(float));
//            if (_nor.size()) std::memcpy(&_vertex.normal[0], _nor.data(), 3 * sizeof(float));
//            if (_tex.size()) std::memcpy(&_vertex.uv[0], _tex.data(), 2 * sizeof(float));
//
//            //TODO:
//            std::memset(&_vertex.blend_weight[0], -1, 4 * sizeof(float));
//            std::memset(&_vertex.blend_indices[0], -1, 4 * sizeof(int));
//
//            if (pAMDTootleOptimizing)
//            {
//                _vertices_positions.insert(_vertices_positions.end(), _pos.begin(), _pos.end());
//            }
//            //store vertices and indices
//            auto _index = _vertices_data.size();
//            _vertex.vertex_index = _index + 1;
//            _indices_data.push_back(_index);
//
//            _vertices_data.push_back(_vertex);
//
//            _pos.clear();
//            _nor.clear();
//            _tex.clear();
//        }
//
//#pragma endregion
//
//        //sort vertices
//        //std::sort(_vertices_data.begin(), _vertices_data.end(), [](_In_ const w_vertex_struct& pA, _In_ const w_vertex_struct& pB)
//        //{
//        //    return pA.vertex_index < pB.vertex_index;
//        //});
//
//        //check for round vertices to four
//        //auto _round_to_four = _vertices_data.size() % 4;
//        //if (_round_to_four)
//        //{
//        //    for (size_t i = 0; i < _round_to_four; ++i)
//        //    {
//        //        w_vertex_struct _vertex_data;
//        //        std::memset(&_vertex_data, 0, sizeof(_vertex_data));
//        //        _vertex_data.vertex_index = (uint32_t)(_vertices_data.size() + i + 1);
//        //        _vertices_data.push_back(_vertex_data);
//        //    }
//        //}
//
//#pragma region SIMPLYGON OPTIMIZING
//
//#ifdef __WIN32
//        if (pSimplygonOptimizing)
//        {
//            bool _simplygon_initialized = W_PASSED;
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
//        }
//#endif
//
//#pragma endregion
//
//#pragma region AMD TOOTLE OPTIMIZING
//        if (pAMDTootleOptimizing)
//        {
//            amd::tootle::apply(_vertices_data, _vertices_positions, _indices_data);
//        }
//#pragma endregion
//
//        if (_nor_index == -1)
//        {
//            //compute tangent
//        }
//
//        auto _mesh = new w_cpipeline_mesh();
//        auto _mat_iter = sLibraryMaterials.find(_triangle->material_name);
//        if (_mat_iter != sLibraryMaterials.end())
//        {
//            auto _effect_iter = sLibraryEffects.find(_mat_iter->second);
//            if (_effect_iter != sLibraryEffects.end())
//            {
//                auto _image_iter = sLibraryImages.find(_effect_iter->second);
//                if (_image_iter != sLibraryImages.end())
//                {
//                    _mesh->textures_path = _image_iter->second;
//
//                    if (_model->_all_sub_meshes_use_same_texture)
//                    {
//                        //check weather all meshes used from same texture or not
//                        auto _find = _texture_paths_temps.find(_mesh->textures_path);
//                        //if model contains at least two different textures
//                        if (_texture_paths_temps.size() && _find == _texture_paths_temps.end())
//                        {
//                            _model->_all_sub_meshes_use_same_texture = false;
//                        }
//                        //store it for next check
//                        _texture_paths_temps.insert(_mesh->textures_path);
//                    }
//                }
//            }
//        }
//
//        _mesh->vertices.swap(_vertices_data);
//        _mesh->indices.swap(_indices_data);
//
//        _mesh->bounding_box.min[0] = _min_vertex[0];
//        _mesh->bounding_box.min[1] = _min_vertex[1];
//        _mesh->bounding_box.min[2] = _min_vertex[2];
//
//        _mesh->bounding_box.max[0] = _max_vertex[0];
//        _mesh->bounding_box.max[1] = _max_vertex[1];
//        _mesh->bounding_box.max[2] = _max_vertex[2];
//
//        //The transform of bounding box is same as model
//        std::memcpy(&_mesh->bounding_box.position[0], &pTransform.position[0], 3 * sizeof(float));
//        std::memcpy(&_mesh->bounding_box.rotation[0], &pTransform.rotation[0], 3 * sizeof(float));
//        
//        _model->_meshes.push_back(*_mesh);
//
//        //free resources
//        if (_vertices_data.size() > 0) _vertices_data.clear();
//        if (_vertices_positions.size() > 0) _vertices_positions.clear();
//        if (_indices_data.size() > 0) _indices_data.clear();
//    }
//    
//    //clear _texture_paths_temps
//    _texture_paths_temps.clear();
//    
//    if (_model)
//    {
//        _model->_skeleton.swap(pBones);
//    }
//    
//    //logger.warning(_messages);
//    //_messages.clear();
//
//    return _model;
//}

void w_cpipeline_model::add_instance(_In_ const w_instance_info& pValue)
{
    this->_instances_info.push_back(pValue);
}

//void w_cpipeline_model::add_lods(_In_ const std::vector<w_cpipeline_mesh*>& pLODs)
//{
//	for (auto _lod : pLODs)
//	{
//		this->_lods.push_back(*_lod);
//	}
//}

//void w_cpipeline_model::add_convex_hulls(_In_ const std::vector<w_cpipeline_model*>& pCHs)
//{
//	for (auto _ch : pCHs)
//	{
//		this->_convex_hulls.push_back(*_ch);
//	}
//}

void w_cpipeline_model::update_world()
{
	this->_world = glm::make_wpv_mat(
        glm::vec3(this->_transform.scale[0], this->_transform.scale[1], this->_transform.scale[2]),
        glm::vec3(this->_transform.rotation[0], this->_transform.rotation[1], this->_transform.rotation[2]),
        glm::vec3(this->_transform.position[0], this->_transform.position[1], this->_transform.position[2]));
}

void w_cpipeline_model::release()
{
    this->_bone_names.clear();
    this->_instanced_geo_name.empty();
    this->_instances_info.clear();

    this->_m_bind_pos.clear();
    this->_v_bind_pos.clear();
    
    //release animation containers
    for (auto& iter : this->_animation_containers)
    {
        iter.second.release();
    }
    this->_animation_containers.clear();

    //release meshes
    for (auto& iter : this->_meshes)
    {
        iter.release();
    }
    this->_meshes.clear();

    //release skeleton
    for (auto& iter : this->_skeleton)
    {
        iter->release();
    }
    this->_skeleton.clear();

    //release temp skeleton
    for (auto& iter : this->_temp_skeleton)
    {
        iter->release();
    }
    this->_temp_skeleton.clear();
}

#pragma region Getters

w_instance_info* w_cpipeline_model::get_instance_at(_In_ const size_t& pIndex)
{
    return pIndex < this->_instances_info.size() ? &this->_instances_info[pIndex] : nullptr;
}

void w_cpipeline_model::get_instances(_Inout_ std::vector<w_instance_info>& pInstances)
{
    auto _size = this->_instances_info.size();
    if (!_size) return;
    
    pInstances = this->_instances_info;
}

size_t w_cpipeline_model::get_meshes_count()
{
    return  this->_meshes.size();
}

void w_cpipeline_model::get_meshes(_Inout_ std::vector<w_cpipeline_mesh*>& pMeshes)
{
    auto _size = this->_meshes.size();
    if (!_size) return;

    for (size_t i = 0; i < _size; ++i)
    {
        pMeshes.push_back(&this->_meshes[i]);
    }
}

//void w_cpipeline_model::get_lods(_Inout_ std::vector<w_cpipeline_model*>& pLODs)
//{
//    auto _size = this->_lods.size();
//    if (!_size) return;
//
//    for (size_t i = 0; i < _size; ++i)
//    {
//        pLODs.push_back(&this->_lods[i]);
//    }
//}

//void w_cpipeline_model::get_convex_hulls(_Inout_ std::vector<w_cpipeline_model*>& pCHs)
//{
//    auto _size = this->_convex_hulls.size();
//    if (!_size) return;
//
//    for (size_t i = 0; i < _size; ++i)
//    {
//        pCHs.push_back(&this->_convex_hulls[i]);
//    }
//}

w_bounding_box* w_cpipeline_model::get_bounding_box(_In_ const size_t& pIndex)
{
    return pIndex < this->_meshes.size() ? &this->_meshes[pIndex].bounding_box : nullptr;
}

//size_t w_cpipeline_model::get_convex_hulls_count()
//{
//    return this->_convex_hulls.size();
//}

//w_cpipeline_model* w_cpipeline_model::get_convex_hull_at(_In_ size_t pIndex)
//{
//    return pIndex < this->_convex_hulls.size() ? &this->_convex_hulls[pIndex] : nullptr;
//}

#pragma endregion

#pragma region Setters

void w_cpipeline_model::set_name(_In_z_ const std::string& pValue)
{
	this->_name = pValue;
}

void w_cpipeline_model::set_id(_In_ const int& pValue)
{
    this->_id = pValue;
}

void w_cpipeline_model::set_instance_geometry_name(_In_z_ const std::string& pValue)
{
    this->_instanced_geo_name = pValue;
}

void w_cpipeline_model::set_transform(_In_ const w_transform_info& pValue)
{
	this->_transform = pValue;
}

#pragma endregion
