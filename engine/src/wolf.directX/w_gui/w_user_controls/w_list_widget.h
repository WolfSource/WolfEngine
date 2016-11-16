/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_list_widget.h
	Description		 : The list widget user control class.This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_LIST_WIDGET_H__
#define __W_LIST_WIDGET_H__

#include <vector>
#include "w_control.h"
#include "w_scroll_bar.h"
#include "w_directX_math_helper.h"

#define W_EVENT_LIST_WIDGET_ITEM_DBLCLK           0x0701
// EVENT_LISTBOX_SELECTION is rised off when the selection changes in a single selection list box.
#define W_EVENT_LIST_WIDGET_SELECTION             0x0702
#define W_EVENT_LIST_WIDGET_SELECTION_END         0x0703

namespace wolf
{
	namespace gui
	{
		struct w_list_widget_item
		{
#ifdef WIN32
			//gdi handle, original x offset, original y offset
			std::vector<std::tuple<HWND, int, int>>			gdi_controls;
#endif
			std::vector<std::tuple<w_control*, int, int>>	gpu_controls;
			void*											data;
			RECT											rectangle_active;
			bool											selected;
		};

		class w_list_widget : public w_control
		{
		public:
			DX_EXP								w_list_widget(_In_opt_ w_widget* pParent = nullptr);
			DX_EXP	virtual						~w_list_widget();

			virtual HRESULT						on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice) override;
			virtual bool						can_have_focus() override;
			virtual bool						handle_keyboard(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam)  override;
			virtual bool						handle_mouse(_In_ UINT pMsg, _In_ const POINT& pPoint, _In_ WPARAM pWParam, _In_ LPARAM pLParam)  override;
			virtual bool						on_msg_proc(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam)  override;

			virtual void						render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;
			virtual void						update_rects() override;

			DX_EXP HRESULT						add_item(_In_ w_list_widget_item* pItem);
			DX_EXP HRESULT						insert_item(_In_ const size_t pIndex, _In_ w_list_widget_item* pItem);
			DX_EXP void							remove_item(_In_ const size_t pIndex);
			DX_EXP void							remove_all_items();
			DX_EXP void							remove_all_items_except_gdi();
			DX_EXP void							select_item(_In_ const size_t pIndex);
			
			void								hide_gdi_controls();

#pragma region Getters

			DX_EXP void							get_item_selected_rectangle_margin(_Inout_ int& pValueTop, _Inout_ int& pValueDown) const;
			DX_EXP bool							get_multi_selection() const											{ return this->multiselection; }
			DX_EXP size_t						get_items_count() const												{ return this->items.size(); }
			DX_EXP size_t						get_scroll_width() const											{ return this->scroll_bar_width; }
			DX_EXP w_list_widget_item*			get_item(_In_ const int pIndex) const;
			DX_EXP size_t						get_item_height() const												{ return this->item_height; }
			
			/*
				For single-selection listbox, returns the index of the selected item.
				For multi-selection, returns the first selected item after the nPreviousSelected position.
				To search for the first selected item, the app passes -1 for nPreviousSelected.  For
				subsequent searches, the app passes the returned index back to GetSelectedIndex as.
				nPreviousSelected.
				Returns -1 on error or if no item is selected.
			*/
			DX_EXP int							get_selected_index(_In_ const int pPreviousSelected = -1) const;
			DX_EXP w_list_widget_item*			get_selected_item(_In_ const int pPreviousSelected = -1) const		{ return get_item(get_selected_index(pPreviousSelected)); }

			DX_EXP w_color						get_list_color() const												{ return w_color::from_hex(this->list_widget_color); }
			DX_EXP w_color						get_list_selected_color() const										{ return DirectX::XMFLOAT4_TO_W_COLOR(this->list_widget_selected_color); }
			DX_EXP w_color						get_list_disabled_color() const										{ return w_color::from_hex(this->list_widget_disabled_color); }

			DX_EXP w_color						get_scroll_color() const											{ return this->list_widget_scroll_color; }
			DX_EXP w_color						get_scroll_background_color() const									{ return this->list_widget_scroll_background_color; }
			DX_EXP w_color						get_scroll_disabled_color() const									{ return this->list_widget_scroll_disabled_color; }

#pragma endregion

#pragma region Setters

			DX_EXP void							set_multi_selection(_In_ const bool pValue)							{ this->multiselection = pValue; }
			DX_EXP void							set_scrollBar_width(int pWidth)										{ this->scroll_bar_width = pWidth; update_rects(); }
			DX_EXP void							set_border(int nBorder, int nMargin)								{ this->border = nBorder; this->margin = nMargin; }
			
			//use set_item_selected_rectangle_margin.y * 2 = item_height for best item height
			DX_EXP void							set_item_height(size_t pValue)										{ this->item_height = pValue; }
			DX_EXP void							set_item_selected_rectangle_margin(_In_ const int pTop, _In_ const int pDown);
			
			DX_EXP void							set_list_color(w_color pColor)										{ this->list_widget_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void							set_list_selected_color(w_color pColor)								{ this->list_widget_selected_color = DirectX::W_COLOR_TO_XMFLOAT4(pColor); }
			DX_EXP void							set_list_disabled_color(w_color pColor)								{ this->list_widget_disabled_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }

			DX_EXP void							set_scroll_color(w_color pColor)									{ this->list_widget_scroll_color = pColor; }
			DX_EXP void							set_scroll_background_color(w_color pColor)							{ this->list_widget_scroll_background_color = pColor; }
			DX_EXP void							set_scroll_disabled_color(w_color pColor)							{ this->list_widget_scroll_disabled_color = pColor; }

#pragma endregion

		protected:
			RECT								rectangle_text;   // Text rendering bound
			RECT								rectangle_selection; // Selection box bound
			w_scroll_bar						scroll_bar;
			size_t								scroll_bar_width;
			int									border;
			int									margin;
			size_t								item_height;  // Height of a single line
			//item_selected_rectangle_margin[0] = Top margin, item_selected_rectangle_margin[1] = Down margin
			int									item_selected_rectangle_margin[2];
			bool								multiselection;
			int									selected_index;    // Index of the selected item for single selection list box
			int									selected_start;    // Index of the item where selection starts (for handling multi-selection)
			bool								drag;       // Whether the user is dragging the mouse to select

			DWORD								list_widget_color;
			DirectX::XMFLOAT4					list_widget_selected_color;
			DWORD								list_widget_disabled_color;

			w_color								list_widget_scroll_color;
			w_color								list_widget_scroll_background_color;
			w_color								list_widget_scroll_disabled_color;

			std::vector<w_list_widget_item*>	items;

		private:
			typedef w_control					_super;
			
			void								_reinitialize_child_controls(_Inout_ std::vector<std::tuple<w_control*, int, int>>& pGUIControls);
			void								_release_list_widget_item(_Inout_ w_list_widget_item* pItem);

			bool								_initialized_scroll_size;
		};
	}
}

#endif
