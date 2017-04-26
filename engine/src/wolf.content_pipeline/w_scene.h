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

namespace wolf
{
	namespace content_pipeline
	{
		class w_scene : public wolf::system::w_object
		{
		public:
			WCP_EXP w_scene();
			WCP_EXP virtual ~w_scene();
			
			WCP_EXP void add_model(w_model* pModel);
			WCP_EXP void add_camera(std::string pName, glm::vec3 pTransform, glm::vec3 pInterest);

#pragma region Getters

			WCP_EXP void get_all_models(_Inout_ std::vector<w_model*>& pModels);
			WCP_EXP void get_models_by_id(const std::string& pID, _Inout_ std::vector<w_model*>& pModels);

			//Get first camera if avaible, else create a default one
			WCP_EXP void get_first_or_default_camera(_Inout_ w_camera** pCamera);
			WCP_EXP void get_all_cameras(_Inout_ std::vector<w_camera*>& pCameras);
			WCP_EXP void get_cameras_by_id(const std::string& pID, _Inout_ std::vector<w_camera*>& pCameras);

#pragma endregion


		private:
			typedef w_object            _super;

			std::vector<w_model*>		_models;
			std::vector<w_camera*>		_cameras;
			
		};
	}
}

#endif
