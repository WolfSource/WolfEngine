/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_line.h
	Description		 : Responsible for drawing direct2D line
	Comment          :
*/

#ifndef __W_LINE_H__
#define __W_LINE_H__

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
				class w_line : public system::w_object, public wolf::graphics::direct2D::Isprite_batch_drawable
				{
				public:
					DX_EXP w_line(const std::shared_ptr<w_graphics_device>& pGDevice, D2D1_POINT_2F pStartPoint, D2D1_POINT_2F pStoptPoint);
					DX_EXP virtual ~w_line();

					//Draw direct2D rectangle
					HRESULT draw() override;

					//Release resources
					DX_EXP ULONG release() override;

#pragma region Getters

					////Set color of direct2d rectangle
					//DX_EXP D2D1::ColorF get_color() const;
					////Set border color of direct2d rectangle
					//DX_EXP D2D1::ColorF get_borderColor() const;
					//Get border color of direct2d rectangle
					DX_EXP D2D1_POINT_2F get_startPoint() const;
					//Get border color of direct2d rectangle
					DX_EXP D2D1_POINT_2F get_stopPoint() const;

#pragma endregion

#pragma region Setters

					//Set color of direct2d line
					DX_EXP void set_color(D2D1::ColorF pColor);
					//Set border color of direct2d line
					DX_EXP void set_borderColor(D2D1::ColorF pColor);
					//Set geometry of line
					DX_EXP void set_geormetry(D2D1_POINT_2F pStartPoint, D2D1_POINT_2F pStopPoint);

#pragma endregion

				private:
					typedef		system::w_object							_super;

					std::shared_ptr<w_graphics_device>						_gDevice;

					D2D1_POINT_2F											_startPoint;
					D2D1_POINT_2F											_stopPoint;
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
