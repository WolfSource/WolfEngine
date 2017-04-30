#include "w_render_pch.h"
#include "w_renderable_model.h"
#include <w_vertex_declaration.h>

using namespace wolf::graphics;
using namespace wolf::framework;
using namespace wolf::content_pipeline;

w_renderable_model::w_renderable_model(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
                                       w_model* pModelData) :
_gDevice(pGDevice),
_view_projection(glm::mat4x4(1)),
_model(nullptr)
{
	if (pModelData)
	{
        this->_model = new w_model();
		//copy memory
		std::memcpy((void*)this->_model, pModelData, sizeof(w_model));
	}
}

w_renderable_model::~w_renderable_model()
{
}

HRESULT w_renderable_model::load()
{
	if (this->_model == nullptr) return S_FALSE;

	//load meshes
	std::vector<w_model::w_mesh*> _model_meshes;
	this->_model->get_meshes(_model_meshes);
   
    HRESULT _hr;
	uint8_t _error = 0;
	for (size_t i = 0; i < _model_meshes.size(); ++i)
	{
        auto _mesh = new wolf::graphics::w_mesh();

        _hr = _mesh->load(this->_gDevice,
                   _model_meshes[i]->just_vertices_pos.data(),
                   static_cast<UINT>(_model_meshes[i]->just_vertices_pos.size()),
                   static_cast<UINT>(_model_meshes[i]->just_vertices_pos.size() * sizeof(float)),
                   _model_meshes[i]->indices.data(),
                   static_cast<UINT>(_model_meshes[i]->indices.size()));
        if (_hr == S_FALSE)
        {
            _error += 1;
            logger.error("Could not create mesh #"+ std::to_string(i) + " for model.");
            continue;
        }
        
        this->_meshes.push_back(_mesh);
    }
    
    return _error > 0 ? S_FALSE : S_OK;
}

ULONG w_renderable_model::release()
{
	if (this->get_is_released()) return 0;
	

	return _super::release();
}

#pragma region Setters

void w_renderable_model::set_view_projection(const glm::mat4x4 pValue)
{
	this->_view_projection = pValue;
}

#pragma endregion
