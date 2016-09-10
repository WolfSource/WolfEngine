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
#include <w_color.h>

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
					DX_EXP w_rectangle(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
						_In_ float pLeft = 0, _In_ float pTop = 0,
						_In_ float pWidth = 400, _In_ float pHeight = 200, 
						_In_ float pRadiusX = 10, _In_ float pRadiusY = 10);
					DX_EXP virtual ~w_rectangle();

					//Draw direct2D rectangle
					HRESULT draw() override;

					//Release resources
					DX_EXP ULONG release() override;

#pragma region Getters

					//Set color of direct2d rectangle
					DX_EXP w_color get_color() const;
					//Set border color of direct2d rectangle
					DX_EXP w_color get_border_color() const;
					
					DX_EXP float get_radiusX() const;
					DX_EXP float get_radiusY() const;
					DX_EXP float get_left() const;
					DX_EXP float get_top() const;
					DX_EXP float get_width() const;
					DX_EXP float get_height() const;

#pragma endregion

#pragma region Setters

					//Set color of direct2d rectangle
					DX_EXP void set_color(_In_ const w_color pColor);
					//Set border color of direct2d rectangle
					DX_EXP void set_border_color(_In_ const w_color pColor);
					//Set geometry of rectangle
					DX_EXP void set_geormetry(_In_ float pLeft, _In_ float pTop, 
						_In_ float pWidth, _In_ float pHeight, 
						_In_ float pRadiusX, _In_ float pRadiusY);

#pragma endregion

				private:
					typedef		system::w_object							_super;

					std::shared_ptr<w_graphics_device>						_gDevice;

					D2D1_ROUNDED_RECT										_rectangle;
					float													_stroke_width;

					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			_brush;
					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			_border_brush;

					D2D1_COLOR_F											_color;
					D2D1_COLOR_F											_border_color;
					bool													_update_color;
					bool													_update_border_color;
				};
			}
		}
	}
}

#endif