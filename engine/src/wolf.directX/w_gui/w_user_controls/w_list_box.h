/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_list_box.h
	Description		 : The list_box user control class.This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_LIST_BOX_H__
#define __W_LIST_BOX_H__

#include <vector>
#include "w_control.h"
#include "w_label.h"
#include "w_image.h"
#include "w_scroll_bar.h"
#include "w_directX_math_helper.h"

#define W_EVENT_LISTBOX_ITEM_DBLCLK           0x0801
// EVENT_LISTBOX_SELECTION is rised off when the selection changes in a single selection list box.
#define W_EVENT_LISTBOX_SELECTION             0x0802
#define W_EVENT_LISTBOX_SELECTION_END         0x0803

namespace wolf
{
	namespace gui
	{
		struct w_list_box_item
		{
			std::wstring					text;
			std::wstring					icon_path;
			w_image*						icon;
			void*							data;
			RECT							rectangle_active;
			bool							selected;
		};

		class w_list_box : public w_control
		{
		public:
			DX_EXP							w_list_box(_In_opt_ w_widget* pParent = nullptr);
			DX_EXP virtual					~w_list_box();

			virtual HRESULT					on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice) override;
			virtual bool					can_have_focus() override;
			virtual bool					handle_keyboard(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam)  override;
			virtual bool					handle_mouse(_In_ UINT pMsg, _In_ const POINT& pPoint, _In_ WPARAM pWParam, _In_ LPARAM pLParam)  override;
			virtual bool					on_msg_proc(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam)  override;

			virtual void					render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;
			virtual void					update_rects() override;

			DX_EXP HRESULT					add_item(_In_z_ const std::wstring& pText, _In_z_ const std::wstring& pIconPath = L"", _In_ bool pRelateivePath = true, _In_opt_ void* pData = nullptr);
			DX_EXP HRESULT					insert_item(_In_ int pIndex, _In_z_ const std::wstring& pText, _In_z_ const std::wstring& pIconPath = L"", _In_ bool pRelateivePath = true, _In_opt_ void* pData = nullptr);
			DX_EXP void						remove_item(_In_ int pIndex);
			DX_EXP void						remove_all_items();
			DX_EXP void						select_item(_In_ int pIndex);

#pragma region Getters

			DX_EXP void						get_item_selected_rectangle_margin(_Inout_ int& pValueTop, _Inout_ int& pValueDown) const;
			DX_EXP int						get_text_margin_x() const										{ return this->text_margin_x; }
			DX_EXP int						get_text_margin_y() const										{ return this->text_margin_y; }
			DX_EXP int						get_icon_margin_x() const										{ return this->icon_margin_x; }
			DX_EXP int						get_icon_margin_y() const										{ return this->icon_margin_y; }
			DX_EXP int						get_icon_height_offset() const									{ return this->icon_height_offset; }

			DX_EXP DirectX::XMFLOAT2		get_icon_scale() const											{ return this->icon_scale; }

			DX_EXP bool						get_multi_selection() const										{ return this->multiselection; }
			DX_EXP size_t					get_items_count() const											{ return this->items.size(); }
			DX_EXP int						get_scrollBar_width() const										{ return this->scroll_bar_width; }
			DX_EXP w_list_box_item*			get_item(_In_ int pIndex) const;

			DX_EXP int						get_item_height() const											{ return this->item_height; }
			
			/*
				For single-selection listbox, returns the index of the selected item.
				For multi-selection, returns the first selected item after the nPreviousSelected position.
				To search for the first selected item, the app passes -1 for nPreviousSelected.  For
				subsequent searches, the app passes the returned index back to GetSelectedIndex as.
				nPreviousSelected.
				Returns -1 on error or if no item is selected.
			*/
			DX_EXP int						get_selected_index(_In_ int pPreviousSelected = -1) const;
			DX_EXP w_list_box_item*			get_selected_item(_In_ int pPreviousSelected = -1) const		{ return get_item(get_selected_index(pPreviousSelected)); }

			DX_EXP w_color					get_list_color() const											{ return w_color::from_hex(this->list_box_color); }
			DX_EXP w_color					get_list_selected_color() const									{ return DirectX::XMFLOAT4_TO_W_COLOR(this->list_box_selected_color); }
			DX_EXP w_color					get_list_disabled_color() const									{ return w_color::from_hex(this->list_box_disabled_color); }

			DX_EXP w_color					get_label_color() const											{ return this->list_box_text_color; }
			DX_EXP w_color					get_label_selected_color() const								{ return this->list_box_text_selected_color; }
			DX_EXP w_color					get_label_disabled_color() const								{ return this->list_box_text_disabled_color; }

			DX_EXP w_color					get_scroll_color() const										{ return this->list_box_scroll_color; }
			DX_EXP w_color					get_scroll_background_color() const								{ return this->list_box_scroll_background_color; }
			DX_EXP w_color					get_scroll_disabled_color() const								{ return this->list_box_scroll_disabled_color; }

#pragma endregion

#pragma region Setters

			DX_EXP void						set_text_margin(int pX, int pY)									{ this->text_margin_x = pX; this->text_margin_y = pY;}
			DX_EXP void						set_icon_margin(int pX, int pY)									{ this->icon_margin_x = pX; this->icon_margin_y = pY; }
			DX_EXP void						set_icon_scale(float pX, float pY)								{ this->icon_scale.x = pX; this->icon_scale.y = pY; }
			DX_EXP void						set_multi_selection(_In_ const bool pValue)						{ this->multiselection = pValue; }
			DX_EXP void						set_scrollBar_width(int nWidth)									{ this->scroll_bar_width = nWidth; update_rects(); }
			DX_EXP void						set_border(int nBorder, int nMargin)							{ this->border = nBorder; this->margin = nMargin; }
			DX_EXP void						set_item_height(int pValue)										{ this->item_height = pValue; }
			DX_EXP void						set_icon_height_offset(int pValue)								{ this->icon_height_offset = pValue; }
			DX_EXP void						set_item_selected_rectangle_margin(const int pTop, const int pDown);
			DX_EXP void						set_list_color(w_color pColor)									{ this->list_box_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void						set_list_selected_color(w_color pColor)							{ this->list_box_selected_color = DirectX::W_COLOR_TO_XMFLOAT4(pColor); }
			DX_EXP void						set_list_disabled_color(w_color pColor)							{ this->list_box_disabled_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }

			DX_EXP void						set_label_color(w_color pColor)									{ this->list_box_text_color = pColor; }
			DX_EXP void						set_label_selected_color(w_color pColor)						{ this->list_box_text_selected_color = pColor; }
			DX_EXP void						set_label_disabled_color(w_color pColor)						{ this->list_box_text_disabled_color = pColor; }

			DX_EXP void						set_scroll_color(w_color pColor)								{ this->list_box_scroll_color = pColor; }
			DX_EXP void						set_scroll_background_color(w_color pColor)						{ this->list_box_scroll_background_color = pColor; }
			DX_EXP void						set_scroll_disabled_color(w_color pColor)						{ this->list_box_scroll_disabled_color = pColor; }

#pragma endregion

		protected:
			RECT														rectangle_text;   // Text rendering bound
			RECT														rectangle_selection; // Selection box bound
			w_scroll_bar												scroll_bar;
			int															scroll_bar_width;
			int															border;
			int															margin;
			int															item_height;  // Height of a single line
			//item_selected_rectangle_margin[0] = Top margin, item_selected_rectangle_margin[1] = Down margin
			int															item_selected_rectangle_margin[2];
			bool														multiselection;
			int															selected_index;    // Index of the selected item for single selection list box
			int															selected_start;    // Index of the item where selection starts (for handling multi-selection)
			bool														drag;       // Whether the user is dragging the mouse to select

			int															text_margin_x;
			int															text_margin_y;
			int															icon_margin_x;
			int															icon_margin_y;
			int															icon_height_offset;

			DirectX::XMFLOAT2											icon_scale;

			DWORD														list_box_color;
			DirectX::XMFLOAT4											list_box_selected_color;
			DWORD														list_box_disabled_color;

			w_color														list_box_text_color;
			w_color														list_box_text_selected_color;
			w_color														list_box_text_disabled_color;

			w_color														list_box_scroll_color;
			w_color														list_box_scroll_background_color;
			w_color														list_box_scroll_disabled_color;

			std::vector<w_list_box_item*>								items;

			w_label*													label;

		private:
			typedef w_control											_super;
			std::shared_ptr<wolf::graphics::w_graphics_device>			_gDevice;
			bool														_initialized_scroll_size;
		};
	}
}

#endif
