#include "w_cpipeline_pch.h"
#include "w_cpipeline_scene.h"

using namespace std;
using namespace wolf::system;
using namespace wolf::content_pipeline;

w_cpipeline_scene::w_cpipeline_scene()
{
}

w_cpipeline_scene::~w_cpipeline_scene()
{
}

void w_cpipeline_scene::add_model(_In_ w_cpipeline_model* pModel)
{
    if (!pModel) return;

    this->_models.push_back(*pModel);
}

void w_cpipeline_scene::add_models(_In_ std::vector<w_cpipeline_model*>& pModel)
{
    for (size_t i = 0; i < pModel.size(); ++i)
    {
        this->_models.push_back(*pModel[i]);
    }
}

void w_cpipeline_scene::add_boundary(_In_ w_bounding_sphere* pBoundary)
{
    if (!pBoundary) return;

    this->_boundaries.push_back(*pBoundary);
}

void w_cpipeline_scene::add_boundaries(_In_ std::vector<w_bounding_sphere*>& pBoundaries)
{
    for (size_t i = 0; i < pBoundaries.size(); ++i)
    {
        this->_boundaries.push_back(*pBoundaries[i]);
    }
}

void w_cpipeline_scene::add_camera(_In_ w_camera* pCamera)
{
    this->_cameras.push_back(*pCamera);
}

ULONG w_cpipeline_scene::release()
{
    for (auto& _iter : this->_models)
    {
        _iter.release();
    }
    this->_models.clear();
    this->_boundaries.clear();
    this->_name.clear();
    this->_cameras.clear();
    return 0;
}

#pragma region Getters

void w_cpipeline_scene::get_model_by_index(_In_ const size_t& pIndex, _Inout_ w_cpipeline_model** pModel)
{
    if (pIndex < this->_models.size())
    {
        *pModel = &this->_models[pIndex];
    }
}

void w_cpipeline_scene::get_models_by_id(_In_z_ const int& pID, std::vector<w_cpipeline_model*>& pModels)
{
	for (size_t i = 0; i < this->_models.size(); ++i)
	{
		if (this->_models[i].get_id() == pID)
		{
			pModels.push_back(&this->_models[i]);
		}
	}
}

void w_cpipeline_scene::get_models_by_name(_In_z_ const std::string& pName, std::vector<w_cpipeline_model*>& pModels)
{
    for (size_t i = 0; i < this->_models.size(); ++i)
    {
        if (this->_models[i].get_name() == pName)
        {
            pModels.push_back(&this->_models[i]);
        }
    }
}

void w_cpipeline_scene::get_all_models(_Inout_ std::vector<w_cpipeline_model*>& pModels)
{
    if (!this->_models.size()) return;
    
    for (size_t i = 0; i < this->_models.size(); ++i)
    {
        pModels.push_back(&this->_models[i]);
    }
}

void w_cpipeline_scene::get_boundaries(_Inout_ std::vector<w_bounding_sphere*>& pBoundaries)
{
    if (!this->_boundaries.size()) return;

    for (size_t i = 0; i < this->_boundaries.size(); ++i)
    {
        pBoundaries.push_back(&this->_boundaries[i]);
    }
}

void w_cpipeline_scene::get_first_camera(_Inout_ w_camera& pCamera)
{
    if (this->_cameras.size() == 0) return;

    pCamera = this->_cameras[0];
}

void w_cpipeline_scene::get_cameras_by_index(_In_ const size_t pIndex, _Inout_ w_camera** pCamera)
{
    if (pIndex < this->_cameras.size())
    {
        *pCamera = &this->_cameras[pIndex];
    }
}

void w_cpipeline_scene::get_cameras_by_name(_In_z_ const std::string& pName, _Inout_ std::vector<w_camera*>& pCameras)
{
	for (size_t i = 0; i < this->_cameras.size(); ++i)
	{
		if (this->_cameras[i].get_name() == pName)
		{
			pCameras.push_back(&this->_cameras.at(i));
		}
	}
}

#pragma endregion
