#include "w_directX_pch.h"
#include "w_tab.h"
#include "w_gui/w_widgets_resource_manager.h"

#define TAB_MARGIN	23

using namespace wolf::gui;

static void CALLBACK on_gui_event(UINT pEvent, int pControlID, wolf::gui::w_control* pControl, void* pUserContext);

w_tab::w_tab(_In_opt_ w_widget* pParent) :
	_super(pParent),
	selected_tab(-1),
	_first_time_initializing(true),
	_pre_button(nullptr),
	_nxt_button(nullptr),
	button_color(w_color::from_hex(w_color::GRAY)),
	button_pressed_color(255, 255, 255, 200),
	button_mouse_over_color(w_color::from_hex(w_color::WHITE)),
	button_focused_color(255, 255, 255, 200),
	button_disabled_color(50, 50, 50, 150),
	text_color(w_color::from_hex(w_color::WHITE)),
	text_pressed_color(w_color::from_hex(w_color::BLACK)),
	text_mouse_over_color(w_color::from_hex(w_color::BLACK)),
	text_focused_color(w_color::from_hex(w_color::WHITE)),
	text_disabled_color(50, 50, 50, 150)
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_TAB;

	this->arrow_button_text_offset.x = 8;
	this->arrow_button_text_offset.y = 5;
}

w_tab::~w_tab()
{
	release();
}

HRESULT w_tab::on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
	W_UNUSED(pGDevice);
	if (_super::parent_widget)
	{
		_super::parent_widget->set_call_back(on_gui_event, this);
	}
	return S_OK;
}

bool w_tab::handle_keyboard(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam)
{
	bool _handle_keyboard = false;
	std::for_each(this->buttons.begin(), this->buttons.end(), [&](w_button* pButton)
	{
		if (pButton)
		{
			if (!_handle_keyboard && pButton->handle_keyboard(pMsg, pWParam, pLParam))
			{
				_handle_keyboard = true;
			}
		}
	});
	return _handle_keyboard;
}

bool w_tab::handle_mouse(_In_ UINT pMsg, _In_ const POINT& pPoint, _In_ WPARAM pWParam, _In_ LPARAM pLParam)
{
	bool _handle_mouse = false;
	std::for_each(this->buttons.begin(), this->buttons.end(), [&](w_button* pButton)
	{
		if (pButton)
		{
			if (!_handle_mouse &&  pButton->handle_mouse(pMsg, pPoint, pWParam, pLParam))
			{
				_handle_mouse = true;
			}
		}
	});
	return _handle_mouse;
}

void w_tab::on_hot_key()
{
	std::for_each(this->buttons.begin(), this->buttons.end(), [](w_button* pButton)
	{
		if (pButton)
		{
			pButton->on_hot_key();
		}
	});
	_super::on_hot_key();
}

bool w_tab::contains_point(_In_ const POINT& pPoint)
{
	bool _contains_point = false;
	std::for_each(this->buttons.begin(), this->buttons.end(), [pPoint, &_contains_point](w_button* pButton)
	{
		if (pButton)
		{
			if (!_contains_point && pButton->contains_point(pPoint))
			{
				_contains_point = true;
			}
		}
	});
	return _contains_point;
}

bool w_tab::can_have_focus()
{
	std::for_each(this->buttons.begin(), this->buttons.end(), [this](w_button* pButton)
	{
		if (pButton)
		{
			pButton->can_have_focus();
		}
	});
	return _super::can_have_focus();
}

void w_tab::render(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	const auto _margin = this->width - TAB_MARGIN;
	auto _size = this->buttons.size();

	if (_size == 0) return;
	
	int _x = 0, _y = 0;
	this->buttons.at(_size - 1)->get_position(_x, _y);
	auto _enable = _x >= _margin;

	this->_nxt_button->set_visible(_enable);
	this->_nxt_button->set_enabled(_enable);

	_x = 0; _y = 0;
	this->buttons.at(0)->get_position(_x, _y);
	_enable = _x <= _super::x;

	this->_pre_button->set_visible(_enable);
	this->_pre_button->set_enabled(_enable);

	//set colors
	this->_pre_button->set_label_color(this->text_color);
	this->_pre_button->set_label_pressed_color(this->text_pressed_color);
	this->_pre_button->set_label_mouse_over_color(this->text_mouse_over_color);
	this->_pre_button->set_label_focused_color(this->text_focused_color);
	this->_pre_button->set_label_disabled_color(this->text_disabled_color);

	this->_pre_button->set_button_color(this->button_color);
	this->_pre_button->set_button_pressed_color(this->button_pressed_color);
	this->_pre_button->set_button_mouse_over_color(this->button_mouse_over_color);
	this->_pre_button->set_button_focused_color(this->button_focused_color);
	this->_pre_button->set_button_disabled_color(this->button_disabled_color);

	this->_nxt_button->set_label_color(this->text_color);
	this->_nxt_button->set_label_pressed_color(this->text_pressed_color);
	this->_nxt_button->set_label_mouse_over_color(this->text_mouse_over_color);
	this->_nxt_button->set_label_focused_color(this->text_focused_color);
	this->_nxt_button->set_label_disabled_color(this->text_disabled_color);

	this->_nxt_button->set_button_color(this->button_color);
	this->_nxt_button->set_button_pressed_color(this->button_pressed_color);
	this->_nxt_button->set_button_mouse_over_color(this->button_mouse_over_color);
	this->_nxt_button->set_button_focused_color(this->button_focused_color);
	this->_nxt_button->set_button_disabled_color(this->button_disabled_color);
	
	std::for_each(this->buttons.begin(), this->buttons.end(), [this, _margin](w_button* pButton)
	{
		if (pButton)
		{
			pButton->set_label_color(this->text_color);
			pButton->set_label_pressed_color(this->text_pressed_color);
			pButton->set_label_mouse_over_color(this->text_mouse_over_color);
			pButton->set_label_focused_color(this->text_focused_color);
			pButton->set_label_disabled_color(this->text_disabled_color);

			pButton->set_button_color(this->button_color);
			pButton->set_button_pressed_color(this->button_pressed_color);
			pButton->set_button_mouse_over_color(this->button_mouse_over_color);
			pButton->set_button_focused_color(this->button_focused_color);
			pButton->set_button_disabled_color(this->button_disabled_color);

			int _x = 0, _y = 0;
			pButton->get_position(_x, _y);

			//do not need to check buttons at the out of range
			if (_x != INT_MIN && _x != INT_MAX)
			{
				//check for max range
				auto _enable = _x + this->button_width <= _margin;

				pButton->set_visible(_enable);
				pButton->set_enabled(_enable);

				if (!_enable)
				{
					//change position
					pButton->set_position(INT_MAX, _y);
				}

				//check for max range
				_enable = _x >= _super::x;

				pButton->set_visible(_enable);
				pButton->set_enabled(_enable);

				if (!_enable)
				{
					//change position
					pButton->set_position(INT_MIN, _y);
				}
			}
		}
	});
}

void w_tab::add_tab_item(_In_z_ const std::wstring& pText, 
	_In_z_ const std::wstring& pIconPath, 
	_In_ bool pRelativePath,
	_In_ int pTextOffsetX, _In_ int pTextOffsetY,
	_In_ int pIconOffsetX, _In_ int pIconOffsetY,
	_In_ float pIconScaleX, _In_ float pIconScaleY,
	_In_ UINT pHotkey,
	_In_ bool pIsDefault,
	_In_ void* pTag)
{
	if (this->_first_time_initializing)
	{
		auto _button_width_over_four = (int)this->button_width / 4 - this->arrow_button_text_offset.x;
		auto _button_height_over_four = (int)this->button_height / 4 - this->arrow_button_text_offset.y;

		//the first button is <<
		_super::parent_widget->add_button(INT_MIN,
			L"<<",
			TAB_MARGIN, this->button_height,
			_super::x, _super::y,
			L"", true,
			-_button_width_over_four, _button_height_over_four,
			10, 10, 
			1, 1,
			0, 
			false,
			&this->_pre_button);

		//the second button is >>
		_super::parent_widget->add_button(INT_MAX,
			L">>",
			TAB_MARGIN, this->button_height,
			this->width - TAB_MARGIN, _super::y,
			L"", true,
			-_button_width_over_four, _button_height_over_four,
			10, 10,
			1, 1,
			0,
			false,
			&this->_nxt_button);
		
		this->_first_time_initializing = false;
	}


	auto _size = static_cast<int>(this->buttons.size());
	w_button* _tab_button = nullptr;
	_super::parent_widget->add_button(this->id + _size,
		pText.c_str(),
		this->button_width, this->button_height,
		_super::x + TAB_MARGIN + _size * static_cast<int>(this->button_width), _super::y,
		pIconPath.c_str(), pRelativePath,
		pTextOffsetX, pTextOffsetY,
		pIconOffsetX, pIconOffsetY,
		pIconScaleX, pIconScaleY,
		pHotkey,
		pIsDefault,
		&_tab_button);
	_tab_button->set_tag(pTag);

	this->buttons.push_back(_tab_button);
}

void w_tab::select_next_tab()
{
	bool _got_first_button_from_hidden_ones = false;
	const auto _last_x = _super::x + TAB_MARGIN + ((this->width - 2 * TAB_MARGIN) / static_cast<int>(this->button_width)) * static_cast<int>(this->button_width);
	
	for (size_t i = 0; i < this->buttons.size(); ++i)
	{
		auto _button = this->buttons.at(i);
		if (_button)
		{
			int _x = 0, _y = 0;
			_button->get_position(_x, _y);

			//do not change values of INT_MIN. also just change first button at the edge of INT_MAX
			if (_got_first_button_from_hidden_ones || _x == INT_MIN)
			{
				continue;
			}
			else
			{
				if (_x == INT_MAX)
				{
					_x = _last_x;
					_got_first_button_from_hidden_ones = true;
				}
			}

			_button->set_position(_x - this->button_width, _y);
		}
	}
}

void w_tab::select_previous_tab()
{
	bool _got_first_button_from_hidden_ones = false;
	const auto _first_x = _super::x + TAB_MARGIN - this->button_width;

	//make sure use int instead of size_t for i--
	for (int i = this->buttons.size() - 1; i >= 0; i--)
	{
		auto _button = this->buttons.at(i);
		if (_button)
		{
			int _x = 0, _y = 0;
			_button->get_position(_x, _y);

			//do not change values of INT_MAX. also just change first button at the edge of INT_MIN
			if (_got_first_button_from_hidden_ones || _x == INT_MAX)
			{
				continue;
			}
			else
			{
				if (_x == INT_MIN)
				{
					_x = _first_x;
					_got_first_button_from_hidden_ones = true;
				}
			}

			_button->set_position(_x + this->button_width, _y);
		}
	}
}

void w_tab::remove_all_tabs()
{
	if (_super::parent_widget)
	{
		std::for_each(this->buttons.begin(), this->buttons.end(), [this](w_button* pButton)
		{
			if (pButton)
			{
				_super::parent_widget->remove_control(pButton->get_ID());
			}
		});
		this->buttons.clear();

		if (this->_nxt_button)
		{
			_super::parent_widget->remove_control(this->_nxt_button->get_ID());
		}
		if (this->_pre_button)
		{
			_super::parent_widget->remove_control(this->_pre_button->get_ID());
		}
	}

	//ready for next time
	this->_first_time_initializing = true;
}

ULONG w_tab::release()
{
	if (this->is_released()) return 0;

	if (_super::parent_widget)
	{
		_super::parent_widget->remove_call_back(on_gui_event);
	}

	return _super::release();
}

static void CALLBACK on_gui_event(UINT pEvent, int pControlID, wolf::gui::w_control* pControl, void* pUserContext)
{
	if (pControl)
	{
		auto _tab = reinterpret_cast<w_tab*>(pUserContext);
		if (_tab)
		{
			if (pControlID == INT_MIN)
			{
				_tab->select_previous_tab();
			}
			else if (pControlID == INT_MAX)
			{
				_tab->select_next_tab();
			}
			else
			{
				_tab->set_selected_tab(pControl->get_ID() - _tab->get_ID() + 1);
			}
		}
	}
}

