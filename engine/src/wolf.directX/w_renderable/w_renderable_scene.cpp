#include "w_directX_pch.h"
#include "w_renderable_scene.h"

using namespace wolf::graphics::directX;
using namespace wolf::content_pipeline;

w_renderable_scene::w_renderable_scene(_In_ wolf::content_pipeline::w_scene* pScene) :
	_scene(pScene)
{
}

void w_renderable_scene::load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
	if (!_scene) return;
	
	//get all models
	std::vector<w_model*> __models;
	_scene->get_all_models(__models);

	this->_model_counts = __models.size();
	this->_models.resize(this->_model_counts);

	for (size_t i = 0; i < this->_model_counts; ++i)
	{
		this->_models[i] = new w_renderable_model(pGDevice, __models[i]);
		if (this->_models[i]->load() == S_FALSE)
		{
			SAFE_RELEASE(this->_models[i]);
		}
	}
}


void w_renderable_scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
	for (size_t i = 0; i < this->_model_counts; ++i)
	{
		if (this->_models[i] != nullptr)
		{
			this->_models[i]->render(pGameTime);
		}
	}
}

void w_renderable_scene::set_view_projection(_In_ const DirectX::XMMATRIX pValue)
{
	for (size_t i = 0; i < this->_model_counts; ++i)
	{
		if (this->_models[i] != nullptr)
		{
			this->_models[i]->set_view_projection(pValue);
		}
	}
}

ULONG w_renderable_scene::release()
{
	if (this->is_released()) return 0;

	for (size_t i = 0; i < this->_model_counts; ++i)
	{
		SAFE_RELEASE(this->_models[i]);
	}

	this->_models.clear();

	if (_scene)
	{
		_scene->release();
	}
	return _super::release();
}

#pragma region Getters

void w_renderable_scene::get_first_or_default_camera(_Inout_ wolf::content_pipeline::w_camera** pCamera)
{
	if (this->_scene)
	{
		this->_scene->get_first_or_default_camera(pCamera);
	}
}

#pragma endregion