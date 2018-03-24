/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_cpipeline_scene.h
	Description		 : This scene class which contains all assets
	Comment          :
*/

#ifndef __W_CPIPELINE_SCENE_H__
#define __W_CPIPELINE_SCENE_H__

#include "w_cpipeline_export.h"
#include <w_object.h>
#include "w_cpipeline_model.h"
#include "collada/c_camera.h"
#include <msgpack/msgpack.hpp>
#include <w_bounding.h>

namespace wolf
{
	namespace content_pipeline
	{
		class w_cpipeline_scene
		{
		public:
			WCP_EXP w_cpipeline_scene();
			WCP_EXP virtual ~w_cpipeline_scene();
			
			WCP_EXP void add_model(_In_ w_cpipeline_model* pModel);
            WCP_EXP void add_models(_In_ std::vector<w_cpipeline_model*>& pModel);
            WCP_EXP void add_boundary(_In_ wolf::system::w_bounding_sphere* pBoundary);
            WCP_EXP void add_boundaries(_In_ std::vector<wolf::system::w_bounding_sphere*>& pBoundaries);
            WCP_EXP void add_camera(_In_ c_camera* pCamera);
			WCP_EXP void add_camera(_In_z_ const std::string& pName, _In_ const glm::vec3 pTransform, _In_ const glm::vec3 pInterest);
            
			WCP_EXP ULONG release();

#pragma region Getters

            WCP_EXP void get_model_by_index(_In_ const size_t& pIndex, _Inout_ w_cpipeline_model** pModel);
			WCP_EXP void get_models_by_id(_In_z_ const std::string& pID, _Inout_ std::vector<w_cpipeline_model*>& pModels);
            WCP_EXP void get_all_models(_Inout_ std::vector<w_cpipeline_model*>& pModels);
            
            WCP_EXP void get_boundaries(_Inout_ std::vector<wolf::system::w_bounding_sphere*>& pBoundaries);

			//Get first camera if avaible, else create a default one
			WCP_EXP void get_first_camera(_Inout_ c_camera& pCamera);
			WCP_EXP void get_cameras_by_id(_In_z_ const std::string& pID, _Inout_ std::vector<c_camera*>& pCameras);
            WCP_EXP void get_cameras_by_index(const size_t pIndex, _Inout_ c_camera** pCamera);

            //Get coordinate system
            WCP_EXP bool get_z_up()                      { return this->_z_up; }
            
            WCP_EXP std::string get_name() const         { return this->_name; }

#pragma endregion

#pragma region Setters

            WCP_EXP void set_name(_In_z_ std::string& pValue)   { this->_name = pValue; }
            WCP_EXP void set_z_up(_In_ const bool& pIsLeftHand);
            
#pragma endregion

			MSGPACK_DEFINE(_name, _cameras, _models, _boundaries, _z_up);
            
#ifdef __PYTHON__
            void py_add_model(_In_ w_cpipeline_model& pModel)
            {
                add_model(&pModel);
            }
            void py_add_boundary(_In_ wolf::system::w_bounding_sphere& pBoundary)
            {
                add_boundary(&pBoundary);
            }
            void py_add_camera_1_arg(_In_ c_camera& pCamera)
            {
                add_camera(&pCamera);
            }
            void py_add_camera_3_args(_In_z_ const std::string& pName, _In_ const glm::w_vec3 pTransform, _In_ const glm::w_vec3 pInterest)
            {
                add_camera(pName, pTransform.data(), pInterest.data());
            }
            w_cpipeline_model py_get_model_by_index(_In_ const size_t& pIndex)
            {
                w_cpipeline_model* _model = nullptr;
                
                get_model_by_index(pIndex, &_model);
                
                return *_model;
            }
            
            boost::python::list py_get_models_by_id(_In_z_ const std::string& pID)
            {
                std::vector<w_cpipeline_model*> _models;
                
                get_models_by_id(pID, _models);
                
                boost::python::list _list;
                for (size_t i = 0; i < _models.size(); ++i)
                {
                    _list.append(&_models[i]);
                }
                return _list;
            }
#endif
            
		private:
            std::string										_name;
			std::vector<c_camera>							_cameras;
            std::vector<w_cpipeline_model>					_models;
            std::vector<wolf::system::w_bounding_sphere>	_boundaries;
            bool											_z_up;
		};
	}
}

#include "python_exporter/py_cpipeline_scene.h"

#endif
