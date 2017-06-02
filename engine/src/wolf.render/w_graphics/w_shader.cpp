#include "w_render_pch.h"
#include "w_shader.h"
#include <w_io.h>
#include <w_convert.h>
#include <w_logger.h>

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
						 _In_z_ const std::wstring& pShaderBinaryPath,
                         _In_ const w_shader_stage pShaderStage,
                         _In_z_ const char* pMainFunctionName)
            {
                this->_gDevice = pGDevice;
                
#if defined(__WIN32) || defined(__UWP)
                auto _path = pShaderBinaryPath;
#else
                auto _path = wolf::system::convert::wstring_to_string(pShaderBinaryPath);
#endif
                
                std::vector<unsigned char> _shader_binary_code;
                int _file_state = 1;
                system::io::read_binary_file(_path.c_str(), _shader_binary_code, _file_state);
                if(_file_state != 1)
                {
                    if(_file_state == -1)
                    {
#if defined(__WIN32) || defined(__UWP)
						logger.error(L"Shader on following path: " + _path + L" not exists.");
#else
						logger.error("Shader on following path: " + _path + " not exists.");
#endif
                        return S_FALSE;
                    }
                    if(_file_state == -2)
                    {
#if defined(__WIN32) || defined(__UWP)
						logger.error(L"Shader on following path: " + _path + L" exists but could not open.");
#else
						logger.error("Shader on following path: " + _path + " exists but could not open.");
#endif
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
#if defined(__WIN32) || defined(__UWP)
					V(S_FALSE, L"creating shader module for shader on following path: " + _path,
						this->_name, 3, false, true);
#else
					V(S_FALSE, "creating shader module for shader on following path: " + _path,
						this->_name, 3, false, true);
#endif
                    return S_FALSE;
                }
                
                //create pipeline shader stage
                VkPipelineShaderStageCreateInfo _pipeline_shader_stage_info = {};
                _pipeline_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                _pipeline_shader_stage_info.module = _shader_module;
                _pipeline_shader_stage_info.pName = pMainFunctionName;
                
                _pipeline_shader_stage_info.stage = (VkShaderStageFlagBits)pShaderStage;
                
                this->_shader_stages.push_back(_pipeline_shader_stage_info);
                this->_shader_modules.push_back(_shader_module);
                
                return S_OK;
            }
            
            HRESULT load_shader_binding_params(_In_ std::vector<w_shader_binding_param> pShaderBindingParams)
            {
                this->_shader_binding_params.swap(pShaderBindingParams);
                if (_prepare_shader_params() == S_FALSE) return S_FALSE;
                update_shader_binding_params(this->_shader_binding_params);
                return S_OK;
            }
            
            void update_shader_binding_params(_In_ std::vector<w_shader_binding_param>& pShaderBindingParams)
            {
                this->_shader_binding_params.swap(pShaderBindingParams);

                std::vector<VkWriteDescriptorSet> _write_descriptor_sets;
                for (auto& _iter : pShaderBindingParams)
                {
                    switch (_iter.type)
                    {
                    case w_shader_binding_type::UNIFORM:
                    {
                        _write_descriptor_sets.push_back(
                        {
                            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // Type
                            nullptr,                                        // Next
                            this->_descriptor_set,                          // DstSet
                            _iter.index,                                    // DstBinding
                            0,                                              // DstArrayElement
                            1,                                              // DescriptorCount
                            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,              // DescriptorType
                            nullptr,                                        // ImageInfo
                            &_iter.uniform_info,                            // BufferInfo
                            nullptr                                         // TexelBufferView
                        });
                    }
                    break;
                    case w_shader_binding_type::SAMPLER:
                    {
                        _write_descriptor_sets.push_back(
                        {
                            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // Type
                            nullptr,                                        // Next
                            this->_descriptor_set,                          // DstSet
                            _iter.index,                                    // DstBinding
                            0,                                              // DstArrayElement
                            1,                                              // DescriptorCount
                            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,      // DescriptorType
                            &_iter.sampler_info,                            // ImageInfo
                            nullptr,
                        });
                    }
                    break;
                    }
                }

                vkUpdateDescriptorSets(this->_gDevice->vk_device,
                    static_cast<UINT32>(_write_descriptor_sets.size()),
                    _write_descriptor_sets.data(),
                    0,
                    nullptr);
            }

            ULONG release()
            {
                this->_shader_stages.clear();
                this->_shader_binding_params.clear();
                
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
            
#pragma region Getters

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
            
            const std::vector<w_shader_binding_param> get_shader_binding_params() const
            {
                return this->_shader_binding_params;
            }

#pragma endregion

        private:
            
            HRESULT _create_descriptor_pool(_In_ const std::vector<VkDescriptorPoolSize> pDescriptorPoolSize)
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
            HRESULT _create_descriptor_set_layout_binding(_In_ const std::vector<VkDescriptorSetLayoutBinding>& pDescriptorSetLayoutBinding)
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
                    V(S_FALSE, "creating descriptor set layout for graphics device :" +
                        this->_gDevice->device_name +
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
            
            HRESULT _prepare_shader_params()
            {
                HRESULT _hr = S_OK;

                uint32_t _number_of_uniforms = 0;
                uint32_t _number_of_samplers = 0;
                std::vector<VkDescriptorSetLayoutBinding> _layout_bindings;
                for (auto& _iter : this->_shader_binding_params)
                {
                    auto _binding_index = _iter.index;
                    auto _binding_type = _iter.type;
                    auto _binding_stage = _iter.stage;

                    switch (_binding_type)
                    {
                    case UNIFORM:
                    {
                        _number_of_uniforms++;
                        _layout_bindings.push_back(
                        {
                            _binding_index,                                     // Binding
                            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                  // DescriptorType
                            1,                                                  // DescriptorCount
                            (VkShaderStageFlags)_binding_stage,                 // StageFlags
                            nullptr                                             // ImmutableSamplers
                        });
                    }
                    break;
                    case SAMPLER:
                    {
                        _number_of_samplers++;
                        _layout_bindings.push_back(
                        {
                            _binding_index,                                     // Binding
                            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,          // DescriptorType
                            1,                                                  // DescriptorCount
                            (VkShaderStageFlags)_binding_stage,                 // StageFlags
                            nullptr                                             // ImmutableSamplers
                        });
                    }
                    break;
                    }
                }

                //we need one image and one uniform for basic shaders
                std::vector<VkDescriptorPoolSize> _descriptor_pool_sizes;
                if (_number_of_uniforms)
                {
                    _descriptor_pool_sizes.push_back(
                    {
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                  // Type
                        _number_of_uniforms                                 // DescriptorCount
                    });
                }
                if (_number_of_samplers)
                {
                    _descriptor_pool_sizes.push_back(
                    {
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,          // Type
                        _number_of_samplers                                 // DescriptorCount
                    });
                }

                if (_descriptor_pool_sizes.size())
                {
                    _hr = _create_descriptor_pool(_descriptor_pool_sizes);
                    if (_hr == S_FALSE)
                    {
                        logger.error("Error on creating shader descriptor pool for mesh: " +
                            this->_name);
                        return S_FALSE;
                    }
                }

                if (_layout_bindings.size())
                {
                    _hr = _create_descriptor_set_layout_binding(_layout_bindings);
                    if (_hr == S_FALSE)
                    {
                        logger.error("Error on creating shader descriptor pool for mesh: " +
                            this->_name);
                        return S_FALSE;
                    }
                }

                return _hr;
            }
            
            std::string                                             _name;
            std::shared_ptr<w_graphics_device>                      _gDevice;
            std::vector<VkPipelineShaderStageCreateInfo>            _shader_stages;
            std::vector<VkShaderModule>                             _shader_modules;
            VkDescriptorPool                                        _descriptor_pool;
            VkDescriptorSetLayout                                   _descriptor_set_layout;
            VkDescriptorSet                                         _descriptor_set;
            std::vector<w_shader_binding_param>                     _shader_binding_params;
        };
    }
}

using namespace wolf::graphics;

std::map<std::string, w_shader*> w_shader::_shared;

w_shader::w_shader() : _pimp(new w_shader_pimp())
{
    _super::set_class_name("w_shader");
}

w_shader::~w_shader()
{
    release();
}

HRESULT w_shader::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
		_In_z_ const std::wstring& pShaderBinaryPath,
        _In_ const w_shader_stage pShaderStage,
        _In_z_ const char* pMainFunctionName) 
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->load(pGDevice, pShaderBinaryPath, pShaderStage, pMainFunctionName);
}

HRESULT w_shader::load_shader_binding_params(_In_ std::vector<w_shader_binding_param> pShaderBindingParams)
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->load_shader_binding_params(pShaderBindingParams);
}

void w_shader::update_shader_binding_params(_In_ std::vector<w_shader_binding_param> pShaderBindingParams)
{
    if (!this->_pimp) return;
    return this->_pimp->update_shader_binding_params(pShaderBindingParams);
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

const std::vector<w_shader_binding_param> w_shader::get_shader_binding_params() const
{
    if (!this->_pimp) return {};
    return this->_pimp->get_shader_binding_params();
}

#pragma endregion

HRESULT w_shader::load_to_shared_shaders(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_z_ const std::string& pName,
    _In_z_ const std::wstring& pVertexShaderPath,
    _In_z_ const std::wstring& pTessellationControlShaderPath,
    _In_z_ const std::wstring& pTessellationEvaluationShaderPath,
    _In_z_ const std::wstring& pFragmentShaderPath,
    _In_ const std::vector<w_shader_binding_param> pShaderBindingParams,
    _Inout_ w_shader** pShader,
    _In_z_ const char* pMainFunctionName)
{
    auto _shader = new (std::nothrow) w_shader();
    if (!_shader)
    {
        logger.error("Could not perform allocation for shared shader name: " + pName);
        return S_FALSE;
    }

    if (!pVertexShaderPath.empty())
    {
        if (_shader->load(pGDevice, pVertexShaderPath, w_shader_stage::VERTEX_SHADER, pMainFunctionName) == S_FALSE)
        {
            return S_FALSE;
        }
    }
    if (!pTessellationControlShaderPath.empty())
    {
        if (_shader->load(pGDevice, pTessellationControlShaderPath, w_shader_stage::TESSELATION_CONTROL, pMainFunctionName) == S_FALSE)
        {
            return S_FALSE;
        }
    }
    if (!pTessellationEvaluationShaderPath.empty())
    {
        if (_shader->load(pGDevice, pTessellationEvaluationShaderPath, w_shader_stage::TESSELATION_EVALUATION, pMainFunctionName) == S_FALSE)
        {
            return S_FALSE;
        }
    }
    if (!pFragmentShaderPath.empty())
    {
        if (_shader->load(pGDevice, pFragmentShaderPath, w_shader_stage::FRAGMENT_SHADER, pMainFunctionName) == S_FALSE)
        {
            return S_FALSE;
        }
    }

    //set shader params
    if (pShaderBindingParams.size())
    {
        if(_shader->load_shader_binding_params(pShaderBindingParams) == S_FALSE)
        {
            return S_FALSE;
        }
    }
    
    //check if already exists
    auto _old_shader = get_shader_from_shared(pName);
    if (_old_shader)
    {
        logger.warning("Shader " + pName + "  already exists. The new one will be replaced with old one.");
        SAFE_RELEASE(_old_shader);
    }
    _shared[pName] = _shader;
    *pShader = _shader;

    return S_OK;
}

w_shader* w_shader::get_shader_from_shared(_In_z_ const std::string& pName)
{
    auto _iter = _shared.find(pName);
    if (_iter != _shared.end())
    {
        return _iter->second;
    }
    return nullptr;
}

ULONG w_shader::release_shared_shaders()
{
    if (!_shared.size()) return 0;

    for (auto _pair : _shared)
    {
        SAFE_RELEASE(_pair.second);
    }
    _shared.clear();
    return 1;
}
