#include "w_render_pch.h"
#include "w_shader.h"
#include <w_io.h>
#include <w_logger.h>

using namespace wolf::graphics;

namespace wolf
{
    namespace graphics
    {
        class w_shader_pimp
        {
        public:
            w_shader_pimp() :
                _name("w_shader"),
                _gDevice(nullptr),
                _descriptor_pool(0),
                _descriptor_set_layout(0),
                _descriptor_set(0)
            {
                
            }
            
            HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                         _In_z_ const char* pShaderBinaryPath,
                         _In_ const w_shader_stage pShaderStage,
                         _In_z_ const char* pMainFunctionName)
            {
                this->_gDevice = pGDevice;
                auto _shader_path = std::string(pShaderBinaryPath);
                
                std::vector<unsigned char> _shader_binary_code;
                int _file_state = 0;
                
                system::io::read_binary_file(pShaderBinaryPath,_shader_binary_code, _file_state);
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
                      this->_name, 3, false, true);
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
            
            HRESULT create_descriptor_pool(_In_ const std::vector<VkDescriptorPoolSize> pDescriptorPoolSize)
            {
                VkDescriptorPoolCreateInfo _descriptor_pool_create_info =
                {
                    VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,    // Type
                    nullptr,                                          // Next
                    0,                                                // Flags
                    1,                                                // MaxSets
                    static_cast<uint32_t>(pDescriptorPoolSize.size()),// PoolSizeCount
                    pDescriptorPoolSize.data()                        // PoolSizes
                };
                
                auto _hr = vkCreateDescriptorPool(this->_gDevice->vk_device,
                                                  &_descriptor_pool_create_info,
                                                  nullptr,
                                                  &this->_descriptor_pool);
                if(_hr)
                {
                    V(S_FALSE, "creating descriptor pool for graphics device: " + this->_gDevice->device_name +
                      " ID:" + std::to_string(this->_gDevice->device_id), this->_name, 3, false, true);
                    return S_FALSE;
                }
                
                return S_OK;
            }
            
            //Create desciption set layout binding
            HRESULT create_descriptor_set_layout_binding(_In_ const std::vector<VkDescriptorSetLayoutBinding>& pDescriptorSetLayoutBinding)
            {
                VkDescriptorSetLayoutCreateInfo _descriptor_set_layout_create_info =
                {
                    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,        // Type
                    nullptr,                                                    // Next
                    0,                                                          // Flags
                    static_cast<UINT32>(pDescriptorSetLayoutBinding.size()),    // BindingCount
                    pDescriptorSetLayoutBinding.data()                          // Bindings
                };
                    
                auto _hr = vkCreateDescriptorSetLayout(this->_gDevice->vk_device,
                                                       &_descriptor_set_layout_create_info,
                                                       nullptr,
                                                       &this->_descriptor_set_layout);
                if(_hr)
                {
                    V(S_FALSE, "creating descriptor set layout for graphics device :" + this->_gDevice->device_name +
                      " ID: " + std::to_string(this->_gDevice->device_id),
                      this->_name, 3,
                      false,
                      true);
                
                      return S_FALSE;
                }
                
                if(!this->_descriptor_pool)
                {
                    logger.error("Descriptor pool not exists");
                    return S_FALSE;
                
                }
                VkDescriptorSetAllocateInfo _descriptor_set_allocate_info =
                {
                    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, // Type
                    nullptr,                                        // Next
                    this->_descriptor_pool,                         // DescriptorPool
                    1,                                              // DescriptorSetCount
                    &this->_descriptor_set_layout                   // SetLayouts
                };
                
                _hr = vkAllocateDescriptorSets(this->_gDevice->vk_device,
                                               &_descriptor_set_allocate_info,
                                               &this->_descriptor_set);
                if(_hr)
                {
                    V(S_FALSE, "creating descriptor set for graphics device :" + this->_gDevice->device_name +
                      " ID: " + std::to_string(this->_gDevice->device_id),
                      this->_name, 3,
                      false,
                      true);
                    return S_FALSE;
                }
                return S_OK;
            }
            
            void update_descriptor_sets(_In_ const std::vector<VkWriteDescriptorSet>& pWriteDescriptorSets)
            {
                vkUpdateDescriptorSets(this->_gDevice->vk_device,
                                       static_cast<UINT32>(pWriteDescriptorSets.size()),
                                       pWriteDescriptorSets.data(),
                                       0,
                                       nullptr );
            }
            
            ULONG release()
            {
                this->_shader_stages.clear();
                for(size_t i = 0; i  < this->_shader_modules.size(); ++i)
                {
                    vkDestroyShaderModule(this->_gDevice->vk_device,
                                          this->_shader_modules[i],
                                          nullptr);
                    this->_shader_modules[i] = 0;
                }
                
                //release descriptor set layout
                if (this->_descriptor_set_layout)
                {
                    vkDestroyDescriptorSetLayout(this->_gDevice->vk_device,
                                                 this->_descriptor_set_layout,
                                                 nullptr);
                    this->_descriptor_set_layout= 0;
                }
                this->_descriptor_set = 0;
                
                //destroy descriptor pool
                vkDestroyDescriptorPool(this->_gDevice->vk_device,
                                        this->_descriptor_pool,
                                        nullptr);
                this->_descriptor_pool = 0;

                
                return 1;
            }
            
            
            const std::vector<VkPipelineShaderStageCreateInfo>* get_shader_stages() const
            {
                return &(this->_shader_stages);
            }
            
            const VkDescriptorSet get_descriptor_set() const
            {
                return this->_descriptor_set;
            }

            const VkDescriptorSetLayout get_descriptor_set_layout_binding() const
            {
                return this->_descriptor_set_layout;
            }
            
        private:
            std::string                                             _name;
            std::shared_ptr<w_graphics_device>                      _gDevice;
            std::vector<VkPipelineShaderStageCreateInfo>            _shader_stages;
            std::vector<VkShaderModule>                             _shader_modules;
            VkDescriptorPool                                        _descriptor_pool;
            VkDescriptorSetLayout                                   _descriptor_set_layout;
            VkDescriptorSet                                         _descriptor_set;
        };
    }
}

w_shader::w_shader() : _pimp(new w_shader_pimp())
{
    _super::set_class_name("w_shader");
}

w_shader::~w_shader()
{
    release();
}

HRESULT w_shader::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
        _In_z_ const char* pShaderBinaryPath,
        _In_ const w_shader_stage pShaderStage,
        _In_z_ const char* pMainFunctionName) 
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->load(pGDevice, pShaderBinaryPath, pShaderStage, pMainFunctionName);
}

HRESULT w_shader::create_descriptor_pool(_In_ const std::vector<VkDescriptorPoolSize> pDescriptorPoolSize)
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->create_descriptor_pool(pDescriptorPoolSize);
}

HRESULT w_shader::create_description_set_layout_binding(_In_ const std::vector<VkDescriptorSetLayoutBinding>& pDescriptorSetLayoutBinding)
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->create_descriptor_set_layout_binding(pDescriptorSetLayoutBinding);
}

void w_shader::update_descriptor_sets(_In_ const std::vector<VkWriteDescriptorSet>& pWriteDescriptorSets)
{
    if(!this->_pimp) return;
    return this->_pimp->update_descriptor_sets(pWriteDescriptorSets);
}

ULONG w_shader::release()
{
    if (_super::get_is_released()) return 0;
 
    //release the private implementation
    SAFE_RELEASE(this->_pimp);
    
    return _super::release();
}

#pragma region Getters

const std::vector<VkPipelineShaderStageCreateInfo>* w_shader::get_shader_stages() const
{
    if(!this->_pimp) return nullptr;
    return this->_pimp->get_shader_stages();
}

const VkDescriptorSet w_shader::get_descriptor_set() const
{
    if(!this->_pimp) return VkDescriptorSet();
    return this->_pimp->get_descriptor_set();
}

const VkDescriptorSetLayout w_shader::get_descriptor_set_layout_binding() const
{
    if(!this->_pimp) return VkDescriptorSetLayout();
    return this->_pimp->get_descriptor_set_layout_binding();
}

#pragma endregion
