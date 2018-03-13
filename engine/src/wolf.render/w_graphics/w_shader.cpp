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
				_entry_point_name(nullptr)
            {
                
            }
            
            W_RESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
						 _In_z_ const std::wstring& pShaderBinaryPath,
                         _In_ const w_shader_stage_flag_bits& pShaderStage,
                         _In_z_ const std::string& pMainFunctionName)
            {
                this->_gDevice = pGDevice;
                
                std::vector<unsigned char> _shader_binary_code;
                int _file_state = 1;
#if defined(__WIN32) || defined(__UWP)
                auto _path = pShaderBinaryPath;
                system::io::read_binary_fileW(_path.c_str(), _shader_binary_code, _file_state);
#else
                auto _path = wolf::system::convert::wstring_to_string(pShaderBinaryPath);
                system::io::read_binary_file(_path.c_str(), _shader_binary_code, _file_state);
#endif
                if(_file_state != 1)
                {
                    if(_file_state == -1)
                    {
#if defined(__WIN32) || defined(__UWP)
						logger.error(L"Shader on following path: " + _path + L" not exists.");
#else
						logger.error("Shader on following path: " + _path + " not exists.");
#endif
                        return W_FAILED;
                    }
                    if(_file_state == -2)
                    {
#if defined(__WIN32) || defined(__UWP)
						logger.error(L"Shader on following path: " + _path + L" exists but could not open.");
#else
						logger.error("Shader on following path: " + _path + " exists but could not open.");
#endif
                        return W_FAILED;
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
					V(W_FAILED, L"creating shader module for shader on following path: " + _path,
						this->_name, 3, false);
#else
					V(W_FAILED, "creating shader module for shader on following path: " + _path,
						this->_name, 3, false);
#endif
                    return W_FAILED;
                }
                
                //create pipeline shader stage
				w_pipeline_shader_stage_create_info _pipeline_shader_stage_info = {};
                _pipeline_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                _pipeline_shader_stage_info.module = _shader_module;
				_pipeline_shader_stage_info.stage = (VkShaderStageFlagBits)pShaderStage;

				this->_entry_point_name = wolf::system::convert::copy_string_to_const_char_ptr(pMainFunctionName);
				_pipeline_shader_stage_info.pName = this->_entry_point_name;

                if (pShaderStage == w_shader_stage_flag_bits::COMPUTE_SHADER)
                {
                    this->_compute_shader_stage = _pipeline_shader_stage_info;
                }
                else
                {
                    this->_shader_stages.push_back(_pipeline_shader_stage_info);
                }
                this->_shader_modules.push_back(_shader_module);
                
				
                return W_PASSED;
            }
            
            W_RESULT set_shader_binding_params(_In_ std::vector<w_shader_binding_param> pShaderBindingParams)
            {
                this->_shader_binding_params.swap(pShaderBindingParams);
                
                if (_prepare_shader_params() == W_FAILED) return W_FAILED;
                
                _update_shader_binding_params(this->_shader_binding_params);
                return W_PASSED;
            }
            
            ULONG release()
            {
                this->_shader_stages.clear();
                this->_shader_binding_params.clear();

                for (size_t i = 0; i < this->_shader_modules.size(); ++i)
                {
                    vkDestroyShaderModule(this->_gDevice->vk_device,
                        this->_shader_modules[i],
                        nullptr);
                    this->_shader_modules[i] = 0;
                }

                //release descriptor set layout
                if (this->_descriptor_set_layout.handle)
                {
                    vkDestroyDescriptorSetLayout(this->_gDevice->vk_device,
                        this->_descriptor_set_layout.handle,
                        nullptr);
                    this->_descriptor_set_layout.handle = 0;
                }
                this->_descriptor_set.handle = 0;

                //release descriptor set layout
                if (this->_compute_descriptor_set_layout.handle)
                {
                    vkDestroyDescriptorSetLayout(this->_gDevice->vk_device,
                        this->_compute_descriptor_set_layout.handle,
                        nullptr);
                    this->_compute_descriptor_set_layout.handle = 0;
                }
                this->_compute_descriptor_set.handle = 0;


                //destroy descriptor pool
                vkDestroyDescriptorPool(this->_gDevice->vk_device,
                    this->_descriptor_pool,
                    nullptr);
                this->_descriptor_pool = 0;

				if (this->_entry_point_name)
				{
					free(this->_entry_point_name);
					this->_entry_point_name = nullptr;
				}

                return 0;
            }
            
#pragma region Getters

            const std::vector<w_pipeline_shader_stage_create_info>* get_shader_stages() const
            {
                return &(this->_shader_stages);
            }
            
            const w_pipeline_shader_stage_create_info get_compute_shader_stage() const
            {
                return this->_compute_shader_stage;
            }

            const w_descriptor_set get_descriptor_set() const
            {
                return this->_descriptor_set;
            }

            const w_descriptor_set get_compute_descriptor_set() const
            {
                return this->_compute_descriptor_set;
            }

            const w_descriptor_set_layout get_descriptor_set_layout() const
            {
                return this->_descriptor_set_layout;
            }

            const w_descriptor_set_layout get_compute_descriptor_set_layout() const
            {
                return this->_compute_descriptor_set_layout;
            }
            
            const std::vector<w_shader_binding_param> get_shader_binding_params() const
            {
                return this->_shader_binding_params;
            }

#pragma endregion

        private:
			void _update_shader_binding_params(_In_ std::vector<w_shader_binding_param>& pShaderBindingParams)
			{
				std::vector<VkWriteDescriptorSet> _write_descriptor_sets;
				std::vector<VkWriteDescriptorSet> _compute_write_descriptor_sets;

				//seperated write descriptor for compute shader and other shader stages
				for (auto& _iter : pShaderBindingParams)
				{
					if (_iter.stage == w_shader_stage_flag_bits::COMPUTE_SHADER)
					{
						_create_write_descriptor_sets(
							_iter,
							this->_compute_descriptor_set.handle,
							_compute_write_descriptor_sets);
					}
					else
					{
						_create_write_descriptor_sets(
							_iter,
							this->_descriptor_set.handle,
							_write_descriptor_sets);
					}
				}

				//update descriptor sets of compute shader stage
				if (_compute_write_descriptor_sets.size())
				{
					vkUpdateDescriptorSets(this->_gDevice->vk_device,
						static_cast<uint32_t>(_compute_write_descriptor_sets.size()),
						_compute_write_descriptor_sets.data(),
						0,
						nullptr);
				}

				//update descriptor sets of all shader stages except compute shader
				if (_write_descriptor_sets.size())
				{
					vkUpdateDescriptorSets(this->_gDevice->vk_device,
						static_cast<uint32_t>(_write_descriptor_sets.size()),
						_write_descriptor_sets.data(),
						0,
						nullptr);
				}
			}

            void _create_write_descriptor_sets(
                _In_ const w_shader_binding_param& pBindingParam,
                _In_ const VkDescriptorSet& pDescriptoSet,
                _Inout_ std::vector<VkWriteDescriptorSet>& pWriteDescriptorSets)
            {
                switch (pBindingParam.type)
                {
                case w_shader_binding_type::UNIFORM:
                {
                    pWriteDescriptorSets.push_back(
                    {
                        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // Type
                        nullptr,                                        // Next
                        pDescriptoSet,                                  // DstSet
                        pBindingParam.index,                            // DstBinding
                        0,                                              // DstArrayElement
                        1,                                              // DescriptorCount
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,              // DescriptorType
                        nullptr,                                        // ImageInfo
                        &pBindingParam.buffer_info,                     // BufferInfo
                        nullptr                                         // TexelBufferView
                    });
                }
                break;
                case w_shader_binding_type::STORAGE:
                {
                    pWriteDescriptorSets.push_back(
                    {
                        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // Type
                        nullptr,                                        // Next
                        pDescriptoSet,                                  // DstSet
                        pBindingParam.index,                            // DstBinding
                        0,                                              // DstArrayElement
                        1,                                              // DescriptorCount
                        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,              // DescriptorType
                        nullptr,                                        // ImageInfo
                        &pBindingParam.buffer_info,                     // BufferInfo
                        nullptr                                         // TexelBufferView
                    });
                }
                break;
                case w_shader_binding_type::SAMPLER2D:
                {
                    pWriteDescriptorSets.push_back(
                    {
                        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // Type
                        nullptr,                                        // Next
                        pDescriptoSet,                                  // DstSet
                        pBindingParam.index,                            // DstBinding
                        0,                                              // DstArrayElement
                        1,                                              // DescriptorCount
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,      // DescriptorType
                        &pBindingParam.image_info,                      // ImageInfo
                        nullptr,
                    });
                }
                break;
				case w_shader_binding_type::IMAGE:
				{
					//Don't need sampler for IMAGE 
					auto _binding_param = pBindingParam;
					_binding_param.image_info.sampler = 0;
					pWriteDescriptorSets.push_back(
					{
						VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // Type
						nullptr,                                        // Next
						pDescriptoSet,                                  // DstSet
						pBindingParam.index,                            // DstBinding
						0,                                              // DstArrayElement
						1,                                              // DescriptorCount
						VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,				// DescriptorType
						&pBindingParam.image_info,                      // ImageInfo
						nullptr,
					});
				}
				break;
				case w_shader_binding_type::SAMPLER:
				{
					//Don't need image view for SAMPLER
					auto _binding_param = pBindingParam;
					_binding_param.image_info.imageView = 0;
					pWriteDescriptorSets.push_back(
					{
						VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // Type
						nullptr,                                        // Next
						pDescriptoSet,                                  // DstSet
						_binding_param.index,                            // DstBinding
						0,                                              // DstArrayElement
						1,                                              // DescriptorCount
						VK_DESCRIPTOR_TYPE_SAMPLER,						// DescriptorType
						&_binding_param.image_info,                     // ImageInfo
						nullptr,
					});
				}
				break;
                }
            }

            W_RESULT _create_descriptor_pool(
                _In_ const std::vector<VkDescriptorPoolSize> pDescriptorPoolSize, 
                _In_ const uint32_t& pMaxPoolSets)
            {
                VkDescriptorPoolCreateInfo _descriptor_pool_create_info =
                {
                    VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,    // Type
                    nullptr,                                          // Next
                    0,                                                // Flags
                    pMaxPoolSets,                                     // MaxSets
                    static_cast<uint32_t>(pDescriptorPoolSize.size()),// PoolSizeCount
                    pDescriptorPoolSize.data()                        // PoolSizes
                };
                
                auto _hr = vkCreateDescriptorPool(this->_gDevice->vk_device,
                                                  &_descriptor_pool_create_info,
                                                  nullptr,
                                                  &this->_descriptor_pool);
                if(_hr)
                {
                    V(W_FAILED, "creating descriptor pool for graphics device: " + this->_gDevice->device_info->get_device_name() +
                      " ID:" + std::to_string(this->_gDevice->device_info->get_device_id()), this->_name, 3, false);
                    return W_FAILED;
                }
                
                return W_PASSED;
            }
            
            void _create_descriptor_layout_bindings(
                _In_    const w_shader_binding_param& pParam,
                _Inout_ uint32_t& pNumberOfUniforms,
                _Inout_ uint32_t& pNumberOfStorages,
                _Inout_ uint32_t& pNumberOfSampler2Ds,
				_Inout_ uint32_t& pNumberOfSamplers,
				_Inout_ uint32_t& pNumberOfImages,
                _Inout_ std::vector<VkDescriptorSetLayoutBinding>& pDescriptorSetLayoutBindings)
            {
                switch (pParam.type)
                {
				case w_shader_binding_type::UNIFORM:
                {
                    pNumberOfUniforms++;
                    pDescriptorSetLayoutBindings.push_back(
                    {
                        pParam.index,                                       // Binding
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                  // DescriptorType
                        1,                                                  // DescriptorCount
                        (VkShaderStageFlags)pParam.stage,                   // StageFlags
                        nullptr                                             // ImmutableSamplers
                    });
                }
                break;
                case w_shader_binding_type::STORAGE:
                {
                    pNumberOfStorages++;
                    pDescriptorSetLayoutBindings.push_back(
                    {
                        pParam.index,                                       // Binding
                        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,                  // DescriptorType
                        1,                                                  // DescriptorCount
                        (VkShaderStageFlags)pParam.stage,                   // StageFlags
                        nullptr                                             // ImmutableSamplers
                    });
                }
                break;
                case w_shader_binding_type::SAMPLER2D:
                {
					pNumberOfSampler2Ds++;
                    pDescriptorSetLayoutBindings.push_back(
                    {
                        pParam.index,                                       // Binding
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,          // DescriptorType
                        1,                                                  // DescriptorCount
                        (VkShaderStageFlags)pParam.stage,                   // StageFlags
                        nullptr                                             // ImmutableSamplers
                    });
                }
                break;
				case w_shader_binding_type::IMAGE:
				{
					pNumberOfImages++;
					pDescriptorSetLayoutBindings.push_back(
					{
						pParam.index,                                       // Binding
						VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,					// DescriptorType
						1,                                                  // DescriptorCount
						(VkShaderStageFlags)pParam.stage,                   // StageFlags
						nullptr                                             // ImmutableSamplers
					});
				}
				break;
				case w_shader_binding_type::SAMPLER:
				{
					pNumberOfSamplers++;
					pDescriptorSetLayoutBindings.push_back(
					{
						pParam.index,                                       // Binding
						VK_DESCRIPTOR_TYPE_SAMPLER,							// DescriptorType
						1,                                                  // DescriptorCount
						(VkShaderStageFlags)pParam.stage,                   // StageFlags
						nullptr                                             // ImmutableSamplers
					});
				}
				break;
                }
            }

            W_RESULT _create_descriptor_set_layout_binding(
                _In_ const std::vector<VkDescriptorSetLayoutBinding>& pDescriptorSetLayoutBinding,
                _Inout_ VkDescriptorSet& pDescriptorSet, 
                _Inout_ VkDescriptorSetLayout& pDescriptorSetLyout)
            {
                VkDescriptorSetLayoutCreateInfo _descriptor_set_layout_create_info =
                {
                    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,        // Type
                    nullptr,                                                    // Next
                    0,                                                          // Flags
                    static_cast<uint32_t>(pDescriptorSetLayoutBinding.size()),    // BindingCount
                    pDescriptorSetLayoutBinding.data()                          // Bindings
                };
                
                auto _hr = vkCreateDescriptorSetLayout(this->_gDevice->vk_device,
                                                       &_descriptor_set_layout_create_info,
                                                       nullptr,
                                                       &pDescriptorSetLyout);
                if(_hr)
                {
                    V(W_FAILED, "creating descriptor set layout for graphics device :" +
                        this->_gDevice->device_info->get_device_name() +
                      " ID: " + std::to_string(this->_gDevice->device_info->get_device_id()),
                      this->_name, 3,
                      false);
                    
                    return W_FAILED;
                }
                
                if(!this->_descriptor_pool)
                {
                    logger.error("Descriptor pool not exists");
                    return W_FAILED;
                    
                }
                VkDescriptorSetAllocateInfo _descriptor_set_allocate_info =
                {
                    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, // Type
                    nullptr,                                        // Next
                    this->_descriptor_pool,                         // DescriptorPool
                    1,                                              // DescriptorSetCount
                    &pDescriptorSetLyout                            // SetLayouts
                };
                
                _hr = vkAllocateDescriptorSets(this->_gDevice->vk_device,
                                               &_descriptor_set_allocate_info,
                                               &pDescriptorSet);
                if(_hr)
                {
                    V(W_FAILED, "creating descriptor set for graphics device :" + this->_gDevice->device_info->get_device_name() +
                      " ID: " + std::to_string(this->_gDevice->device_info->get_device_id()),
                      this->_name, 3,
                      false);
                    return W_FAILED;
                }
                return W_PASSED;
            }
            
            W_RESULT _prepare_shader_params()
            {
                W_RESULT _hr = W_PASSED;

                uint32_t _number_of_uniforms = 0;
                uint32_t _number_of_storages = 0;
                uint32_t _number_of_sampler2ds = 0;
				uint32_t _number_of_images = 0;
				uint32_t _number_of_samplers = 0;

                std::vector<VkDescriptorSetLayoutBinding> _layout_bindings;
                std::vector<VkDescriptorSetLayoutBinding> _compute_layout_bindings;

                for (auto& _iter : this->_shader_binding_params)
                {
                    if (_iter.stage == w_shader_stage_flag_bits::COMPUTE_SHADER)
                    {
                        _create_descriptor_layout_bindings(
                            _iter,
                            _number_of_uniforms,
                            _number_of_storages,
                            _number_of_sampler2ds,
							_number_of_samplers,
							_number_of_images,
                            _compute_layout_bindings);
                    }
                    else
                    {
                        _create_descriptor_layout_bindings(
                            _iter,
							_number_of_uniforms,
							_number_of_storages,
							_number_of_sampler2ds,
							_number_of_samplers,
							_number_of_images,
                            _layout_bindings);
                    }
                }

                std::vector<VkDescriptorPoolSize> _descriptor_pool_sizes;
                if (_number_of_uniforms)
                {
                    _descriptor_pool_sizes.push_back(
                    {
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                  // Type
                        _number_of_uniforms                                 // DescriptorCount
                    });
                }
                if (_number_of_storages)
                {
                    _descriptor_pool_sizes.push_back(
                    {
                        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,                  // Type
                        _number_of_storages                                 // DescriptorCount
                    });
                }
                if (_number_of_sampler2ds)
                {
                    _descriptor_pool_sizes.push_back(
                    {
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,          // Type
                        _number_of_sampler2ds                               // DescriptorCount
                    });
                }
				if (_number_of_samplers)
				{
					_descriptor_pool_sizes.push_back(
					{
						VK_DESCRIPTOR_TYPE_SAMPLER,							// Type
						_number_of_samplers                                 // DescriptorCount
					});
				}
				if (_number_of_images)
				{
					_descriptor_pool_sizes.push_back(
					{
						VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,					// Type
						_number_of_images                                   // DescriptorCount
					});
				}

                if (_descriptor_pool_sizes.size())
                {
                    _hr = _create_descriptor_pool(
                        _descriptor_pool_sizes,
                        _compute_layout_bindings.size() ? 2 : 1);
                    if (_hr == W_FAILED)
                    {
                        logger.error("Error on creating shader descriptor pool for mesh: " +
                            this->_name);
                        return W_FAILED;
                    }
                }

                if (_layout_bindings.size())
                {
                    _hr = _create_descriptor_set_layout_binding(
                        _layout_bindings, 
                        this->_descriptor_set.handle, 
                        this->_descriptor_set_layout.handle);
                    if (_hr == W_FAILED)
                    {
                        logger.error("Error on creating shader descriptor pool for mesh: " +
                            this->_name);
                        return W_FAILED;
                    }
                }
                if (_compute_layout_bindings.size())
                {
                    _hr = _create_descriptor_set_layout_binding(
                        _compute_layout_bindings, 
                        this->_compute_descriptor_set.handle, 
                        this->_compute_descriptor_set_layout.handle);
                    if (_hr == W_FAILED)
                    {
                        logger.error("Error on creating shader descriptor pool for mesh: " +
                            this->_name);
                        return W_FAILED;
                    }
                }

                return _hr;
            }
            
            std::string                                             _name;
            std::shared_ptr<w_graphics_device>                      _gDevice;
            std::vector<w_pipeline_shader_stage_create_info>        _shader_stages;
			w_pipeline_shader_stage_create_info						_compute_shader_stage;
            std::vector<VkShaderModule>                             _shader_modules;
            VkDescriptorPool                                        _descriptor_pool;
            w_descriptor_set_layout                                 _descriptor_set_layout;
            w_descriptor_set_layout                                 _compute_descriptor_set_layout;
			w_descriptor_set                                        _descriptor_set;
			w_descriptor_set                                        _compute_descriptor_set;
            std::vector<w_shader_binding_param>                     _shader_binding_params;
			char*													_entry_point_name;
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

W_RESULT w_shader::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_z_ const std::wstring& pShaderBinaryPath,
	_In_ const w_shader_stage_flag_bits& pShaderStage,
	_In_z_ const std::string& pMainFunctionName)
{
	if (!this->_pimp) return W_FAILED;

	_super::load_state = LOAD_STATE::LOADING;
	auto _hr = this->_pimp->load(pGDevice, pShaderBinaryPath, pShaderStage, pMainFunctionName);
	_super::load_state = LOAD_STATE::LOADED;

	return _hr;
}

ULONG w_shader::release()
{
    if (_super::get_is_released()) return 1;
 
    //release the private implementation
    SAFE_RELEASE(this->_pimp);
    
    return _super::release();
}

#pragma region Getters

const std::vector<w_pipeline_shader_stage_create_info>* w_shader::get_shader_stages() const
{
    if(!this->_pimp) return nullptr;
    return this->_pimp->get_shader_stages();
}


const w_pipeline_shader_stage_create_info w_shader::get_compute_shader_stage() const
{
	if (!this->_pimp)
	{
		w_pipeline_shader_stage_create_info _shader_stage_create_info = {};
		return _shader_stage_create_info;
	}
    return this->_pimp->get_compute_shader_stage();
}

const w_descriptor_set w_shader::get_descriptor_set() const
{
    if(!this->_pimp) return w_descriptor_set();
    return this->_pimp->get_descriptor_set();
}

const w_descriptor_set w_shader::get_compute_descriptor_set() const
{
    if (!this->_pimp) return w_descriptor_set();
    return this->_pimp->get_compute_descriptor_set();
}

const w_descriptor_set_layout w_shader::get_descriptor_set_layout() const
{
    if(!this->_pimp) return w_descriptor_set_layout();
    return this->_pimp->get_descriptor_set_layout();
}

const w_descriptor_set_layout w_shader::get_compute_descriptor_set_layout() const
{
    if (!this->_pimp) return w_descriptor_set_layout();
    return this->_pimp->get_compute_descriptor_set_layout();
}

const std::vector<w_shader_binding_param> w_shader::get_shader_binding_params() const
{
    if (!this->_pimp) return {};
    return this->_pimp->get_shader_binding_params();
}

#pragma endregion

#pragma region Setters

W_RESULT w_shader::set_shader_binding_params(_In_ std::vector<w_shader_binding_param> pShaderBindingParams)
{
    if (!this->_pimp) return W_FAILED;
    return this->_pimp->set_shader_binding_params(pShaderBindingParams);
}

#pragma endregion

W_RESULT w_shader::load_shader(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_z_ const std::string& pName,
    _In_z_ const std::wstring& pVertexShaderPath,
    _In_z_ const std::wstring& pTessellationControlShaderPath,
    _In_z_ const std::wstring& pTessellationEvaluationShaderPath,
    _In_z_ const std::wstring& pGeometryShaderPath,
    _In_z_ const std::wstring& pFragmentShaderPath,
    _In_z_ const std::wstring& pComputeShaderPath,
    _In_ const std::vector<w_shader_binding_param> pShaderBindingParams,
    _In_ const bool pStoreToSharedShaders,
    _Inout_ w_shader** pShader,
    _In_z_ const std::string& pMainFunctionName)
{
    //check already exists
    w_shader* _shader = nullptr;

    //already exists or not?
    if (pStoreToSharedShaders)
    {
        _shader = get_shader_from_shared(pName);
        if (_shader != nullptr)
        {
            *pShader = _shader;
            return W_PASSED;
        }
    }

    _shader = new (std::nothrow) w_shader();
    if (!_shader)
    {
        logger.error("Could not perform allocation for shared shader name: " + pName);
        return W_FAILED;
    }

    if (!pVertexShaderPath.empty())
    {
        if (_shader->load(pGDevice, pVertexShaderPath, w_shader_stage_flag_bits::VERTEX_SHADER, pMainFunctionName) == W_FAILED)
        {
            return W_FAILED;
        }
    }
    if (!pTessellationControlShaderPath.empty())
    {
        if (_shader->load(pGDevice, pTessellationControlShaderPath, w_shader_stage_flag_bits::TESSELATION_CONTROL, pMainFunctionName) == W_FAILED)
        {
            return W_FAILED;
        }
    }
    if (!pTessellationEvaluationShaderPath.empty())
    {
        if (_shader->load(pGDevice, pTessellationEvaluationShaderPath, w_shader_stage_flag_bits::TESSELATION_EVALUATION, pMainFunctionName) == W_FAILED)
        {
            return W_FAILED;
        }
    }
    if (!pGeometryShaderPath.empty())
    {
        if (_shader->load(pGDevice, pGeometryShaderPath, w_shader_stage_flag_bits::GEOMETRY_SHADER, pMainFunctionName) == W_FAILED)
        {
            return W_FAILED;
        }
    }
    if (!pFragmentShaderPath.empty())
    {
        if (_shader->load(pGDevice, pFragmentShaderPath, w_shader_stage_flag_bits::FRAGMENT_SHADER, pMainFunctionName) == W_FAILED)
        {
            return W_FAILED;
        }
    }
    if (!pComputeShaderPath.empty())
    {
        if (_shader->load(pGDevice, pComputeShaderPath, w_shader_stage_flag_bits::COMPUTE_SHADER, pMainFunctionName) == W_FAILED)
        {
            return W_FAILED;
        }
    }

    //set shader params
    if (pShaderBindingParams.size())
    {
        if (_shader->set_shader_binding_params(pShaderBindingParams) == W_FAILED)
        {
            return W_FAILED;
        }
    }

    if (pStoreToSharedShaders)
    {
        _shared[pName] = _shader;
    }

    *pShader = _shader;

    return W_PASSED;
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
    if (!_shared.size()) return 1;

    for (auto _pair : _shared)
    {
        SAFE_RELEASE(_pair.second);
    }
    _shared.clear();
    return 1;
}
