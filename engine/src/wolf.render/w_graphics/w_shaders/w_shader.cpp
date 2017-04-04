#include "w_render_pch.h"
#include "w_shader.h"
#include <w_io.h>
#include <w_logger.h>

using namespace wolf::graphics;

w_shader::w_shader()
{
    _super::set_class_name("w_shader");
}

w_shader::~w_shader()
{
    release();
}

HRESULT w_shader::load(_In_ std::shared_ptr<w_graphics_device>& pGDevice, 
        _In_z_ const char* pShaderBinaryPath, _In_ const w_shader_stage pShaderStage,
        _In_z_ const char* pMainFunctionName) 
{
    this->_gDevice = pGDevice;
    auto _shader_path = std::string(pShaderBinaryPath);
    
    std::vector<unsigned char> _shader_binary_code;
    int _file_state = 0;
    
    wolf::system::io::read_binary_file(pShaderBinaryPath,_shader_binary_code, _file_state);
    if(_file_state != 1)
    {
        if(_file_state == -1)
        {
            logger.error("Shader on following path: " + _shader_path +  " not exists.");
            return S_FALSE;
        }
        if(_file_state == -2)
        {
            logger.error("Shader on following path: " + _shader_path +  " exists but could not open.");
            return S_FALSE;
        }
    }
    
    VkShaderModule _shader_module = VK_NULL_HANDLE;
    
    //now create shader module
    VkShaderModuleCreateInfo _shader_module_create_info = 
    {
        VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,                    // VkStructureType                sType
        nullptr,                                                        // const void                    *pNext
        0,                                                              // VkShaderModuleCreateFlags      flags
        _shader_binary_code.size(),                                     // size_t                         codeSize
        reinterpret_cast<const uint32_t*>(&_shader_binary_code[0])      // const uint32_t                *pCode
    };
    auto _hr = vkCreateShaderModule(this->_gDevice->vk_device, 
            &_shader_module_create_info, 
            nullptr, 
            &_shader_module);  
    if(_hr)
    {
        V(S_FALSE, "creating shader module for shader on following path: " + _shader_path + ".", 
                this->name, 3, false, true);
        return S_FALSE;
    }
    
    //create pipeline shader stage
    VkPipelineShaderStageCreateInfo _pipeline_shader_stage_info = {};
    _pipeline_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    _pipeline_shader_stage_info.module = _shader_module;
    _pipeline_shader_stage_info.pName = pMainFunctionName;
                   
    switch(pShaderStage)
    {
        case w_shader_stage::VERTEX_SHADER:
            _pipeline_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case w_shader_stage::FRAGMENT_SHADER:
            _pipeline_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        case w_shader_stage::GEOMETRY_SHADER:
            _pipeline_shader_stage_info.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            break;
        case w_shader_stage::TESSELATION_CONTROL:
            _pipeline_shader_stage_info.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            break;
        case w_shader_stage::TESSELATION_EVALUATION:
            _pipeline_shader_stage_info.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            break;    
        case w_shader_stage::COMPUTE_SHADER:
            _pipeline_shader_stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            break;  
        case w_shader_stage::ALL_STAGES:
            _pipeline_shader_stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            break;  
    };
    
    this->_shader_stages.push_back(_pipeline_shader_stage_info);
    this->_shader_modules.push_back(_shader_module);
    
    return S_OK;
}

void w_shader::apply()
{

}

ULONG w_shader::release()
{
    if (_super::get_is_released()) return 0;
    
    this->_shader_stages.clear();
    for(size_t i = 0; i  < this->_shader_modules.size(); ++i)
    {
        vkDestroyShaderModule(this->_gDevice->vk_device,
                this->_shader_modules[i],
                nullptr);
        this->_shader_modules[i] = VK_NULL_HANDLE;
    }
    
    return _super::release();
}
