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
                _descriptor_set(0),
                _shader_type(w_shader_type::BASIC_SHADER)
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
            
#pragma region Getters

            const w_shader_type get_shader_type() const
            {
                return this->_shader_type;
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
            
            const std::vector<w_shader_binding_param> get_shader_binding_params() const
            {
                return this->_shader_binding_params;
            }

#pragma endregion

#pragma region Setters
            
            void set_shader_type(_In_ const w_shader_type pShaderType)
            {
                this->_shader_type = pShaderType;
            }

            void set_shader_binding_param(_In_ const w_shader_binding_param& pShaderParam)
            {
                this->_shader_binding_params.push_back(pShaderParam);
            }

#pragma endregion

        private:
            std::string                                             _name;
            std::shared_ptr<w_graphics_device>                      _gDevice;
            std::vector<VkPipelineShaderStageCreateInfo>            _shader_stages;
            std::vector<VkShaderModule>                             _shader_modules;
            VkDescriptorPool                                        _descriptor_pool;
            VkDescriptorSetLayout                                   _descriptor_set_layout;
            VkDescriptorSet                                         _descriptor_set;
            w_shader_type                                           _shader_type;
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
    this->_pimp->update_descriptor_sets(pWriteDescriptorSets);
}

ULONG w_shader::release()
{
    if (_super::get_is_released()) return 0;
 
    //release the private implementation
    SAFE_RELEASE(this->_pimp);
    
    return _super::release();
}

#pragma region Getters

const w_shader_type w_shader::get_shader_type() const
{
    if (!this->_pimp) return w_shader_type::UNKNOWN;
    return this->_pimp->get_shader_type();
}

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

#pragma region Setters

void w_shader::set_shader_binding_param(_In_ const w_shader_binding_param& pShaderParam)
{
    if (!this->_pimp) return;
    this->_pimp->set_shader_binding_param(pShaderParam);
}

void w_shader::set_shader_type(_In_ const w_shader_type pShaderType)
{
    if (!this->_pimp) return;
    this->_pimp->set_shader_type(pShaderType);
}

#pragma endregion

HRESULT w_shader::load_to_shared_shaders(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_z_ const std::string& pName,
    _In_z_ const std::wstring& pVertexShaderPath,
    _In_z_ const std::wstring& pFragmentShaderPath,
    _In_ const w_shader_type& pShaderType,
    _In_ const std::vector<w_shader_binding_param>& pShaderBindings,
    _Inout_ w_shader** pShader,
    _In_z_ const char* pMainFunctionName)
{
    auto _shader = new (std::nothrow) w_shader();
    if (!_shader)
    {
        logger.error(L"Could not perform allocation for shared shader: " + pVertexShaderPath + L" , " +
            pFragmentShaderPath);
        return S_FALSE;
    }

    //set shader params
    std::for_each(pShaderBindings.begin(), pShaderBindings.end(), [_shader](_In_ const w_shader_binding_param& pParam)
    {
        _shader->set_shader_binding_param(pParam);
    });
    //set shader type
    _shader->set_shader_type(pShaderType);

    if (!pVertexShaderPath.empty())
    {
        if (_shader->load(pGDevice, pVertexShaderPath, w_shader_stage::VERTEX_SHADER, pMainFunctionName) == S_FALSE)
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