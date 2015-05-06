/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_Geometry.h
	Description		 : Responsible for drawing geometry
	Comment          :
*/

#pragma once

#include <array>
#include <vector>
#include <string>
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
				class W_Geometry : public System::W_Object, public Wolf::Graphics::Direct2D::ISpriteBatchDrawable
				{
				public:
					API W_Geometry(const std::shared_ptr<Wolf::Graphics::W_GraphicsDevice>& pGDevice);
					API virtual ~W_Geometry();

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
					//Get fill mode
					API D2D1_FILL_MODE GetFillMode() const;
					//Get figure begin mode
					API D2D1_FIGURE_BEGIN GetFigureBegin() const;
					//Get figure end mode
					API D2D1_FIGURE_END GetFigureEnd() const;

#pragma endregion

#pragma region Setters

					//Set color of direct2d line
					API void SetColor(D2D1::ColorF pColor);
					
					//Set border color of direct2d line
					API void SetBorderColor(D2D1::ColorF pColor);
					
					//Begins the process of setting geometries. Begin must be called successfully before any of following methods "AddLines, AddBezier, AddArc" can be called.
					API void Begin();

					//Add lines to the geometry
					API void AddLines(_In_ const D2D1_POINT_2F& pStartPoint, 
						_In_ const std::vector<D2D1_POINT_2F> pPoints,
						D2D1_FILL_MODE pFillMode = D2D1_FILL_MODE_WINDING,
						D2D1_FIGURE_BEGIN pFigureBegin = D2D1_FIGURE_BEGIN_FILLED,
						D2D1_FIGURE_END pFigureEnd = D2D1_FIGURE_END_CLOSED);
					
					//Add list of beziers to the geometry
					API void AddBeziers(_In_ const D2D1_POINT_2F& pStartPoint,
						_In_ const std::vector<std::array<D2D1_POINT_2F, 3>> pPoints,
						D2D1_FILL_MODE pFillMode = D2D1_FILL_MODE_WINDING,
						D2D1_FIGURE_BEGIN pFigureBegin = D2D1_FIGURE_BEGIN_FILLED,
						D2D1_FIGURE_END pFigureEnd = D2D1_FIGURE_END_CLOSED);

					//Add single arc to the geometry
					API void AddArc(_In_ const D2D1_POINT_2F& pStartPoint,
						_In_ const D2D1_POINT_2F& pPoint,
						_In_ const D2D1_SIZE_F& pSize,
						float pRotationAngle,
						D2D1_SWEEP_DIRECTION sweepDirection,
						D2D1_ARC_SIZE arcSize,
						D2D1_FILL_MODE pFillMode = D2D1_FILL_MODE_WINDING,
						D2D1_FIGURE_BEGIN pFigureBegin = D2D1_FIGURE_BEGIN_FILLED,
						D2D1_FIGURE_END pFigureEnd = D2D1_FIGURE_END_CLOSED);

					//Ends the process of setting geometries. Begin cannot be called again until End has been successfully called.
					API void End();

#pragma endregion

				private:
					enum SpriteState : byte { STARTED, ENDED };
					SpriteState												state;

					std::shared_ptr<W_GraphicsDevice>						gDevice;
					Microsoft::WRL::ComPtr<ID2D1PathGeometry>				pathGeometry;

					float													strokeWidth;

					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			brush;
					Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>			borderBrush;

					Microsoft::WRL::ComPtr<ID2D1GeometrySink>				sink;

					D2D1_COLOR_F											color;
					D2D1_COLOR_F											borderColor;
					bool updateColor;
					bool updateBorderColor;
				};
			}
		}
	}
}

