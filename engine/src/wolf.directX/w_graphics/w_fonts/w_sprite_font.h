/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_sprite_font.h
	Description		 : Font class style
	Comment          :
*/

#ifndef __W_SPRITE_FONT_H__
#define __W_SPRITE_FONT_H__

#include "w_graphics_device_manager.h"

namespace wolf
{
	namespace graphics
	{
		//The structure of direct2D font style
		struct w_sprite_font_style
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
		class w_sprite_font : public system::w_object
		{
		public:
			//Constructor of W_SpriteFont
			DX_EXP w_sprite_font(w_sprite_font_style pFontStyle =
			{ 
				L"Calibri", 
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL, 
				DWRITE_TEXT_ALIGNMENT_LEADING, 
				DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
				20.0,
				L"en-US"
			});
			//Destructor of W_SpriteFont
			DX_EXP virtual ~w_sprite_font();

			//Load font style
			DX_EXP HRESULT load(_In_ ID2D1DeviceContext* pContext, _In_ IDWriteFactory* pWriteFactory);
			//Set color of font
			DX_EXP HRESULT set_color(_In_ ID2D1DeviceContext* pContext, D2D1::ColorF color);

			//Release all resources
			DX_EXP virtual ULONG release() override;

#pragma region Getters

			//Get pointer to the font
			DX_EXP IDWriteTextFormat*								get_font()  const							{ return this->_text_format.Get(); }
			//Get pointer to brush of the font
			DX_EXP ID2D1SolidColorBrush*							get_brush() const							{ return this->_brush.Get(); }
			//Get the text layout 
			DX_EXP IDWriteTextLayout*								get_text_layout()const						{ return this->_text_layout.Get(); }
			//Get font height in pixel
			DX_EXP float											get_font_size() const						{ return this->_fontStyle.SIZE; }
			//Get is right to left
			DX_EXP const bool										get_right_to_left() const					{ return this->_right_to_left; }
			//Get font height in pixel
			DX_EXP DWRITE_FONT_METRICS								get_font_metrics() const;
			
#pragma endregion

#pragma region Setters

			//Set is right toleft
			DX_EXP void											set_right_to_left(bool pValue)				{ this->_right_to_left = pValue; }
			//Set the text layout 
			void												set_text_layout(IDWriteTextLayout* pValue)	{ this->_text_layout.Attach(pValue); }

#pragma endregion

		private:
			typedef		system::w_object						_super;

			bool												_right_to_left;
			w_sprite_font_style									_fontStyle;
			Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>		_brush;
			Microsoft::WRL::ComPtr<IDWriteTextFormat>			_text_format;
			Microsoft::WRL::ComPtr<IDWriteTextLayout>			_text_layout;
			
		};
	}
}

#endif