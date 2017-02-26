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

					//Set fill geometry
					DX_EXP bool get_fill_geometry() const						{ return this->_fill_geometry; }

					//Set color of direct2d rectangle
					DX_EXP w_color		get_color() const;
					//Set border color of direct2d rectangle
					DX_EXP w_color		get_border_color() const;
					//Get border color of direct2d rectangle
					DX_EXP D2D1_POINT_2F get_start_point() const;
					//Get border color of direct2d rectangle
					DX_EXP D2D1_POINT_2F get_stop_point() const;
					//Get fill mode
					DX_EXP D2D1_FILL_MODE get_fill_mode() const;
					//Get figure begin mode
					DX_EXP D2D1_FIGURE_BEGIN get_figure_begin() const;
					//Get figure end mode
					DX_EXP D2D1_FIGURE_END get_figure_end() const;

#pragma endregion

#pragma region Setters

					//Set fill geometry style
					DX_EXP HRESULT set_style(D2D1_STROKE_STYLE_PROPERTIES pStyle);

					//Set fill geometry
					DX_EXP void set_fill_geometry(_In_ const bool pValue)					{ this->_fill_geometry = pValue; }

					//Set fill geometry
					DX_EXP void set_stroke_width(_In_ const float pValue)					{ this->_stroke_width = pValue; }

					//Set color of direct2d line
					DX_EXP void set_color(_In_ const w_color pColor);
					
					//Set border color of direct2d line
					DX_EXP void set_border_color(_In_ const w_color pColor);
					
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
					Microsoft::WRL::ComPtr<ID2D1PathGeometry>				_path_geometry;
					bool													_fill_geometry;

					float													_stroke_width;

					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			_brush;
					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			_border_brush;
					Microsoft::WRL::ComPtr<ID2D1StrokeStyle>				_style;

					Microsoft::WRL::ComPtr<ID2D1GeometrySink>				_sink;

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

