/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
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
			API HRESULT CreateColorBar(_In_ ID3D11Device1* pDevice,
				UINT pWidth, 
				UINT pHeight,
				D3D11_USAGE pUsage = D3D11_USAGE_DYNAMIC,
				D3D11_CPU_ACCESS_FLAG pCpuAccessFlags = D3D11_CPU_ACCESS_WRITE,
				DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				D3D11_BIND_FLAG pBind = D3D11_BIND_SHADER_RESOURCE);

			//Create a texture and initialize it with specific rgba color
			API HRESULT Create(_In_ ID3D11Device1* pDevice,
				UINT pWidth, 
				UINT pHeight,
				D2D1::ColorF pColor,
				D3D11_USAGE pUsage = D3D11_USAGE_DEFAULT,
				D3D11_CPU_ACCESS_FLAG pCpuAccessFlags = D3D11_CPU_ACCESS_READ,
				DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				D3D11_BIND_FLAG pBind = D3D11_BIND_SHADER_RESOURCE);

			//Create a texture and initialize it with subresource data
			API HRESULT Create(_In_ ID3D11Device1* pDevice,
				UINT pWidth, 
				UINT pHeight,
				D3D11_USAGE pUsage = D3D11_USAGE_DEFAULT,
				D3D11_CPU_ACCESS_FLAG pCpuAccessFlags = D3D11_CPU_ACCESS_READ,
				DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				D3D11_BIND_FLAG pBind = D3D11_BIND_SHADER_RESOURCE,
				D3D11_SUBRESOURCE_DATA* pInitialData = nullptr);

			//Load texture with resource and shader resource view
			API void LoadResource(_In_ Microsoft::WRL::ComPtr<ID3D11Resource> resource,
				_In_ Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView);

			//Release all resources
			API virtual ULONG Release() override;

			//Capture screen shot
			API static HRESULT CaptureScreen(_In_ ID3D11DeviceContext* pContext,
				_In_ ID3D11Resource* pSource,
				_In_z_ LPCWSTR pFileName);

			//Load texture2D from path
			static HRESULT LoadTexture(_In_ ID3D11Device1* pD3dDevice,
				_Inout_ W_Texture2D* pTexture,
				std::wstring pPath,
				size_t pMaxSize = 0,
				DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				UINT pBindFlags = D3D11_BIND_SHADER_RESOURCE);

			//Create a sampler
			static void CreateSampler(const std::shared_ptr<W_GraphicsDevice>& pGDevice,
				const D3D11_FILTER pFilter,
				const D3D11_TEXTURE_ADDRESS_MODE pAddress[3]);

#pragma region Getters

			//Get resource of Texture2D
			API ID3D11Resource* GetResource();
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