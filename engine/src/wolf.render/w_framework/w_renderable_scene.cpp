#include "w_render_pch.h"
#include "w_renderable_scene.h"

using namespace wolf::graphics;
using namespace wolf::framework;
using namespace wolf::content_pipeline;

w_renderable_scene::w_renderable_scene(_In_ wolf::content_pipeline::w_scene* pScene) :
	_scene(pScene)
{
}

void w_renderable_scene::load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
    _In_ const w_render_pass* pRenderPass,
    _In_ const std::string& pPipelineCacheName)
{
	if (!_scene) return;
	
	//get all models
	std::vector<w_model*> __models;
	_scene->get_all_models(__models);

	this->_model_counts = __models.size();
	this->_models.resize(this->_model_counts);

	for (size_t i = 0; i < this->_model_counts; ++i)
	{
		this->_models[i] = new w_renderable_model(pGDevice, __models[i], _scene->get_z_up());
		if (this->_models[i]->load(pRenderPass, pPipelineCacheName) == S_FALSE)
		{
			SAFE_RELEASE(this->_models[i]);
		}
	}
}

void w_renderable_scene::update(_In_ const glm::mat4& pViewProjection)
{
    std::for_each(this->_models.begin(), this->_models.end(), [&pViewProjection](_In_ w_renderable_model* pModel)
    {
        pModel->update(pViewProjection);
    });
}

void w_renderable_scene::render(_In_ const VkCommandBuffer& pBuffer)
{
    std::for_each(this->_models.begin(), this->_models.end(), [&pBuffer](_In_ w_renderable_model* pModel)
    {
        pModel->render(pBuffer);
    });
}

ULONG w_renderable_scene::release()
{
	if (this->get_is_released()) return 0;

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

uint32_t w_renderable_scene::get_instances_count(_In_ const size_t pIndex) const
{
    if (pIndex < this->_models.size() && this->_models[pIndex] != nullptr)
    {
        return this->_models[pIndex]->get_instances_count();
    }
    return 0;
}

#pragma endregion

