/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_scene.h
	Description		 : This scene class which contains all assets
	Comment          :
*/

#ifndef __W_SCENE_H__
#define __W_SCENE_H__

#include "w_cpipeline_export.h"
#include <w_object.h>
#include "w_model.h"
#include "w_camera.h"
#include <msgpack.hpp>

namespace wolf
{
	namespace content_pipeline
	{
        struct w_scene_pack
        {
        public:
            std::string                _scene_name = "";
            std::string                _model_json = "";
            std::string                _camera_json = "";

            MSGPACK_DEFINE(_scene_name, _model_json, _camera_json);
        };

		class w_scene : public wolf::system::w_object
		{
		public:
			WCP_EXP w_scene();
			WCP_EXP virtual ~w_scene();
			
			WCP_EXP void add_model(w_model* pModel);
            WCP_EXP void add_models(std::vector<w_model*>& pModel);
			WCP_EXP void add_camera(std::string pName, glm::vec3 pTransform, glm::vec3 pInterest);
            
#pragma region Getters

			WCP_EXP void get_all_models(_Inout_ std::vector<w_model*>& pModels);
			WCP_EXP void get_models_by_id(const std::string& pID, _Inout_ std::vector<w_model*>& pModels);

			//Get first camera if avaible, else create a default one
			WCP_EXP void get_first_or_default_camera(_Inout_ w_camera** pCamera);
			WCP_EXP void get_all_cameras(_Inout_ std::vector<w_camera*>& pCameras);
			WCP_EXP void get_cameras_by_id(const std::string& pID, _Inout_ std::vector<w_camera*>& pCameras);

            WCP_EXP HRESULT get_scene_pack(_Inout_ w_scene_pack& pScenePack);

            WCP_EXP std::wstring get_scene_name() const         { return this->_scene_name; }

#pragma endregion

#pragma region Setters

            WCP_EXP void set_name(_In_z_ std::wstring pValue)   { this->_scene_name = pValue; }

#pragma endregion
		private:
			typedef w_object            _super;

            std::wstring                _scene_name;
			std::vector<w_model*>		_models;
			std::vector<w_camera*>		_cameras;
		};
	}
}

#endif
