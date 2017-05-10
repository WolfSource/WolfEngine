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
    
    std::vector<w_model::w_instance_info> _model_instances;
    this->_model->get_instances(_model_instances);
    if (_model_instances.size())
    {
        _create_instance_buffer(_model_instances.data(),
                                static_cast<UINT>(_model_instances.size() * sizeof(w_model::w_instance_info)));
    }
    
    HRESULT _hr;
	uint8_t _error = 0;
    for (size_t i = 0; i < _model_meshes.size(); ++i)
	{
        auto _model_mesh = _model_meshes[i];
        auto _mesh = new wolf::graphics::w_mesh();

        //prepare vertices
        std::vector<float> _v_data;
        for (size_t j = 0; j < _model_mesh->vertices.size(); ++j)
        {
            auto _pos = _model_mesh->vertices[j].position;
            auto _uv = _model_mesh->vertices[j].uv;
            
            _v_data.push_back(_pos[0]);
            _v_data.push_back(_pos[1]);
            _v_data.push_back(_pos[2]);

            _v_data.push_back(_uv[0]);
            _v_data.push_back(_uv[1]);
        }

        //prepare instances
        
        //load mesh
        _hr = _mesh->load(this->_gDevice,
                   _v_data.data(),
                   static_cast<UINT>(_v_data.size()),
                   static_cast<UINT>(_v_data.size() * sizeof(float)),
                   _model_mesh->indices.data(),
                   static_cast<UINT>(_model_mesh->indices.size()));

        _v_data.clear();
        if (_hr == S_FALSE)
        {
            _error += 1;
            logger.error("Could not create mesh #"+ std::to_string(i) + " for model: " + this->_model->get_name());
            continue;
        }
        
        this->_meshes.push_back(_mesh);
    }
    
    return _error > 0 ? S_FALSE : S_OK;
}

HRESULT w_renderable_model::_create_instance_buffer(_In_ const void* const pInstancedData, _In_ const UINT pInstancedSize)
{
    if (!pInstancedData || pInstancedSize == 0) return S_OK;
    
    HRESULT _hr;
    
    //create DRAM
    w_buffer _staging_buffer;
    
    _hr = _staging_buffer.load_as_staging(this->_gDevice, pInstancedSize);
    if(_hr == S_FALSE)
    {
        logger.error("Could not create staging instance buffer of model: " + this->_model->get_name());
        return _hr;
    }
    
    _hr = _staging_buffer.set_data(pInstancedData);
    if(_hr == S_FALSE)
    {
        logger.error("Setting staging instance buffer of model: " + this->_model->get_name());
        return _hr;
    }
    
    _hr = _staging_buffer.bind();
    if(_hr == S_FALSE)
    {
        logger.error("Could not bind to staging instance buffer of model: " + this->_model->get_name());
        return _hr;
    }
    
    
    // create VRAM buffers
    _hr = this->_instance_buffer.load(this->_gDevice,
                                      pInstancedSize,
                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
                                      
    if(_hr == S_FALSE)
    {
        logger.error("Could not create instance buffer of model: " + this->_model->get_name());
        return _hr;
    }
    
    _hr = this->_instance_buffer.bind();
    if(_hr == S_FALSE)
    {
        logger.error("Could not bind to instance buffer of model: " + this->_model->get_name());
        return _hr;
    }
    
    //create one command buffer
    auto _copy_command_buffer = new w_command_buffers();
    _copy_command_buffer->load(this->_gDevice, 1);
    
    _copy_command_buffer->begin(0);
    {
        auto _copy_cmd = _copy_command_buffer->get_command_at(0);
        
        VkBufferCopy _copy_region = {};
        
        _copy_region.size = pInstancedSize;
        vkCmdCopyBuffer(_copy_cmd,
                        _staging_buffer.get_handle(),
                        this->_instance_buffer.get_handle(),
                        1,
                        &_copy_region);
    }
    _copy_command_buffer->flush(0);
    
    SAFE_DELETE(_copy_command_buffer);
    
    _staging_buffer.release();
    
    return S_OK;
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
