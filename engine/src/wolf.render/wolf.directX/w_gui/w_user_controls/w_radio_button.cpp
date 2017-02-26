#include "w_directX_pch.h"
#include "w_radio_button.h"
#include "w_gui/w_widgets_resource_manager.h"

using namespace wolf::gui;

w_radio_button::w_radio_button(_In_opt_ w_widget* pParent) : _super(pParent)
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_RADIOBUTTON;
	_super::parent_widget = pParent;
}

_Use_decl_annotations_
bool w_radio_button::handle_keyboard(UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	W_UNUSED(pLParam);

	if (!_super::enabled || !_super::visible) return false;

	switch (pMsg)
	{
	case WM_KEYDOWN:
	{
		switch (pWParam)
		{
		case VK_SPACE:
			_super::pressed = true;
			return true;
		}
	}

	case WM_KEYUP:
	{
		switch (pWParam)
		{
		case VK_SPACE:
			if (_super::pressed == true)
			{
				_super::pressed = false;

				_super::parent_widget->clear_radioButton_group(this->button_group);
				_super::checked = !_super::checked;

				_super::parent_widget->send_event(W_EVENT_RADIOBUTTON_CHANGED, true, this);
			}
			return true;
		}
	}
	}
	return false;
}

_Use_decl_annotations_
bool w_radio_button::handle_mouse(UINT pMsg, const POINT& pPoint, WPARAM pWParam, LPARAM pLParam)
{
	W_UNUSED(pWParam);
	W_UNUSED(pLParam);

	if (!_super::enabled || !_super::visible) return false;

	switch (pMsg)
	{
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
				SetCapture(_hwnd);
			}
			
			if (!_super::has_focus)
			{
				_super::parent_widget->request_focus(this);
			}

			return true;
		}

		break;
	}

	case WM_LBUTTONUP:
	{
		if (_super::pressed)
		{
			_super::pressed = false;
			ReleaseCapture();

			// Button click
			if (contains_point(pPoint))
			{
				_super::parent_widget->clear_radioButton_group(this->button_group);
				_super::checked = !_super::checked;

				_super::parent_widget->send_event(W_EVENT_RADIOBUTTON_CHANGED, true, this);
			}

			return true;
		}

		break;
	}
	};

	return false;
}

void w_radio_button::on_hot_key()
{
	if (_super::parent_widget->get_is_keyboard_input_enabled())
	{
		_super::parent_widget->request_focus(this);
	}
	set_checked_internal(true, true, true);
}

_Use_decl_annotations_
void w_radio_button::set_checked_internal(bool pChecked, bool pClearGroup, bool pFromInput)
{
	if (pChecked && pClearGroup)
	{
		_super::parent_widget->clear_radioButton_group(this->button_group);
	}

	_super::checked = pChecked;
	_super::parent_widget->send_event(W_EVENT_RADIOBUTTON_CHANGED, pFromInput, this);
}