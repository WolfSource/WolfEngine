/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_shader.h
	Description		 : A class which is responsible to manage shaders for materials
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_SHADER_H__
#define __W_SHADER_H__

#include "w_graphics_device_manager.h"
#include "w_texture.h"
#include <python_exporter/w_boost_python_helper.h>

namespace wolf
{
	namespace render
	{
		namespace vulkan
		{
			enum w_shader_binding_type
			{
				SAMPLER2D = 0,
				SAMPLER,
				UNIFORM,
				IMAGE,
				STORAGE
			};

			struct w_pipeline_shader_stage_create_info : public
#ifdef __VULKAN__
				VkPipelineShaderStageCreateInfo
#endif
			{
			};

			struct w_shader_binding_param
			{
				//index of shader variable
				uint32_t                    index;
				//type of shader variable
				w_shader_binding_type       type;
				//shader stage flags
				w_shader_stage_flag_bits    stage;
				//descriptor buffer info
				w_descriptor_buffer_info    buffer_info;
				//descriptor image info
				w_descriptor_image_info     image_info;
			};

			class w_shader_pimp;
			class w_shader : public system::w_object
			{
			public:
				W_EXP w_shader();
				W_EXP virtual ~w_shader();

				//load shader from binary file
				W_EXP W_RESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
					_In_z_ const std::wstring& pShaderBinaryPath,
					_In_ const w_shader_stage_flag_bits& pShaderStage,
					_In_z_ const std::string& pMainFunctionName = "main");

				//release all resources
				W_EXP virtual ULONG release() override;

#pragma region Getters

				W_EXP const std::vector<w_shader_binding_param> get_shader_binding_params() const;
				W_EXP const std::vector<w_pipeline_shader_stage_create_info>* get_shader_stages() const;
				W_EXP const w_pipeline_shader_stage_create_info get_compute_shader_stage() const;

				W_EXP const w_descriptor_set get_descriptor_set() const;
				W_EXP const w_descriptor_set get_compute_descriptor_set() const;

				W_EXP const w_descriptor_set_layout get_descriptor_set_layout() const;
				W_EXP const w_descriptor_set_layout get_compute_descriptor_set_layout() const;

#pragma endregion

#pragma region Setters

				//set and update shader binding params
				W_EXP W_RESULT set_shader_binding_params(_In_ std::vector<w_shader_binding_param> pShaderBindingParams);

#pragma endregion

				W_EXP static W_RESULT load_shader(
					_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
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
					_In_z_ const std::string& pMainFunctionName = "main");

				W_EXP static w_shader* get_shader_from_shared(_In_z_ const std::string& pName);
				W_EXP static ULONG release_shared_shaders();

#ifdef __PYTHON__

				W_RESULT py_load(
					_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
					_In_z_ const std::wstring& pShaderBinaryPath,
					_In_ const w_shader_stage_flag_bits& pShaderStage,
					_In_z_ const std::string& pMainFunctionName = "main")
				{
					if (!pGDevice.get()) return W_FAILED;
					//boost::shared to std::shared
					auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

					auto _hr = load(
						_gDevice,
						pShaderBinaryPath,
						pShaderStage,
						pMainFunctionName.c_str());

					//reset local shared_ptr
					_gDevice.reset();

					return _hr;
				}

				boost::python::list py_get_shader_binding_params()
				{
					boost::python::list _list;
					auto _binding_params = get_shader_binding_params();
					for (size_t i = 0; i < _binding_params.size(); ++i)
					{
						_list.append(_binding_params[i]);
					}
					_binding_params.clear();
					return _list;
				}

				boost::python::list py_get_shader_stages()
				{
					boost::python::list _list;
					auto _stages = get_shader_stages();
					if (_stages)
					{
						for (size_t i = 0; i < _stages->size(); ++i)
						{
							_list.append(_stages[i]);
						}
					}
					return _list;
				}

				W_RESULT py_set_shader_binding_params(_In_ boost::python::list pShaderBindingParams)
				{
					std::vector<w_shader_binding_param> _shader_binding_params;
					//get command buffers
					for (size_t i = 0; i < len(pShaderBindingParams); ++i)
					{
						boost::python::extract<w_shader_binding_param> _param(pShaderBindingParams[i]);
						if (_param.check())
						{
							_shader_binding_params.push_back(_param());
						}
					}

					if (_shader_binding_params.size())
					{
						return set_shader_binding_params(_shader_binding_params);
					}
					return W_FAILED;
				}

				static W_RESULT py_load_shader(
					_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
					_In_z_ const std::string& pName,
					_In_z_ const std::wstring& pVertexShaderPath,
					_In_z_ const std::wstring& pTessellationControlShaderPath,
					_In_z_ const std::wstring& pTessellationEvaluationShaderPath,
					_In_z_ const std::wstring& pGeometryShaderPath,
					_In_z_ const std::wstring& pFragmentShaderPath,
					_In_z_ const std::wstring& pComputeShaderPath,
					_In_ boost::python::list pShaderBindingParams,
					_In_z_ const std::string& pMainFunctionName = "main")
				{
					if (!pGDevice.get()) return W_FAILED;
					//boost::shared to std::shared
					auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

					std::vector<w_shader_binding_param> _shader_binding_params;
					//get command buffers
					for (size_t i = 0; i < len(pShaderBindingParams); ++i)
					{
						boost::python::extract<w_shader_binding_param> _param(pShaderBindingParams[i]);
						if (_param.check())
						{
							_shader_binding_params.push_back(_param());
						}
					}

					w_shader* _shader = nullptr;
					auto _hr = load_shader(
						_gDevice,
						pName,
						pVertexShaderPath,
						pTessellationControlShaderPath,
						pTessellationEvaluationShaderPath,
						pGeometryShaderPath,
						pFragmentShaderPath,
						pComputeShaderPath,
						_shader_binding_params,
						true,
						&_shader,
						pMainFunctionName.c_str());

					_shader_binding_params.clear();

					//reset local shared_ptr
					_gDevice.reset();

					return _hr;
				}
#endif

			private:
				typedef	system::w_object                                _super;
				w_shader_pimp*                                          _pimp;

				static std::map<std::string, w_shader*>                 _shared;
			};
		}
	}
}

#include "python_exporter/py_shader.h"

#endif
