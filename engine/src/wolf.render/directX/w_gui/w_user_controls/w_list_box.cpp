#include "w_directX_pch.h"
#include "w_list_box.h"
#include "w_gui/w_widgets_resource_manager.h"

using namespace wolf::gui;

w_list_box::w_list_box(_In_opt_ w_widget* pParent) : _super(pParent),
text_margin_x(2),
text_margin_y(25),
icon_margin_x(0),
icon_margin_y(0),
icon_width(32),
icon_height(32),
use_default_icon_size(false),
icon_height_offset(0),
list_box_text_color(w_color::from_hex(w_color::BLACK)),
list_box_text_selected_color(w_color::from_hex(w_color::WHITE)),
list_box_text_disabled_color(w_color::from_hex(w_color::GRAY))
{
	_super::type = W_GUI_CONTROL_LISTBOX;
	_super::item_height = 170;//35;
	_super::set_item_selected_rectangle_margin(4, 150);// 15);
}

w_list_box::~w_list_box()
{
	remove_all_items();
}

HRESULT w_list_box::add_item(_In_z_ const std::wstring& pText, _In_z_ const std::wstring& pIconPath, _In_ bool pIsAbsolutePath, _In_opt_  void* pData)
{
	auto _widget_item = new w_list_widget_item();
	if (!_widget_item) return E_OUTOFMEMORY;

	auto _new_list_item = new (std::nothrow) w_list_box_item();
	if (!_new_list_item) return E_OUTOFMEMORY;

	_new_list_item->text = pText;
	_new_list_item->icon_path = pIconPath;
	_new_list_item->data = pData;
	//_new_list_item->selected = false;
	//SetRect(&_new_list_item->rectangle_active, 0, 0, 0, 0);

	if (!pIconPath.empty())
	{
		//auto _path = pRelateivePath ? wolf::framework::w_game::get_content_directory() + pIconPath : pIconPath;

		auto _icon = new w_image(_super::parent_widget);
		_icon->set_path(pIconPath, pIsAbsolutePath);
		_icon->set_use_default_size(this->use_default_icon_size);
		_icon->set_size(this->icon_width, this->icon_height);

		_icon->on_initialize(_super::gDevice);

		//add to list widget
		_widget_item->gpu_controls.push_back(std::make_tuple(_icon, this->icon_margin_x, this->icon_margin_y));
	}

	auto _label = new w_label(_super::parent_widget);
	_label->set_text(pText);
	_label->set_label_color(this->list_box_text_color);
	_label->set_current_color_state(this->list_box_text_color);
	_label->set_label_focused_color(this->list_box_text_selected_color);
	_label->set_label_disabled_color(this->list_box_text_disabled_color);
	_label->set_force_use_current_color_state(true);

	//add to list box
	this->list_items.push_back(_new_list_item);

	//add to list widget
	_widget_item->gpu_controls.push_back(std::make_tuple(_label, this->text_margin_x, this->text_margin_y));

	_super::add_item(_widget_item);

	return S_OK;
}

HRESULT w_list_box::insert_item(int pIndex, _In_z_ const std::wstring& pText, _In_z_ const std::wstring& pIconPath, _In_ bool pIsAbsolutePath, _In_opt_ void* pData)
{
	auto _widget_item = new w_list_widget_item();
	if (!_widget_item) return E_OUTOFMEMORY;

	auto _new_list_item = new (std::nothrow) w_list_box_item();
	if (!_new_list_item) return E_OUTOFMEMORY;

	_new_list_item->text = pText;
	_new_list_item->icon_path = pIconPath;
	_new_list_item->data = pData;
	//_new_list_item->selected = false;
	//SetRect(&_new_list_item->rectangle_active, 0, 0, 0, 0);

	if (!pIconPath.empty())
	{
		//auto _path = pRelateivePath ? wolf::framework::w_game::get_content_directory() + pIconPath : pIconPath;

		auto _icon = new w_image(_super::parent_widget);
		_icon->set_path(pIconPath, pIsAbsolutePath);
		_icon->set_use_default_size(this->use_default_icon_size);
		_icon->set_size(this->icon_width, this->icon_height);

		//_icon->set_position(_super::x + this->icon_margin_x, _super::y + this->icon_margin_y);

		_icon->on_initialize(_super::gDevice);

		//add to list widget
		_widget_item->gpu_controls.push_back(std::make_tuple(_icon, this->icon_margin_x, this->icon_margin_y));
	}

	auto _label = new w_label(_super::parent_widget);
	_label->set_text(pText);
	_label->set_label_color(this->list_box_text_color);
	_label->set_current_color_state(this->list_box_text_color);
	_label->set_label_focused_color(this->list_box_text_selected_color);
	_label->set_label_disabled_color(this->list_box_text_disabled_color);
	_label->set_force_use_current_color_state(true);

	//add to list box
	this->list_items.insert(this->list_items.begin() + pIndex, _new_list_item);

	//add to list widget
	_widget_item->gpu_controls.push_back(std::make_tuple(_label, this->text_margin_x, this->text_margin_y));

	_super::insert_item(pIndex, _widget_item);

	return S_OK;
}

void w_list_box::remove_item(_In_ const size_t pIndex)
{
	if (pIndex < 0 || pIndex >= this->list_items.size()) return;

	auto _iter = this->list_items.begin() + pIndex;
	auto _item = *_iter;

	this->list_items.erase(_iter);

	delete _item;

	_super::remove_item(pIndex);
}

void w_list_box::remove_all_items()
{
	this->list_items.clear();
	_super::remove_all_items();
}

void w_list_box::render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (!_super::visible) return;

	//auto _element = _super::elements[0];

	//_element->texture_color.color_states[W_GUI_STATE_NORMAL] = this->list_box_color;
	//_element->texture_color.color_states[W_GUI_STATE_DISABLED] = this->list_box_disabled_color;

	//_element->texture_color.blend(W_GUI_STATE_NORMAL, pElapsedTime);
	//_element->font_color.blend(W_GUI_STATE_NORMAL, pElapsedTime);

	//auto _selected_element = _super::elements[1];

	//_selected_element->texture_color.current_color_state = this->list_box_selected_color;
	//_selected_element->texture_color.blend(W_GUI_STATE_NORMAL, pElapsedTime);	
	//
	//_super::parent_widget->draw_sprite(_element, &(_super::bounding_box), W_GUI_FAR_BUTTON_DEPTH);

	//std::vector<w_image*> _drawing_icons;
	//// Render the text
	//if (!this->items.empty())
	//{
	//	// Find out the height of a single line of text
	//	RECT _rectangle = this->rectangle_text;
	//	RECT rcSel = this->rectangle_selection;
	//	_rectangle.bottom = _rectangle.top + _super::parent_widget->get_manager()->get_font_node(_element->index_font)->height;

	//	// Update the line height formation
	//	if (this->item_height == 0)
	//	{
	//		this->item_height = _rectangle.bottom - _rectangle.top;
	//	}

	//	if (!this->_initialized_scroll_size)
	//	{
	//		// Update the page size of the scroll bar
	//		if (this->item_height)
	//		{
	//			this->scroll_bar.set_page_size((this->rectangle_text.bottom - this->rectangle_text.top) / this->item_height);
	//		}
	//		else
	//		{
	//			this->scroll_bar.set_page_size((this->rectangle_text.bottom - this->rectangle_text.top));
	//		}
	//		this->_initialized_scroll_size = true;
	//	}

	//	_rectangle.right = this->rectangle_text.right;
	//	for (int j = 0, i = this->scroll_bar.get_track_pos(); i < (int) this->items.size(); ++i, j++)
	//	{
	//		if (_rectangle.bottom > this->rectangle_text.bottom) break;

	//		auto pItem = this->items[i];

	//		// Determine if we need to render this item with the
	//		// selected element.
	//		bool _selected_style = false;

	//		if (!this->multiselection && i == this->selected_index)
	//		{
	//			_selected_style = true;
	//		}
	//		else if (this->multiselection)
	//		{
	//			if (this->drag && ((i >= this->selected_index && i < this->selected_start) || (i <= this->selected_index && i > this->selected_start)))
	//			{
	//				_selected_style = this->items[this->selected_start]->selected;
	//			}
	//			else if (pItem->selected)
	//			{
	//				_selected_style = true;
	//			}
	//		}

	//		//this->_label->set_label_disabled_color(this->_label_disabled_color);
	//		if (_selected_style)
	//		{
	//			rcSel.top = _rectangle.top - item_selected_rectangle_margin[0];
	//			rcSel.bottom = _rectangle.bottom + item_selected_rectangle_margin[1];

	//			//Draw selected rectangle
	//			_super::parent_widget->draw_sprite(_selected_element, &rcSel, W_GUI_NEAR_BUTTON_DEPTH);

	//			this->label->set_text(pItem->text);
	//			this->label->set_text_offset_x(_rectangle.left + this->text_margin_x);
	//			this->label->set_text_offset_y(_rectangle.top + this->text_margin_y);

	//			this->label->set_current_color_state(this->list_box_text_selected_color);

	//			this->label->render(pGDevice, pElapsedTime);
	//		}
	//		else
	//		{
	//			this->label->set_text(pItem->text);
	//			this->label->set_text_offset_x(_rectangle.left + this->text_margin_x);
	//			this->label->set_text_offset_y(_rectangle.top + this->text_margin_y);

	//			this->label->set_current_color_state(this->list_box_text_color);

	//			this->label->render(pGDevice, pElapsedTime);
	//		}

	//		if (pItem && pItem->icon)
	//		{
	//			pItem->icon->set_position(_rectangle.left + this->icon_margin_x, _rectangle.top + this->icon_margin_y + j * this->icon_height_offset);
	//			pItem->icon->set_scale(this->icon_scale.x, this->icon_scale.y);
	//			_drawing_icons.push_back(pItem->icon);
	//		}

	//		OffsetRect(&_rectangle, 0, this->item_height);
	//	}
	//}

	////Render all selected icon
	//auto _size = _drawing_icons.size();
	//if (_size)
	//{
	//	for (size_t i = 0; i < _size; ++i)
	//	{
	//		auto _image = _drawing_icons.at(i);
	//		_image->redraw = this->redraw;
	//		_image->render(pGDevice, pElapsedTime);
	//	}
	//}
	//_drawing_icons.clear();

	//_super::redraw = false;

	_super::render(pGDevice, pElapsedTime);
}

#pragma region Getters

w_list_box_item* w_list_box::get_list_box_item(_In_ int nIndex) const
{
	if (nIndex < 0 || nIndex >= (int) this->list_items.size()) return nullptr;

	return this->list_items[nIndex];
}

#pragma endregion

#pragma region Setters

#pragma endregion
