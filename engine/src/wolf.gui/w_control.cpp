#include "w_gui_pch.h"
#include "w_control.h"

using namespace wolf::gui;

w_control::w_control(_In_ w_widget* pParent) :
    parent_widget(pParent),
    type(W_GUI_CONTROL_CONTROL),
    id(0),
    hot_key(0),
    index(0),
    tag(nullptr),
    enabled(true),
    visible(true),
    mouse_over(false),
    has_focus(false),
    x(0),
    y(0),
    width(0),
    height(0),
    z_order(0),
    redrawing(false)
{
    _super::set_class_name("w_control");
    std::memset(&this->bounding_box, 0, sizeof(this->bounding_box));
}

w_control::~w_control()
{
    release();
}

HRESULT w_control::on_initialize()
{
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
    this->bounding_box.left = this->x;
    this->bounding_box.top = this->y;
    this->bounding_box.right = this->x + this->width;
    this->bounding_box.bottom = this->y + this->height;
}

void w_control::render(_In_ float pElapsedTime)
{ 
	W_UNUSED(pElapsedTime);
}

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
bool w_control::handle_mouse(_In_ UINT pMsg, _In_ const w_point& pPoint, _In_ WPARAM pWParam, _In_ LPARAM pLParam)
{
	W_UNUSED(pMsg);
	W_UNUSED(pPoint);
	W_UNUSED(pWParam);
	W_UNUSED(pLParam);

	return false;
}

bool w_control::contains_point(_In_ const w_point& pPoint)
{
    return pPoint.x >= this->bounding_box.left && pPoint.x <= this->bounding_box.right &&
        pPoint.y >= this->bounding_box.bottom && pPoint.y <= this->bounding_box.top;
}

ULONG w_control::release()
{
    if (_super::get_is_released()) return 0;
    
    for (auto it = this->elements.begin(); it != this->elements.end(); ++it)
    {
        SAFE_DELETE(*it);
    }
    this->elements.clear();

    return _super::release();
}

#pragma region Setters

HRESULT w_control::set_element(_In_ UINT pIndexElement, _In_ w_element* pElement)
{
	if (!pElement) return E_INVALIDARG;

    HRESULT _hr = S_OK;
	// Make certain the array is this large
	for (size_t i = this->elements.size(); i <= pIndexElement; i++)
	{
		auto pNewElement = new (std::nothrow) w_element();
        if (!pNewElement)
        {
            _hr = E_OUTOFMEMORY;
            break;
        }
		this->elements.push_back(pNewElement);
	}

    if (_hr != S_OK)
    {
        return _hr;
    }

	// Update the data
	auto pCurElement = this->elements[pIndexElement];
	*pCurElement = *pElement;

	return _hr;
}

#pragma endregion