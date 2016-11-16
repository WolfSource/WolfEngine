/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_texture_2D.h
	Description		 : Create and handle 2D texture resource
	Comment          : encoding texture with two channel based on http://devkk.net/index.php?tag=articles&id=24
*/

#ifndef __W_TEXTURE_2D_H__
#define __W_TEXTURE_2D_H__

#include "w_graphics_device_manager.h"
#include "DDSTextureLoader.h"
#include <w_logger.h>
#include <w_io.h>
#include <w_timer.h>
#include <glm/vec3.hpp>
#include <glm/fwd.hpp>

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
			DX_EXP HRESULT create_color_bar(_In_ ID3D11Device1* pDevice,
				_In_ UINT pWidth, 
				_In_ UINT pHeight,
				_In_ D3D11_USAGE pUsage = D3D11_USAGE_DYNAMIC,
				_In_ UINT pCpuAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
				_In_ DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				_In_ UINT pBindFlags = D3D11_BIND_SHADER_RESOURCE);

			//Create a texture and initialize it with specific rgba color
			DX_EXP HRESULT create(_In_ ID3D11Device1* pDevice,
				_In_ UINT pWidth, 
				_In_ UINT pHeight,
				_In_ D2D1::ColorF pColor,
				_In_ D3D11_USAGE pUsage = D3D11_USAGE_DYNAMIC,
				_In_ UINT pCpuAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
				_In_ DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				_In_ UINT pBindFlags = D3D11_BIND_SHADER_RESOURCE,
				_In_ GPGPU_TYPE pGPGPU_Type = GPGPU_TYPE::CPP_AMP);

			//Create a texture and initialize it with subresource data
			DX_EXP HRESULT create(_In_ ID3D11Device1* pDevice,
				_In_ UINT pWidth, 
				_In_ UINT pHeight,
				_In_ D3D11_USAGE pUsage = D3D11_USAGE_DYNAMIC,
				_In_ UINT pCpuAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
				_In_ DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				_In_ UINT pBindFlags = D3D11_BIND_SHADER_RESOURCE,
				_In_ D3D11_SUBRESOURCE_DATA* pInitialData = nullptr);

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
			DX_EXP static HRESULT load_texture_2D_from_file(_In_ ID3D11Device1* pDevice,
				_Inout_ std::unique_ptr<w_texture_2D>& pTexture,
				_In_ std::wstring pPath,
				_In_ bool pIsAbsolutePath = false,
				_In_ bool pEnableTextureMemoryUsage2ChannelEncoding = false,
				_In_ glm::i32vec3 pRGBWeights = glm::i32vec3(25, 100, 50),
				_In_ size_t pMaxSize = 0,
				_In_ DXGI_FORMAT pFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
				_In_ UINT pBindFlags = D3D11_BIND_SHADER_RESOURCE);

			//Create a sampler
			DX_EXP static void create_sampler(const std::shared_ptr<w_graphics_device>& pGDevice,
				const D3D11_FILTER pFilter,
				const D3D11_TEXTURE_ADDRESS_MODE pAddress[3]);

			DX_EXP static HRESULT encode_texture_2D(_In_ ID3D11Device1* pDevice,
				_In_ glm::i32vec3 pRGBWeights,
				_Inout_ std::unique_ptr<w_texture_2D>& pSrcTexture,
				_Inout_ w_texture_2D** pEncodedTexture);

			DX_EXP static HRESULT decode_texture_2D(_In_ ID3D11Device1* pDevice,
				_In_ glm::vec3 pBaseA,
				_In_ glm::vec3 pBaseB,
				_In_ w_texture_2D* pEncodedTexture,
				_Inout_ w_texture_2D** pDecodedTexture);

			/*
				-1: sizes are not equal
				1 = two textures have same pixels
				0 = two textures have not same pixels
			*/
			DX_EXP static int compare(_In_ ID3D11Device1* pDevice,
				_In_ w_texture_2D* pTextureA,
				_In_ w_texture_2D* pTextureB,
				_In_ UINT pEpsilon = 0);

#pragma region Getters

			//Get the pointer of texture2D
			DX_EXP ID3D11Texture2D*				get_texture_2D() const;

			//Get resource of Texture2D
			DX_EXP ID3D11Resource*				get_resource() const;
			//Get shader resource view of Texture2D
			DX_EXP ID3D11ShaderResourceView*	get_srv() const;

			//Get texture 2D description
			DX_EXP D3D11_TEXTURE2D_DESC			get_description() const;

			//get base_a, used when you want to decrease texture memory usage by encoding
			DX_EXP glm::vec3					get_encode_base_a() const					{ return this->_2_channel_encoding_base_a; }
			//get base_a, used when you want to decrease texture memory usage by encoding
			DX_EXP glm::vec3					get_encode_base_b() const					{ return this->_2_channel_encoding_base_b; }

#pragma endregion

#pragma region Setters

			DX_EXP void set_encode_bases(_In_ const glm::vec3 pBaseA, _In_ const glm::vec3 pBaseB)
			{
				this->_2_channel_encoding_base_a = pBaseA;
				this->_2_channel_encoding_base_b = pBaseB;
			}

#pragma endregion

		private:
			typedef system::w_object						_super;

			HRESULT _create(_In_ ID3D11Device1* pDevice,
				_In_ UINT pWidth,
				_In_ UINT pHeight,
				_In_ D2D1::ColorF pColor,
				_In_ D3D11_USAGE pUsage,
				_In_ UINT pCpuAccessFlags,
				_In_ DXGI_FORMAT pFormat,
				_In_ UINT pBindFlags);

			HRESULT _create_shader_resource_view(_In_ ID3D11Device1* pDevice, DXGI_FORMAT pFormat, UINT pArraySize, UINT pMipMapLevels);
			
			static void			_encode_texture(_In_ UINT* pSrc, 
				_In_ D3D11_TEXTURE2D_DESC pTextureDescription, 
				_In_ glm::i32vec3 pRGBWeights,
				_Inout_ glm::vec3& pBaseA, 
				_Inout_ glm::vec3& pBaseB,
				_Inout_ UINT** pEncodedPixels);

			static glm::vec3	_estimate_image(_In_ UINT* pSrc, _In_ D3D11_TEXTURE2D_DESC pTextureDescription, _In_ glm::i32vec3 pRGBWeights);
			static void			_stamp_color_probe(_Inout_ UINT* pSrc, _In_ D3D11_TEXTURE2D_DESC pTextureDescription);
			static void			_encode_pixels(_In_ UINT* pSrc, _In_ glm::vec3 n, _In_ D3D11_TEXTURE2D_DESC pTextureDescription, 
				_Inout_ glm::vec3& pBaseA, _Inout_ glm::vec3& pBaseB, _Inout_ UINT** pEncodedPixels);
			static float		_xcos(float f);
			static void			_find_components(_In_ const glm::vec3& n, _Inout_ glm::vec3& ca, _Inout_ glm::vec3& cb);
			static bool			_cross_test(_Inout_ const glm::vec3& p1, _Inout_ const glm::vec3& p2, _Inout_ const glm::vec3& n, _Inout_ glm::vec3& out);

			Microsoft::WRL::ComPtr<ID3D11Texture2D>				_texture_buffer;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	_srv;
			D3D11_TEXTURE2D_DESC								_description;

			//used when you want to decrease texture memory usage by encoding
			glm::vec3											_2_channel_encoding_base_a;
			glm::vec3											_2_channel_encoding_base_b;

		};
	}
}

#endif