/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_render_target2D.h
	Description		 : Create a 2D render target
	Comment          :
*/

#ifndef __W_RENDER_TARGET_2D_H__
#define __W_RENDER_TARGET_2D_H__

//#include <d3d11_2.h>
#include <w_object.h>
#include <w_graphics_device_manager.h>

namespace wolf
{
	namespace graphics
	{
		//Contains a 2D texture that can be used as a render target capture.
		class w_render_target_2D : system::w_object
		{
		public:
			DX_EXP w_render_target_2D();
			DX_EXP virtual ~w_render_target_2D();

			//Load render target
			DX_EXP HRESULT			load(_In_ ID3D11Device1* pDevice, int pWidth, int pHeight);
			//Begin capture
			DX_EXP void			begin(_In_ ID3D11DeviceContext1* pContext, DirectX::XMFLOAT4 pClearColor = DirectX::XMFLOAT4(0.251f, 0.251f, 0.251f, 1.0f));
			//End capture
			DX_EXP void			end(_In_ ID3D11DeviceContext1* pContext);
			//Release resources
			DX_EXP virtual ULONG	release();

#pragma region Getters

			//Get pointer to render target view
			ID3D11RenderTargetView*		get_render_target_view() const		{ return this->_rtv.Get(); }
			//Get shader resource view of render target
			ID3D11ShaderResourceView*	get_srv() const						{ return this->_srv.Get(); }
			//Get direct 3d11 resource of render target
			DX_EXP ID3D11Resource*			get_resource();

#pragma endregion

		private:
			typedef system::w_object										_super;
			CD3D11_VIEWPORT													_viewport;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>					_depthStencilView;
			Microsoft::WRL::ComPtr<ID3D11Texture2D>							_rt;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>					_rtv;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>				_srv;
		};
	}
}

#endif

