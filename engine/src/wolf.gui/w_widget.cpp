#include "w_gui_pch.h"
#include "w_widget.h"
#include <w_timer_callback.h>

using namespace wolf::gui;

//all widgets
std::vector<w_widget*>      w_widget::widgets;

wolf::system::w_time_span   w_widget::s_time_refresh = wolf::system::w_time_span::zero();
w_control*					w_widget::s_pControlFocus = nullptr;        // The control which has focus
w_control*					w_widget::s_pControlPressed = nullptr;      // The control currently pressed

w_widget::w_widget(UINT pParentWindowWidth, UINT pParentWindowHeight) :
    _parent_window_width(pParentWindowWidth),
    _parent_window_height(pParentWindowHeight),
    _name("widget"),
    _width(400),
    _height(200),
    _draggable(false),
    _is_dragging(false),
    _visible(true),
    _minimized(false),
    _x(0),
    _y(0),
    _enabled(true),
    _z_order(0),
    _background_color_top_left(216, 238, 249, 200),
    _background_color_top_right(216, 238, 249, 200),
    _background_color_bottom_left(71, 188, 242, 200),
    _background_color_bottom_right(71, 188, 242, 200),
    _active_background_color_top_left(216, 238, 249, 255),
    _active_background_color_top_right(216, 238, 249, 255),
    _active_background_color_bottom_left(71, 188, 242, 255),
    _active_background_color_bottom_right(71, 188, 242, 255),
    _is_caption_enabled(true),
    _caption_background_color(57, 57, 57, 200),
    _caption_active_background_color(57, 57, 57, 255),
    _caption_height(25),
    _caption_margin_left(0),
    _caption_margin_top(0),
    _control_mouse_over(nullptr),
    _is_mouse_over_widget(false),
    _is_double_click(true),
    non_user_events(false),
    keyboard_input(false),
    mouse_input(true),
    on_mouse_move_callback(nullptr),
    on_mouse_enter_callback(nullptr),
    on_mouse_leave_callback(nullptr)
{
    this->_caption[0] = L'\0';
    time_last_refresh = system::w_time_span::zero();
    this->_next_widget = this;
    this->_prev_widget = this;

    std::memset(&this->_global_mouse_point, 0, sizeof(this->_global_mouse_point));
    _initialize();
}

w_widget::~w_widget()
{
	release();
}

void w_widget::_initialize()
{
    this->_redrawing = true;

    w_element _element;
    w_rectangle _rectangle_texture;

    //-------------------------------------
    // caption
    //-------------------------------------
    _rectangle_texture.left = 142;
    _rectangle_texture.top = 269;
    _rectangle_texture.right = 243;
    _rectangle_texture.bottom = 287;

    this->_caption_element.set_texture(0, &_rectangle_texture);
    this->_caption_element.texture_color.blend(W_GUI_STATE_NORMAL, 10.0f);

    //-------------------------------------
    // w_label
    //-------------------------------------
    set_default_element(W_GUI_CONTROL_LABEL, 0, &_element);

    //-------------------------------------
    // w_image
    //-------------------------------------
    set_default_element(W_GUI_CONTROL_IMAGE, 0, &_element);
    //-------------------------------------
    // w_tab
    //-------------------------------------
    set_default_element(W_GUI_CONTROL_TAB, 0, &_element);

    //-------------------------------------
    // w_button
    //-------------------------------------
    _rectangle_texture.left = 0;
    _rectangle_texture.top = 0;
    _rectangle_texture.right = 136;
    _rectangle_texture.bottom = 54;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_BUTTON, 0, &_element);

    //-------------------------------------
    // fill layer
    //-------------------------------------
    _rectangle_texture.left = 136;
    _rectangle_texture.top = 0;
    _rectangle_texture.right = 252;
    _rectangle_texture.bottom = 54;
    _element.set_texture(0, &_rectangle_texture, RGBA_TO_HEX_COLOR(255, 255, 255, 0));
    set_default_element(W_GUI_CONTROL_BUTTON, 1, &_element);

    //-------------------------------------
    // w_check_box - text
    //-------------------------------------
    _rectangle_texture.left = 0;
    _rectangle_texture.top = 54;
    _rectangle_texture.right = 27;
    _rectangle_texture.bottom = 81;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_CHECKBOX, 0, &_element);

    //-------------------------------------
    // w_check_box - check
    //-------------------------------------
    _rectangle_texture.left = 27;
    _rectangle_texture.top = 54;
    _rectangle_texture.right = 54;
    _rectangle_texture.bottom = 81;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_CHECKBOX, 1, &_element);

    //-------------------------------------
    // w_radio_button - Box
    //-------------------------------------
    _rectangle_texture.left = 54;
    _rectangle_texture.top = 54;
    _rectangle_texture.right = 81;
    _rectangle_texture.bottom = 81;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_RADIOBUTTON, 0, &_element);

    //-------------------------------------
    // w_radio_button - check
    //-------------------------------------
    _rectangle_texture.left = 81;
    _rectangle_texture.top = 54;
    _rectangle_texture.right = 108;
    _rectangle_texture.bottom = 81;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_RADIOBUTTON, 1, &_element);

    //-------------------------------------
    // w_combo_bbox - main
    //-------------------------------------
    _rectangle_texture.left = 7;
    _rectangle_texture.top = 81;
    _rectangle_texture.right = 247;
    _rectangle_texture.bottom = 123;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_COMBOBOX, 0, &_element);

    //-------------------------------------
    // combo_box - Button
    //-------------------------------------
    _rectangle_texture.left = 98;
    _rectangle_texture.top = 189;
    _rectangle_texture.right = 151;
    _rectangle_texture.bottom = 238;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_COMBOBOX, 1, &_element);

    //-------------------------------------
    // w_combo_box - drop_down
    //-------------------------------------
    _rectangle_texture.left = 13;
    _rectangle_texture.top = 123;
    _rectangle_texture.right = 241;
    _rectangle_texture.bottom = 160;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_COMBOBOX, 2, &_element);

    //-------------------------------------
    // w_combo_box - selection
    //-------------------------------------
    _rectangle_texture.left = 12;
    _rectangle_texture.top = 163;
    _rectangle_texture.right = 239;
    _rectangle_texture.bottom = 183;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_COMBOBOX, 3, &_element);

    //-------------------------------------
    // w_slider - track
    //-------------------------------------
    _rectangle_texture.left = 1;
    _rectangle_texture.top = 187;
    _rectangle_texture.right = 93;
    _rectangle_texture.bottom = 228;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_SLIDER, 0, &_element);

    //-------------------------------------
    // w_slider - button
    //-------------------------------------
    _rectangle_texture.left = 151;
    _rectangle_texture.top = 193;
    _rectangle_texture.right = 192;
    _rectangle_texture.bottom = 234;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_SLIDER, 1, &_element);

    //-------------------------------------
    // w_scroll_bar - track
    //-------------------------------------
    int _scroll_bar_start_x = 196;
    int _scroll_bar_start_y = 191;
    _rectangle_texture.left = _scroll_bar_start_x + 0;
    _rectangle_texture.top = _scroll_bar_start_y + 21;
    _rectangle_texture.right = _scroll_bar_start_x + 22;
    _rectangle_texture.bottom = _scroll_bar_start_y + 32;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_SCROLLBAR, 0, &_element);

    //-------------------------------------
    // w_scroll_bar - up arrow
    //-------------------------------------
    _rectangle_texture.left = _scroll_bar_start_x + 0;
    _rectangle_texture.top = _scroll_bar_start_y + 1;
    _rectangle_texture.right = _scroll_bar_start_x + 22;
    _rectangle_texture.bottom = _scroll_bar_start_y + 21;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_SCROLLBAR, 1, &_element);

    //-------------------------------------
    // w_scroll_bar - down arrow
    //-------------------------------------
    _rectangle_texture.left = _scroll_bar_start_x + 0;
    _rectangle_texture.top = _scroll_bar_start_y + 32;
    _rectangle_texture.right = _scroll_bar_start_x + 22;
    _rectangle_texture.bottom = _scroll_bar_start_y + 53;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_SCROLLBAR, 2, &_element);

    //-------------------------------------
    // w_scroll_bar - button
    //-------------------------------------
    _rectangle_texture.left = 220;
    _rectangle_texture.top = 192;
    _rectangle_texture.right = 238;
    _rectangle_texture.bottom = 234;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_SCROLLBAR, 3, &_element);

    //-------------------------------------
    // w_list_box - main
    //-------------------------------------
    _rectangle_texture.left = 13;
    _rectangle_texture.top = 123;
    _rectangle_texture.right = 241;
    _rectangle_texture.bottom = 160;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_LISTBOX, 0, &_element);

    //-------------------------------------
    // w_list_box - selection
    //-------------------------------------
    _rectangle_texture.left = 16;
    _rectangle_texture.top = 166;
    _rectangle_texture.right = 240;
    _rectangle_texture.bottom = 183;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_LISTBOX, 1, &_element);

    //-------------------------------------
    // w_list_widget - main
    //-------------------------------------
    _rectangle_texture.left = 13;
    _rectangle_texture.top = 123;
    _rectangle_texture.right = 241;
    _rectangle_texture.bottom = 160;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_LISTWIDGET, 0, &_element);

    //-------------------------------------
    // w_list_widget - selection
    //-------------------------------------
    _rectangle_texture.left = 16;
    _rectangle_texture.top = 166;
    _rectangle_texture.right = 240;
    _rectangle_texture.bottom = 183;
    _element.set_texture(0, &_rectangle_texture);
    set_default_element(W_GUI_CONTROL_LISTWIDGET, 1, &_element);
}

HRESULT w_widget::render(_In_ const float pElapsedTime)
{
	if (this->get_is_released()) return S_FALSE;
	
	// See if the dialog needs to be refreshed
	if (this->time_last_refresh < s_time_refresh)
	{
		this->time_last_refresh = system::w_time_span::now();
		refresh();
	}

	// For invisible dialog, return now.
	if (!this->_visible || (this->_minimized && !this->_is_caption_enabled))
	{
		return S_OK;
	}

	//apply dragging
	if (this->_is_dragging)
	{
		this->_x = this->_global_mouse_point.x;
		this->_y = this->_global_mouse_point.y;
		
		//enable redraw
		this->_redrawing = true;
	}

    if (!this->_minimized)
    {
        if (this->_redrawing)
        {
            this->_vertex_declarations_2d.clear();

            w_color _corner_colors[4];
            if (this->_is_mouse_over_widget)
            {
                _corner_colors[0] = this->_active_background_color_top_left;
                _corner_colors[2] = this->_active_background_color_top_right;
                _corner_colors[1] = this->_active_background_color_bottom_left;
                _corner_colors[3] = this->_active_background_color_bottom_right;
            }
            else
            {
                _corner_colors[0] = this->_background_color_top_left;
                _corner_colors[2] = this->_background_color_top_right;
                _corner_colors[1] = this->_background_color_bottom_left;
                _corner_colors[3] = this->_background_color_bottom_right;
            }

            w_control::generate_2d_vertices(
                glm::vec2((float)this->_parent_window_width, (float)this->_parent_window_height),
                glm::vec2((float)this->_width, (float)this->_height),
                glm::vec2((float)this->_x, (float)this->_y),
                _corner_colors,
                this->_vertex_declarations_2d
            );
        }
    }
	
	// Render the caption if it's enabled.
	if (this->_is_caption_enabled)
	{
		//set the color of background caption
		//this->_caption_element.texture_color.current_color_state = DirectX::W_COLOR_TO_XMFLOAT4(
		//	this->_is_mouse_over_widget ? this->_caption_active_background_color : _caption_background_color);
		////set the index of this widget's texture to element
		//this->_caption_element.index_texture = this->_texture_cache_index;

		//RECT rc = { 0, -(LONG)this->_caption_height, (LONG)this->_width, 0 };
		//draw_sprite(&this->_caption_element, &rc, 0.99f);

		//wchar_t _caption_extended_msg[256];
		//wcscpy_s(_caption_extended_msg, 256, this->_caption);
		//if (this->_minimized)
		//{
		//	wcscat_s(_caption_extended_msg, 256, L"-Minimized");
		//}
		//draw_text(_caption_extended_msg,
		//	DirectX::XMFLOAT2(
		//	(float) (this->_x + this->_caption_margin_left),
		//		(float) (this->_y + this->_caption_margin_top)),
		//	this->_brush);
	}

	// If the dialog is minimized, skip rendering its controls.
	if (this->_minimized)
	{
		//minimize all gdi controls
		//for (auto it = this->_controls.cbegin(); it != this->_controls.cend(); ++it)
		//{
		//	if ((*it)->get_type() == W_GUI_CONTROL_TYPE::W_GUI_CONTROL_LISTWIDGET)
		//	{
		//		auto _list_widget = static_cast<w_list_widget*>(*it);
		//		if (_list_widget)
		//		{
		//			_list_widget->hide_gdi_controls();
		//		}
		//	}
		//}
	}
    else
    {
        if (this->_redrawing)
        {
            for (auto it = this->_controls.cbegin(); it != this->_controls.cend(); ++it)
            {
                // Focused control is drawn last
                if (*it == s_pControlFocus) continue;
                (*it)->render_2d(pElapsedTime, this->_vertex_declarations_2d);
            }

            if (s_pControlFocus && s_pControlFocus->get_parent_widget() == this)
            {
                s_pControlFocus->render_2d(pElapsedTime, this->_vertex_declarations_2d);
            }
        }
    }
		
	this->_redrawing = false;

	return S_OK;
}

void w_widget::remove_control(_In_ int pID)
{
	for (auto it = this->_controls.begin(); it != this->_controls.end(); ++it)
	{
		if ((*it)->get_ID() == pID)
		{
			// Clean focus first
			clear_focus();

			// Clear references to this control
			if (s_pControlFocus == (*it))
			{
				s_pControlFocus = nullptr;
			}
			if (s_pControlPressed == (*it))
			{
				s_pControlPressed = nullptr;
			}
			if (this->_control_mouse_over == (*it))
			{
				this->_control_mouse_over = nullptr;
			}
			SAFE_RELEASE((*it));
			this->_controls.erase(it);

			return;
		}
	}
}

void w_widget::remove_all_controls()
{
	if (s_pControlFocus && s_pControlFocus->get_parent_widget() == this)
	{
		s_pControlFocus = nullptr;
	}
	if (s_pControlPressed && s_pControlPressed->get_parent_widget() == this)
	{
		s_pControlPressed = nullptr;
	}
	this->_control_mouse_over = nullptr;

	for (auto it = this->_controls.begin(); it != this->_controls.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	this->_controls.clear();
}

void w_widget::refresh()
{
	if (s_pControlFocus)
	{
		s_pControlFocus->on_focus_out();
	}
	if (this->_control_mouse_over)
	{
		this->_control_mouse_over->on_mouse_leave();
	}
	s_pControlFocus = nullptr;
	s_pControlPressed = nullptr;
	this->_control_mouse_over = nullptr;

	for (auto it = this->_controls.begin(); it != this->_controls.end(); ++it)
	{
		(*it)->refresh();
	}
}

void w_widget::send_event(UINT pEvent, bool pTriggeredByUser, w_control* pControl)
{
	// If no callback has been registered there's nowhere to send the event to
	if (this->_call_back_events.size() == 0) return;

	// Discard events triggered programatically if these types of events haven't been
	// enabled
	if (!pTriggeredByUser && !this->non_user_events) return;

	for (size_t i = 0; i < this->_call_back_events.size(); ++i)
	{
		auto _event = this->_call_back_events.at(i);
		auto _context = this->_call_back_events_user_contexts.at(i);

		if (_event)
		{
			_event(pEvent, pControl->get_ID(), pControl, _context);
		}
	}
}

//HRESULT w_widget::add_label(_In_ int pID,
//	_In_z_ std::wstring pText,
//	_In_ int pX, _In_ int pY,
//	_In_ int pWidth, _In_ int pHeight,
//	_In_ bool pIsDefault,
//	_Out_opt_ w_label** pCreatedControl)
//{
//	auto _control = new (std::nothrow) w_label(this);
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = _control;
//	}
//
//	if (!_control) return E_OUTOFMEMORY;
//
//	// Set the ID and list index
//	_control->set_ID(pID);
//	_control->set_text(pText);
//	_control->set_position_2d(pX, pY);
//	_control->set_size(pWidth, pHeight);
//	_control->is_default = pIsDefault;
//
//	auto hr = add_control(_control);
//	if (FAILED(hr))
//	{
//		return hr;
//	}
//
//	return S_OK;
//}

HRESULT w_widget::add_image(_In_ int pID,
	_In_ int pX, _In_ int pY,
	_In_z_ std::wstring pPath,
	_In_ bool pAbsolutePath,
	_In_ float pRotationX, _In_ float pRotationY,
	_In_ bool pUseDefaultSize,
	_In_ UINT pWidth, _In_ UINT pHeight,
	_Out_opt_ w_image** pCreatedControl)
{
	auto _control = new (std::nothrow) w_image(this);

	if (pCreatedControl)
	{
		*pCreatedControl = _control;
	}

	if (!_control) return E_OUTOFMEMORY;

	//auto _path = pRelativePath ? wolf::framework::w_game::get_content_directory() + pPath : pPath;

	// Set the ID and list index
	_control->set_ID(pID);
	_control->set_path(pPath, pAbsolutePath);
	_control->set_position_2d(pX, pY);
	_control->set_use_default_size(pUseDefaultSize);
	_control->set_size(pWidth, pHeight);
	_control->set_rotation(glm::vec2(pRotationX, pRotationY));

	return add_control(_control);
}

//HRESULT w_widget::add_button(_In_ int pID,
//	_In_z_ std::wstring pText,
//	_In_ int pX, _In_ int pY,
//	_In_ UINT pWidth, _In_ UINT pHeight,
//	_In_z_ std::wstring pIconPath,
//	_In_ bool pIconAbsolutePath,
//	_In_ int pTextOffsetX, _In_ int pTextOffsetY,
//	_In_ int pIconOffsetX, _In_ int pIconOffsetY,
//	_In_ bool pIconUseDefaultSize,
//	_In_ UINT pIconWidth, _In_ UINT pIconHeight,
//	_In_ UINT pHotkey,
//	_In_ bool pIsDefault,
//	_Out_opt_ w_button** pCreatedControl)
//{
//	return add_button(
//		pID, 
//		pText, 
//		pWidth, pHeight,
//		pX, pY, 
//		pIconPath, 
//		pIconAbsolutePath,
//		pTextOffsetX, pTextOffsetY, 
//		pIconOffsetX, pIconOffsetY,
//		pIconUseDefaultSize,
//		pIconWidth, pIconHeight,
//		pHotkey,
//		pIsDefault,
//		w_color(255, 255, 255, 255),
//		w_color(255, 255, 255, 160),
//		w_color(255, 255, 255, 200),
//		w_color(255, 255, 255, 30),
//		w_color(100, 100, 100, 30),
//		w_color(255, 255, 255, 255),
//		w_color(0, 0, 0, 255),
//		w_color(0, 0, 0, 255),
//		w_color(255, 255, 255, 255),
//		w_color(100, 100, 100, 30),
//		0.0f, 1.0f,
//		pCreatedControl);
//}
//
//HRESULT w_widget::add_button(_In_ int pID,
//	_In_z_ std::wstring pText,
//	_In_ int pX, _In_ int pY,
//	_In_ UINT pWidth, _In_ UINT pHeight,
//	_In_z_ std::wstring pIconPath,
//	_In_ bool pIconAbsolutePath,
//	_In_ int pTextOffsetX, int pTextOffsetY,
//	_In_ int pIconOffsetX, int pIconOffsetY,
//	_In_ bool pIconUseDefaultSize,
//	_In_ UINT pIconWidth, _In_ UINT pIconHeight,
//	_In_ UINT pHotkey,
//	_In_ bool pIsDefault,
//	_In_ w_color pColor, _In_ w_color pMouseOverColor, _In_ w_color pPressedColor, _In_ w_color pFocusedColor, _In_ w_color pDisabledColor,
//	_In_ w_color pTextColor, _In_ w_color pTextMouseOverColor,_In_ w_color pTextPressedColor, _In_ w_color pTextFocusedColor, _In_ w_color pTextDisabledColor,
//	_In_ float pEffectMinValue, _In_ float pEffectMaxValue,
//	_Out_opt_ w_button** pCreatedControl)
//{
//	auto _control = new (std::nothrow) w_button(this);
//	if (!_control) return E_OUTOFMEMORY;
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = _control;
//	}
//
//	// Set the ID and list index
//	_control->set_ID(pID);
//	_control->set_position_2d(pX, pY);
//	_control->set_size(pWidth, pHeight);
//	_control->set_text(pText);
//	
//	_control->set_original_text_offset_x(pTextOffsetX);
//	_control->set_original_text_offset_y(pTextOffsetY);
//	
//	_control->set_icon_offset(pIconOffsetX, pIconOffsetY);
//	_control->set_use_default_icon_size(pIconUseDefaultSize);
//	_control->set_icon_size(pIconWidth, pIconHeight);
//	
//	_control->set_button_color(pColor);
//	_control->set_button_mouse_over_color(pMouseOverColor);
//	_control->set_button_pressed_color(pPressedColor);
//	_control->set_button_focused_color(pFocusedColor);
//	_control->set_button_disabled_color(pDisabledColor);
//	
//	_control->set_label_color(pTextColor);
//	_control->set_label_mouse_over_color(pTextMouseOverColor);
//	_control->set_label_pressed_color(pTextPressedColor);
//	_control->set_label_focused_color(pTextFocusedColor);
//	_control->set_label_disabled_color(pTextDisabledColor);
//
//	_control->set_effect_min_max_range(pEffectMinValue, pEffectMaxValue);
//
//	_control->set_hot_key(pHotkey);
//	_control->is_default = pIsDefault;
//
//	if (!pIconPath.empty())
//	{
//		//auto _icon_path = pRelativePath ? wolf::framework::w_game::get_content_directory() +  pIconPath : pIconPath;
//		_control->set_icon_path(pIconPath, pIconAbsolutePath);
//	}
//
//	auto hr = add_control(_control);
//	if (FAILED(hr)) return hr;
//
//	return S_OK;
//}
//
//HRESULT w_widget::add_check_box(_In_ int pID,
//	_In_z_ std::wstring pText, 
//	_In_ int pX, _In_ int pY,
//	_In_ UINT pWidth, _In_ UINT pHeight,
//	_In_ bool pChecked,
//	_In_ int pTextOffsetX, _In_ int pTextOffsetY,
//	_In_ UINT pHotkey,
//	_In_ bool pIsDefault,
//	_In_ w_color pColor, _In_ w_color pMouseOverColor, _In_ w_color pPressedColor, _In_ w_color pFocusedColor, _In_ w_color pDisabledColor,
//	_Out_opt_ w_check_box** pCreatedControl)
//{
//	auto _control = new (std::nothrow) w_check_box(this);
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = _control;
//	}
//
//	if (!_control) return E_OUTOFMEMORY;
//
//	// Set the ID and list index
//	_control->set_ID(pID);
//	_control->set_text(pText);
//	_control->set_position_2d(pX, pY);
//	_control->set_size(pWidth, pHeight);
//	_control->set_checked(pChecked);
//	_control->set_original_text_offset_x(pTextOffsetX);
//	_control->set_original_text_offset_y(pTextOffsetY);
//	_control->set_hot_key(pHotkey);
//
//	_control->set_check_box_color(pColor);
//	_control->set_check_box_mouse_over_color(pMouseOverColor);
//	_control->set_check_box_pressed_color(pPressedColor);
//	_control->set_check_box_focused_color(pFocusedColor);
//	_control->set_check_box_disabled_color(pDisabledColor);
//
//	_control->is_default = pIsDefault;
//
//	auto hr = add_control(_control);
//	if (FAILED(hr)) return hr;
//
//	return S_OK;
//}
//
//HRESULT w_widget::add_radio_button(_In_ int pID,
//	_In_ UINT pButtonGroup,
//	_In_z_ std::wstring pText,
//	_In_ int pX, _In_ int pY,
//	_In_ UINT pWidth, _In_ UINT pHeight,
//	_In_ bool pChecked,
//	_In_ int pTextOffsetX, _In_ int pTextOffsetY,
//	_In_ UINT pHotkey,
//	_In_ bool pIsDefault,
//	_In_ w_color pColor, _In_ w_color pMouseOverColor, _In_ w_color pPressedColor, _In_ w_color pFocusedColor, _In_ w_color pDisabledColor,
//	_Inout_ w_radio_button** pCreatedControl)
//{
//	auto _control = new (std::nothrow) w_radio_button(this);
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = _control;
//	}
//	if (!_control) return E_OUTOFMEMORY;
//
//	// Set the ID and list index
//	_control->set_ID(pID);
//	_control->set_text(pText);
//	_control->set_button_group(pButtonGroup);
//	_control->set_position_2d(pX, pY);
//	_control->set_size(pWidth, pHeight);
//	_control->set_checked(pChecked);
//	_control->set_original_text_offset_x(pTextOffsetX);
//	_control->set_original_text_offset_y(pTextOffsetY);
//	_control->set_hot_key(pHotkey);
//
//	_control->set_check_box_color(pColor);
//	_control->set_check_box_mouse_over_color(pMouseOverColor);
//	_control->set_check_box_pressed_color(pPressedColor);
//	_control->set_check_box_focused_color(pFocusedColor);
//	_control->set_check_box_disabled_color(pDisabledColor);
//
//	_control->is_default = pIsDefault;
//
//	auto hr = add_control(_control);
//	if (FAILED(hr)) return hr;
//
//	return S_OK;
//}
//
//DX_EXP HRESULT w_widget::add_combo_box(_In_ int pID,
//	_In_ int pX, _In_ int pY,
//	_In_ UINT pWidth, _In_ UINT pHeight,
//	_In_ int pTextOffsetX, _In_ int pTextOffsetY,
//	_In_ UINT pHotkey,
//	_In_ bool pIsDefault,
//	_Inout_ w_combo_box** pCreatedControl)
//{
//	return add_combo_box(pID,
//		pX, pY,
//		pWidth, pHeight,
//		pTextOffsetX, pTextOffsetY,
//		pHotkey,
//		pIsDefault,
//		w_color(200, 200, 200, 255),
//		w_color(255, 255, 0, 255),
//		w_color(255, 255, 255, 200),
//		w_color(255, 255, 0, 170),
//		w_color(50, 50, 50, 150),
//		w_color(255, 255, 255, 255),
//		w_color(255, 255, 0, 255),
//		w_color(150, 150, 150, 255),
//		w_color(255, 255, 255, 200),
//		w_color(50, 50, 50, 150),
//		w_color(255, 255, 255, 255),
//		w_color(255, 0, 0, 155),
//		w_color(255, 255, 255, 255),
//		pCreatedControl);
//}
//
//HRESULT w_widget::add_combo_box(_In_ int pID,
//	_In_ int pX, _In_ int pY,
//	_In_ UINT pWidth, _In_ UINT pHeight,
//	_In_ int pTextOffsetX, _In_ int pTextOffsetY,
//	_In_ UINT pHotkey,
//	_In_ bool pIsDefault,
//	_In_ w_color pColor, _In_ w_color pMouseOverColor, _In_ w_color pPressedColor, _In_ w_color pFocusedColor, _In_ w_color pDisabledColor,
//	_In_ w_color pButtonColor, _In_ w_color pButtonMouseOverColor, _In_ w_color pButtonPressedColor, _In_ w_color pButtonFocusedColor, _In_ w_color pButtonDisabledColor,
//	_In_ w_color pDropDownMainColor,
//	_In_ w_color pSelectionColor, _In_ w_color pSelectionFontColor,
//	_Inout_ w_combo_box** pCreatedControl)
//{
//	auto _control = new (std::nothrow) w_combo_box(this);
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = _control;
//	}
//	if (!_control) return E_OUTOFMEMORY;
//
//	// Set the ID and list index
//	_control->set_ID(pID);
//
//	auto _hr = add_control(_control);
//	if (FAILED(_hr)) return _hr;
//
//	_control->set_position_2d(pX, pY);
//	_control->set_size(pWidth, pHeight);
//	_control->set_original_text_offset_x(pTextOffsetX);
//	_control->set_original_text_offset_y(pTextOffsetY);
//	_control->set_hot_key(pHotkey);
//	_control->is_default = pIsDefault;
//	_control->set_font_size(this->_sprite_batch->get_debug_font_size());
//
//	//set comboBox colors
//	_control->set_combo_box_color(pColor);
//	_control->set_combo_box_mouse_over_color(pMouseOverColor);
//	_control->set_combo_box_pressed_color(pPressedColor);
//	_control->set_combo_box_focused_color(pFocusedColor);
//	_control->set_combo_box_disabled_color(pDisabledColor);
//
//	//set comboBox button color
//	_control->set_combo_box_button_color(pButtonColor);
//	_control->set_combo_box_button_mouse_over_color(pButtonMouseOverColor);
//	_control->set_combo_box_button_pressed_color(pButtonPressedColor);
//	_control->set_combo_box_button_focused_color(pButtonFocusedColor);
//	_control->set_combo_box_button_disabled_color(pButtonDisabledColor);
//
//	//set comboBox drop-down color
//	_control->set_combo_box_drop_down_context_color(pDropDownMainColor);
//
//	//set comboBox selection color
//	_control->set_combo_box_selected_color(pSelectionColor);
//	_control->set_combo_box_selection_font_color(pSelectionFontColor);
//
//	return S_OK;
//}
//
//HRESULT w_widget::add_slider(_In_ int pID,
//	_In_ int pX, _In_ int pY,
//	_In_ UINT pWidth, _In_ UINT pHeight,
//	_In_ int pMin, _In_ int pMax,
//	_In_ int pValue,
//	_In_ UINT pHotkey,
//	_In_ bool pIsDefault,
//	_In_ w_color pMainColor, _In_ w_color pMainMouseOverColor, _In_ w_color pMainPressedColor, _In_ w_color pMainFocusedColor, _In_ w_color pMainDisabledColor,
//	_In_ w_color pButtonColor, _In_ w_color pButtonMouseOverColor, _In_ w_color pButtonPressedColor, _In_ w_color pButtonFocusedColor, _In_ w_color pButtonDisabledColor,
//	_Out_opt_ w_slider** pCreatedControl)
//{
//	auto _control = new (std::nothrow) w_slider(this);
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = _control;
//	}
//	if (!_control) return E_OUTOFMEMORY;
//
//	// Set the ID and list index
//	_control->set_ID(pID);
//	_control->set_position_2d(pX, pY);
//	_control->set_size(pWidth, pHeight);
//	_control->set_range(pMin, pMax);
//	_control->set_value(pValue);
//	_control->set_hot_key(pHotkey);
//
//	_control->set_main_color(pMainColor);
//	_control->set_main_mouse_over_color(pMainMouseOverColor);
//	_control->set_main_pressed_color(pMainPressedColor);
//	_control->set_main_focused_color(pMainFocusedColor);
//	_control->set_main_disabled_color(pMainDisabledColor);
//
//	_control->set_button_color(pButtonColor);
//	_control->set_button_mouse_over_color(pButtonMouseOverColor);
//	_control->set_button_pressed_color(pButtonPressedColor);
//	_control->set_button_focused_color(pButtonFocusedColor);
//	_control->set_button_disabled_color(pButtonDisabledColor);
//
//	_control->is_default = pIsDefault;
//
//	auto _hr = add_control(_control);
//	if (FAILED(_hr)) return _hr;
//
//	_control->update_rects();
//
//	return S_OK;
//}
//
//HRESULT w_widget::add_list_box(_In_ int pID,
//	_In_ int pX, _In_ int pY,
//	_In_ UINT pWidth, _In_ UINT pHeight,
//	_In_ bool pMultiSelection,
//	_Out_opt_ w_list_box** pCreatedControl)
//{
//	auto _control = new (std::nothrow) w_list_box(this);
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = _control;
//	}
//
//	if (!_control) return E_OUTOFMEMORY;
//
//	// Set the ID and position
//	_control->set_ID(pID);
//
//	auto hr = add_control(_control);
//	if (FAILED(hr)) return hr;
//
//	_control->set_size(pWidth, pHeight);
//	_control->set_position_2d(pX, pY);
//	_control->set_multi_selection(pMultiSelection);
//
//	return S_OK;
//}
//
//HRESULT w_widget::add_tree_list(_In_ int pID,
//	_In_ int pX, _In_ int pY,
//	_In_ UINT pWidth, _In_ UINT pHeight,
//	_In_ bool pMultiSelection,
//	_Out_opt_ w_tree_list** pCreatedControl)
//{
//	auto _control = new (std::nothrow) w_tree_list(this);
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = _control;
//	}
//
//	if (!_control) return E_OUTOFMEMORY;
//
//	// Set the ID and position
//	_control->set_ID(pID);
//
//	auto hr = add_control(_control);
//	if (FAILED(hr)) return hr;
//
//	_control->set_size(pWidth, pHeight);
//	_control->set_position_2d(pX, pY);
//	_control->set_multi_selection(pMultiSelection);
//
//	return S_OK;
//}
//
//HRESULT w_widget::add_list_widget(_In_ int pID,
//	_In_ int pX, _In_ int pY,
//	_In_ UINT pWidth, _In_ UINT pHeight,
//	_In_ bool pMultiSelection,
//	_Out_opt_ w_list_widget** pCreatedControl)
//{
//	auto _control = new (std::nothrow) w_list_widget(this);
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = _control;
//	}
//
//	if (!_control) return E_OUTOFMEMORY;
//
//	// Set the ID and position
//	_control->set_ID(pID);
//
//	auto hr = add_control(_control);
//	if (FAILED(hr)) return hr;
//
//	_control->set_size(pWidth, pHeight);
//	_control->set_position_2d(pX, pY);
//	_control->set_multi_selection(pMultiSelection);
//
//	return S_OK;
//}
//
//HRESULT w_widget::add_tab(_In_ int pID,
//	_In_ int pX, _In_ int pY,
//	_In_ UINT pWidth, _In_ UINT pHeight,
//	_In_ UINT pTabButtonWidth, _In_ UINT pTabButtonHeight,
//	_In_ int pArrowButtonTextOffsetX, _In_ int pArrowButtonTextOffsetY,
//	_Out_opt_ w_tab** pCreatedControl)
//{
//	auto _control = new (std::nothrow) w_tab(this);
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = _control;
//	}
//
//	if (!_control) return E_OUTOFMEMORY;
//
//	// Set the ID and position
//	_control->set_ID(pID);
//
//	auto hr = add_control(_control);
//	if (FAILED(hr)) return hr;
//
//	_control->set_size(pWidth, pHeight);
//	_control->set_button_size(pTabButtonWidth, pTabButtonHeight);
//	_control->set_button_text_offset(pArrowButtonTextOffsetX, pArrowButtonTextOffsetY);
//	_control->set_position_2d(pX, pY);
//
//	return S_OK;
//}
//
//HRESULT w_widget::add_line_shape(_In_ int pID,
//	_In_ int pStartX, _In_ int pStartY,
//	_In_ int pStopX, _In_ int pStopY,
//	_In_ UINT pStrokeWidth,
//	_In_ bool pIsDefault,
//	_In_ w_color pColor,
//	_In_ w_color pDisabledColor,
//	_Out_opt_ w_line_shape** pCreatedControl)
//{
//	auto _control = new (std::nothrow) w_line_shape(this);
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = _control;
//	}
//
//	if (!_control) return E_OUTOFMEMORY;
//
//	// Set the ID and position
//	_control->set_ID(pID);
//	_control->set_color(pColor);
//	_control->set_stroke_width(pStrokeWidth);
//	_control->set_disabled_color(pDisabledColor);
//
//	auto hr = add_control(_control);
//	if (FAILED(hr)) return hr;
//
//	_control->set_geormetry((float) pStartX, (float) pStartY, (float) pStopX, (float) pStopY);
//
//	return S_OK;
//}
//
//HRESULT w_widget::add_rounded_rectangle_shape(_In_ int pID,
//	_In_ int pX, _In_ int pY,
//	_In_ UINT pWidth, _In_ UINT pHeight,
//	_In_ float pRadiusX, _In_ float pRadiusY,
//	_In_ float pStrokeWidth,
//	_In_ bool pIsDefault,
//	_In_ w_color pFillColor,
//	_In_ w_color pBorderColor,
//	_In_ w_color pMouseOverColor,
//	_In_ w_color pDisabledColor,
//	_Out_opt_ w_rounded_rectangle_shape** pCreatedControl )
//{
//	auto _control = new (std::nothrow) w_rounded_rectangle_shape(this);
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = _control;
//	}
//
//	if (!_control) return E_OUTOFMEMORY;
//
//	// Set the ID and position
//	_control->set_ID(pID);
//	_control->set_stroke_width(pStrokeWidth);
//	_control->set_fill_color(pFillColor);
//	_control->set_border_color(pBorderColor);
//	_control->set_mouse_over_color(pMouseOverColor);
//	_control->set_disabled_color(pDisabledColor);
//
//	auto hr = add_control(_control);
//	if (FAILED(hr)) return hr;
//
//	_control->set_geormetry((float)pX, (float) pY, (float) pWidth, (float) pHeight, pRadiusX, pRadiusY);
//
//	return S_OK;
//}
//
//HRESULT w_widget::add_ellipse_shape(_In_ int pID,
//	_In_ float pCenterX, _In_ float pCenterY,
//	_In_ float pRadiusX, _In_ float pRadiusY,
//	_In_ UINT pStrokeWidth,
//	_In_ bool pIsDefault,
//	_In_ w_color pFillColor,
//	_In_ w_color pBorderColor,
//	_In_ w_color pMouseOverColor,
//	_In_ w_color pDisabledColor,
//	_Out_opt_ w_ellipse_shape** pCreatedControl)
//{
//	auto _control = new (std::nothrow) w_ellipse_shape(this);
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = _control;
//	}
//
//	if (!_control) return E_OUTOFMEMORY;
//
//	// Set the ID and position
//	_control->set_ID(pID);
//	_control->set_stroke_width(pStrokeWidth);
//	_control->set_fill_color(pFillColor);
//	_control->set_border_color(pBorderColor);
//	_control->set_mouse_over_color(pMouseOverColor);
//	_control->set_disabled_color(pDisabledColor);
//
//	auto hr = add_control(_control);
//	if (FAILED(hr)) return hr;
//
//	_control->set_geormetry((float) pCenterX, (float) pCenterY, (float) pRadiusX, (float) pRadiusY);
//
//	return S_OK;
//}
//
//HRESULT w_widget::add_image_to_resource_manager(_In_z_ const wchar_t* pPath)
//{
//	return this->_sprite_batch->add_image_from_file(pPath);
//}

#ifdef __WIN32
bool w_widget::on_msg_proc(_In_ const HWND pHWnd, _In_ const UINT pMsg, _In_ const WPARAM pWParam, _In_ const LPARAM pLParam)
{
	bool bHandled = false;

	// For invisible dialog, do not handle anything.
	if (!this->_visible || !this->_enabled) return false;

	// If automation command-line switch is on, enable this dialog's keyboard input
	// upon any key press or mouse click.
	if (/*DXUTGetAutomation() &&*/
		(WM_LBUTTONDOWN == pMsg || WM_LBUTTONDBLCLK == pMsg || WM_KEYDOWN == pMsg))
	{
		//this->_widgets_resource_manager->enable_keyboard_input_for_all_widgets();
	}

	// If caption is enable, check for clicks in the caption area.
	if (this->_is_caption_enabled)
	{
		POINT _mouse_point =
		{
			short(LOWORD(pLParam)), short(HIWORD(pLParam))
		};

		if (pMsg == WM_LBUTTONDOWN)
		{
			wolf::system::w_timer_callback _tc;
			_tc.do_async(GetDoubleClickTime() / 2, [this, _mouse_point]()
			{
				if (!this->_is_double_click)
				{
					//check for dragging
					if (this->get_is_dragable() &&
						_mouse_point.x >= this->_x && _mouse_point.x < this->_x + this->_width &&
						_mouse_point.y >= this->_y && _mouse_point.y < this->_y + this->_caption_height)
					{
						this->_is_dragging = true;
						//SetCapture(this->_hwnd);
					}
				}
				this->_is_double_click = false;
			});
		}
		else if (pMsg == WM_LBUTTONDBLCLK)
		{
			this->_is_double_click = true;
			if (_mouse_point.x >= this->_x && _mouse_point.x < this->_x + this->_width &&
				_mouse_point.y >= this->_y && _mouse_point.y < this->_y + this->_caption_height)
			{
				this->_minimized = !this->_minimized;
			}
			this->_is_dragging = false;
		}
		else if (pMsg == WM_LBUTTONUP)
		{
			//this->_is_double_click = false;
			if (this->_is_dragging)
			{
				if (_mouse_point.x >= this->_x && _mouse_point.x < this->_x + this->_width &&
					_mouse_point.y >= this->_y && _mouse_point.y < this->_y + this->_caption_height)
				{
					ReleaseCapture();
					this->_is_dragging = false;
					return true;
				}
			}
		}
	}

	// If the dialog is minimized, don't send any messages to controls.
	if (this->_minimized) return false;

	// If a control is in focus, it belongs to this dialog, and it's enabled, then give
	// it the first chance at handling the message.
	if (s_pControlFocus && s_pControlFocus->get_parent_widget() == this && s_pControlFocus->get_enabled())
	{
		// If the control MsgProc handles it, then we don't.
		if (s_pControlFocus->on_msg_proc(pMsg, pWParam, pLParam)) return true;
	}

	switch (pMsg)
	{
	case WM_SIZE:
	case WM_MOVE:
	{
		// Handle sizing and moving messages so that in case the mouse cursor is moved out
		// of an UI control because of the window adjustment, we can properly
		// unhighlight the highlighted control.
		w_point pt =
		{
			-1, -1
		};
		on_mouse_move(pt);
		break;
	}
	case WM_ACTIVATEAPP:
		/*
		Call on_focus_in()/on_focus_out() of the control that currently has the focus
		as the application is activated/deactivated.  This matches the Windows behavior.
		*/
		if (s_pControlFocus && s_pControlFocus->get_parent_widget() == this && s_pControlFocus->get_enabled())
		{
			if (pWParam)
			{
				s_pControlFocus->on_focus_in();
			}
			else
			{
				s_pControlFocus->on_focus_out();
			}
		}
		break;

		// Keyboard messages
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		/*
			If a control is in focus, it belongs to this dialog, and it's enabled, then give
			it the first chance at handling the message.
		*/
		if (s_pControlFocus && s_pControlFocus->get_parent_widget() == this && s_pControlFocus->get_enabled())
		{
			for (auto it = this->_controls.cbegin(); it != this->_controls.cend(); ++it)
			{
				if (s_pControlFocus->handle_keyboard(pMsg, pWParam, pLParam))
				{
					return true;
				}
			}
		}
		/*
			Not yet handled, see if this matches a control's hotkey
			Activate the hotkey if the focus doesn't belong to an
			edit box.
		*/
		//if (pMsg == WM_KEYDOWN && (!s_pControlFocus ||
		//	(s_pControlFocus->get_type() != W_GUI_CONTROL_EDITBOX
		//		&& s_pControlFocus->get_type() != W_GUI_CONTROL_IMEEDITBOX)))
		//{
		//	for (auto it = this->_controls.begin(); it != this->_controls.end(); ++it)
		//	{
		//		if ((*it)->get_hot_key() == pWParam)
		//		{
		//			(*it)->on_hot_key();
		//			return true;
		//		}
		//	}
		//}

		// Not yet handled, check for focus messages
		if (pMsg == WM_KEYDOWN)
		{
			// If keyboard input is not enabled, this message should be ignored
			if (!this->keyboard_input) return false;

			switch (pWParam)
			{
			case VK_RIGHT:
			case VK_DOWN:
				if (s_pControlFocus)
				{
					return on_cycle_focus(true);
				}
				break;

			case VK_LEFT:
			case VK_UP:
				if (s_pControlFocus)
				{
					return on_cycle_focus(false);
				}
				break;

			case VK_TAB:
			{
				bool bShiftDown = ((GetKeyState(VK_SHIFT) & 0x8000) != 0);
				return on_cycle_focus(!bShiftDown);
			}
			}
		}

		break;
	}

	// Mouse messages
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
	{
		// If not accepting mouse input, return false to indicate the message should still be handled by the application (usually to move the camera).
		if (!this->mouse_input) return false;

		this->_global_mouse_point.x = short(LOWORD(pLParam));
		this->_global_mouse_point.y = short(HIWORD(pLParam));

		w_point _local_mouse_point = this->_global_mouse_point;
		_local_mouse_point.x -= this->_x;
		_local_mouse_point.y -= this->_y;

		// If caption is enabled, offset the Y coordinate by the negative of its height.
		if (this->_is_caption_enabled)
		{
			_local_mouse_point.y -= this->_caption_height;
		}

		//If a control is in focus, it belongs to this dialog, and it's enabled, then give it the first chance at handling the message.
		if (s_pControlFocus &&
			s_pControlFocus->get_parent_widget() == this &&
			s_pControlFocus->get_enabled())
		{
			if (s_pControlFocus->handle_mouse(pMsg, _local_mouse_point, pWParam, pLParam))
			{
				return true;
			}
		}

		// Not yet handled, see if the mouse is over any controls
		auto pControl = get_control_at_point(_local_mouse_point);
		if (pControl && pControl->get_enabled())
		{
			bHandled = pControl->handle_mouse(pMsg, _local_mouse_point, pWParam, pLParam);
			if (bHandled)
			{
				return true;
			}
		}
		else
		{
			// Mouse not over any controls in this dialog, if there was a control which had focus it just lost it
			if (pMsg == WM_LBUTTONDOWN && s_pControlFocus && s_pControlFocus->get_parent_widget() == this)
			{
				s_pControlFocus->on_focus_out();
				s_pControlFocus = nullptr;
			}
		}

		// Still not handled, hand this off to the dialog. Return false to indicate the
		// message should still be handled by the application (usually to move the camera).
		switch (pMsg)
		{
		case WM_MOUSEMOVE:
			on_mouse_move(_local_mouse_point);
			return false;
		}

		break;
	}

	case WM_CAPTURECHANGED:
	{
		// The application has lost mouse capture.
		// The dialog object may not have received
		// a WM_MOUSEUP when capture changed. Reset
		// m_bDrag so that the dialog does not mistakenly
		// think the mouse button is still held down.
		//if ((HWND) pLParam != this->_hwnd)
		//{
		//	this->_is_dragging = false;
		//}
	}
	}

	return false;
}
#endif

void w_widget::on_mouse_up(_In_ const w_point& pPoint)
{
	W_UNUSED(pPoint);
	s_pControlPressed = nullptr;
	this->_control_mouse_over = nullptr;
	this->_is_mouse_over_widget = false;
}

void w_widget::on_mouse_move(_In_ const w_point& pPoint)
{
	// Figure out which control the mouse is over now
	auto pControl = get_control_at_point(pPoint);

	bool _mouse_was_in = false;
	if (this->_is_mouse_over_widget)
	{
		_mouse_was_in = true;
	}

	this->_is_mouse_over_widget = false;
	if (pPoint.x >= 0 && pPoint.x <= this->_width &&
		pPoint.y >= 0 && pPoint.y <= this->_height)
	{
		this->_is_mouse_over_widget = true;

		//rise the mouse move
		if (on_mouse_move_callback)
		{
			on_mouse_move_callback(pPoint);
		}

		//rise the mouse enter
		if (!_mouse_was_in)
		{
			this->_redrawing = true;
			if (on_mouse_enter_callback)
			{
				on_mouse_enter_callback(pPoint);
			}
		}
	}

	//if mouse is not over widget
	if (!_is_mouse_over_widget && _mouse_was_in)
	{
		this->_redrawing = true;
		if (on_mouse_leave_callback)
		{
			on_mouse_leave_callback(pPoint);
		}
	}
	
	// If the mouse is still over the same control, nothing needs to be done
	if (pControl == this->_control_mouse_over)
	{
		return;
	}
	// Handle mouse leaving the old control
	if (this->_control_mouse_over)
	{
		this->_control_mouse_over->on_mouse_leave();
	}
	// Handle mouse entering the new control
	this->_control_mouse_over = pControl;
	if (pControl)
	{
		this->_control_mouse_over->on_mouse_enter();
	}
}

HRESULT w_widget::initialize_control(_In_ w_control* pControl)
{
	if (!pControl)
	{
		return E_INVALIDARG;
	}
	pControl->set_index(static_cast<UINT>(this->_controls.size()));

	// Look for a default Element entries
	for (auto it = this->_default_elements.begin(); it != this->_default_elements.end(); ++it)
	{
		if ((*it)->control_type == pControl->get_type())
		{
			pControl->set_element((*it)->index_element, &(*it)->element);
		}
	}

	return S_OK;
}

HRESULT w_widget::add_control(_In_ w_control* pControl)
{
	auto _hr = initialize_control(pControl);
	V(_hr, L"initializing control", _super::name, 2);

	// Add to the list
	this->_controls.push_back(pControl);

	return S_OK;
}

//void w_widget::clear_radioButton_group(_In_ UINT pButtonGroup)
//{
//	// Find all radio buttons with the given group number
//	for (auto it = this->_controls.cbegin(); it != this->_controls.cend(); ++it)
//	{
//		////////////////////////////////if ((*it)->get_type() == W_GUI_CONTROL_RADIOBUTTON)
//		////////////////////////////////{
//		////////////////////////////////	auto pRadioButton = (w_radioButton*) *it;
//
//		////////////////////////////////	if (pRadioButton->get_button_group() == pButtonGroup)
//		////////////////////////////////	{
//		////////////////////////////////		pRadioButton->set_checked(false, false);
//		////////////////////////////////	}
//		////////////////////////////////}
//	}
//}

//void w_widget::clear_comboBox(_In_ int pID)
//{
//	////////////////////////////////auto _comboBox = reinterpret_cast<w_comboBox*>(get_control(pID, W_GUI_CONTROL_COMBOBOX));
//	////////////////////////////////if (!_comboBox)	return;
//	////////////////////////////////_comboBox->remove_all_items();
//}

void w_widget::request_focus(_In_ w_control* pControl)
{
    if (s_pControlFocus == pControl) return;

    if (!pControl->can_have_focus()) return;

    if (s_pControlFocus)
    {
        s_pControlFocus->on_focus_out();
    }
    pControl->on_focus_in();
    s_pControlFocus = pControl;
}

//HRESULT w_widget::draw_sprite(w_element* pElement, const RECT* pRectDest, float pDepth)
//{
//	// No need to draw fully transparent layers
//	if (pElement->texture_color.current_color_state.w == 0) return S_OK;
//
//	RECT _rectangle_texture = pElement->rect_texture;
//
//	RECT rcScreen = *pRectDest;
//	OffsetRect(&rcScreen, this->_x, this->_y);
//
//	// If caption is enabled, offset the Y position by its height.
//	if (this->_is_caption_enabled)
//	{
//		OffsetRect(&rcScreen, 0, this->_caption_height);
//	}
//	auto _texture_node = get_texture(this->_texture_cache_index);
//	if (!_texture_node) return E_FAIL;
//
//	float fBBWidth = (float)this->_widgets_resource_manager->back_buffer_width;
//	float fBBHeight = (float)this->_widgets_resource_manager->back_buffer_height;
//	float fTexWidth = (float) _texture_node->width;
//	float fTexHeight = (float) _texture_node->height;
//
//	float fRectLeft = rcScreen.left / fBBWidth;
//	float fRectTop = 1.0f - rcScreen.top / fBBHeight;
//	float fRectRight = rcScreen.right / fBBWidth;
//	float fRectBottom = 1.0f - rcScreen.bottom / fBBHeight;
//
//	fRectLeft = fRectLeft * 2.0f - 1.0f;
//	fRectTop = fRectTop * 2.0f - 1.0f;
//	fRectRight = fRectRight * 2.0f - 1.0f;
//	fRectBottom = fRectBottom * 2.0f - 1.0f;
//
//	float fTexLeft = _rectangle_texture.left / fTexWidth;
//	float fTexTop = _rectangle_texture.top / fTexHeight;
//	float fTexRight = _rectangle_texture.right / fTexWidth;
//	float fTexBottom = _rectangle_texture.bottom / fTexHeight;
//
//	// Add 6 sprite vertices
//	wolf::content_pipeline::vertex_declaration_structs::vertex_color_uv _sprite_vertex;
//
//	// tri1
//	_sprite_vertex.position = glm::vec3(fRectLeft, fRectTop, pDepth);
//	_sprite_vertex.uv = glm::vec2(fTexLeft, fTexTop);
//	_sprite_vertex.color.x = pElement->texture_color.current_color_state.x;
//	_sprite_vertex.color.y = pElement->texture_color.current_color_state.y;
//	_sprite_vertex.color.z = pElement->texture_color.current_color_state.z;
//	_sprite_vertex.color.w = pElement->texture_color.current_color_state.w;
//
//	this->_widgets_resource_manager->m_SpriteVertices.push_back(_sprite_vertex);
//
//	_sprite_vertex.position = glm::vec3(fRectRight, fRectTop, pDepth);
//	_sprite_vertex.uv = glm::vec2(fTexRight, fTexTop);
//	_sprite_vertex.color.x = pElement->texture_color.current_color_state.x;
//	_sprite_vertex.color.y = pElement->texture_color.current_color_state.y;
//	_sprite_vertex.color.z = pElement->texture_color.current_color_state.z;
//	_sprite_vertex.color.w = pElement->texture_color.current_color_state.w;
//
//	this->_widgets_resource_manager->m_SpriteVertices.push_back(_sprite_vertex);
//
//	_sprite_vertex.position = glm::vec3(fRectLeft, fRectBottom, pDepth);
//	_sprite_vertex.uv = glm::vec2(fTexLeft, fTexBottom);
//	_sprite_vertex.color.x = pElement->texture_color.current_color_state.x;
//	_sprite_vertex.color.y = pElement->texture_color.current_color_state.y;
//	_sprite_vertex.color.z = pElement->texture_color.current_color_state.z;
//	_sprite_vertex.color.w = pElement->texture_color.current_color_state.w;
//
//	this->_widgets_resource_manager->m_SpriteVertices.push_back(_sprite_vertex);
//
//	// tri2
//	_sprite_vertex.position = glm::vec3(fRectRight, fRectTop, pDepth);
//	_sprite_vertex.uv = glm::vec2(fTexRight, fTexTop);
//	_sprite_vertex.color.x = pElement->texture_color.current_color_state.x;
//	_sprite_vertex.color.y = pElement->texture_color.current_color_state.y;
//	_sprite_vertex.color.z = pElement->texture_color.current_color_state.z;
//	_sprite_vertex.color.w = pElement->texture_color.current_color_state.w;
//
//	this->_widgets_resource_manager->m_SpriteVertices.push_back(_sprite_vertex);
//
//	_sprite_vertex.position = glm::vec3(fRectRight, fRectBottom, pDepth);
//	_sprite_vertex.uv = glm::vec2(fTexRight, fTexBottom);
//	_sprite_vertex.color.x = pElement->texture_color.current_color_state.x;
//	_sprite_vertex.color.y = pElement->texture_color.current_color_state.y;
//	_sprite_vertex.color.z = pElement->texture_color.current_color_state.z;
//	_sprite_vertex.color.w = pElement->texture_color.current_color_state.w;
//
//	this->_widgets_resource_manager->m_SpriteVertices.push_back(_sprite_vertex);
//
//	_sprite_vertex.position = glm::vec3(fRectLeft, fRectBottom, pDepth);
//	_sprite_vertex.uv = glm::vec2(fTexLeft, fTexBottom);
//	_sprite_vertex.color.x = pElement->texture_color.current_color_state.x;
//	_sprite_vertex.color.y = pElement->texture_color.current_color_state.y;
//	_sprite_vertex.color.z = pElement->texture_color.current_color_state.z;
//	_sprite_vertex.color.w = pElement->texture_color.current_color_state.w;
//
//	this->_widgets_resource_manager->m_SpriteVertices.push_back(_sprite_vertex);
//
//	// Why are we drawing the sprite every time?  This is very inefficient, but the sprite workaround doesn't have support for sorting now, so we have to
//	// draw a sprite every time to keep the order correct between sprites and text.
//	this->_widgets_resource_manager->end_sprites(this->_gDevice->device.Get(), this->_gDevice->context.Get());
//
//	return S_OK;
//}

//HRESULT w_widget::draw_image(_In_z_ const wchar_t* pIconName,
//	const DirectX::XMFLOAT2 pPosition,
//	const DirectX::XMFLOAT2 pScale, const DirectX::XMFLOAT2 pScaleCenter,
//	const float pRotationAngle, const DirectX::XMFLOAT2 pRotationCenter,
//	const DirectX::XMFLOAT2 pTranslation)
//{
//	this->_sprite_batch->begin();
//	this->_sprite_batch->draw_image(pIconName, pPosition, pScale, pScaleCenter, pRotationAngle, pRotationCenter, pTranslation);
//	this->_sprite_batch->end();
//
//	return S_OK;
//}

//HRESULT w_widget::draw_image(_In_z_ const wchar_t* pIconName,
//	const DirectX::XMFLOAT2 pPosition, const D2D1_RECT_F pRectangle,
//	const DirectX::XMFLOAT2 pScale, const DirectX::XMFLOAT2 pScaleCenter,
//	const float pRotationAngle, const DirectX::XMFLOAT2 pRotationCenter,
//	const DirectX::XMFLOAT2 pTranslation)
//{
//	this->_sprite_batch->begin();
//	this->_sprite_batch->draw_image(pIconName, pPosition, pRectangle, pScale, pScaleCenter, pRotationAngle, pRotationCenter, pTranslation);
//	this->_sprite_batch->end();
//
//	return S_OK;
//}

//HRESULT w_widget::draw_text(_In_z_ std::wstring pText, DirectX::XMFLOAT2 pPosition, _In_ ID2D1SolidColorBrush* pBrush)
//{
//	if (this->_sprite_batch != nullptr)
//	{
//		this->_sprite_batch->begin();
//		this->_sprite_batch->draw_string(pText, pPosition, pBrush);
//		this->_sprite_batch->end();
//	}
//
//	return S_OK;
//}

//HRESULT w_widget::draw_shape(_In_ wolf::graphics::direct2D::Isprite_batch_drawable* pShape)
//{
//	if (this->_sprite_batch != nullptr)
//	{
//		this->_sprite_batch->begin();
//		pShape->draw();
//		this->_sprite_batch->end();
//	}
//
//	return S_OK;
//}

void w_widget::clear_focus()
{
	if (s_pControlFocus)
	{
		s_pControlFocus->on_focus_out();
		s_pControlFocus = nullptr;
	}

#ifdef __WIN32
	ReleaseCapture();
#endif
}

bool w_widget::on_cycle_focus(_In_ bool pForward)
{
	w_control* pControl = nullptr;
    // pDialog and pLastDialog are used to track wrapping of
	w_widget* pWidget = nullptr; 
    // focus from first control to last or vice versa.
	w_widget* pLastWidget;    

	if (!s_pControlFocus)
	{
		/*
            If s_pControlFocus is nullptr, we focus the first control of first dialog in
		    the case that bForward is true, and focus the last control of last dialog when
		    bForward is false.
        */
		if (pForward)
		{
			// Search for the first control from the start of the dialog array.
			for (auto it = w_widget::widgets.cbegin(); it != w_widget::widgets.cend(); ++it)
			{
				pWidget = pLastWidget = *it;
				if (pWidget && !pWidget->_controls.empty())
				{
					pControl = pWidget->_controls[0];
					break;
				}
			}

			if (!pWidget || !pControl)
			{
				/*
                    No dialog has been registered yet or no controls have been
				    added to the dialogs. Cannot proceed.
                */
				return true;
			}
		}
		else
		{
			// Search for the first control from the end of the dialog array.
			for (auto it = w_widget::widgets.crbegin(); it != w_widget::widgets.crend(); ++it)
			{
				pWidget = pLastWidget = *it;
				if (pWidget && !pWidget->_controls.empty())
				{
					pControl = pWidget->_controls[pWidget->_controls.size() - 1];
					break;
				}
			}

			if (!pWidget || !pControl)
			{
				/* 
                    No dialog has been registered yet or no controls have been
				    added to the dialogs. Cannot proceed.
                */
				return true;
			}
		}
	}
	else if (s_pControlFocus->get_parent_widget() != this)
	{
		// If a control belonging to another dialog has focus, let that other dialog handle this event by returning false.
		return false;
	}
	else
	{
		// Focused control belongs to this dialog. Cycle to the next/previous control.
		if (!pControl) return false;
		//_Analysis_assume_(pControl != 0);
		pLastWidget = s_pControlFocus->get_parent_widget();
		pControl = (pForward) ? get_next_control(s_pControlFocus) : get_prev_control(s_pControlFocus);
		pWidget = pControl->get_parent_widget();
	}

	if (!pControl) return false;

	for (int i = 0; i < 0xffff; i++)
	{
		/*
            If we just wrapped from last control to first or vice versa,
		    set the focused control to nullptr. This state, where no control
		    has focus, allows the camera to work.
        */
		int nLastDialogIndex = -1;
		auto fit = std::find(w_widget::widgets.cbegin(), w_widget::widgets.cend(), pLastWidget);
		if (fit != w_widget::widgets.cend())
		{
			nLastDialogIndex = int(fit - w_widget::widgets.begin());
		}

		int nDialogIndex = -1;
		fit = std::find(w_widget::widgets.cbegin(), w_widget::widgets.cend(), pWidget);
		if (fit != w_widget::widgets.cend())
		{
			nDialogIndex = int(fit - w_widget::widgets.begin());
		}

		if ((!pForward && nLastDialogIndex < nDialogIndex) || (pForward && nDialogIndex < nLastDialogIndex))
		{
			if (s_pControlFocus)
			{
				s_pControlFocus->on_focus_out();
			}
			s_pControlFocus = nullptr;
			return true;
		}

		// If we've gone in a full circle then focus doesn't change
		if (pControl == s_pControlFocus)
		{
			return true;
		}

		// If the dialog accepts keybord input and the control can have focus then move focus
        auto _parent_widget = pControl->get_parent_widget();
		if (_parent_widget && _parent_widget->keyboard_input && pControl->can_have_focus())
		{
			if (s_pControlFocus)
			{
				s_pControlFocus->on_focus_out();
			}
			s_pControlFocus = pControl;
			if (s_pControlFocus)
			{
				s_pControlFocus->on_focus_in();
			}
			return true;
		}

		pLastWidget = pWidget;
		pControl = (pForward) ? get_next_control(pControl) : get_prev_control(pControl);
		pWidget = pControl->get_parent_widget();
	}

	// If we reached this point, the chain of dialogs didn't form a complete loop
	V(S_FALSE, L"multiple dialogs are improperly chained together", _super::name, 2);
	return false;
}

void w_widget::remove_call_back(_In_ W_CALLBACK_GUI_EVENT pCallback)
{
	for (size_t i = 0; i < this->_call_back_events.size(); ++i)
	{
		if (this->_call_back_events.at(i) == pCallback)
		{
			this->_call_back_events.erase(this->_call_back_events.begin() + i);
			break;
		}
	}
}

ULONG w_widget::release()
{
    if (_super::get_is_released()) return 0;
	
    this->_vertex_declarations_2d.clear();

	remove_all_controls();
    
	for (auto it = this->_default_elements.begin(); it != this->_default_elements.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	this->_default_elements.clear();

	//callbacks
	if (this->on_mouse_move_callback) this->on_mouse_move_callback = nullptr;
	if (this->on_mouse_enter_callback) this->on_mouse_enter_callback = nullptr;
	if (this->on_mouse_leave_callback) this->on_mouse_leave_callback = nullptr;

	for (size_t i = 0; i < this->_call_back_events.size(); ++i)
	{
		if (this->_call_back_events.at(i))
		{
			this->_call_back_events.at(i) = nullptr;
		}
	}

	this->_call_back_events.clear();

	for (size_t i = 0; i < this->_call_back_events_user_contexts.size(); ++i)
	{
		if (this->_call_back_events_user_contexts.at(i))
		{
			this->_call_back_events_user_contexts.at(i) = nullptr;
		}
	}
	this->_call_back_events_user_contexts.clear();

	if (this->_control_mouse_over) this->_control_mouse_over = nullptr;
	if (this->_next_widget) this->_next_widget = nullptr;
	if (this->_prev_widget) this->_prev_widget = nullptr;
	if (this->s_pControlFocus) this->s_pControlFocus = nullptr;
	if (this->s_pControlPressed) this->s_pControlPressed = nullptr;
    
	return _super::release();
}

#pragma region Getters

void w_widget::get_all_controls_id(_Inout_ std::vector<INT64>& pIDs)
{
	std::for_each(this->_controls.begin(), this->_controls.end(), [&](w_control* pControl)
	{
		if (pControl)
		{
			pIDs.push_back(pControl->get_ID());
		}
	});
}

w_control* w_widget::get_control_at_point(_In_ const w_point& pPoint) const
{
	/* Search through all child controls for the first one which contains the mouse point
    */
	for (auto it = this->_controls.cbegin(); it != this->_controls.cend(); ++it)
	{
		if (!*it)
		{
			continue;
		}

		/*
            We only return the current control if it is visible
		    and enabled.  Because GetControlAtPoint() is used to do mouse
		    hittest, it makes sense to perform this filtering.
        */
		if ((*it)->contains_point(pPoint) && (*it)->get_enabled() && (*it)->get_visible())
		{
			return *it;
		}
	}

	return nullptr;
}

bool w_widget::get_control_enabled(_In_ int pID) const
{
	auto pControl = get_control(pID);
	if (!pControl)
	{
		return false;
	}
	return pControl->get_enabled();
}

w_control* w_widget::get_control(_In_ int pID) const
{
	// Try to find the control with the given ID
	for (auto it = this->_controls.cbegin(); it != this->_controls.cend(); ++it)
	{
		if ((*it)->get_ID() == pID)
		{
			return *it;
		}
	}

	// Not found
	return nullptr;
}

w_control* w_widget::get_control(_In_  int pID, _In_  UINT pControlType) const
{
	// Try to find the control with the given ID
	for (auto it = this->_controls.cbegin(); it != this->_controls.cend(); ++it)
	{
		if ((*it)->get_ID() == pID && (*it)->get_type() == pControlType)
		{
			return *it;
		}
	}

	// Not found
	return nullptr;
}

w_control* w_widget::get_next_control(_In_ w_control* pControl)
{
	int index = pControl->get_index() + 1;

	auto pWidget = pControl->get_parent_widget();

	/*
	    Cycle through dialogs in the loop to find the next control. Note
	    that if only one control exists in all looped dialogs it will
	    be the returned 'next' control.
	*/
	while (index >= (int) pWidget->_controls.size())
	{
		pWidget = pWidget->_next_widget;
		index = 0;
	}

	return pWidget->_controls[index];
}

w_control* w_widget::get_prev_control(_In_ w_control* pControl)
{
	int index = pControl->get_index() - 1;

	auto _widget = pControl->get_parent_widget();

	/*
	    Cycle through dialogs in the loop to find the next control. Note
	    that if only one control exists in all looped dialogs it will
	    be the returned 'previous' control.
	*/
	while (index < 0)
	{
		_widget = _widget->_prev_widget;
		if (!_widget)
		{
			_widget = pControl->get_parent_widget();
		}
		index = int(_widget->_controls.size()) - 1;
	}

	return _widget->_controls[index];
}

w_element* w_widget::get_default_element(UINT pControlType, UINT pIndexElement) const
{
	for (auto it = this->_default_elements.cbegin(); it != this->_default_elements.cend(); ++it)
	{
		if ((*it)->control_type == pControlType &&
			(*it)->index_element == pIndexElement)
		{
			return &(*it)->element;
		}
	}

	return nullptr;
}

UINT w_widget::get_background_buffer_width() const
{
	return this->_parent_window_width;
}

UINT w_widget::get_background_buffer_height() const
{
	return this->_parent_window_height;
}

std::vector<w_gui_vertex_2d> w_widget::get_vertex_declarations_2d()
{
    return this->_vertex_declarations_2d;
}

#pragma endregion

#pragma region Setters

void w_widget::set_call_back(_In_ W_CALLBACK_GUI_EVENT pCallback, _In_opt_ void* pUserContext)
{
    this->_call_back_events.push_back(pCallback);
    this->_call_back_events_user_contexts.push_back(pUserContext);
}

void w_widget::set_control_enabled(_In_ int pID, _In_ bool pEnabled)
{
	auto pControl = get_control(pID);
	if (!pControl)
		return;

	pControl->set_enabled(pEnabled);
}

HRESULT w_widget::set_default_element(_In_ const UINT pControlType, _In_ const UINT iElement, _In_ const w_element* pElement)
{
	// If this Element type already exist in the list, simply update the stored Element
	for (auto it = this->_default_elements.begin(); it != this->_default_elements.end(); ++it)
	{
		if ((*it)->control_type == pControlType &&
			(*it)->index_element == iElement)
		{
			(*it)->element = *pElement;
			return S_OK;
		}
	}

	// Otherwise, add a new entry
	w_element_holder* _new_holder;
	_new_holder = new (std::nothrow) w_element_holder;
	if (!_new_holder)
	{
		return E_OUTOFMEMORY;
	}
	_new_holder->control_type = pControlType;
	_new_holder->index_element = iElement;
	_new_holder->element = *pElement;

	this->_default_elements.push_back(_new_holder);

	return S_OK;
}

void w_widget::set_next_widget(_In_ w_widget* pNextWidget)
{
	if (!pNextWidget)
	{
		pNextWidget = this;
	}
	this->_next_widget = pNextWidget;
	if (pNextWidget)
	{
		this->_next_widget->_prev_widget = this;
	}
}

void w_widget::set_background_colors(_In_ const w_color pColorForAllCorners)
{
    set_background_colors(pColorForAllCorners, pColorForAllCorners, pColorForAllCorners, pColorForAllCorners);
}

void w_widget::set_background_colors(
    _In_ const w_color pColorTopLeft,
    _In_ const w_color pColorTopRight,
    _In_ const w_color pColorBottomLeft,
    _In_ const w_color pColorBottomRight)
{
	this->_background_color_top_left = pColorTopLeft;
	this->_background_color_top_right = pColorTopRight;
	this->_background_color_bottom_left = pColorBottomLeft;
	this->_background_color_bottom_right = pColorBottomRight;
}

void w_widget::set_active_background_colors(
    _In_ const w_color pColorTopLeft, 
    _In_ const w_color pColorTopRight, 
    _In_ const w_color pColorBottomLeft, 
    _In_ const w_color pColorBottomRight)
{
	this->_active_background_color_top_left = pColorTopLeft;
	this->_active_background_color_top_right = pColorTopRight;
	this->_active_background_color_bottom_left = pColorBottomLeft;
	this->_active_background_color_bottom_right = pColorBottomRight;
}

void w_widget::set_enabled(_In_ const bool pEnabled)
{
	for (auto _control : this->_controls)
	{
		_control->set_enabled(pEnabled);
	}
}

#pragma endregion
