#include "w_render_pch.h"
#include "w_model.h"
#include <w_vertex_declaration.h>

using namespace wolf::graphics;
using namespace wolf::framework;
using namespace wolf::content_pipeline;

w_model::w_model(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
    _In_ w_cpipeline_model* pModelData, _In_ bool pZUp) :
_gDevice(pGDevice),
_pipeline_model(nullptr),
_z_up(pZUp)
{
	if (pModelData)
	{
        this->_pipeline_model = new w_cpipeline_model();
		//copy memory
		std::memcpy((void*)this->_pipeline_model, pModelData, sizeof(w_cpipeline_model));
	}
}

w_model::~w_model()
{
    release();
}

HRESULT w_model::load(_In_ w_shader* pShader, 
    _In_ const w_render_pass* pRenderPass, 
    _In_ const std::string& pPipelineCacheName)
{
    if (this->_pipeline_model == nullptr || pShader == nullptr) return S_FALSE;

	//load meshes
	std::vector<w_cpipeline_model::w_mesh*> _model_meshes;
	this->_pipeline_model->get_meshes(_model_meshes);
    
    std::vector<w_cpipeline_model::w_instance_info> _model_instances;
    this->_pipeline_model->get_instances(_model_instances);
    if (_model_instances.size())
    {
        _create_instances_buffer(_model_instances.data(),
            static_cast<UINT>(_model_instances.size() * sizeof(w_cpipeline_model::w_instance_info)));
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
            _v_data.push_back(1 - _uv[1]);
        }
               

        //load mesh
        _hr = _mesh->load(this->_gDevice,
                   _v_data.data(),
                   static_cast<UINT>(_v_data.size()),
                   static_cast<UINT>(_v_data.size() * sizeof(float)),
                   _model_mesh->indices.data(),
                   static_cast<UINT>(_model_mesh->indices.size()),
                   pShader,
                   pRenderPass,
                   pPipelineCacheName,
                   _z_up);

        _v_data.clear();
        if (_hr == S_FALSE)
        {
            _error += 1;
            logger.error("Could not create mesh #" + std::to_string(i) + 
                " for model: " + this->_pipeline_model->get_name());
            continue;
        }
             
        this->_meshes.push_back(_mesh);
    }
    
    return _error > 0 ? S_FALSE : S_OK;
}

HRESULT w_model::_create_instances_buffer(_In_ const void* const pInstancedData, _In_ const UINT pInstancedSize)
{
    if (!pInstancedData || pInstancedSize == 0) return S_OK;
    
    HRESULT _hr;
    
    //create DRAM
    w_buffer _staging_buffer;
    
    _hr = _staging_buffer.load_as_staging(this->_gDevice, pInstancedSize);
    if(_hr == S_FALSE)
    {
        logger.error("Could not create staging instance buffer of model: " + this->_pipeline_model->get_name());
        return _hr;
    }
    
    _hr = _staging_buffer.set_data(pInstancedData);
    if(_hr == S_FALSE)
    {
        logger.error("Setting staging instance buffer of model: " + this->_pipeline_model->get_name());
        return _hr;
    }
    
    _hr = _staging_buffer.bind();
    if(_hr == S_FALSE)
    {
        logger.error("Could not bind to staging instance buffer of model: " + this->_pipeline_model->get_name());
        return _hr;
    }
    
    
    // create VRAM buffers
    _hr = this->_instances_buffer.load(this->_gDevice,
                                       pInstancedSize,
                                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
                                      
    if(_hr == S_FALSE)
    {
        logger.error("Could not create instance buffer of model: " + this->_pipeline_model->get_name());
        return _hr;
    }
    
    _hr = this->_instances_buffer.bind();
    if(_hr == S_FALSE)
    {
        logger.error("Could not bind to instance buffer of model: " + this->_pipeline_model->get_name());
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
                        this->_instances_buffer.get_handle(),
                        1,
                        &_copy_region);
    }
    _copy_command_buffer->flush(0);
    
    SAFE_DELETE(_copy_command_buffer);
    
    _staging_buffer.release();
    
    return S_OK;
}

void w_model::render(_In_ const VkCommandBuffer& pCommandBuffer)
{
    auto _instance_count = get_instances_count();
    auto _instance_handle = this->_instances_buffer.get_handle();

    std::for_each(this->_meshes.begin(), this->_meshes.end(), [&pCommandBuffer, &_instance_handle, &_instance_count](_In_ w_mesh* pMesh)
    {
        pMesh->render(pCommandBuffer, _instance_handle, _instance_count);
    });
}

ULONG w_model::release()
{
	if (this->get_is_released()) return 0;
    
	return _super::release();
}
