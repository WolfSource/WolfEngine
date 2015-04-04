/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @PooyaEimandar on twitter
	Name			 : W_Texture2D.h
	Description		 : Create and handle 2D texture resource
	Comment          :
*/

#pragma once

#include "W_GraphicsDeviceManager.h"
#include "DDSTextureLoader.h"
#include <W_Logger.h>
#include <W_IO.h>

namespace Wolf
{
	namespace Graphics
	{
		//Represents a 2D grid of texels.
		class W_Texture2D : public System::W_Object
		{
		public:
			API W_Texture2D();
			API virtual ~W_Texture2D();

			//Create and initialize a default colorbar
			HRESULT CreateColorBar(_In_ ID3D11Device1* pDevice, 
				UINT width, 
				UINT height,
				DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM,
				D3D11_USAGE usage = D3D11_USAGE_DYNAMIC,
				D3D11_BIND_FLAG bind = D3D11_BIND_SHADER_RESOURCE,
				D3D11_CPU_ACCESS_FLAG cpuAccessFlags = D3D11_CPU_ACCESS_WRITE);

			//Create a texture and initialize it with specific rgba color
			HRESULT Create(_In_ ID3D11Device1* pDevice, 
				UINT width, 
				UINT height,
				D2D1::ColorF color,
				DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM,
				D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
				D3D11_BIND_FLAG bind = D3D11_BIND_SHADER_RESOURCE,
				D3D11_CPU_ACCESS_FLAG cpuAccessFlags = D3D11_CPU_ACCESS_WRITE);

			//Create a texture and initialize it with subresource data
			HRESULT Create(_In_ ID3D11Device1* pDevice, 
				UINT width, 
				UINT height,
				D3D11_SUBRESOURCE_DATA* initialData = nullptr,
				DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM,
				D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
				D3D11_BIND_FLAG bind = D3D11_BIND_SHADER_RESOURCE,
				D3D11_CPU_ACCESS_FLAG cpuAccessFlags = D3D11_CPU_ACCESS_WRITE);

			//Load texture with resource and shader resource view
			void LoadResource(_In_ Microsoft::WRL::ComPtr<ID3D11Resource> resource, 
				_In_ Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView);

			//Release all resources
			virtual ULONG Release() override;

			//Load texture2D from path
			static HRESULT LoadTexture(_In_ ID3D11Device1* pD3dDevice, 
				_Inout_ W_Texture2D* pTexture, 
				std::wstring pPath, 
				size_t pMaxSize = 0, 
				DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM, 
				UINT pBindFlags = D3D11_BIND_SHADER_RESOURCE)
			{
				using namespace std;
				using namespace System::IO;

				std::unique_ptr<uint8_t []> data;
				size_t dataSize;
				int fileState;

				auto _path = GetContentDirectory() + pPath;
				auto hr = System::IO::ReadBinaryFile(_path, data, &dataSize, &fileState);
				if (FAILED(hr))
				{
					string msg = "";
					switch (fileState)
					{
					case -1:
						msg = "Could not find the texture : ";
						break;
					case -2:
						msg = "Texture is corrupted : ";
						break;
					}


					OnFailed(S_FALSE, msg + string(pPath.begin(), pPath.end()), "Texture2D", false, false);

					return hr;
				}

				Microsoft::WRL::ComPtr<ID3D11Resource> resource = nullptr;
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;

				CreateDDSTextureFromMemory(
					pD3dDevice,
					data.get(),
					dataSize,
					&resource,
					&srv,
					pMaxSize,
					pFormat,
					pBindFlags);

				pTexture->LoadResource(resource, srv);

				return S_OK;
			}

			//Create a sampler
			static void CreateSampler(const std::shared_ptr<W_GraphicsDevice>& gDevice, 
				const D3D11_FILTER filter, 
				const D3D11_TEXTURE_ADDRESS_MODE Address[3])
			{
				// Once the texture view is created, create a sampler.  This defines how the color
				// for a particular texture coordinate is determined using the relevant texture data.
				D3D11_SAMPLER_DESC samplerDesc;
				ZeroMemory(&samplerDesc, sizeof(samplerDesc));

				samplerDesc.Filter = filter;

				// The sampler does not use anisotropic filtering, so this parameter is ignored.
				samplerDesc.MaxAnisotropy = 0;

				// Specify how texture coordinates outside of the range 0..1 are resolved.
				samplerDesc.AddressU = Address[0];
				samplerDesc.AddressV = Address[1];
				samplerDesc.AddressW = Address[2];

				// Use no special MIP clamping or bias.
				samplerDesc.MipLODBias = 0.0f;
				samplerDesc.MinLOD = 0;
				samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

				// Don't use a comparison function.
				samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

				// Border address mode is not used, so this parameter is ignored.
				samplerDesc.BorderColor[0] = 0.0f;
				samplerDesc.BorderColor[1] = 0.0f;
				samplerDesc.BorderColor[2] = 0.0f;
				samplerDesc.BorderColor[3] = 0.0f;

				ID3D11SamplerState* _sampler = nullptr;
				{
					auto hr = gDevice->device->CreateSamplerState(&samplerDesc, &_sampler);
					OnFailed(hr, "Error on create sampler for texture2D", "Texture2D", false, true);

					gDevice->samplers.push_back(_sampler);
				}
			}

#pragma region Getters

			//Get resource of Texture2D
			ID3D11Resource* GetResource();
			//Get shader resource view of Texture2D
			ID3D11ShaderResourceView* GetSRV() const		    { return this->srv.Get(); }

#pragma endregion

		private:
			Microsoft::WRL::ComPtr<ID3D11Texture2D>				texture2D;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	srv;
			D3D11_TEXTURE2D_DESC								description;
		};
	}
}