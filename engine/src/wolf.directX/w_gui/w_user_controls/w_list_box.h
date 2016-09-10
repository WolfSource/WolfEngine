/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_list_box.h
	Description		 : The listBox user control class.This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_LIST_BOX_H__
#define __W_LIST_BOX_H__

#include "w_control.h"
#include "w_label.h"
#include "w_image.h"
#include "w_scroll_bar.h"
#include <vector>
#include "w_directX_math_helper.h"

#define W_EVENT_LISTBOX_ITEM_DBLCLK           0x0701
// EVENT_LISTBOX_SELECTION is fired off when the selection changes in a single selection list box.
#define W_EVENT_LISTBOX_SELECTION             0x0702
#define W_EVENT_LISTBOX_SELECTION_END         0x0703

namespace wolf
{
	namespace gui
	{
		struct w_list_box_item
		{
			std::wstring text;
			std::wstring icon_path;
			w_image* icon;
			void* data;
			RECT rectangle_active;
			bool selected;

			std::vector<w_control*> controls;
		};

		class w_list_box : public w_control
		{
		public:
			DX_EXP w_list_box(_In_opt_ w_widget* pParent = nullptr);
			DX_EXP virtual ~w_list_box();

			virtual HRESULT on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice) override;
			virtual bool	can_have_focus() override;
			virtual bool	handle_keyboard(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam)  override;
			virtual bool	handle_mouse(_In_ UINT pMsg, _In_ const POINT& pPoint, _In_ WPARAM pWParam, _In_ LPARAM pLParam)  override;
			virtual bool	on_msg_proc(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam)  override;

			virtual void	render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;
			virtual void	update_rects() override;

			DX_EXP HRESULT	add_item(_In_z_ const std::wstring& pText, _In_z_ const std::wstring& pIconPath = L"", _In_ bool pRelateivePath = true, _In_opt_ void* pData = nullptr);
			DX_EXP HRESULT	insert_item(_In_ int pIndex, _In_z_ const std::wstring& pText, _In_z_ const std::wstring& pIconPath = L"", _In_ bool pRelateivePath = true, _In_opt_ void* pData = nullptr);
			DX_EXP void		remove_item(_In_ int pIndex);
			DX_EXP void		remove_all_items();
			DX_EXP void		select_item(_In_ int pIndex);

			enum STYLE { MULTISELECTION = 1 };

#pragma region Getters

			DX_EXP int					get_text_margin_x() const										{ return this->_text_margin_x; }
			DX_EXP int					get_text_margin_y() const										{ return this->_text_margin_y; }
			DX_EXP int					get_icon_margin_x() const										{ return this->_icon_margin_x; }
			DX_EXP int					get_icon_margin_y() const										{ return this->_icon_margin_y; }
			DX_EXP int					get_item_selected_margin_top_down() const						{ return this->_item_selected_rectangle_margin_top; }
			DX_EXP int					get_item_selected_rectangle_margin_top()						{ return this->_item_selected_rectangle_margin_top; }
			DX_EXP int					get_item_selected_rectangle_margin_down()						{ return this->_item_selected_rectangle_margin_down; }

			DX_EXP DirectX::XMFLOAT2	get_icon_scale() const											{ return this->_icon_scale; }

			DX_EXP DWORD				get_style() const												{ return this->style; }
			DX_EXP size_t				get_size() const												{ return this->items.size(); }
			DX_EXP int					get_scrollBar_width() const										{ return this->scrollBar_Width; }
			DX_EXP w_list_box_item*		get_item(_In_ int pIndex) const;

			DX_EXP int					get_item_height() const											{ return this->_item_height; }
			
			/*
				For single-selection listbox, returns the index of the selected item.
				For multi-selection, returns the first selected item after the nPreviousSelected position.
				To search for the first selected item, the app passes -1 for nPreviousSelected.  For
				subsequent searches, the app passes the returned index back to GetSelectedIndex as.
				nPreviousSelected.
				Returns -1 on error or if no item is selected.
			*/
			DX_EXP int					get_selected_index(_In_ int pPreviousSelected = -1) const;
			DX_EXP w_list_box_item*		get_selected_item(_In_ int pPreviousSelected = -1) const		{ return get_item(get_selected_index(pPreviousSelected)); }

			DX_EXP w_color				get_listBox_color() const										{ return w_color::from_hex(this->_listBox_color); }
			DX_EXP w_color				get_listBox_selected_color() const								{ return DirectX::XMFLOAT4_TO_W_COLOR(this->_listBox_selected_color); }
			DX_EXP w_color				get_listBox_disabled_color() const								{ return w_color::from_hex(this->_listBox_disabled_color); }

			DX_EXP w_color				get_label_color() const											{ return this->_listBox_label_color; }
			DX_EXP w_color				get_label_selected_color() const								{ return this->_listBox_label_selected_color; }
			DX_EXP w_color				get_label_disabled_color() const								{ return this->_listBox_label_disabled_color; }

			DX_EXP w_color				get_scroll_color() const										{ return this->_listBox_scroll_color; }
			DX_EXP w_color				get_scroll_background_color() const								{ return this->_listBox_scroll_background_color; }
			DX_EXP w_color				get_scroll_disabled_color() const								{ return this->_listBox_scroll_disabled_color; }

#pragma endregion

#pragma region Setters

			DX_EXP void				set_text_margin(int pX, int pY)									{ this->_text_margin_x = pX; this->_text_margin_y = pY;}
			DX_EXP void				set_icon_margin(int pX, int pY)									{ this->_icon_margin_x = pX; this->_icon_margin_y = pY; }
			DX_EXP void				set_icon_scale(float pX, float pY)								{ this->_icon_scale.x = pX; this->_icon_scale.y = pY; }
			DX_EXP void				set_style(DWORD dwStyle)										{ this->style = dwStyle; }
			DX_EXP void				set_scrollBar_width(int nWidth)									{ this->scrollBar_Width = nWidth; update_rects(); }
			DX_EXP void				set_border(int nBorder, int nMargin)							{ this->border = nBorder; this->margin = nMargin; }
			DX_EXP void				set_item_height(int pValue)										{ this->_item_height = pValue; }
			DX_EXP void				set_item_selected_rectangle_margin_top(int pValue)				{ this->_item_selected_rectangle_margin_top = pValue; }
			DX_EXP void				set_item_selected_rectangle_margin_down(int pValue)				{ this->_item_selected_rectangle_margin_down = pValue; }

			DX_EXP void				set_listBox_color(w_color pColor)								{ this->_listBox_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }
			DX_EXP void				set_listBox_selected_color(w_color pColor)						{ this->_listBox_selected_color = DirectX::W_COLOR_TO_XMFLOAT4(pColor); }
			DX_EXP void				set_listBox_disabled_color(w_color pColor)						{ this->_listBox_disabled_color = RGBA_TO_HEX_COLOR(pColor.r, pColor.g, pColor.b, pColor.a); }

			DX_EXP void				set_label_color(w_color pColor)									{ this->_listBox_label_color = pColor; }
			DX_EXP void				set_label_selected_color(w_color pColor)						{ this->_listBox_label_selected_color = pColor; }
			DX_EXP void				set_label_disabled_color(w_color pColor)						{ this->_listBox_label_disabled_color = pColor; }

			DX_EXP void				set_scroll_color(w_color pColor)								{ this->_listBox_scroll_color = pColor; }
			DX_EXP void				set_scroll_background_color(w_color pColor)						{ this->_listBox_scroll_background_color = pColor; }
			DX_EXP void				set_scroll_disabled_color(w_color pColor)						{ this->_listBox_scroll_disabled_color = pColor; }

#pragma endregion

		protected:
			RECT rectangle_text;   // Text rendering bound
			RECT rectangle_selection; // Selection box bound
			w_scroll_bar scrollBar;
			int scrollBar_Width;
			int border;
			int margin;
			int _item_height;  // Height of a single line
			int _item_selected_rectangle_margin_top;
			int _item_selected_rectangle_margin_down;
			DWORD style;    // List box style
			int selected_index;    // Index of the selected item for single selection list box
			int selected_start;    // Index of the item where selection starts (for handling multi-selection)
			bool drag;       // Whether the user is dragging the mouse to select

			int _text_margin_x;
			int _text_margin_y;
			int _icon_margin_x;
			int _icon_margin_y;

			DirectX::XMFLOAT2 _icon_scale;

			DWORD _listBox_color;
			DirectX::XMFLOAT4 _listBox_selected_color;
			DWORD _listBox_disabled_color;

			w_color _listBox_label_color;
			w_color _listBox_label_selected_color;
			w_color _listBox_label_disabled_color;

			w_color _listBox_scroll_color;
			w_color _listBox_scroll_background_color;
			w_color _listBox_scroll_disabled_color;

			std::vector<w_list_box_item*> items;

			w_label* _label;

		private:
			typedef w_control _super;
		};
	}
}

#endif
