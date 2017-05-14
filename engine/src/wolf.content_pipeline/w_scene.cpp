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

void w_scene::add_model(_In_ w_model* pModel)
{
    if (!pModel) return;

    this->_models.push_back(*pModel);
}

void w_scene::add_models(_Inout_ std::vector<w_model*>& pModel)
{
    for (size_t i = 0; i < pModel.size(); ++i)
    {
        this->_models.push_back(*pModel[i]);
    }
}

void w_scene::add_camera(_In_ w_camera* pCamera)
{
    this->_cameras.push_back(*pCamera);
}

void w_scene::add_camera(_In_z_ const std::string& pID, _In_ const glm::vec3 pTranslate, _In_ const glm::vec3 pInterest)
{
    auto _camera = new w_camera();
    _camera->set_name(pID);
    _camera->set_aspect_ratio(800.0f / 600.0f);
    _camera->set_translate(pTranslate);
    _camera->set_interest(pInterest);

    this->_cameras.push_back(*_camera);
}

ULONG w_scene::release()
{
    return 1;
}

#pragma region Getters

void w_scene::get_models_by_index(_In_ const size_t pIndex, _Inout_ w_model** pModel)
{
    if (pIndex < this->_models.size())
    {
        *pModel = &this->_models[pIndex];
    }
}

void w_scene::get_models_by_id(const std::string& pID, std::vector<w_model*>& pModels)
{
	for (size_t i = 0; i < this->_models.size(); ++i)
	{
		if (this->_models[i].get_name() == pID)
		{
			pModels.push_back(&this->_models[i]);
		}
	}
}

void w_scene::get_all_models(_Inout_ std::vector<w_model*>& pModels)
{
    if (!this->_models.size()) return;
    
    for (size_t i = 0; i < this->_models.size(); ++i)
    {
        pModels.push_back(&this->_models[i]);
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

	*pCamera = &(this->_cameras[0]);
}

void w_scene::get_cameras_by_index(_In_ const size_t pIndex, _Inout_ w_camera** pCamera)
{
    if (pIndex < this->_cameras.size())
    {
        *pCamera = &this->_cameras[pIndex];
    }
}

void w_scene::get_cameras_by_id(const std::string& pID, _Inout_ std::vector<w_camera*>& pCameras)
{
	for (size_t i = 0; i < this->_cameras.size(); ++i)
	{
		if (this->_cameras[i].get_name() == pID)
		{
			pCameras.push_back(&this->_cameras.at(i));
		}
	}
}

#pragma endregion

#pragma region Setters

void w_scene::set_coordiante_system(_In_ const bool pZUp)
{
    this->_z_up = pZUp;
}

#pragma endregion
