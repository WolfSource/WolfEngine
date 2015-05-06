/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_Rectangle2D.h
	Description		 : Responsible for drawing direct2D rectangle
	Comment          :
*/

#pragma once

#include "ISpriteBatchDrawable.h"
#include "W_GraphicsDeviceManager.h"

namespace Wolf
{
	namespace Graphics
	{
		namespace Direct2D
		{
			namespace Shapes
			{
				class W_Rectangle : public System::W_Object, public Wolf::Graphics::Direct2D::ISpriteBatchDrawable
				{
				public:
					API W_Rectangle(const std::shared_ptr<W_GraphicsDevice>& pGDevice,
						float pLeft = 0, float pTop = 0, float pWidth = 400, float pHeight = 200, float pRadiusX = 10, float pRadiusY = 10);
					API virtual ~W_Rectangle();

					//Draw direct2D rectangle
					HRESULT Draw() override;

					//Release resources
					API ULONG Release() override;

#pragma region Getters

					//Set color of direct2d rectangle
					API D2D1::ColorF GetColor() const;
					//Set border color of direct2d rectangle
					API D2D1::ColorF GetBorderColor() const;
					
					API float GetRadiusX() const;
					API float GetRadiusY() const;
					API float GetLeft() const;
					API float GetTop() const;
					API float GetWidth() const;
					API float GetHeight() const;

#pragma endregion

#pragma region Setters

					//Set color of direct2d rectangle
					API void SetColor(D2D1::ColorF pColor);
					//Set border color of direct2d rectangle
					API void SetBorderColor(D2D1::ColorF pColor);
					//Set geometry of rectangle
					API void SetGeormetry(float pLeft, float pTop, float pWidth, float pHeight, float pRadiusX, float pRadiusY);

#pragma endregion

				private:
					std::shared_ptr<W_GraphicsDevice>						gDevice;

					D2D1_ROUNDED_RECT										rectangle;
					float													strokeWidth;

					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			brush;
					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			borderBrush;

					D2D1_COLOR_F											color;
					D2D1_COLOR_F											borderColor;
					bool updateColor;
					bool updateBorderColor;
				};
			}
		}
	}
}

