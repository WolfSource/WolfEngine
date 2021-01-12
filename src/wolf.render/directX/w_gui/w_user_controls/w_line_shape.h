/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_line_shape.h
	Description		 : The line shape control

	Comment          :
*/

#ifndef __W_LINE_SHAPE__
#define __W_LINE_SHAPE__

#include "w_control.h"
#include <w_graphics/w_direct2D/w_line.h>
#include <w_color.h>

namespace wolf
{
	namespace gui
	{
		class w_line_shape : public w_control
		{
		public:
			DX_EXP w_line_shape(_In_opt_ w_widget* pParent);
			DX_EXP ~w_line_shape();

			virtual HRESULT									on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice) override;
			virtual bool									contains_point(_In_ const POINT& pt) override;
			virtual bool									can_have_focus() override;
			virtual void									render(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;
			virtual ULONG									release() override;

#pragma region Getter
			
			DX_EXP w_color									get_color() const								{ return this->color; }
			DX_EXP w_color									get_disabled_color() const						{ return this->disabled_color; }
			DX_EXP D2D1_POINT_2F							get_line_start_point() const					{ return this->start_point; }
			DX_EXP D2D1_POINT_2F							get_line_stop_point() const						{ return this->stop_point; }
			DX_EXP float									get_stroke_width() const						{ return this->stroke_width; }

#pragma endregion

#pragma region Setter

			DX_EXP void										set_color(_In_ const w_color pValue)			{ this->color = pValue; }
			DX_EXP void										set_disabled_color(_In_ const w_color pValue)	{ this->disabled_color = pValue; }
			DX_EXP void										set_geormetry(_In_ const float pStartPointX, _In_ const float pStartPointY,
				_In_ const float pStopPointX, _In_ const float pStopPointY);
			DX_EXP void										set_stroke_width(_In_ const float pValue);
			
#pragma endregion

		protected:
			
			w_color											color;
			w_color											disabled_color;
			D2D1_POINT_2F									start_point;
			D2D1_POINT_2F									stop_point;
			float											stroke_width;

		private:
			typedef w_control								_super;

			wolf::graphics::direct2D::shapes::w_line*		_line;
		};
	}
}

#endif
