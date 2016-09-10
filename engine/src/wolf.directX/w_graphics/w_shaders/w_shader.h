/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_shader.h
	Description		 : A class which is responsible to manage shaders for materials
	Comment          :
*/

#ifndef __W_SHADER_H__
#define __W_SHADER_H__

#include "w_graphics_device_manager.h"
#include <w_vertex_declaration.h>
#include <w_io.h>
#include <w_logger.h>

enum w_shader_type : byte
{
	VERTEX_SHADER,
	PIXEL_SHADER,
	GEOMETRY_SHADER,
	DOMAIN_SHADER,
	HULL_SHADER,
	COMPUTE_SHADER
};

namespace wolf
{
	namespace graphics
	{
		class w_shader : public system::w_object
		{
		public:
			DX_EXP w_shader();
			DX_EXP virtual ~w_shader();

			//Create vertex shader from binary file
			virtual HRESULT create_vertex_shader(_In_ ID3D11Device1* pDevice, const void* pBytes, size_t pLength, wolf::content_pipeline::w_vertex_declaration pVDeclaration);
			//create pixel shader from binary file
			virtual HRESULT create_pixel_shader(_In_ ID3D11Device1* pDevice, const void* pBytes, size_t pLength, size_t& pIndexCreated);

			//Load a shader from file path
			static HRESULT load_shader(_In_ ID3D11Device1* pDevice, std::wstring pPath, w_shader_type pShaderType, wolf::content_pipeline::w_vertex_declaration pVDeclaration, _Inout_ w_shader* pShader, _Out_ size_t* pPixelShaderCreatedIndex = nullptr);

			//Apply shader, this must be called before drawing primitives
			void apply(_In_ ID3D11DeviceContext1* pContext, size_t pPixelShaderIndex = 0);
			void release_pixel_shaders();
			virtual ULONG release() override;

#pragma region Getters
			//Get the pixel shader
			ID3D11PixelShader*	get_pixel_shader(size_t pIndex = 0) const	{ return (pIndex < this->_pixel_shaders.size()) ? this->_pixel_shaders.at(pIndex) : nullptr; }
			//Get the pixel shader
			ID3D11VertexShader* get_vertex_shader() const					{ return this->_vertex_shader.Get(); }
			//Get input layout
			ID3D11InputLayout*	get_input_layout() const					{ return this->_input_layout.Get(); };

#pragma endregion

#pragma region Setters

			//Set vertex shader
			void set_vertex_shader(_In_ ID3D11VertexShader* pVS);
			//Set pixel shader
			void set_pixel_shader(_In_ ID3D11PixelShader* pPS, size_t& pIndex);
			//Set constant buffer
			void set_constant_buffer(_In_ ID3D11DeviceContext1* pContext, UINT pStartSlot, UINT pNumBuffer, _In_ ID3D11Buffer* pCBuffer);
			//Set input layout
			void set_input_layout(ID3D11InputLayout* pInput);
			//Set the shader resource view
			void set_srv(_In_ ID3D11DeviceContext1* pContext, UINT pStartSLot, UINT pNumViews, _In_ ID3D11ShaderResourceView* pSRV);

#pragma endregion

			//static Concurrency::task<void> LoadShaderAsync(Platform::String^ path, ShaderType shaderType,
			//	VertexDeclaration VDeclaration, _Inout_ Shader^ &shader)
			//{
			//	return DX::ReadFileAsync(path)
			//		.then([shaderType, VDeclaration, shader](Array<byte>^ bytes)
			//	{
			//		switch (shaderType)
			//		{
			//		case ShaderType::VertexShader:
			//			shader->CreateVertexShader(bytes->Data, bytes->Length, VDeclaration);
			//			break;
			//		case ShaderType::PixelShader:
			//			shader->CreatePixelShader(bytes->Data, bytes->Length);
			//			break;
			//		case ShaderType::HullShader:
			//			//Not implemented
			//			break;
			//		case ShaderType::DomainShader:
			//			//Not implemented
			//			break;
			//		case ShaderType::GeometryShader:
			//			//Not implemented
			//			break;
			//		case ShaderType::ComputeShader:
			//			//Not implemented
			//			break;
			//		default:
			//			throw ref new Exception(0, "Unknown shader type");
			//		}
			//	});
			//}
		private:
			typedef	system::w_object							_super;

			HRESULT _create_input_layout(_In_ ID3D11Device1* pDevice, const D3D11_INPUT_ELEMENT_DESC*, const UINT, const void*, const SIZE_T);
			HRESULT _choose_input_layout(_In_ ID3D11Device1* pDevice, content_pipeline::w_vertex_declaration, const void*, const SIZE_T);

			Microsoft::WRL::ComPtr<ID3D11InputLayout>			_input_layout;
			Microsoft::WRL::ComPtr<ID3D11VertexShader>			_vertex_shader;
			std::vector<ID3D11PixelShader*>						_pixel_shaders;
		};
	}
}

#endif
