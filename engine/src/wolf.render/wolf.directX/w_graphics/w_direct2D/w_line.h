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

					//Set color of direct2d rectangle
					DX_EXP w_color			get_color() const;
					//Get border color of direct2d rectangle
					DX_EXP D2D1_POINT_2F	get_start_point() const;
					//Get border color of direct2d rectangle
					DX_EXP D2D1_POINT_2F	get_stop_point() const;
					//Get stroke width
					DX_EXP float			get_stroke_width() const						{ return this->_stroke_width; }

#pragma endregion

#pragma region Setters

					//Set color of direct2d line
					DX_EXP void				set_color(_In_ const w_color pColor);
					//Set geometry of line
					DX_EXP void				set_geormetry(_In_ const D2D1_POINT_2F pStartPoint, _In_ const D2D1_POINT_2F pStopPoint);
					//Set stroke width
					DX_EXP void				set_stroke_width(_In_ const float pValue)		{ this->_stroke_width = pValue; }

#pragma endregion

				private:
					typedef		system::w_object							_super;

					std::shared_ptr<w_graphics_device>						_gDevice;

					D2D1_POINT_2F											_start_point;
					D2D1_POINT_2F											_stop_point;
					float													_stroke_width;

					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			_brush;

					D2D1_COLOR_F											_color;
					bool													_update_color;
					bool													_update_border_color;
				};
			}
		}
	}
}


#endif
