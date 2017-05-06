#include "w_cpipeline_pch.h"
#include "w_scene.h"

using namespace std;
using namespace wolf::system;
using namespace wolf::content_pipeline;

w_scene::w_scene()
{

}

w_scene::~w_scene()
{
}

void w_scene::add_model(w_model* pModel)
{
	this->_models.push_back(pModel);
}

void w_scene::add_models(std::vector<w_model*>& pModel)
{
    this->_models.insert(this->_models.end(), pModel.begin(), pModel.end());
}

void w_scene::add_camera(std::string pID, glm::vec3 pTransform, glm::vec3 pInterest)
{
	auto _camera = new w_camera();
	_camera->set_name(pID);
	_camera->set_aspect_ratio(800.0f / 600.0f);
	_camera->set_transform(pTransform);
	_camera->set_interest(pInterest);

	this->_cameras.push_back(_camera);
}

#pragma region Getters

void w_scene::get_all_models(_Inout_ std::vector<w_model*>& pModels)
{
	pModels = this->_models;
}

void w_scene::get_models_by_id(const std::string& pID, std::vector<w_model*>& pModels)
{
	for (size_t i = 0; i < this->_models.size(); ++i)
	{
		if (this->_models[i]->get_name() == pID)
		{
			pModels.push_back(this->_models.at(i));
		}
	}
}

void w_scene::get_first_or_default_camera(_Inout_ w_camera** pCamera)
{
	if (this->_cameras.size() == 0)
	{
		if (*pCamera != nullptr)
		{
			*pCamera = nullptr;
		}
		*pCamera = new w_camera();
		(*pCamera)->set_aspect_ratio(800.0f / 600.0f);
		return;
	}

	*pCamera = this->_cameras.at(0);
}

void w_scene::get_all_cameras(_Inout_ std::vector<w_camera*>& pCameras)
{
	pCameras = this->_cameras;
}

void w_scene::get_cameras_by_id(const std::string& pID, _Inout_ std::vector<w_camera*>& pCameras)
{
	for (size_t i = 0; i < this->_cameras.size(); ++i)
	{
		if (this->_cameras[i]->get_name() == pID)
		{
			pCameras.push_back(this->_cameras.at(i));
		}
	}
}

HRESULT w_scene::get_scene_pack(_Inout_ w_scene_pack& pScenePack)
{
    pScenePack._scene_name = wolf::system::convert::wstring_to_string(this->_scene_name);

    return S_OK;
}

#pragma endregion
