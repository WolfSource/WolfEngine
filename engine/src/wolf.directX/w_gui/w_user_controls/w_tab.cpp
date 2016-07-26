#include "w_directX_pch.h"
#include "w_tab.h"
#include "w_gui/w_widget.h"

using namespace wolf::gui;

w_tab::w_tab(_In_opt_ w_widget* pParent) : _super(pParent)
{
	_super::set_class_name(typeid(this).name());
	_super::type = W_GUI_CONTROL_TAB;
}

HRESULT w_tab::on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
	add_tab();
	W_UNUSED(pGDevice);
	return S_OK;
}

void w_tab::add_tab()
{
	w_button* _button;
	_super::parent_widget->add_button(101, L"tab1", _super::x, _super::y, 64, 15, L"", -10, -10, 10, 10, 0, false, &_button);
	this->buttons.push_back(_button);
}

void w_tab::remove_all_tabs()
{
	std::for_each(this->buttons.begin(), this->buttons.end(), [](w_button* pItem)
	{
		SAFE_RELEASE(pItem);
	});
	this->buttons.clear();
	
	std::for_each(this->widgets.begin(), this->widgets.end(), [](w_widget* pItem)
	{
		SAFE_RELEASE(pItem);
	});
	this->widgets.clear();
}

void w_tab::render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{
	if (_super::visible == false) return;

	W_GUI_CONTROL_STATE _state = W_GUI_STATE_NORMAL;

	if (!_super::enabled)
	{
		_state = W_GUI_STATE_DISABLED;
	}

	if (this->buttons.size() > 0)
	{
		auto _button = this->buttons.at(0);
		_button->render(pGDevice, pElapsedTime);
	}
}


