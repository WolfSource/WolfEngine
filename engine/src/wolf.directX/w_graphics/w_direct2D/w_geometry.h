/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_geometry.h
	Description		 : Responsible for drawing geometry
	Comment          :
*/

#ifndef __W_GEOMETRY_H__
#define __W_GEOMETRY_H__

#include <array>
#include <vector>
#include <string>
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
				class w_geometry : public system::w_object, public wolf::graphics::direct2D::Isprite_batch_drawable
				{
				public:
					DX_EXP w_geometry(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);
					DX_EXP virtual ~w_geometry();

					//Draw direct2D rectangle
					HRESULT draw() override;

					//Release resources
					DX_EXP ULONG release() override;

#pragma region Getters

					//Set color of direct2d rectangle
					DX_EXP D2D1::ColorF get_color() const;
					//Set border color of direct2d rectangle
					DX_EXP D2D1::ColorF get_borderColor() const;
					//Get border color of direct2d rectangle
					DX_EXP D2D1_POINT_2F get_startPoint() const;
					//Get border color of direct2d rectangle
					DX_EXP D2D1_POINT_2F get_stopPoint() const;
					//Get fill mode
					DX_EXP D2D1_FILL_MODE get_fillMode() const;
					//Get figure begin mode
					DX_EXP D2D1_FIGURE_BEGIN get_figureBegin() const;
					//Get figure end mode
					DX_EXP D2D1_FIGURE_END get_figureEnd() const;

#pragma endregion

#pragma region Setters

					//Set color of direct2d line
					DX_EXP void set_color(D2D1::ColorF pColor);
					
					//Set border color of direct2d line
					DX_EXP void set_borderColor(D2D1::ColorF pColor);
					
					//Begins the process of setting geometries. Begin must be called successfully before any of following methods "AddLines, AddBezier, AddArc" can be called.
					DX_EXP void begin();

					//Add lines to the geometry
					DX_EXP void add_lines(_In_ const D2D1_POINT_2F& pStartPoint,
						_In_ const std::vector<D2D1_POINT_2F> pPoints,
						D2D1_FILL_MODE pFillMode = D2D1_FILL_MODE_WINDING,
						D2D1_FIGURE_BEGIN pFigureBegin = D2D1_FIGURE_BEGIN_FILLED,
						D2D1_FIGURE_END pFigureEnd = D2D1_FIGURE_END_CLOSED);
					
					//Add list of beziers to the geometry
					DX_EXP void add_beziers(_In_ const D2D1_POINT_2F& pStartPoint,
						_In_ const std::vector<std::array<D2D1_POINT_2F, 3>> pPoints,
						D2D1_FILL_MODE pFillMode = D2D1_FILL_MODE_WINDING,
						D2D1_FIGURE_BEGIN pFigureBegin = D2D1_FIGURE_BEGIN_FILLED,
						D2D1_FIGURE_END pFigureEnd = D2D1_FIGURE_END_CLOSED);

					//Add single arc to the geometry
					DX_EXP void add_arc(_In_ const D2D1_POINT_2F& pStartPoint,
						_In_ const D2D1_POINT_2F& pPoint,
						_In_ const D2D1_SIZE_F& pSize,
						float pRotationAngle,
						D2D1_SWEEP_DIRECTION sweepDirection,
						D2D1_ARC_SIZE arcSize,
						D2D1_FILL_MODE pFillMode = D2D1_FILL_MODE_WINDING,
						D2D1_FIGURE_BEGIN pFigureBegin = D2D1_FIGURE_BEGIN_FILLED,
						D2D1_FIGURE_END pFigureEnd = D2D1_FIGURE_END_CLOSED);

					//Ends the process of setting geometries. Begin cannot be called again until End has been successfully called.
					DX_EXP void end();

#pragma endregion

				private:
					typedef	 system::w_object								_super;

					enum SpriteState : byte { STARTED, ENDED };
					SpriteState												_state;

					std::shared_ptr<w_graphics_device>						_gDevice;
					Microsoft::WRL::ComPtr<ID2D1PathGeometry>				_pathGeometry;

					float													_strokeWidth;

					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			_brush;
					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			_borderBrush;

					Microsoft::WRL::ComPtr<ID2D1GeometrySink>				_sink;

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

