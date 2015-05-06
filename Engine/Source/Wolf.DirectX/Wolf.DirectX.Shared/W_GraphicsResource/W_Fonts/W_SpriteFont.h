/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_SpriteFont.h
	Description		 : Font class style
	Comment          :
*/


#pragma once

#include "W_GraphicsDeviceManager.h"

namespace Wolf
{
	namespace Graphics
	{
		//The structure of direct2D font style
		struct W_SpriteFontStyle
		{
			const WCHAR*				NAME;
			DWRITE_FONT_WEIGHT			WEIGHT;
			DWRITE_FONT_STYLE			STYLE;
			DWRITE_FONT_STRETCH			STRETCH;
			DWRITE_TEXT_ALIGNMENT		TEXT_ALIGNMENT;
			DWRITE_PARAGRAPH_ALIGNMENT	PARAGRAPH_ALIGNMENT;
			float						SIZE;
			const WCHAR*				LOCAL;
		};

		//Represents a direct2D font asset
		class W_SpriteFont : public System::W_Object
		{
		public:
			//Constructor of W_SpriteFont
			API W_SpriteFont(const std::shared_ptr<W_GraphicsDevice>& pGDevice, W_SpriteFontStyle pFontStyle = 
			{ 
				L"Calibri", 
				DWRITE_FONT_WEIGHT_DEMI_BOLD, 
				DWRITE_FONT_STYLE_ITALIC, 
				DWRITE_FONT_STRETCH_NORMAL, 
				DWRITE_TEXT_ALIGNMENT_LEADING, 
				DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
				20.0,
				L"en-US"
			});
			//Destructor of W_SpriteFont
			API virtual ~W_SpriteFont();

			//Load font style
			API HRESULT Load();
			//Set color of font
			API HRESULT SetColor(D2D1::ColorF color);

			//Release all resources
			API virtual ULONG Release() override;

#pragma region Getters
			//Get pointer to the font
			IDWriteTextFormat*									GetFont()  const	{ return this->font.Get(); }
			//Get pointer to brush of the font
			ID2D1SolidColorBrush*								GetBrush() const	{ return this->brush.Get(); }
#pragma endregion

		private:
			W_SpriteFontStyle									fontStyle;
			std::shared_ptr<W_GraphicsDevice>					gDevice;
			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>		brush;
			Microsoft::WRL::ComPtr<IDWriteTextFormat>			font;
			Microsoft::WRL::ComPtr<IDWriteTextLayout>			textLayout;
		};
	}
}
