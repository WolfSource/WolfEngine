/*
	Project			 : Wolf PlayOut, powered by Wolf Engine Copyright (c) Pooya Eimandar. https://github.com/PooyaEimandar/WolfEngine
					   Please direct any bug to hello@WolfStudio.co
	File Name        : W_Shader.h
	File Description : A class which is responsible to manage shaders for materials
	Comment          :
*/

#pragma once

#include "W_GraphicsDeviceManager.h"
#include "W_GraphicsResource\W_Geometries\W_VertexTypes.h"
#include <W_IO.h>
#include <W_Logger.h>

enum W_ShaderType : byte
{
	VertexShader,
	PixelShader,
	GeometryShader,
	DomainShader,
	HullShader,
	ComputeShader
};

namespace Wolf
{
	namespace Graphics
	{
		class W_Shader : public System::W_Object
		{
		public:
			W_Shader(const std::shared_ptr<Graphics::W_GraphicsDevice>& gDevice);
			virtual ~W_Shader();

			//Create vertex shader from binary file
			virtual HRESULT CreateVertexShader(const void* bytes, SIZE_T Length, W_VertexDeclaration VDeclaration);
			//create pixel shader from binary file
			virtual HRESULT CreatePixelShader(const void* bytes, SIZE_T Length);

			//Load a shader from file path
			static HRESULT LoadShader(std::wstring path, W_ShaderType shaderType,
				W_VertexDeclaration vDeclaration, _Inout_ W_Shader* shader)
			{
				std::unique_ptr<uint8_t[]> data;
				size_t dataSize;
				int fileState;

				auto _path = System::IO::GetContentDirectory() + path;

				auto hr = System::IO::ReadBinaryFile(_path, data, &dataSize, &fileState);
				OnFailed(hr, "Could not read binary data", "Shader.h", false);
				if (FAILED(hr)) return hr;

				switch (shaderType)
				{
				case W_ShaderType::VertexShader:
					hr = shader->CreateVertexShader(data.get(), dataSize, vDeclaration);
					break;
				case W_ShaderType::PixelShader:
					hr = shader->CreatePixelShader(data.get(), dataSize);
					break;
				case W_ShaderType::HullShader:
					//Not implemented
					break;
				case W_ShaderType::DomainShader:
					//Not implemented
					break;
				case W_ShaderType::GeometryShader:
					//Not implemented
					break;
				case W_ShaderType::ComputeShader:
					//Not implemented
					break;
				default:
					std::string msg = "Unknown shader type";
					MessageBox(NULL, msg.c_str(), "Error", MB_OK);
					OnFailed(S_FALSE, msg, "Shader.h", true);
					msg.clear();
				}

				return hr;
			}

			//Apply shader, this must be called before drawing primitives
			void Apply();
			virtual ULONG Release() override;

#pragma region Getters
			//Get the pixel shader
			ID3D11PixelShader* GetPixelShader() const			{ return this->pShader.Get(); }
#pragma endregion

#pragma region Setters

			//Set vertex shader
			virtual void SetVertexShader(_In_ ID3D11VertexShader* VS);
			//Set pixel shader
			virtual void SetPixelShader(_In_ ID3D11PixelShader* PS);
			//Set constant buffer
			virtual void SetConstantBuffer(UINT startSlot, UINT numBuffer, _In_ ID3D11Buffer* CBuffer);
			//Set input layout
			virtual void SetInputLayout(ID3D11InputLayout* value);
			//Set the shader resource view
			virtual void SetSRV(UINT startSLot, UINT numViews, _In_ ID3D11ShaderResourceView* srv);

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
			std::shared_ptr<Graphics::W_GraphicsDevice>			gDevice;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;
			Microsoft::WRL::ComPtr<ID3D11VertexShader>			vShader;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>			pShader;

			HRESULT CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC*, const UINT, const void*, const SIZE_T);
			HRESULT ChooseInputLayout(W_VertexDeclaration, const void*, const SIZE_T);
		};
	}
}
