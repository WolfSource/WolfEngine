/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_RenderTarget2D.h
	Description		 : Create a 2D render target
	Comment          :
*/

#pragma once

#include <d3d11_1.h>
#include <W_Object.h>
#include <W_GraphicsDeviceManager.h>

namespace Wolf
{
	namespace Graphics
	{
		//Contains a 2D texture that can be used as a render target capture.
		class W_RenderTarget2D : System::W_Object
		{
		public:
			API W_RenderTarget2D(const std::shared_ptr<W_GraphicsDevice>& pGDevice);
			API virtual ~W_RenderTarget2D();

			//Load render target
			API HRESULT			Load(int pWidth, int pHeight);
			//Begin capture
			API void			Begin();
			//End capture
			API void			End();
			//Release resources
			API virtual ULONG	Release();

#pragma region Getters

			//Get pointer to render target view
			ID3D11RenderTargetView*		GetRenderTargetView() const		{ return this->rtv.Get(); }
			//Get shader resource view of render target
			ID3D11ShaderResourceView*	GetSRV() const					{ return this->srv.Get(); }
			//Get direct 3d11 resource of render target
			API ID3D11Resource*			GetResource();

#pragma endregion

		private:
			std::shared_ptr<W_GraphicsDevice>								gDevice;
			CD3D11_VIEWPORT													viewport;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>					depthStencilView;
			Microsoft::WRL::ComPtr<ID3D11Texture2D>							rt;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>					rtv;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>				srv;
		};
	}
}

