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
        _In_z const char* pShaderBinaryPath, _In_ const w_shader_stage pShaderStage) 
{
    this->_gDevice = pGDevice;
    auto _shader_path = std::string(pShaderBinaryPath);
    
    std::vector<unsigned char> _shader_binary_code;
    int _file_state = 0;
    
    auto _hr = wolf::system::io::read_binary_file(pShaderBinaryPath,_shader_binary_code, _file_state);
    if(_file_state == -1)
    {
        logger.error("Shader on following path: " + _shader_path +  " not exists.");
        return;
    }
    if(_file_state == -2)
    {
        logger.error("Shader on following path: " + _shader_path +  " exists but could not open.");
        return;
    }
    
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
            this->_shader_module);
    
    if(_hr)
    {
        V(S_FALSE, "creating shader module for shader on following path: " + _shader_path + ".", 3, false, true);
        return;
    }
    
    return S_OK;
}

void w_shader::apply()
{

}

ULONG w_shader::release()
{
    if (_super::get_is_released()) return 0;
	
    return _super::release();
}
