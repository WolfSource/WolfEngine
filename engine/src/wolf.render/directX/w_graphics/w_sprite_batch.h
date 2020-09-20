/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : W_SpriteBatch.h
	Description		 : Responsible for drawing texture2D & 2D texts
	Comment          :
*/

#ifndef __W_SPRITE_BATCH_H__
#define __W_SPRITE_BATCH_H__

#include <Wincodec.h>
#include <DirectXMath.h>
#include "w_graphics_device_manager.h"
#include "w_fonts\w_sprite_font.h"
#include "w_direct2D\Isprite_batch_drawable.h"
#include "w_direct2D\w_rectangle.h"
#include "w_direct2D\w_ellipse.h"
#include "w_direct2D\w_line.h"

namespace wolf
{
	namespace graphics
	{
		//Enables a group of sprites or direct2D texts using the same settings, also represent methods for drawing 2D texts.
		class w_sprite_batch : public system::w_object
		{
		public:
			DX_EXP w_sprite_batch(const std::shared_ptr<w_graphics_device>& pGDevice);
			DX_EXP virtual ~w_sprite_batch();

			//Load SpriteBatch
			DX_EXP void load();
			//Add image from file
			DX_EXP HRESULT add_image_from_file(_In_z_ const wchar_t* pPath);
			//Add direct2D effect
			DX_EXP void add_effect(_In_ REFCLSID pEffectId);

			// resize render target
			DX_EXP void resize_render_target(int pWidth, int pHeight);

			//Begins a sprite batch operation. Begin must be called successfully before any spriteBatch draw method can be called.
			DX_EXP void begin(_In_ bool pSaveState = false);
			//Begins a sprite batch operation. Begin must be called successfully before any spriteBatch draw method can be called.
			DX_EXP void begin(_In_ D2D1::ColorF pClearColor, _In_ bool pRenderToTarget = false, _In_ bool pSaveState = false);

			//Draw a Direct2D spriteBatch drawable component
			DX_EXP HRESULT draw(_In_ wolf::graphics::direct2D::Isprite_batch_drawable* pSpriteBatchDrawableComp);
			
			//Draw 2d text using rectangle
			DX_EXP HRESULT draw_string(_In_z_ const std::wstring& pText, 
				const D2D1_RECT_F pRectangle,
				_In_ ID2D1SolidColorBrush* pBrush = nullptr, 
				_In_ w_sprite_font* pSpriteFont = nullptr,
				const D2D1_DRAW_TEXT_OPTIONS pOptions = D2D1_DRAW_TEXT_OPTIONS::D2D1_DRAW_TEXT_OPTIONS_NONE,
				const DWRITE_MEASURING_MODE pMeasuringMode = DWRITE_MEASURING_MODE::DWRITE_MEASURING_MODE_NATURAL,
				D2D1::Matrix3x2F pWorld = D2D1::Matrix3x2F::Identity());
			
			//Draw 2D text using texture layout
			DX_EXP HRESULT draw_string(_In_z_ const std::wstring& pText, 
				_In_ const DirectX::XMFLOAT2 pPosition, 
				_In_ ID2D1SolidColorBrush* const pBrush,
				_In_ const w_sprite_font* pSpriteFont = nullptr, 
				_In_ D2D1::Matrix3x2F pWorld = D2D1::Matrix3x2F::Identity());
			
			//Draw image using direct2D
			DX_EXP HRESULT draw_image(_In_z_ const wchar_t* pImageName, 
				_In_ const DirectX::XMFLOAT2 pPosition,
				_In_ const DirectX::XMFLOAT2 pScale = DirectX::XMFLOAT2(1, 1), _In_ const DirectX::XMFLOAT2 pScaleCenter = DirectX::XMFLOAT2(0, 0),
				_In_ const float pRotationAngle = 0, _In_ const DirectX::XMFLOAT2 pRotationCenter = DirectX::XMFLOAT2(0, 0),
				_In_ const DirectX::XMFLOAT2 pTranslation = DirectX::XMFLOAT2(0, 0));

			//Draw image using direct2D
			DX_EXP HRESULT draw_image(_In_z_ const wchar_t* pImageName,
				_In_ const DirectX::XMFLOAT2 pPosition,
				_In_ D2D1_RECT_F pRectangle,
				_In_ const DirectX::XMFLOAT2 pScale = DirectX::XMFLOAT2(1, 1), _In_ const DirectX::XMFLOAT2 pScaleCenter = DirectX::XMFLOAT2(0, 0),
				_In_ const float pRotationAngle = 0, _In_ const DirectX::XMFLOAT2 pRotationCenter = DirectX::XMFLOAT2(0, 0),
				_In_ const DirectX::XMFLOAT2 pTranslation = DirectX::XMFLOAT2(0, 0));

			//Ends a sprite batch operation. Begin cannot be called again until End has been successfully called.
			DX_EXP HRESULT end();
			
			//Release all resources
			DX_EXP virtual ULONG release() override;

#pragma region Getters

			//Get the resource of render target texture 2D
			DX_EXP ID3D11Resource*										get_resource() const;
			DX_EXP ID3D11ShaderResourceView*							get_srv() const;
			DX_EXP DWRITE_FONT_METRICS									get_debug_font_metrics() const							{ return this->_debug_sprite_font->get_font_metrics(); }
			DX_EXP float												get_debug_font_size() const								{ return this->_debug_sprite_font->get_font_size(); }

#pragma endregion

#pragma region Setters

			//set image format converter to bitmap source effect
			DX_EXP void set_bitmap_effect_source(IWICFormatConverter* pWICFormatConverter);
			//Set Guassian blur properties
			DX_EXP void set_gaussian_blur_value(D2D1_GAUSSIANBLUR_PROP pEffectProperty, float pValue);
			//Set scale properties
			DX_EXP void set_scale_value(D2D1_SCALE_PROP pEffectProperty, float pValue);

#pragma endregion

		private:
			typedef  system::w_object								_super;

			enum SpriteState : byte { STARTED, ENDED };
			SpriteState												_state;
			std::shared_ptr<w_graphics_device>						_gDevice;
			std::wstring											_text;
	
			Microsoft::WRL::ComPtr<ID2D1Effect>						_bitmap_source_effect;
			std::map<std::wstring, IWICFormatConverter*>			_WIC_format_converters;//Unique path of image with related _WIC_format_converter
						
			Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1>			_state_block;

			//Effects
			Microsoft::WRL::ComPtr<ID2D1Effect>						_directional_blur_effect;
			Microsoft::WRL::ComPtr<ID2D1Effect>						_gaussian_blur_effect;
			Microsoft::WRL::ComPtr<ID2D1Effect>						_scale_effect;

			
			Microsoft::WRL::ComPtr<ID2D1Bitmap1>					_render_target;
			Microsoft::WRL::ComPtr<ID3D11Texture2D>					_render_target_texture_2D;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		_render_target_texture_srv;

			//SpriteFont object of debug font
			std::unique_ptr<wolf::graphics::w_sprite_font>			_debug_sprite_font;
			
		};
	}
}

#endif