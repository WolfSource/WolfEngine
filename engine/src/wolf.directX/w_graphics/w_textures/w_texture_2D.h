/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_texture_2D.h
	Description		 : Create and handle 2D texture resource
	Comment          :
*/

#ifndef __W_TEXTURE_2D_H__
#define __W_TEXTURE_2D_H__

#include "w_graphics_device_manager.h"
#include "DDSTextureLoader.h"
#include <w_logger.h>
#include <w_io.h>
#include <w_timer.h>

namespace wolf
{
	namespace graphics
	{
		//Represents a 2D grid of texels.
		class w_texture_2D : public system::w_object
		{
		public:
			DX_EXP w_texture_2D();
			DX_EXP virtual ~w_texture_2D();

			//Create and initialize a default colorbar
			DX_EXP HRESULT create_colorBar(_In_ ID3D11Device1* pDevice,
				UINT pWidth, 
				UINT pHeight,
				D3D11_USAGE pUsage = D3D11_USAGE_DYNAMIC,
				UINT pCpuAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
				DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				UINT pBindFlags = D3D11_BIND_SHADER_RESOURCE,
				GPGPU_TYPE pGPGPU_Type = GPGPU_TYPE::CPP_AMP);

			//Create a texture and initialize it with specific rgba color
			DX_EXP HRESULT create(_In_ ID3D11Device1* pDevice,
				UINT pWidth, 
				UINT pHeight,
				D2D1::ColorF pColor,
				D3D11_USAGE pUsage = D3D11_USAGE_DYNAMIC,
				UINT pCpuAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
				DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				UINT pBindFlags = D3D11_BIND_SHADER_RESOURCE,
				GPGPU_TYPE pGPGPU_Type = GPGPU_TYPE::CPP_AMP);

			//Create a texture and initialize it with subresource data
			DX_EXP HRESULT create(_In_ ID3D11Device1* pDevice,
				UINT pWidth, 
				UINT pHeight,
				D3D11_USAGE pUsage = D3D11_USAGE_DYNAMIC,
				UINT pCpuAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
				DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				UINT pBindFlags = D3D11_BIND_SHADER_RESOURCE,
				D3D11_SUBRESOURCE_DATA* pInitialData = nullptr);

			//Load texture with resource and shader resource view
			DX_EXP void load_resource(_In_ Microsoft::WRL::ComPtr<ID3D11Resource> resource,
				_In_ Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView);
			
			//Release all resources
			DX_EXP virtual ULONG release() override;

			//Capture screen shot
			DX_EXP static HRESULT capture_screen(_In_ ID3D11DeviceContext* pContext,
				_In_ ID3D11Resource* pSource,
				_In_z_ LPCWSTR pFileName);

			//Load texture2D from path
			DX_EXP static HRESULT load_texture_2D(_In_ ID3D11Device1* pD3dDevice,
				_Inout_ w_texture_2D* pTexture,
				std::wstring pPath,
				bool pIsAbsolutePath = false,
				size_t pMaxSize = 0,
				DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				UINT pBindFlags = D3D11_BIND_SHADER_RESOURCE);

			//Create a sampler
			DX_EXP static void create_sampler(const std::shared_ptr<w_graphics_device>& pGDevice,
				const D3D11_FILTER pFilter,
				const D3D11_TEXTURE_ADDRESS_MODE pAddress[3]);

#pragma region Getters

			//Get the pointer of texture2D
			DX_EXP ID3D11Texture2D* get_texture_2D() const;

			//Get resource of Texture2D
			DX_EXP ID3D11Resource* get_resource() const;
			//Get shader resource view of Texture2D
			DX_EXP ID3D11ShaderResourceView* get_srv() const;

#pragma endregion

		private:
			typedef system::w_object						_super;

			//Create color bar with pure c++
			HRESULT _create_colorBar(_In_ ID3D11Device1* pDevice, 
				UINT pWidth, 
				UINT pHeight, 
				D3D11_USAGE pUsage,
				UINT pCpuAccessFlags, 
				DXGI_FORMAT pFormat, 
				UINT pBindFlags);

			HRESULT _create(_In_ ID3D11Device1* pDevice,
				UINT pWidth,
				UINT pHeight,
				D2D1::ColorF pColor,
				D3D11_USAGE pUsage,
				UINT pCpuAccessFlags,
				DXGI_FORMAT pFormat,
				UINT pBindFlags);

			HRESULT _create_shader_resource_view(_In_ ID3D11Device1* pDevice, DXGI_FORMAT pFormat, UINT pArraySize, UINT pMipMapLevels);

			Microsoft::WRL::ComPtr<ID3D11Texture2D>				_texture_2D;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	_srv;
			D3D11_TEXTURE2D_DESC								_description;
		};
	}
}

#endif