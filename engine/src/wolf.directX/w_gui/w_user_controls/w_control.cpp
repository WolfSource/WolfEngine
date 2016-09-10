#include "w_directX_pch.h"
#include "w_control.h"

using namespace wolf::gui;

w_control::w_control(_In_opt_ w_widget* pParent) :
	parent_widget(pParent),
	type(W_GUI_CONTROL_BUTTON), 
	id(0), 
	hot_key(0), 
	index(0), 
	tag(nullptr), 
	enabled(true),
	visible(true), 
	mouse_over(false), 
	has_focus(false), 
	is_default(false), 
	x(0), 
	y(0), 
	width(0), 
	height(0),
	z_order(0)
{
	_super::set_class_name(typeid(this).name());
	std::memset(&this->boundingBox, 0, sizeof(this->boundingBox));
}

w_control::~w_control()
{
	for (auto it = this->elements.begin(); it != this->elements.end(); ++it)
	{
		auto pElement = *it;
		delete pElement;
	}
	this->elements.clear();
}

HRESULT w_control::on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
	W_UNUSED(pGDevice);
	return S_OK; 
}

void w_control::refresh()
{
	this->mouse_over = false;
	this->has_focus = false;

	for (auto it = this->elements.begin(); it != this->elements.end(); ++it)
	{
		(*it)->refresh();
	}
}

void w_control::update_rects()
{
	SetRect(&this->boundingBox, this->x, this->y, this->x + this->width, this->y + this->height);
}

void w_control::render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime)
{ 
	W_UNUSED(pGDevice.get());
	W_UNUSED(pElapsedTime);
}

// Windows message handler
bool w_control::on_msg_proc(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam)
{
	W_UNUSED(pMsg);
	W_UNUSED(pWParam);
	W_UNUSED(pLParam);

	return false;
}

bool w_control::handle_keyboard(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam)
{
	W_UNUSED(pMsg);
	W_UNUSED(pWParam);
	W_UNUSED(pLParam);

	return false;
}
bool w_control::handle_mouse(_In_ UINT pMsg, _In_ const POINT& pPoint, _In_ WPARAM pWParam, _In_ LPARAM pLParam)
{
	W_UNUSED(pMsg);
	W_UNUSED(pPoint);
	W_UNUSED(pWParam);
	W_UNUSED(pLParam);

	return false;
}

#pragma region Setters

HRESULT w_control::set_element(_In_ UINT pIndexElement, _In_ w_element* pElement)
{
	if (!pElement) return E_INVALIDARG;

	// Make certain the array is this large
	for (size_t i = this->elements.size(); i <= pIndexElement; i++)
	{
		auto pNewElement = new (std::nothrow) w_element();
		if (!pNewElement) return E_OUTOFMEMORY;

		this->elements.push_back(pNewElement);
	}

	// Update the data
	auto pCurElement = this->elements[pIndexElement];
	*pCurElement = *pElement;

	return S_OK;
}


#pragma endregion