/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_button.h
	Description		 : The button user control class.This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_BUTTON_H__
#define __W_BUTTON_H__

#include "w_label.h"
#include "w_image.h"

#define W_GUI_EVENT_BUTTON_CLICKED  0x0101
#define W_GUI_NEAR_BUTTON_DEPTH		0.6f
#define W_GUI_FAR_BUTTON_DEPTH		0.8f

namespace wolf
{
	namespace gui
	{
		class w_button : public w_label
		{
		public:
			DX_EXP						w_button(_In_opt_ w_widget* pParent = nullptr);
			DX_EXP	virtual				~w_button();

			virtual HRESULT				on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice) override;
			virtual bool				handle_keyboard(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual bool				handle_mouse(_In_ UINT pMsg, _In_ const POINT& pPoint, _In_ WPARAM pwParam, _In_ LPARAM pLParam) override;
			virtual void				on_hot_key() override;
			virtual bool				contains_point(_In_ const POINT& pPoint) override;
			virtual bool				can_have_focus() override;
			virtual void				render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;
			virtual ULONG				release() override;

#pragma region Getters				  
				
			DX_EXP float				get_effect_min_value() const						{ return this->effect_min_value; }
			DX_EXP float				get_effect_max_value() const					    { return this->effect_max_value; }
			
			DX_EXP w_color				get_button_color() const { return w_color::from_hex(this->button_color); }
			DX_EXP w_color				get_button_pressed_color() const					{ return w_color::from_hex(this->button_pressed_color); }
			DX_EXP w_color				get_button_mouse_over_color() const					{ return w_color::from_hex(this->button_mouse_over_color); }
			DX_EXP w_color				get_button_focused_color() const					{ return w_color::from_hex(this->button_focused_color); }
			DX_EXP w_color				get_button_disabled_color() const					{ return w_color::from_hex(this->button_disabled_color); }

			DX_EXP const wchar_t*		get_icon_path() const								{ return this->icon_path.c_str(); }
			DX_EXP DirectX::XMFLOAT2	get_icon_offset( )const								{ return DirectX::XMFLOAT2((float)this->icon_offset_x , (float)this->icon_offset_y); }
			DX_EXP DirectX::XMFLOAT2	get_icon_scale()const								{ return DirectX::XMFLOAT2(this->icon_scale_x, this->icon_scale_y); }
									  
#pragma endregion					  
									  
#pragma region Setters				  
									  
			DX_EXP void					set_effect_min_max_range(float pMin, float pMax)	{ this->effect_min_value = pMin; this->effect_max_value = pMax; }
										
			DX_EXP void					set_button_color(w_color pValue)					{ this->button_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void					set_button_pressed_color(w_color pValue)			{ this->button_pressed_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void					set_button_mouse_over_color(w_color pValue)			{ this->button_mouse_over_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void					set_button_focused_color(w_color pValue)			{ this->button_focused_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void					set_button_disabled_color(w_color pValue)			{ this->button_disabled_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }

			DX_EXP void					set_icon_path(_In_z_ const wchar_t* pValue)			{ this->icon_path = std::wstring(pValue); }
			DX_EXP void					set_icon_offset(int pX, int pY)						{ this->icon_offset_x = pX; this->icon_offset_y = pY; }
			DX_EXP void					set_icon_scale(float pX, float pY)					{ this->icon_scale_x = pX; this->icon_scale_y = pY; }

#pragma endregion					  
									  
		protected:					  
			bool						pressed;
			float						effect_min_value;
			float						effect_max_value;
			w_image*					icon;

			DWORD						button_color;
			DWORD						button_pressed_color;
			DWORD						button_mouse_over_color;
			DWORD						button_focused_color;
			DWORD						button_disabled_color;

			std::wstring				icon_path;
			int							icon_offset_x;
			int							icon_offset_y;
			float						icon_scale_x;
			float						icon_scale_y;

		private:
			typedef w_label				_super;


		};
	}
}

#endif
