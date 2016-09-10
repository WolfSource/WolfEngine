#include "w_directX_pch.h"
#include "w_list_box.h"
#include "w_gui/w_widgets_resource_manager.h"

using namespace wolf::gui;

w_list_box::w_list_box(_In_opt_ w_widget* pParent) : _super(pParent),
scrollBar(pParent),
_text_margin_x(0),
_text_margin_y(0),
_icon_margin_x(0),
_icon_margin_y(0),
_icon_scale(1, 1),
style(0),
border(6),
margin(5),
_item_selected_rectangle_margin_top(0),
_item_selected_rectangle_margin_down(0),
scrollBar_Width(16),
selected_index(-1),
selected_start(0),
drag(false),
_item_height(0),
_listBox_label_color(w_color(0, 0, 0, 255)),
_listBox_label_selected_color(w_color(255, 255, 255, 255)),
_listBox_label_disabled_color(w_color(0, 0, 0, 155)),
_listBox_color(w_color::WHITE),
_listBox_disabled_color(RGBA_TO_HEX_COLOR(255, 255, 255, 155)), 
_listBox_selected_color(1, 0, 0, 0.6f),
_listBox_scroll_color(w_color(200, 200, 200, 255)),
_listBox_scroll_background_color(w_color(255, 255, 255, 200)),
_listBox_scroll_disabled_color(w_color(255, 255, 255, 70))
{
	_super::type = W_GUI_CONTROL_LISTBOX;
	_super::parent_widget = pParent;

	this->_label = new w_label(pParent);
	this->_label->set_force_use_current_color_state(true);
}

w_list_box::~w_list_box()
{
	SAFE_RELEASE(this->_label);
	remove_all_items();
}

HRESULT w_list_box::on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
	this->_label->set_element(0, _super::elements[0]);

	return _super::parent_widget->initialize_control(&this->scrollBar);
}

bool w_list_box::can_have_focus()
{
	return _super::visible && _super::enabled;
}

void w_list_box::update_rects()
{
	_super::update_rects();

	this->rectangle_selection = _super::boundingBox;
	this->rectangle_selection.right -= this->scrollBar_Width;

	InflateRect(&this->rectangle_selection, -this->border, -this->border);
	this->rectangle_text = this->rectangle_selection;
	InflateRect(&this->rectangle_text, -this->margin, 0);

	// Update the scrollbar's rects
	this->scrollBar.set_position(_super::boundingBox.right - this->scrollBar_Width, _super::boundingBox.top);
	this->scrollBar.set_size(this->scrollBar_Width, _super::height);

	auto pFontNode = _super::parent_widget->get_manager()->get_font_node(_super::elements[0]->index_font);
	if (pFontNode && pFontNode->height)
	{
		this->scrollBar.set_page_size((this->rectangle_text.bottom - this->rectangle_text.top) / pFontNode->height);

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		this->scrollBar.show_item(this->selected_index);
	}
}

_Use_decl_annotations_
HRESULT w_list_box::add_item(_In_z_ const std::wstring& pText, _In_z_ const std::wstring& pIconPath, _In_ bool pRelateivePath, _In_opt_  void* pData)
{
	auto pNewItem = new (std::nothrow) w_list_box_item();
	if (!pNewItem) return E_OUTOFMEMORY;

	auto _path = pRelateivePath ? wolf::framework::w_game::get_content_directory() + pIconPath : pIconPath;

	pNewItem->text = pText;
	pNewItem->icon_path = _path;
	pNewItem->data = pData;
	SetRect(&pNewItem->rectangle_active, 0, 0, 0, 0);
	pNewItem->selected = false;

	if (!pIconPath.empty())
	{
		auto _hr = _super::parent_widget->add_image_to_resource_manager(_path.c_str());
		if (_hr == S_FALSE)
		{
			logger.write(L"Could not add image to resource manager from following path : " + std::wstring(_path));
		}
		else
		{
			auto _icon = new w_image(_super::parent_widget);
			_icon->set_path(_path);
			pNewItem->icon = _icon;
		}
	}

	this->items.push_back(pNewItem);
	this->scrollBar.set_track_range(0, (int) this->items.size());

	return S_OK;
}

_Use_decl_annotations_
HRESULT w_list_box::insert_item(int pIndex, _In_z_ const std::wstring& pText, _In_z_ const std::wstring& pIconPath, _In_ bool pRelateivePath, _In_opt_ void* pData)
{
	auto pNewItem = new (std::nothrow) w_list_box_item;
	if (!pNewItem) return E_OUTOFMEMORY;

	auto _path = pRelateivePath ? wolf::framework::w_game::get_content_directory() + pIconPath : pIconPath;

	pNewItem->text = pText;
	pNewItem->icon_path = _path;
	pNewItem->data = pData;
	SetRect(&pNewItem->rectangle_active, 0, 0, 0, 0);
	pNewItem->selected= false;


	//first remove the resource of image
	if (this->items[pIndex]->icon)
	{
		SAFE_RELEASE(this->items[pIndex]->icon);
	}

	//Add image
	if (!pIconPath.empty())
	{
		auto _hr = _super::parent_widget->add_image_to_resource_manager(_path.c_str());
		if (_hr == S_FALSE)
		{
			logger.write(L"Could not add image to resource manager from following path : " + _path);
		}
		else
		{
			auto _icon = new w_image(_super::parent_widget);
			_icon->set_path(_path);
			pNewItem->icon = _icon;
		}
	}


	this->items[pIndex] = pNewItem;
	this->scrollBar.set_track_range(0, (int) this->items.size());

	return S_OK;
}

void w_list_box::remove_item(_In_ int pIndex)
{
	if (pIndex < 0 || pIndex >= (int) this->items.size()) return;

	auto it = this->items.begin() + pIndex;
	auto pItem = *it;
	
	//release resource of icon
	SAFE_RELEASE(pItem->icon);

	delete pItem;

	this->items.erase(it);
	this->scrollBar.set_track_range(0, (int) this->items.size());
	if (this->selected_index >= (int) this->items.size())
	{
		this->selected_index = int(this->items.size()) - 1;
	}
	_super::parent_widget->send_event(W_EVENT_LISTBOX_SELECTION, true, this);
}

void w_list_box::remove_all_items()
{
	if (this->items.size() == 0) return;

	for (auto it = this->items.begin(); it != this->items.end(); ++it)
	{
		auto pItem = *it;

		//release resource of icon
		SAFE_RELEASE(pItem->icon);

		delete pItem;
	}

	this->items.clear();
	this->scrollBar.set_track_range(0, 1);
}

w_list_box_item* w_list_box::get_item(_In_ int nIndex) const
{
	if (nIndex < 0 || nIndex >= (int) this->items.size()) return nullptr;

	return this->items[nIndex];
}

int w_list_box::get_selected_index(_In_ int pPreviousSelected) const
{
	if (pPreviousSelected < -1) return -1;

	if (this->style & MULTISELECTION)
	{
		// Multiple selection enabled. Search for the next item with the selected flag.
		for (int i = pPreviousSelected + 1; i < (int) this->items.size(); ++i)
		{
			auto pItem = this->items[i];

			if (pItem->selected) return i;
		}

		return -1;
	}
	else
	{
		// Single selection
		return this->selected_index;
	}
}

void w_list_box::select_item(_In_ int pNewIndex)
{
	// If no item exists, do nothing.
	if (this->items.size() == 0) return;

	int nOldSelected = this->selected_index;

	// Adjust m_nSelected
	this->selected_index = pNewIndex;

	// Perform capping
	if (this->selected_index < 0)
	{
		this->selected_index = 0;
	}
	if (this->selected_index >= (int) this->items.size())
	{
		this->selected_index = int(this->items.size()) - 1;
	}
	if (nOldSelected != this->selected_index)
	{
		if (this->style & MULTISELECTION)
		{
			this->items[this->selected_index]->selected = true;
		}

		// Update selection start
		this->selected_start = this->selected_index;

		// Adjust scroll bar
		this->scrollBar.show_item(this->selected_index);
	}

	_super::parent_widget->send_event(W_EVENT_LISTBOX_SELECTION, true, this);
}

_Use_decl_annotations_
bool w_list_box::handle_keyboard(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!_super::enabled || !_super::visible) return false;

	// Let the scroll bar have a chance to handle it first
	if (this->scrollBar.handle_keyboard(uMsg, wParam, lParam)) return true;

	switch (uMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
		case VK_DOWN:
		case VK_NEXT:
		case VK_PRIOR:
		case VK_HOME:
		case VK_END:
		{
			// If no item exists, do nothing.
			if (this->items.size() == 0) return true;

			int nOldSelected = this->selected_index;

			// Adjust m_nSelected
			switch (wParam)
			{
			case VK_UP:
				--this->selected_index;
				break;
			case VK_DOWN:
				++this->selected_index;
				break;
			case VK_NEXT:
				this->selected_index += this->scrollBar.get_page_size() - 1;
				break;
			case VK_PRIOR:
				this->selected_index -= this->scrollBar.get_page_size() - 1;
				break;
			case VK_HOME:
				this->selected_index = 0;
				break;
			case VK_END:
				this->selected_index = int(this->items.size()) - 1;
				break;
			}

			// Perform capping
			if (this->selected_index < 0)
			{
				this->selected_index = 0;
			}
			if (this->selected_index >= (int) this->items.size())
			{
				this->selected_index = int(this->items.size()) - 1;
			}
			if (nOldSelected != this->selected_index)
			{
				if (this->style & MULTISELECTION)
				{
					// Multiple selection

					// Clear all selection
					for (int i = 0; i < (int) this->items.size(); ++i)
					{
						auto pItem = this->items[i];
						pItem->selected = false;
					}

					if (GetKeyState(VK_SHIFT) < 0)
					{
						// Select all items from m_nSelStart to
						// m_nSelected
						int nEnd = std::max(this->selected_start, this->selected_index);

						for (int n = std::min(this->selected_start, this->selected_index); n <= nEnd; ++n)
						{
							this->items[n]->selected = true;
						}
					}
					else
					{
						this->items[this->selected_index]->selected = true;

						// Update selection start
						this->selected_start = this->selected_index;
					}
				}
				else
					this->selected_start = this->selected_index;

				// Adjust scroll bar
				this->scrollBar.show_item(this->selected_index);

				// Send notification
				_super::parent_widget->send_event(W_EVENT_LISTBOX_SELECTION, true, this);
			}
			return true;
		}

		// Space is the hotkey for double-clicking an item.
		//
		case VK_SPACE:
			_super::parent_widget->send_event(W_EVENT_LISTBOX_ITEM_DBLCLK, true, this);
			return true;
		}
		break;
	}

	return false;
}

_Use_decl_annotations_
bool w_list_box::handle_mouse(UINT uMsg, const POINT& pt, WPARAM wParam, LPARAM lParam)
{
	if (!_super::enabled || !_super::visible) return false;

	// First acquire focus
	if (WM_LBUTTONDOWN == uMsg)
	{
		if (!_super::has_focus)
		{
			_super::parent_widget->request_focus(this);
		}
	}

	// Let the scroll bar handle it first.
	if (this->scrollBar.handle_mouse(uMsg, pt, wParam, lParam)) return true;

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		// Check for clicks in the text area
		if (!this->items.empty() && PtInRect(&this->rectangle_selection, pt))
		{
			// Compute the index of the clicked item

			int nClicked;
			if (this->_item_height)
			{
				nClicked = this->scrollBar.get_track_pos() + (pt.y - this->rectangle_text.top) / this->_item_height;
			}
			else
			{
				nClicked = -1;
			}
			// Only proceed if the click falls on top of an item.

			if (nClicked >= this->scrollBar.get_track_pos() &&
				nClicked < (int) this->items.size() &&
				nClicked < this->scrollBar.get_track_pos() + this->scrollBar.get_page_size())
			{
				if (this->parent_widget)
				{
					auto _hwnd = this->parent_widget->get_hwnd();
					SetCapture(_hwnd);
				}
				this->drag = true;

				// If this is a double click, fire off an event and exit
				// since the first click would have taken care of the selection
				// updating.
				if (uMsg == WM_LBUTTONDBLCLK)
				{
					_super::parent_widget->send_event(W_EVENT_LISTBOX_ITEM_DBLCLK, true, this);
					return true;
				}

				this->selected_index = nClicked;
				if (!(wParam & MK_SHIFT))
				{
					this->selected_start = this->selected_index;
				}

				// If this is a multi-selection listbox, update per-item selection data.
				if (this->style & MULTISELECTION)
				{
					// Determine behavior based on the state of Shift and Ctrl

					auto pSelItem = this->items[this->selected_index];
					if ((wParam & (MK_SHIFT | MK_CONTROL)) == MK_CONTROL)
					{
						// Control click. Reverse the selection of this item.
						pSelItem->selected = !pSelItem->selected;
					}
					else if ((wParam & (MK_SHIFT | MK_CONTROL)) == MK_SHIFT)
					{
						// Shift click. Set the selection for all items
						// from last selected item to the current item.
						// Clear everything else.

						int nBegin = std::min(this->selected_start, this->selected_index);
						int nEnd = std::max(this->selected_start, this->selected_index);

						for (int i = 0; i < nBegin; ++i)
						{
							auto pItem = this->items[i];
							pItem->selected = false;
						}

						for (int i = nEnd + 1; i < (int) this->items.size(); ++i)
						{
							auto pItem = this->items[i];
							pItem->selected = false;
						}

						for (int i = nBegin; i <= nEnd; ++i)
						{
							auto pItem = this->items[i];
							pItem->selected = true;
						}
					}
					else if ((wParam & (MK_SHIFT | MK_CONTROL)) == (MK_SHIFT | MK_CONTROL))
					{
						// Control-Shift-click.

						// The behavior is:
						//   Set all items from m_nSelStart to m_nSelected to
						//     the same state as m_nSelStart, not including m_nSelected.
						//   Set m_nSelected to selected.

						int nBegin = std::min(this->selected_start, this->selected_index);
						int nEnd = std::max(this->selected_start, this->selected_index);

						// The two ends do not need to be set here.

						bool bLastSelected = this->items[this->selected_start]->selected;
						for (int i = nBegin + 1; i < nEnd; ++i)
						{
							auto pItem = this->items[i];
							pItem->selected = bLastSelected;
						}

						pSelItem->selected = true;

						// Restore m_nSelected to the previous value
						// This matches the Windows behavior
						this->selected_index = this->selected_start;
					}
					else
					{
						// Simple click.  Clear all items and select the clicked item.

						for (int i = 0; i < (int) this->items.size(); ++i)
						{
							auto pItem = this->items[i];
							pItem->selected = false;
						}

						pSelItem->selected = true;
					}
				}  // End of multi-selection case

				_super::parent_widget->send_event(W_EVENT_LISTBOX_SELECTION, true, this);
			}

			return true;
		}
		break;

	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		this->drag = false;

		if (this->selected_index != -1)
		{
			// Set all items between m_nSelStart and m_nSelected to
			// the same state as m_nSelStart
			int nEnd = std::max(this->selected_start, this->selected_index);

			for (int n = std::min(this->selected_start, this->selected_index) + 1; n < nEnd; ++n)
			{
				this->items[n]->selected = this->items[this->selected_start]->selected;
			}
			this->items[this->selected_index]->selected = this->items[this->selected_start]->selected;

			// If m_nSelStart and m_nSelected are not the same,
			// the user has dragged the mouse to make a selection.
			// Notify the application of this.
			if (this->selected_start != this->selected_index)
			{
				_super::parent_widget->send_event(W_EVENT_LISTBOX_SELECTION, true, this);
			}
			_super::parent_widget->send_event(W_EVENT_LISTBOX_SELECTION_END, true, this);
		}
		return false;
	}

	case WM_MOUSEMOVE:
		if (this->drag)
		{
			// Compute the index of the item below cursor

			int nItem;
			if (this->_item_height)
			{
				nItem = this->scrollBar.get_track_pos() + (pt.y - this->rectangle_text.top) / this->_item_height;
			}
			else
			{
				nItem = -1;
			}

			// Only proceed if the cursor is on top of an item.

			if (nItem >= (int) this->scrollBar.get_track_pos() &&
				nItem < (int) this->items.size() &&
				nItem < this->scrollBar.get_track_pos() + this->scrollBar.get_page_size())
			{
				this->selected_index = nItem;
				_super::parent_widget->send_event(W_EVENT_LISTBOX_SELECTION, true, this);
			}
			else if (nItem < (int) this->scrollBar.get_track_pos())
			{
				// User drags the mouse above window top
				this->scrollBar.scroll(-1);
				this->selected_index = this->scrollBar.get_track_pos();
				_super::parent_widget->send_event(W_EVENT_LISTBOX_SELECTION, true, this);
			}
			else if (nItem >= this->scrollBar.get_track_pos() + this->scrollBar.get_page_size())
			{
				// User drags the mouse below window bottom
				this->scrollBar.scroll(1);
				this->selected_index = std::min((int) this->items.size(), this->scrollBar.get_track_pos() + this->scrollBar.get_page_size()) - 1;
				_super::parent_widget->send_event(W_EVENT_LISTBOX_SELECTION, true, this);
			}
		}
		break;

	case WM_MOUSEWHEEL:
	{
		UINT uLines = 0;
		if (!SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &uLines, 0))
		{
			uLines = 0;
		}
		int nScrollAmount = int((short) HIWORD(wParam)) / WHEEL_DELTA * uLines;
		this->scrollBar.scroll(-nScrollAmount);
		return true;
	}
	}

	return false;
}

_Use_decl_annotations_
bool w_list_box::on_msg_proc(UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	W_UNUSED(pWParam);

	if (WM_CAPTURECHANGED == pMsg)
	{
		if (this->parent_widget)
		{
			auto _hwnd = this->parent_widget->get_hwnd();
			if (_hwnd)
			{
				// The application just lost mouse capture. We may not have gotten
				// the WM_MOUSEUP message, so reset m_bDrag here.
				if ((HWND) pLParam != _hwnd)
				{
					this->drag = false;
				}
			}
		}

	}

	return false;
}

void w_list_box::render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (!_super::visible) return;

	auto _element = _super::elements[0];

	_element->texture_color.color_states[W_GUI_STATE_NORMAL] = this->_listBox_color;
	_element->texture_color.color_states[W_GUI_STATE_DISABLED] = this->_listBox_disabled_color;

	_element->texture_color.blend(W_GUI_STATE_NORMAL, pElapsedTime);
	_element->font_color.blend(W_GUI_STATE_NORMAL, pElapsedTime);

	auto _selected_element = _super::elements[1];

	_selected_element->texture_color.current_color_state = this->_listBox_selected_color;
	//_selected_element->texture_color.blend(W_GUI_STATE_NORMAL, pElapsedTime);	
	
	_super::parent_widget->draw_sprite(_element, &(_super::boundingBox), W_GUI_FAR_BUTTON_DEPTH);

	std::vector<w_image*> _selected_icons;
	// Render the text
	if (!this->items.empty())
	{
		// Find out the height of a single line of text
		RECT _rectangle = this->rectangle_text;
		RECT rcSel = this->rectangle_selection;
		_rectangle.bottom = _rectangle.top + _super::parent_widget->get_manager()->get_font_node(_element->index_font)->height;

		// Update the line height formation
		if (this->_item_height == 0)
		{
			this->_item_height = _rectangle.bottom - _rectangle.top;
		}

		static bool bSBInit;
		if (!bSBInit)
		{
			// Update the page size of the scroll bar
			if (this->_item_height)
			{
				this->scrollBar.set_page_size((this->rectangle_text.bottom - this->rectangle_text.top) / this->_item_height);
			}
			else
			{
				this->scrollBar.set_page_size((this->rectangle_text.bottom - this->rectangle_text.top));
			}
			bSBInit = true;
		}

		_rectangle.right = this->rectangle_text.right;
		for (int i = this->scrollBar.get_track_pos(); i < (int) this->items.size(); ++i)
		{
			if (_rectangle.bottom > this->rectangle_text.bottom) break;

			auto pItem = this->items[i];

			// Determine if we need to render this item with the
			// selected element.
			bool bSelectedStyle = false;

			if (!(this->style & MULTISELECTION) && i == this->selected_index)
			{
				bSelectedStyle = true;
			}
			else if (this->style & MULTISELECTION)
			{
				if (this->drag && ((i >= this->selected_index && i < this->selected_start) || (i <= this->selected_index && i > this->selected_start)))
				{
					bSelectedStyle = this->items[this->selected_start]->selected;
				}
				else if (pItem->selected)
				{
					bSelectedStyle = true;
				}
			}

			//this->_label->set_label_disabled_color(this->_label_disabled_color);
			if (bSelectedStyle)
			{
				rcSel.top = _rectangle.top - _item_selected_rectangle_margin_top;
				rcSel.bottom = _rectangle.bottom + _item_selected_rectangle_margin_down;

				//Draw selected rectangle
				_super::parent_widget->draw_sprite(_selected_element, &rcSel, W_GUI_NEAR_BUTTON_DEPTH);

				this->_label->set_text(pItem->text);
				this->_label->set_text_offset_x(_rectangle.left + this->_text_margin_x);
				this->_label->set_text_offset_y(_rectangle.top + this->_text_margin_y);

				this->_label->set_current_color_state(this->_listBox_label_selected_color);

				this->_label->render(pGDevice, pElapsedTime);

				if (pItem->icon)
				{
					pItem->icon->set_position(_rectangle.left + this->_icon_margin_x, _rectangle.top + this->_icon_margin_y);
					pItem->icon->set_scale(_icon_scale);
					_selected_icons.push_back(pItem->icon);
				}
			}
			else
			{
				this->_label->set_text(pItem->text);
				this->_label->set_text_offset_x(_rectangle.left + this->_text_margin_x);
				this->_label->set_text_offset_y(_rectangle.top + this->_text_margin_y);

				this->_label->set_current_color_state(this->_listBox_label_color);

				this->_label->render(pGDevice, pElapsedTime);

				//render icon
				if (pItem->icon)
				{
					pItem->icon->set_position(_rectangle.left + this->_icon_margin_x, _rectangle.top + this->_icon_margin_y);
					pItem->icon->set_scale(_icon_scale);
					_selected_icons.push_back(pItem->icon);
				}
			}
			OffsetRect(&_rectangle, 0, this->_item_height);
		}
	}

	// Render the scroll bar
	this->scrollBar.set_scroll_color(this->_listBox_scroll_color);
	this->scrollBar.set_scroll_background_color(this->_listBox_scroll_background_color);
	this->scrollBar.set_scroll_disabled_color(this->_listBox_scroll_disabled_color);
	this->scrollBar.render(pGDevice, pElapsedTime);

	//Render all selected icon
	auto _size = _selected_icons.size();
	if (_size)
	{
		for (size_t i = 0; i < _size; ++i)
		{
			_selected_icons.at(i)->render(pGDevice, pElapsedTime);
		}
	}
}
