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
				class W_Line : public System::W_Object, public Wolf::Graphics::Direct2D::ISpriteBatchDrawable
				{
				public:
					API W_Line(const std::shared_ptr<W_GraphicsDevice>& pGDevice, D2D1_POINT_2F pStartPoint, D2D1_POINT_2F pStoptPoint);
					API virtual ~W_Line();

					//Draw direct2D rectangle
					HRESULT Draw() override;

					//Release resources
					API ULONG Release() override;

#pragma region Getters

					//Set color of direct2d rectangle
					API D2D1::ColorF GetColor() const;
					//Set border color of direct2d rectangle
					API D2D1::ColorF GetBorderColor() const;
					//Get border color of direct2d rectangle
					API D2D1_POINT_2F GetStartPoint() const;
					//Get border color of direct2d rectangle
					API D2D1_POINT_2F GetStopPoint() const;

#pragma endregion

#pragma region Setters

					//Set color of direct2d line
					API void SetColor(D2D1::ColorF pColor);
					//Set border color of direct2d line
					API void SetBorderColor(D2D1::ColorF pColor);
					//Set geometry of line
					API void SetGeormetry(D2D1_POINT_2F pStartPoint, D2D1_POINT_2F pStopPoint);

#pragma endregion

				private:
					std::shared_ptr<W_GraphicsDevice>						gDevice;

					D2D1_POINT_2F											startPoint;
					D2D1_POINT_2F											stopPoint;
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

