/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_combo_box.h
	Description		 : The comboBox user control class.This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_COMBOBOX_H__
#define __W_COMBOBOX_H__

#include "w_button.h"
#include "w_scroll_bar.h"
#include <vector>
#include "w_directX_math_helper.h"

#define W_EVENT_COMBOBOX_SELECTION_CHANGED    0x0201

namespace wolf
{
	namespace gui
	{
		struct w_combo_box_item
		{
			wchar_t text[256];
			void*	data;
			RECT	active;
			bool	visible;
		};

		class w_combo_box : public w_button
		{
		public:
			w_combo_box(_In_opt_ w_widget* pParent = nullptr);
			virtual ~w_combo_box();

			virtual HRESULT on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice) override;

#pragma region Getters

			DX_EXP	bool				get_is_open() const											{ return this->is_opened; }
			DX_EXP	int					get_scroll_bar_width() const								{ return this->scrollBar_width; }
			DX_EXP	int					get_selected_index() const									{ return m_iSelected; }
			DX_EXP	UINT				get_num_items()												{ return static_cast<UINT>(this->items.size()); }
			DX_EXP	w_combo_box_item*	get_item(_In_ UINT index)									{ return this->items[index]; }
			DX_EXP	void*				get_item_data(_In_z_ const wchar_t* strText) const;
			DX_EXP	void*				get_item_data(_In_ int nIndex) const;
			DX_EXP	void*				get_selected_data() const;
			DX_EXP	w_combo_box_item*	get_selected_item() const;
			
			DX_EXP	UINT				get_item_height() const										{ return this->_item_height; }

			DX_EXP  int					get_item_margin_x() const									{ return static_cast<int>(this->_item_margin.x); }
			DX_EXP  int					get_item_margin_y() const									{ return static_cast<int>(this->_item_margin.y); }

			DX_EXP w_color				get_combo_box_color() const									{ return w_color::from_hex(this->_combo_box_main_color); }
			DX_EXP w_color				get_combo_box_pressed_color() const							{ return w_color::from_hex(this->_combo_box_main_pressed_color); }
			DX_EXP w_color				get_combo_box_mouse_over_color() const						{ return w_color::from_hex(this->_combo_box_main_mouse_over_color); }
			DX_EXP w_color				get_combo_box_focused_color() const							{ return w_color::from_hex(this->_combo_box_main_focused_color); }
			DX_EXP w_color				get_combo_box_disabled_color() const						{ return w_color::from_hex(this->_combo_box_main_disabled_color); }

			DX_EXP w_color				get_combo_box_button_color() const							{ return w_color::from_hex(this->_combo_box_button_color); }
			DX_EXP w_color				get_combo_box_button_mouse_over_color() const				{ return w_color::from_hex(this->_combo_box_button_mouse_over_color); }
			DX_EXP w_color				get_combo_box_button_pressed_color() const					{ return w_color::from_hex(this->_combo_box_button_pressed_color); }
			DX_EXP w_color				get_combo_box_button_focused_color() const					{ return w_color::from_hex(this->_combo_box_button_focused_color); }
			DX_EXP w_color				get_combo_box_button_disabled_color() const					{ return w_color::from_hex(this->_combo_box_button_disabled_color); }
								 
			DX_EXP w_color				get_combo_box_drop_down_context_color() const				{ return w_color::from_hex(this->_combo_box_drop_down_context_color); }
								 
			DX_EXP w_color				get_combo_box_selected_color() const						{ return DirectX::XMFLOAT4_TO_W_COLOR(this->_combo_box_selected_color); }
			DX_EXP w_color				get_combo_box_selected_font_color() const					{ return this->_combo_box_selected_font_color; }
								 
			DX_EXP w_color				get_combo_box_scroll_color() const							{ return this->_combo_box_scroll_color; }
			DX_EXP w_color				get_combo_box_scroll_background_color() const				{ return this->_combo_box_scroll_background_color; }
			DX_EXP w_color				get_combo_box_scroll_disabled_color() const					{ return this->_combo_box_scroll_disabled_color; }

#pragma endregion

#pragma region Setters

			DX_EXP void					set_drop_down_context_height(_In_ UINT pHeight);
			DX_EXP void					set_scroll_bar_width(_In_ int nWidth);
			DX_EXP HRESULT				set_selected_by_index(_In_ UINT index);
			DX_EXP HRESULT				set_selected_by_text(_In_z_ const wchar_t* strText);
			DX_EXP HRESULT				set_selected_by_data(_In_ void* pData);

			DX_EXP void					set_item_height(UINT pValue)								{ this->_item_height = pValue; }
			DX_EXP void					set_item_margin(int pX, int pY)								{ this->_item_margin.x = pX; this->_item_margin.y = pY;}
			
			DX_EXP void					set_combo_box_color(w_color pValue)							{ this->_combo_box_main_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void					set_combo_box_pressed_color(w_color pValue)					{ this->_combo_box_main_pressed_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void					set_combo_box_mouse_over_color(w_color pValue)				{ this->_combo_box_main_mouse_over_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void					set_combo_box_focused_color(w_color pValue)					{ this->_combo_box_main_focused_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void					set_combo_box_disabled_color(w_color pValue)				{ this->_combo_box_main_disabled_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }

			DX_EXP void					set_combo_box_button_color(w_color pValue)					{ this->_combo_box_button_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void					set_combo_box_button_mouse_over_color(w_color pValue)		{ this->_combo_box_button_mouse_over_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void					set_combo_box_button_pressed_color(w_color pValue)			{ this->_combo_box_button_pressed_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void					set_combo_box_button_focused_color(w_color pValue)			{ this->_combo_box_button_focused_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }
			DX_EXP void					set_combo_box_button_disabled_color(w_color pValue)			{ this->_combo_box_button_disabled_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }

			DX_EXP void					set_combo_box_drop_down_context_color(w_color pValue)		{ this->_combo_box_drop_down_context_color = RGBA_TO_HEX_COLOR(pValue.r, pValue.g, pValue.b, pValue.a); }

			DX_EXP void					set_combo_box_selected_color(w_color pValue)				{ this->_combo_box_selected_color = DirectX::W_COLOR_TO_XMFLOAT4(pValue); }
			DX_EXP void					set_combo_box_selection_font_color(w_color pValue)			{ this->_combo_box_selected_font_color = pValue; }
			
			DX_EXP void					set_combo_box_scroll_color(w_color pValue)					{ this->_combo_box_scroll_color = pValue; }
			DX_EXP void					set_combo_box_scroll_background_color(w_color pValue)		{ this->_combo_box_scroll_background_color = pValue; }
			DX_EXP void					set_combo_box_scroll_disabled_color(w_color pValue)			{ this->_combo_box_scroll_disabled_color = pValue; }
			
#pragma endregion

			virtual bool				handle_keyboard(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual bool				handle_mouse(UINT pMsg, _In_ const POINT& pPoint, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual void				on_hot_key() override;
			virtual bool				can_have_focus() override;
			virtual void				on_focus_out() override;
			virtual void				render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;
			virtual void				update_rects() override;
			DX_EXP HRESULT				add_item(_In_z_ const wchar_t* pText, _In_opt_ void* pData);
			DX_EXP void					remove_all_items();
			DX_EXP void					remove_item(_In_ UINT index);
			DX_EXP bool					contains_item(_In_z_ const wchar_t* pText, _In_ UINT iStart = 0);
			DX_EXP int					find_item(_In_z_ const wchar_t* pText, _In_ UINT iStart = 0) const;
			
		protected:
			int m_iSelected;
			int m_iFocused;
			int drop_context_height;
			w_scroll_bar scroll_bar;
			int scrollBar_width;
			bool is_opened;
			RECT rectangle_text;
			RECT rectangle_button;
			RECT rectangle_drop_down;
			RECT rectangle_drop_down_text;
			std::vector<w_combo_box_item*> items;

		private:
			typedef w_button _super;

			DWORD _combo_box_main_color;
			DWORD _combo_box_main_pressed_color;
			DWORD _combo_box_main_mouse_over_color;
			DWORD _combo_box_main_focused_color;
			DWORD _combo_box_main_disabled_color;

			DWORD _combo_box_button_color;
			DWORD _combo_box_button_mouse_over_color;
			DWORD _combo_box_button_pressed_color;
			DWORD _combo_box_button_focused_color;
			DWORD _combo_box_button_disabled_color;

			w_color _combo_box_scroll_color;
			w_color _combo_box_scroll_background_color;
			w_color _combo_box_scroll_disabled_color;

			DWORD _combo_box_drop_down_context_color;

			w_color _combo_box_selected_font_color;
			DirectX::XMFLOAT4 _combo_box_selected_color;

			UINT	_item_height;
			POINT	_item_margin;
		};
	}
}

#endif