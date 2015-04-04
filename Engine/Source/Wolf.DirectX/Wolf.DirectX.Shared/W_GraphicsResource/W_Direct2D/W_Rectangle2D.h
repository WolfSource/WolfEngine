/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @PooyaEimandar on twitter
	Name			 : W_Rectangle2D.h
	Description		 : Responsible for drawing direct2D rectangle
	Comment          :
*/

#pragma once

#include "W_GraphicsDeviceManager.h"

namespace Wolf
{
	namespace Graphics
	{
		namespace Direct2D
		{
			class W_Rectangle2D : System::W_Object
			{
			public:
				API W_Rectangle2D(const std::shared_ptr<W_GraphicsDevice>& pGDevice);
				API virtual ~W_Rectangle2D();
				
				//Draw direct2D rectangle
				HRESULT Draw();

				API ULONG Release() override;

#pragma region Getters

				//Set color of direct2d rectangle
				API D2D1::ColorF GetColor();
				//Set border color of direct2d rectangle
				API D2D1::ColorF GetBorderColor();
				API float GetLeft();
				API float GetTop();
				API float GetWidth();
				API float GetHeight();

#pragma endregion

#pragma region Setters

				//Set color of direct2d rectangle
				API void SetColor(D2D1::ColorF pColor);
				//Set border color of direct2d rectangle
				API void SetBorderColor(D2D1::ColorF pColor);
				//Set geometry of rectangle
				API void SetGeormetry(float pLeft, float pTop, float pWidth, float pHeight);
				
#pragma endregion

			private:
				std::shared_ptr<W_GraphicsDevice>						gDevice;
				D2D1_RECT_F												rectangle;
				float													strokeWidth;
				Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			brush;
				Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			border;
				D2D1_COLOR_F											color;
				D2D1_COLOR_F											borderColor;
				bool updateColor;
				bool updateBorderColor;
			};
		}
	}
}

