/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_Ellipse.h
	Description		 : Responsible for drawing ellipse
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
				class W_Ellipse : public System::W_Object, public Wolf::Graphics::Direct2D::ISpriteBatchDrawable
				{
				public:
					API W_Ellipse(const std::shared_ptr<W_GraphicsDevice>& pGDevice, 
						float pCenterX = 50, float pCenterY = 50, float pRadiusX = 17, float pRadiusY = 17);
					API virtual ~W_Ellipse();

					//Draw direct2D ellipse
					HRESULT Draw() override;

					API ULONG Release() override;

#pragma region Getters

					//Set color of direct2d ellipse
					API D2D1::ColorF GetColor() const;
					//Set border color of direct2d ellipse
					API D2D1::ColorF GetBorderColor() const;
					
					API D2D1_POINT_2F GetRadius() const;
					API D2D1_POINT_2F GetCenter() const;

#pragma endregion

#pragma region Setters

					//Set color of direct2d ellipse
					API void SetColor(D2D1::ColorF pColor);
					//Set border color of direct2d ellipse
					API void SetBorderColor(D2D1::ColorF pColor);
					//Set geometry of ellipse
					API void SetGeormetry(float pCenterX, float pCenterY, float pRadiusX, float pRadiusY);

#pragma endregion

				private:
					std::shared_ptr<W_GraphicsDevice>						gDevice;

					D2D1_ELLIPSE											ellipse;
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

