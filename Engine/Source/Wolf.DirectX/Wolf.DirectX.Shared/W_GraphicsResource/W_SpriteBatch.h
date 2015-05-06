/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_SpriteBatch.h
	Description		 : Responsible for drawing texture2D & 2D texts
	Comment          :
*/

#pragma once

#include <Wincodec.h>
#include <DirectXMath.h>
#include "W_GraphicsDeviceManager.h"
#include "W_Fonts\W_SpriteFont.h"
#include "W_Direct2D\ISpriteBatchDrawable.h"
#include "W_Direct2D\W_Rectangle.h"
#include "W_Direct2D\W_Ellipse.h"
#include "W_Direct2D\W_Line.h"

namespace Wolf
{
	namespace Graphics
	{
		//Enables a group of sprites or direct2D texts using the same settings, also represent methods for drawing 2D texts.
		class W_SpriteBatch : public System::W_Object
		{
		public:
			API W_SpriteBatch(const std::shared_ptr<W_GraphicsDevice>& pGdevice);
			API virtual ~W_SpriteBatch();

			//Load SpriteBatch
			API void Load();
			//Add direct2D effect
			API void AddEffect(_In_ REFCLSID pEffectId);
			//Set WIC bitmap source
			API void SetWICBitmapSource(Microsoft::WRL::ComPtr<IWICFormatConverter>  pWicConverter);
			//Set Guassian blur amount
			API void SetGaussianBlurValue(D2D1_GAUSSIANBLUR_PROP pEffectProperty, float pValue);

			//Begins a sprite batch operation. Begin must be called successfully before any spriteBatch draw method can be called.
			API void Begin();

			//Draw a Direct2D spriteBatch drawable component
			API HRESULT Draw(_In_ Wolf::Graphics::Direct2D::ISpriteBatchDrawable* pSpriteBatchDrawableComp);
			
			//Draw constant 2D text using direct2D
			API HRESULT DrawString(const std::wstring& pText, _In_ DirectX::XMFLOAT2* const pPosition, _In_ const W_SpriteFont* pSpriteFont);
			//Draw 2D text using direct2D
			API HRESULT DrawString(std::wstring& pText, _In_ DirectX::XMFLOAT2* const pPosition, _In_ const W_SpriteFont* pSpriteFont);
			//Draw constant 2D text using direct2D
			API HRESULT DrawString(const std::wstring& pText, _In_ DirectX::XMFLOAT2* const pPosition, _In_ ID2D1SolidColorBrush* const pBrush,
				_In_ const W_SpriteFont* pSpriteFont, D2D1::Matrix3x2F pWorld);
			//Draw 2D text using direct2D
			API HRESULT DrawString(std::wstring& pText, _In_ DirectX::XMFLOAT2* const pPosition, _In_ ID2D1SolidColorBrush* const pBrush,
				_In_ const W_SpriteFont* pSpriteFont, D2D1::Matrix3x2F pWorld);

			//Draw texture2D using direct2D
			API void Draw(DirectX::XMFLOAT2 pPosition);

			//Ends a sprite batch operation. Begin cannot be called again until End has been successfully called.
			API HRESULT End();

			//Release all resources
			API virtual ULONG Release() override;

		private:
			enum SpriteState : byte { STARTED, ENDED };
			SpriteState												state;
			std::shared_ptr<W_GraphicsDevice>						gDevice;
			std::wstring											text;
	
			Microsoft::WRL::ComPtr<ID2D1Effect>						bitmapSourceEffect;

			Microsoft::WRL::ComPtr<IDWriteTextLayout>				textLayout;
			Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1>			stateBlock;

			//Effects
			Microsoft::WRL::ComPtr<ID2D1Effect>						directionalBlurEffect;
			Microsoft::WRL::ComPtr<ID2D1Effect>						gaussianBlurEffect;
		};
	}
}