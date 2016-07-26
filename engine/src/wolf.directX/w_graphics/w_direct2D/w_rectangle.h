/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_rectangle.h
	Description		 : Responsible for drawing direct2D rectangle
	Comment          :
*/

#ifndef __W_RECTANGLE_H__
#define __W_RECTANGLE_H__

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
				class w_rectangle : public system::w_object, public wolf::graphics::direct2D::Isprite_batch_drawable
				{
				public:
					DX_EXP w_rectangle(const std::shared_ptr<w_graphics_device>& pGDevice,
						float pLeft = 0, float pTop = 0, float pWidth = 400, float pHeight = 200, float pRadiusX = 10, float pRadiusY = 10);
					DX_EXP virtual ~w_rectangle();

					//Draw direct2D rectangle
					HRESULT draw() override;

					//Release resources
					DX_EXP ULONG release() override;

#pragma region Getters

					//Set color of direct2d rectangle
					DX_EXP D2D1::ColorF get_color() const;
					//Set border color of direct2d rectangle
					DX_EXP D2D1::ColorF get_borderColor() const;
					
					DX_EXP float get_radiusX() const;
					DX_EXP float get_radiusY() const;
					DX_EXP float get_left() const;
					DX_EXP float get_top() const;
					DX_EXP float get_width() const;
					DX_EXP float get_height() const;

#pragma endregion

#pragma region Setters

					//Set color of direct2d rectangle
					DX_EXP void set_color(D2D1::ColorF pColor);
					//Set border color of direct2d rectangle
					DX_EXP void set_borderColor(D2D1::ColorF pColor);
					//Set geometry of rectangle
					DX_EXP void set_geormetry(float pLeft, float pTop, float pWidth, float pHeight, float pRadiusX, float pRadiusY);

#pragma endregion

				private:
					typedef		system::w_object							_super;

					std::shared_ptr<w_graphics_device>						_gDevice;

					D2D1_ROUNDED_RECT										_rectangle;
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