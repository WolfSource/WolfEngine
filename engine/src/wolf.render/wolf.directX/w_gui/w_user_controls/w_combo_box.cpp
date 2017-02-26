#include "w_directX_pch.h"
#include "w_combo_box.h"
#include "w_gui/w_widgets_resource_manager.h"

#define ARIAL_FONT_HEIGHT_SIZE_12	84

using namespace wolf::gui;

w_combo_box::w_combo_box(_In_opt_ w_widget* pParent) : scroll_bar(pParent), _super(pParent),
scrollBar_width(16),
is_opened(false),
drop_context_height(100),
m_iSelected(-1),
m_iFocused(-1),
_item_height(21),
_combo_box_main_color(RGBA_TO_HEX_COLOR(200, 200, 200, 150)),
_combo_box_main_pressed_color(RGBA_TO_HEX_COLOR(255, 255, 255, 200)),
_combo_box_main_mouse_over_color(w_color::YELLOW),
_combo_box_main_focused_color(RGBA_TO_HEX_COLOR(255, 255, 0, 70)),
_combo_box_main_disabled_color(RGBA_TO_HEX_COLOR(200, 200, 200, 70)),
_combo_box_button_color(RGBA_TO_HEX_COLOR(255, 255, 255, 150)),
_combo_box_button_pressed_color(RGBA_TO_HEX_COLOR(150, 150, 150, 255)),
_combo_box_button_mouse_over_color(w_color::YELLOW),
_combo_box_button_focused_color(RGBA_TO_HEX_COLOR(255, 255, 255, 200)),
_combo_box_button_disabled_color(RGBA_TO_HEX_COLOR(255, 255, 255, 70)),
_combo_box_drop_down_context_color(RGBA_TO_HEX_COLOR(255, 255, 255, 255)),
_combo_box_selected_font_color(w_color(255, 255, 255, 255)),
_combo_box_selected_color(1, 0, 0, 0.6f),
_combo_box_scroll_color(w_color(200, 200, 200, 255)),
_combo_box_scroll_background_color(w_color(255, 255, 255, 200)),
_combo_box_scroll_disabled_color(w_color(255, 255, 255, 70))
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_COMBOBOX;
	_super::parent_widget = pParent;

	_super::original_text_offset_x = -130;
	_super::original_text_offset_y = -7;

	_item_margin.x = 45;
	_item_margin.y = -5;

	_super::label_color = RGBA_TO_HEX_COLOR(0, 0, 0, 255);
	_super::label_pressed_color = RGBA_TO_HEX_COLOR(0, 0, 0, 255);
	_super::label_mouse_over_color = RGBA_TO_HEX_COLOR(0, 0, 0, 255);
	_super::label_focused_color = RGBA_TO_HEX_COLOR(0, 0, 0, 255);
	_super::label_disabled_color = RGBA_TO_HEX_COLOR(200, 200, 200, 200);
}

w_combo_box::~w_combo_box()
{
	remove_all_items();
}

HRESULT w_combo_box::on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
	return _super::parent_widget->initialize_control(&this->scroll_bar);
}

void w_combo_box::remove_all_items()
{
	for (auto it = this->items.begin(); it != this->items.end(); ++it)
	{
		auto pItem = *it;
		SAFE_DELETE(pItem);
	}

	this->items.clear();
	this->scroll_bar.set_track_range(0, 1);
	m_iFocused = m_iSelected = -1;
}

void w_combo_box::update_rects()
{
	w_button::update_rects();

	this->rectangle_button = _super::bounding_box;
	this->rectangle_button.left = this->rectangle_button.right - /*RectHeight*/(this->rectangle_button.bottom - this->rectangle_button.top);

	this->rectangle_text = _super::bounding_box;
	this->rectangle_text.right = this->rectangle_button.left;

	this->rectangle_drop_down = this->rectangle_text;
	OffsetRect(&this->rectangle_drop_down, 0, static_cast<int>(0.90f * /*RectHeight*/(this->rectangle_text.bottom - this->rectangle_text.top)));
	this->rectangle_drop_down.bottom += this->drop_context_height;
	this->rectangle_drop_down.right -= this->scrollBar_width;

	auto _rect_height = this->rectangle_drop_down.bottom - this->rectangle_drop_down.top;
	auto _rect_width = this->rectangle_drop_down.right - this->rectangle_drop_down.left;

	this->rectangle_drop_down_text = this->rectangle_drop_down;
	this->rectangle_drop_down_text.left += static_cast<int>(0.1f * _rect_width);
	this->rectangle_drop_down_text.right -= static_cast<int>(0.1f * _rect_width);
	this->rectangle_drop_down_text.top += static_cast<int>(0.1f * _rect_height);
	this->rectangle_drop_down_text.bottom -= static_cast<int>(0.1f * _rect_height);

	// Update the scrollbar's rects
	this->scroll_bar.set_position_2d(this->rectangle_drop_down.right, this->rectangle_drop_down.top + 2);
	this->scroll_bar.set_size(this->scrollBar_width, _rect_height - 2);
	auto pFontNode = _super::parent_widget->get_manager()->get_font_node(this->elements[2]->index_font);
	if (pFontNode && pFontNode->height)
	{
		this->scroll_bar.set_page_size(/*RectHeight*/(this->rectangle_drop_down_text.bottom - this->rectangle_drop_down_text.top) / pFontNode->height);

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		this->scroll_bar.show_item(m_iSelected);
	}
}

void w_combo_box::on_focus_out()
{
	_super::on_focus_out();
	this->is_opened = false;
}

_Use_decl_annotations_
bool w_combo_box::handle_keyboard(UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	const DWORD REPEAT_MASK = (0x40000000);

	if (!_super::enabled || !_super::visible) return false;

	// Let the scroll bar have a chance to handle it first
	if (this->scroll_bar.handle_keyboard(pMsg, pWParam, pLParam)) return true;

	switch (pMsg)
	{
	case WM_KEYDOWN:
	{
		switch (pWParam)
		{
		case VK_RETURN:
			if (this->is_opened)
			{
				if (m_iSelected != m_iFocused)
				{
					m_iSelected = m_iFocused;
					_super::parent_widget->send_event(W_EVENT_COMBOBOX_SELECTION_CHANGED, true, this);
				}
				this->is_opened = false;

				if (!_super::parent_widget->keyboard_input)
				{
					_super::parent_widget->clear_focus();
				}
				return true;
			}
			break;

		case VK_F4:
			// Filter out auto-repeats
			if (pLParam & REPEAT_MASK)
				return true;

			this->is_opened = !this->is_opened;

			if (!this->is_opened)
			{
				_super::parent_widget->send_event(W_EVENT_COMBOBOX_SELECTION_CHANGED, true, this);

				if (!_super::parent_widget->keyboard_input)
				{
					_super::parent_widget->clear_focus();
				}
			}

			return true;

		case VK_LEFT:
		case VK_UP:
			if (m_iFocused > 0)
			{
				m_iFocused--;
				m_iSelected = m_iFocused;

				if (!this->is_opened)
				{
					_super::parent_widget->send_event(W_EVENT_COMBOBOX_SELECTION_CHANGED, true, this);
				}
			}

			return true;

		case VK_RIGHT:
		case VK_DOWN:
			if (m_iFocused + 1 < (int)get_num_items())
			{
				m_iFocused++;
				m_iSelected = m_iFocused;

				if (!this->is_opened)
				{
					_super::parent_widget->send_event(W_EVENT_COMBOBOX_SELECTION_CHANGED, true, this);
				}
			}

			return true;
		}
		break;
	}
	}

	return false;
}

_Use_decl_annotations_
bool w_combo_box::handle_mouse(UINT pMsg, const POINT& pPoint, WPARAM pWParam, LPARAM pLParam)
{
	if (!_super::enabled || !_super::visible) return false;

	// Let the scroll bar handle it first.
	if (this->scroll_bar.handle_mouse(pMsg, pPoint, pWParam, pLParam)) return true;

	switch (pMsg)
	{
	case WM_MOUSEMOVE:
	{
		if (this->is_opened && PtInRect(&this->rectangle_drop_down, pPoint))
		{
			// Determine which item has been selected
			for (size_t i = 0; i < this->items.size(); i++)
			{
				auto pItem = this->items[i];
				if (pItem->visible && PtInRect(&pItem->active, pPoint))
				{
					m_iFocused = static_cast<int>(i);
				}
			}
			return true;
		}
		break;
	}

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	{
		if (contains_point(pPoint))
		{
			// Pressed while inside the control
			_super::pressed = true;

			if (this->parent_widget)
			{
				auto _hwnd = this->parent_widget->get_hwnd();
				if (_hwnd)
				{
					SetCapture(_hwnd);
				}
			}
			
			if (!this->has_focus)
			{
				_super::parent_widget->request_focus(this);
			}

			// Toggle dropdown
			if (this->has_focus)
			{
				this->is_opened = !this->is_opened;

				if (!this->is_opened)
				{
					if (!_super::parent_widget->keyboard_input)
					{
						_super::parent_widget->clear_focus();
					}
				}
			}

			return true;
		}

		// Perhaps this click is within the dropdown
		if (this->is_opened && PtInRect(&this->rectangle_drop_down, pPoint))
		{
			// Determine which item has been selected
			for (size_t i = this->scroll_bar.get_track_pos(); i < this->items.size(); i++)
			{
				auto pItem = this->items[i];
				if (pItem->visible && PtInRect(&pItem->active, pPoint))
				{
					m_iFocused = m_iSelected = static_cast<int>(i);
					_super::parent_widget->send_event(W_EVENT_COMBOBOX_SELECTION_CHANGED, true, this);
					this->is_opened = false;

					if (!_super::parent_widget->keyboard_input)
					{
						_super::parent_widget->clear_focus();
					}
					break;
				}
			}

			return true;
		}

		// Mouse click not on main control or in dropdown, fire an event if needed
		if (this->is_opened)
		{
			m_iFocused = m_iSelected;

			_super::parent_widget->send_event(W_EVENT_COMBOBOX_SELECTION_CHANGED, true, this);
			this->is_opened = false;
		}

		// Make sure the control is no longer in a pressed state
		_super::pressed = false;

		// Release focus if appropriate
		if (!_super::parent_widget->keyboard_input)
		{
			_super::parent_widget->clear_focus();
		}

		break;
	}

	case WM_LBUTTONUP:
	{
		if (_super::pressed && contains_point(pPoint))
		{
			// Button click
			_super::pressed = false;
			ReleaseCapture();
			return true;
		}

		break;
	}

	case WM_MOUSEWHEEL:
	{
		int zDelta = (short)HIWORD(pWParam) / WHEEL_DELTA;
		if (this->is_opened)
		{
			UINT uLines = 0;
			if (!SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &uLines, 0))
			{
				uLines = 0;
			}
			this->scroll_bar.scroll(-zDelta * uLines);
		}
		else
		{
			if (zDelta > 0)
			{
				if (m_iFocused > 0)
				{
					m_iFocused--;
					m_iSelected = m_iFocused;

					if (!this->is_opened)
					{
						_super::parent_widget->send_event(W_EVENT_COMBOBOX_SELECTION_CHANGED, true, this);
					}
				}
			}
			else
			{
				if (m_iFocused + 1 < (int)get_num_items())
				{
					m_iFocused++;
					m_iSelected = m_iFocused;

					if (!this->is_opened)
					{
						_super::parent_widget->send_event(W_EVENT_COMBOBOX_SELECTION_CHANGED, true, this);
					}
				}
			}
		}
		return true;
	}
	};

	return false;
}

void w_combo_box::on_hot_key()
{
	if (this->is_opened || m_iSelected == -1) return;

	if (_super::parent_widget->get_is_keyboard_input_enabled())
	{
		_super::parent_widget->request_focus(this);
	}
	m_iSelected++;

	if (m_iSelected >= (int)this->items.size())
	{
		m_iSelected = 0;
	}
	m_iFocused = m_iSelected;
	_super::parent_widget->send_event(W_EVENT_COMBOBOX_SELECTION_CHANGED, true, this);
}

bool w_combo_box::can_have_focus()
{
	return (_super::visible && _super::enabled);
}

void w_combo_box::render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (_super::visible == false) return;

	W_GUI_CONTROL_STATE _state = W_GUI_STATE_NORMAL;

	if (!this->is_opened)
	{
		_state = W_GUI_STATE_HIDDEN;
	}

	// Dropdown box
	auto _element = this->elements[2];

	// If we have not initialized the scroll bar page size, do that now.
	static bool bSBInit;
	if (!bSBInit)
	{
		// Update the page size of the scroll bar
		if (_super::parent_widget->get_manager()->get_font_node(_element->index_font)->height)
		{
			this->scroll_bar.set_page_size((this->rectangle_drop_down_text.bottom - this->rectangle_drop_down_text.top) / _super::parent_widget->get_manager()->get_font_node(_element->index_font)->height);
		}
		else
		{
			this->scroll_bar.set_page_size(this->rectangle_drop_down_text.bottom - this->rectangle_drop_down_text.top);
		}
		bSBInit = true;
	}

	// Scroll bar
	if (this->is_opened)
	{
		this->scroll_bar.set_scroll_color(this->_combo_box_scroll_color);
		this->scroll_bar.set_scroll_background_color(this->_combo_box_scroll_background_color);
		this->scroll_bar.set_scroll_disabled_color(this->_combo_box_scroll_disabled_color);

		this->scroll_bar.render(pGDevice, pElapsedTime);
	}

	_element->texture_color.color_states[W_GUI_STATE_NORMAL] = this->_combo_box_drop_down_context_color;

	// Blend current color
	_element->texture_color.blend(_state, pElapsedTime);
	_element->font_color.blend(_state, pElapsedTime);

	_super::parent_widget->draw_sprite(_element, &this->rectangle_drop_down, W_GUI_NEAR_BUTTON_DEPTH);

	// Selection outline
	auto _selection_element = this->elements[3];
	_selection_element->texture_color.current_color_state = this->_combo_box_selected_color;

	auto _font_height_pixel = static_cast<int>(_super::font_size) + 2; // 2 is const value to set the margin of fonts
	int curY = this->rectangle_drop_down_text.top;
	int nRemainingHeight = this->rectangle_drop_down_text.bottom - this->rectangle_drop_down_text.top;

	auto _width_over_2 = _super::width / 2;
	auto _height_over_2 = _super::height / 2;

	auto _items_size = this->items.size();
	for (size_t i = this->scroll_bar.get_track_pos(), index = 0; i < _items_size; i++, index++)
	{
		auto pItem = this->items[i];

		// Make sure there's room left in the dropdown
		nRemainingHeight -= _font_height_pixel;
		if (nRemainingHeight < 0)
		{
			pItem->visible = false;
			continue;
		}

		SetRect(&pItem->active, this->rectangle_drop_down_text.left, curY, this->rectangle_drop_down_text.right, curY + _font_height_pixel);
		curY += _font_height_pixel;

		pItem->visible = true;

		if (this->is_opened)
		{
			w_label::force_use_current_color_state = false;
			if ((int) i == m_iFocused)
			{
				RECT rc;
				SetRect(&rc, this->rectangle_drop_down.left, pItem->active.top, this->rectangle_drop_down.right, pItem->active.bottom + 2);
				_super::parent_widget->draw_sprite(_selection_element, &rc, W_GUI_NEAR_BUTTON_DEPTH);
				//_super::parent_widget->draw_text(pDevice, pContext, pItem->text, pSelectionElement, &pItem->active);

				_super::text = pItem->text;
				_super::text_offset_x = _width_over_2 + this->original_text_offset_x + this->_item_margin.x;
				_super::text_offset_y = _height_over_2 + (int)index * (int)this->_item_height + this->_item_margin.y;

				w_label::current_color_state = this->_combo_box_selected_font_color;
				w_label::force_use_current_color_state = true;
				w_label::render(pGDevice, pElapsedTime);
			}
			else
			{
				_super::text = pItem->text;
				_super::text_offset_x = _width_over_2 + this->original_text_offset_x + this->_item_margin.x;
				_super::text_offset_y = _height_over_2 + (int)index * (int)this->_item_height + this->_item_margin.y;

				w_label::render(pGDevice, pElapsedTime);
			}
		}
	}

	int nOffsetX = 0;
	int nOffsetY = 0;

	_state = W_GUI_STATE_NORMAL;

	if (_super::visible == false)
	{
		_state = W_GUI_STATE_HIDDEN;
	}
	else if (_super::enabled == false)
	{
		_state = W_GUI_STATE_DISABLED;
	}
	else if (_super::pressed)
	{
		_state = W_GUI_STATE_PRESSED;

		nOffsetX = 1;
		nOffsetY = 2;
	}
	else if (_super::mouse_over)
	{
		_state = W_GUI_STATE_MOUSEOVER;

		nOffsetX = -1;
		nOffsetY = -2;
	}
	else if (_super::has_focus)
	{
		_state = W_GUI_STATE_FOCUS;
	}
	float fBlendRate = (_state == W_GUI_STATE_PRESSED) ? 0.0f : 0.8f;

	//Set colors of comboBox - Button
	_element = this->elements[1];

	_element->texture_color.color_states[W_GUI_STATE_NORMAL] = this->_combo_box_button_color;
	_element->texture_color.color_states[W_GUI_STATE_MOUSEOVER] = this->_combo_box_button_mouse_over_color;
	_element->texture_color.color_states[W_GUI_STATE_PRESSED] = this->_combo_box_button_pressed_color;
	_element->texture_color.color_states[W_GUI_STATE_FOCUS] = this->_combo_box_button_focused_color;
	_element->texture_color.color_states[W_GUI_STATE_DISABLED] = this->_combo_box_button_disabled_color;

	// Blend current color
	_element->texture_color.blend(_state, pElapsedTime, fBlendRate);

	RECT rcWindow = this->rectangle_button;
	OffsetRect(&rcWindow, nOffsetX, nOffsetY);
	_super::parent_widget->draw_sprite(_element, &rcWindow, W_GUI_FAR_BUTTON_DEPTH);

	if (this->is_opened)
	{
		_state = W_GUI_STATE_PRESSED;
	}
	// Main box
	_element = this->elements[0];

	//Set colors of comboBox - Box
	_element->texture_color.color_states[W_GUI_STATE_NORMAL] = this->_combo_box_main_color;
	_element->texture_color.color_states[W_GUI_STATE_FOCUS] = this->_combo_box_main_focused_color;
	_element->texture_color.color_states[W_GUI_STATE_PRESSED] = this->_combo_box_main_pressed_color;
	_element->texture_color.color_states[W_GUI_STATE_MOUSEOVER] = this->_combo_box_main_mouse_over_color;
	_element->texture_color.color_states[W_GUI_STATE_DISABLED] = this->_combo_box_main_disabled_color;

	//Set colors of text
	_element->font_color.color_states[W_GUI_STATE_MOUSEOVER] = _super::label_mouse_over_color;
	_element->font_color.color_states[W_GUI_STATE_PRESSED] = _super::label_pressed_color;
	_element->font_color.color_states[W_GUI_STATE_DISABLED] = _super::label_disabled_color;
	_element->font_color.color_states[W_GUI_STATE_FOCUS] = _super::label_focused_color;

	// Blend current color
	_element->texture_color.blend(_state, pElapsedTime, fBlendRate);
	_element->font_color.blend(_state, pElapsedTime, fBlendRate);

	_super::parent_widget->draw_sprite(_element, &this->rectangle_text, W_GUI_NEAR_BUTTON_DEPTH);

	if (m_iSelected >= 0 && m_iSelected < (int)this->items.size())
	{
		auto pItem = this->items[m_iSelected];
		if (pItem)
		{
			//render text
			_super::text = pItem->text;
			_super::text_offset_x = _width_over_2 + _super::original_text_offset_x;
			_super::text_offset_y = _height_over_2 + _super::original_text_offset_y;

			w_label::render(pGDevice, pElapsedTime);
		}
	}
}

_Use_decl_annotations_
HRESULT w_combo_box::add_item(const wchar_t* pText, void* pData)
{
	// Validate parameters
	if (!pText) return E_INVALIDARG;

	// Create a new item and set the data
	auto pItem = new (std::nothrow) w_combo_box_item;
	if (!pItem)
	{
		V(S_FALSE, L"E_OUTOFMEMORY", _super::name, 2);
		return S_FALSE;
	}

	std::memset(pItem, 0, sizeof(w_combo_box_item));
	wcscpy_s(pItem->text, 256, pText);
	pItem->data = pData;

	this->items.push_back(pItem);

	// Update the scroll bar with new range
	this->scroll_bar.set_track_range(0, (int)this->items.size());

	// If this is the only item in the list, it's selected
	if (get_num_items() == 1)
	{
		m_iSelected = 0;
		m_iFocused = 0;
		_super::parent_widget->send_event(W_EVENT_COMBOBOX_SELECTION_CHANGED, false, this);
	}

	return S_OK;
}

void w_combo_box::remove_item(_In_ UINT pIndex)
{
	auto it = this->items.begin() + pIndex;
	auto pItem = *it;
	SAFE_DELETE(pItem);

	this->items.erase(it);
	this->scroll_bar.set_track_range(0, (int)this->items.size());
	if (m_iSelected >= (int)this->items.size())
	{
		m_iSelected = (int)this->items.size() - 1;
	}
}

bool w_combo_box::contains_item(_In_z_ const wchar_t* pText, _In_ UINT pIndexStart)
{
	return (-1 != find_item(pText, pIndexStart));
}

int w_combo_box::find_item(_In_z_ const wchar_t* pText, _In_ UINT pIndexStart) const
{
	if (!pText) return -1;

	for (size_t i = pIndexStart; i < this->items.size(); i++)
	{
		auto pItem = this->items[i];

		if (0 == wcscmp(pItem->text, pText))
		{
			return static_cast<int>(i);
		}
	}

	return -1;
}

#pragma region Getters

void* w_combo_box::get_item_data(_In_z_ const wchar_t* pText) const
{
	int index = find_item(pText);
	if (index == -1) return nullptr;

	auto _item = this->items[index];
	if (!_item)
	{
		V(S_FALSE, L"w_comboBox::get_item_data", _super::name, 2, false, false);
		return nullptr;
	}

	return _item->data;
}

void* w_combo_box::get_item_data(_In_ int pIndex) const
{
	if (pIndex < 0 || pIndex >= (int)this->items.size()) return nullptr;

	return this->items[pIndex]->data;
}

void* w_combo_box::get_selected_data() const
{
	if (m_iSelected < 0) return nullptr;

	auto _item = this->items[m_iSelected];
	return _item->data;
}

w_combo_box_item* w_combo_box::get_selected_item() const
{
	if (m_iSelected < 0) return nullptr;

	return this->items[m_iSelected];
}

#pragma endregion

#pragma region Setters

void w_combo_box::set_drop_down_context_height(_In_ UINT pHeight)
{
	this->drop_context_height = pHeight;
	update_rects();
}

void w_combo_box::set_scroll_bar_width(_In_ int pWidth)
{
	this->scrollBar_width = pWidth;
	update_rects();
}

HRESULT w_combo_box::set_selected_by_index(_In_ UINT pIndex)
{
	if (pIndex >= get_num_items()) return E_INVALIDARG;

	m_iFocused = m_iSelected = pIndex;
	(_super::parent_widget)->send_event(W_EVENT_COMBOBOX_SELECTION_CHANGED, false, this);

	return S_OK;
}

HRESULT w_combo_box::set_selected_by_text(_In_z_ const wchar_t* pText)
{
	if (!pText) return E_INVALIDARG;

	int index = find_item(pText);
	if (index == -1) return E_FAIL;

	m_iFocused = m_iSelected = index;
	(_super::parent_widget)->send_event(W_EVENT_COMBOBOX_SELECTION_CHANGED, false, this);

	return S_OK;
}

HRESULT w_combo_box::set_selected_by_data(_In_ void* pData)
{
	for (size_t i = 0; i < this->items.size(); i++)
	{
		auto pItem = this->items[i];

		if (pItem->data == pData)
		{
			m_iFocused = m_iSelected = static_cast<int>(i);
			(_super::parent_widget)->send_event(W_EVENT_COMBOBOX_SELECTION_CHANGED, false, this);
			return S_OK;
		}
	}

	return E_FAIL;
}

#pragma endregion

