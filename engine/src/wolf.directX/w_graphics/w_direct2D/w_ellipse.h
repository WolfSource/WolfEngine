/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_ellipse.h
	Description		 : Responsible for drawing ellipse
	Comment          :
*/

#ifndef __W_ELLIPSE_H__
#define __W_ELLIPSE_H__

#include "Isprite_batch_drawable.h"
#include "w_graphics_device_manager.h"

namespace wolf
{
	namespace graphics
	{
		namespace direct2D
		{
			namespace shapes
			{
				class w_ellipse : public system::w_object, public wolf::graphics::direct2D::Isprite_batch_drawable
				{
				public:
					DX_EXP w_ellipse(const std::shared_ptr<w_graphics_device>& pGDevice,
						float pCenterX = 50, float pCenterY = 50, float pRadiusX = 17, float pRadiusY = 17);
					DX_EXP virtual ~w_ellipse();

					//Draw direct2D ellipse
					HRESULT draw() override;

					DX_EXP ULONG release() override;

#pragma region Getters

					//Set color of direct2d ellipse
					DX_EXP D2D1::ColorF get_color() const;
					//Set border color of direct2d ellipse
					DX_EXP D2D1::ColorF get_borderColor() const;
					
					DX_EXP D2D1_POINT_2F get_radius() const;
					DX_EXP D2D1_POINT_2F get_center() const;

#pragma endregion

#pragma region Setters

					//Set color of direct2d ellipse
					DX_EXP void set_color(D2D1::ColorF pColor);
					//Set border color of direct2d ellipse
					DX_EXP void set_borderColor(D2D1::ColorF pColor);
					//Set geometry of ellipse
					DX_EXP void set_geormetry(float pCenterX, float pCenterY, float pRadiusX, float pRadiusY);

#pragma endregion

				private:
					typedef	system::w_object								_super;
					std::shared_ptr<w_graphics_device>						_gDevice;

					D2D1_ELLIPSE											_ellipse;
					float													_strokeWidth;

					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			_brush;
					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			_borderBrush;

					D2D1_COLOR_F											_color;
					D2D1_COLOR_F											_borderColor;
					bool													_updateColor;
					bool													_updateBorderColor;
				};
			}
		}
	}
}

#endif
