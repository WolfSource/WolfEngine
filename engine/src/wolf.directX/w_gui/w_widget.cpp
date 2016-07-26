#include "w_directX_pch.h"
#include "w_widgets_resource_manager.h"
#include <w_math.h>

using namespace wolf::gui;

wolf::system::w_time_span   w_widget::s_time_refresh = wolf::system::w_time_span::zero();
w_control*					w_widget::s_pControlFocus = nullptr;        // The control which has focus
w_control*					w_widget::s_pControlPressed = nullptr;      // The control currently pressed

w_widget::w_widget(int pParentWindowWidth, int pParentWindowHeight) :
	_parent_window_width(pParentWindowWidth),
	_parent_window_height(pParentWindowHeight),
	_x(0),
	_y(0),
	_width(0),
	_height(0),
	_widgets_resource_manager(nullptr),
	visible(true),
	caption_enable(false),
	minimized(false),
	drag(false),
	_caption_height(18),
	color_top_left(0),
	color_top_right(0),
	color_bottom_left(0),
	color_bottom_right(0),
	callBack_event(nullptr),
	callBack_event_user_context(nullptr),
	control_mouse_over(nullptr),
	_default_control_ID(0xffff),
	nonUser_events(false),
	keyboard_input(false),
	mouse_input(true),
	_is_mouse_over_widget(false)
{
	this->caption[0] = L'\0';
	time_last_refresh = system::w_time_span::zero();
	this->_next_widget = this;
	this->_prev_widget = this;
}

w_widget::~w_widget()
{
	remove_all_controls();

	this->fonts.clear();
	this->textures.clear();

	for (auto it = this->default_elements.begin(); it != this->default_elements.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	this->default_elements.clear();
}

_Use_decl_annotations_
void w_widget::initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
	_In_ wolf::graphics::w_sprite_batch* pSpriteBatch,
	w_widgets_resource_manager* pWidgetResourceManager,
	bool bRegisterDialog)
{
	this->_gDevice = pGDevice;
	this->_sprite_batch = pSpriteBatch;
	this->_widgets_resource_manager = pWidgetResourceManager;

	if (bRegisterDialog)
	{
		this->_widgets_resource_manager->register_widget(this);
	}

	set_texture(0, MAKEINTRESOURCE(0xFFFF), (HMODULE) 0xFFFF);
	initialize_default_elements();
}

_Use_decl_annotations_
void w_widget::initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
	_In_ wolf::graphics::w_sprite_batch* pSpriteBatch,
	w_widgets_resource_manager* pWidgetResourceManager,
	bool bRegisterDialog,
	LPCWSTR pControlTextureFilename)
{
	this->_gDevice = pGDevice;
	this->_sprite_batch = pSpriteBatch;
	this->_widgets_resource_manager = pWidgetResourceManager;

	if (bRegisterDialog)
	{
		this->_widgets_resource_manager->register_widget(this);
	}
	set_texture(0, pControlTextureFilename);
	initialize_default_elements();
}

_Use_decl_annotations_
void w_widget::initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
	_In_ wolf::graphics::w_sprite_batch* pSpriteBatch,
	w_widgets_resource_manager* pWidgetResourceManager,
	bool bRegisterDialog,
	LPCWSTR pControlTextureResourceName,
	HMODULE pControlTextureResourceModule)
{
	this->_gDevice = pGDevice;
	this->_sprite_batch = pSpriteBatch;
	this->_widgets_resource_manager = pWidgetResourceManager;

	if (bRegisterDialog)
	{
		this->_widgets_resource_manager->register_widget(this);
	}

	set_texture(0, pControlTextureResourceName, pControlTextureResourceModule);
	initialize_default_elements();
}

HRESULT w_widget::on_render(_In_ float pElapsedTime)
{
	// See if the dialog needs to be refreshed
	if (this->time_last_refresh < s_time_refresh)
	{
		this->time_last_refresh = system::w_time_span::now();
		refresh();
	}

	// For invisible dialog, out now.
	if (!this->visible || (this->minimized && !this->caption_enable))
	{
		return S_OK;
	}
	auto _context = this->_gDevice->context.Get();
	auto _device = this->_gDevice->device.Get();

	// Set up a state block here and restore it when finished drawing all the controls
	this->_widgets_resource_manager->store_D3D_state(_context);

	BOOL bBackgroundIsVisible = (this->color_top_left | this->color_top_right | this->color_bottom_right | this->color_bottom_left) & 0xff000000;
	if (!this->minimized && bBackgroundIsVisible)
	{
		// Convert the draw rectangle from screen coordinates to clip space coordinates.
		float Left, Right, Top, Bottom;
		Left = this->_x * 2.0f / this->_widgets_resource_manager->backBuffer_width - 1.0f;
		Right = (this->_x + this->_width) * 2.0f / this->_widgets_resource_manager->backBuffer_width - 1.0f;
		Top = 1.0f - this->_y * 2.0f / this->_widgets_resource_manager->backBuffer_height;
		Bottom = 1.0f - (this->_y + this->_height) * 2.0f / this->_widgets_resource_manager->backBuffer_height;

		w_gui_screen_vertex vertices[4] =
		{
			Left,  Top,    0.5f, DirectX::DWORD_COLOR_TO_XMFLOAT4(color_top_left), 0.0f, 0.0f,
			Right, Top,    0.5f, DirectX::DWORD_COLOR_TO_XMFLOAT4(color_top_right), 1.0f, 0.0f,
			Left,  Bottom, 0.5f, DirectX::DWORD_COLOR_TO_XMFLOAT4(color_bottom_left), 0.0f, 1.0f,
			Right, Bottom, 0.5f, DirectX::DWORD_COLOR_TO_XMFLOAT4(color_bottom_right), 1.0f, 1.0f,
		};

		//DXUT_SCREEN_VERTEX_10 *pVB;
		D3D11_MAPPED_SUBRESOURCE _mapped_data;
		if (SUCCEEDED(_context->Map(this->_widgets_resource_manager->m_pVBScreenQuad11, 0, D3D11_MAP_WRITE_DISCARD, 0, &_mapped_data)))
		{
			memcpy(_mapped_data.pData, vertices, sizeof(vertices));
			_context->Unmap(this->_widgets_resource_manager->m_pVBScreenQuad11, 0);
		}

		// Set the quad VB as current
		UINT stride = sizeof(w_gui_screen_vertex);
		UINT offset = 0;
		_context->IASetVertexBuffers(0, 1, &this->_widgets_resource_manager->m_pVBScreenQuad11, &stride, &offset);
		_context->IASetInputLayout(this->_widgets_resource_manager->m_pInputLayout11);
		_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// Setup for rendering
		this->_widgets_resource_manager->apply_render_UI_Untex(_context);
		_context->Draw(4, 0);
	}

	auto pTextureNode = get_texture(0);
	if (pTextureNode == nullptr) return S_FALSE;

	_context->PSSetShaderResources(0, 1, &pTextureNode->pTexResView11);

	// Sort depth back to front
	this->_widgets_resource_manager->begin_sprites();
	this->_widgets_resource_manager->begin_text();

	this->_widgets_resource_manager->apply_render_UI(_context);

	// Render the caption if it's enabled.
	if (this->caption_enable)
	{
		// DrawSprite will offset the rect down by
		// m_nCaptionHeight, so adjust the rect higher
		// here to negate the effect.
		RECT rc = { 0, -this->_caption_height, this->_width, 0 };
		draw_sprite(&caption_element, &rc, 0.99f);
		rc.left += 5; // Make a left margin
		WCHAR wszOutput[256];
		wcscpy_s(wszOutput, 256, caption);
		if (this->minimized)
		{
			wcscat_s(wszOutput, 256, L" (Minimized)");
		}
		//////////////////////////draw_text(wszOutput, &caption_element, &rc, true);
	}

	// If the dialog is minimized, skip rendering its controls.
	if (!this->minimized)
	{
		for (auto it = this->controls.cbegin(); it != this->controls.cend(); ++it)
		{
			// Focused control is drawn last
			if (*it == s_pControlFocus) continue;

			(*it)->render(this->_gDevice, pElapsedTime);
		}

		if (s_pControlFocus && s_pControlFocus->parent_widget == this)
		{
			s_pControlFocus->render(this->_gDevice, pElapsedTime);
		}
	}

	// End sprites
	if (this->caption_enable)
	{
		this->_widgets_resource_manager->end_sprites(_device, _context);
		this->_widgets_resource_manager->end_text(_device, _context);
	}
	this->_widgets_resource_manager->restore_D3D_state(_context);

	return S_OK;
}

void w_widget::remove_control(_In_ int pID)
{
	for (auto it = this->controls.begin(); it != this->controls.end(); ++it)
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
			if (this->control_mouse_over == (*it))
			{
				this->control_mouse_over = nullptr;
			}
			SAFE_DELETE((*it));
			this->controls.erase(it);

			return;
		}
	}
}

void w_widget::remove_all_controls()
{
	if (s_pControlFocus && s_pControlFocus->parent_widget == this)
	{
		s_pControlFocus = nullptr;
	}
	if (s_pControlPressed && s_pControlPressed->parent_widget == this)
	{
		s_pControlPressed = nullptr;
	}
	this->control_mouse_over = nullptr;

	for (auto it = this->controls.begin(); it != this->controls.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	this->controls.clear();
}

void w_widget::refresh()
{
	if (s_pControlFocus)
	{
		s_pControlFocus->on_focus_out();
	}
	if (this->control_mouse_over)
	{
		this->control_mouse_over->on_mouse_leave();
	}
	s_pControlFocus = nullptr;
	s_pControlPressed = nullptr;
	this->control_mouse_over = nullptr;

	for (auto it = this->controls.begin(); it != this->controls.end(); ++it)
	{
		(*it)->refresh();
	}

	if (this->keyboard_input)
	{
		focus_default_control();
	}
}

_Use_decl_annotations_
VOID w_widget::send_event(UINT pEvent, bool pTriggeredByUser, w_control* pControl)
{
	// If no callback has been registered there's nowhere to send the event to
if (!this->callBack_event) return;

// Discard events triggered programatically if these types of events haven't been
// enabled
if (!pTriggeredByUser && !this->nonUser_events) return;

this->callBack_event(pEvent, pControl->get_ID(), pControl, this->callBack_event_user_context);
}

_Use_decl_annotations_
HRESULT w_widget::add_label(int pID,
	_In_z_ std::wstring pText,
	int pX, int pY,
	int pWidth, int pHeight,
	bool pIsDefault,
	_Out_opt_ w_label** pCreatedControl)
{
	auto _label = new (std::nothrow) w_label(this);

	if (pCreatedControl)
	{
		*pCreatedControl = _label;
	}

	if (!_label) return E_OUTOFMEMORY;

	// Set the ID and list index
	_label->set_ID(pID);
	_label->set_text(pText);
	_label->set_location(pX, pY);
	_label->set_size(this->_width, pHeight);
	_label->is_default = pIsDefault;

	auto hr = add_control(_label);
	if (FAILED(hr))
	{
		return hr;
	}
	return S_OK;
}

_Use_decl_annotations_
HRESULT w_widget::add_image(int pID,
	_In_ int pX, _In_ int pY,
	_In_z_ std::wstring pPath,
	_In_ bool pRelativePath,
	_In_ float pGuassianEffectValue,
	_In_ float pScaleX, _In_ float pScaleY,
	_In_ int pWidth, _In_ int pHeight,
	_In_ float pRotationAngle, _In_ int pSetRotationCenterX, _In_ int pSetRotationCenterY,
	_In_ float pTranslationX, _In_ float pTranslationY,
	_In_ bool pIsDefault,
	_Out_opt_ w_image** pCreatedControl)
{
	auto _image = new (std::nothrow) w_image(this);

	if (pCreatedControl)
	{
		*pCreatedControl = _image;
	}

	if (!_image) return E_OUTOFMEMORY;

	auto _path = pRelativePath ? wolf::framework::w_game::get_content_directory() + pPath : pPath;

	// Set the ID and list index
	_image->set_ID(pID);
	_image->set_path(_path.c_str());
	_image->set_location(pX, pY);
	_image->set_size(pWidth, pHeight);
	_image->set_scale(DirectX::XMFLOAT2(pScaleX, pScaleY));
	_image->set_rotation_angle(pRotationAngle);
	_image->set_rotation_center(DirectX::XMFLOAT2((float) pSetRotationCenterX, (float) pSetRotationCenterY));
	_image->set_translation(DirectX::XMFLOAT2(pTranslationX, pTranslationY));
	_image->is_default = pIsDefault;

	//Add image to memory
	add_image_to_resource_manager(_path.c_str());

	auto _hr = add_control(_image);
	if (FAILED(_hr))
	{
		return _hr;
	}
	return S_OK;
}

_Use_decl_annotations_
HRESULT w_widget::add_button(_In_ int pID,
	_In_z_ std::wstring pText,
	_In_ int pX, _In_ int pY,
	_In_ int pWidth, _In_ int pHeight,
	_In_z_ std::wstring pIconPath,
	_In_ bool pRelativePath,
	_In_ int pTextOffsetX, _In_ int pTextOffsetY,
	_In_ int pIconOffsetX, _In_ int pIconOffsetY,
	_In_ float pIconScaleX, _In_ float pIconScaleY,
	_In_ UINT pHotkey,
	_In_ bool pIsDefault,
	_Out_opt_ w_button** pCreatedControl)
{
	return add_button(
		pID, 
		pText, 
		pX, pY, 
		pWidth, pHeight, 
		pIconPath, 
		pRelativePath,
		pTextOffsetX, pTextOffsetY, 
		pIconOffsetX, pIconOffsetY, 
		pIconScaleX, pIconScaleY,
		W_COLOR(255, 255, 255, 255),
		W_COLOR(255, 255, 255, 160),
		W_COLOR(255, 255, 255, 200),
		W_COLOR(255, 255, 255, 30),
		W_COLOR(255, 255, 255, 255),
		W_COLOR(0, 0, 0, 255),
		W_COLOR(0, 0, 0, 255),
		W_COLOR(255, 255, 255, 255),
		0.0f, 1.0f,
		pHotkey,
		pIsDefault,
		pCreatedControl);
}

HRESULT w_widget::add_button(_In_ int pID,
	_In_z_ std::wstring pText,
	_In_ int pX, _In_ int pY,
	_In_ int pWidth, _In_ int pHeight,
	_In_z_ std::wstring pIconPath,
	_In_ bool pRelativePath,
	_In_ int pTextOffsetX, _In_ int pTextOffsetY,
	_In_ int pIconOffsetX, _In_ int pIconOffsetY,
	_In_ float pIconScaleX, _In_ float pIconScaleY,
	_In_ W_COLOR pColor, _In_ W_COLOR pMouseOverColor, _In_ W_COLOR pPressedColor, _In_ W_COLOR pFocusedColor, 
	_In_ W_COLOR pTextColor, _In_ W_COLOR pTextMouseOverColor,_In_ W_COLOR pTextPressedColor, _In_ W_COLOR pTextFocusedColor,
	_In_ float pEffectMinValue, _In_ float pEffectMaxValue,
	_In_ UINT pHotkey,
	_In_ bool pIsDefault,
	_Out_opt_ w_button** pCreatedControl)
{
	auto _button = new (std::nothrow) w_button(this);

	if (pCreatedControl)
	{
		*pCreatedControl = _button;
	}

	if (!_button) return E_OUTOFMEMORY;

	// Set the ID and list index
	_button->set_ID(pID);
	_button->set_location(pX, pY);
	_button->set_size(pWidth, pHeight);
	_button->set_text(pText);
	
	_button->set_original_text_offset_x(pTextOffsetX);
	_button->set_original_text_offset_y(pTextOffsetY);
	
	_button->set_icon_offset(pIconOffsetX, pIconOffsetY);
	_button->set_icon_scale(pIconScaleX, pIconScaleY);
	
	_button->set_button_color(pColor);
	_button->set_button_mouse_over_color(pMouseOverColor);
	_button->set_button_pressed_color(pPressedColor);
	_button->set_button_focused_color(pFocusedColor);
	
	_button->set_label_color(pTextColor);
	_button->set_label_mouse_over_color(pTextMouseOverColor);
	_button->set_label_pressed_color(pTextPressedColor);
	_button->set_label_focused_color(pTextFocusedColor);

	_button->set_effect_min_max_range(pEffectMinValue, pEffectMaxValue);

	_button->set_hotKey(pHotkey);
	_button->is_default = pIsDefault;

	if (!pIconPath.empty())
	{
		auto _icon_path = pRelativePath ? wolf::framework::w_game::get_content_directory() +  pIconPath : pIconPath;
		auto _icon_path_str = _icon_path.c_str();
		_button->set_icon_path(_icon_path_str);
		//Add image to memory
		this->_sprite_batch->add_image_from_file(_icon_path_str);
	}

	auto hr = add_control(_button);
	if (FAILED(hr)) return hr;

	return S_OK;
}

_Use_decl_annotations_
HRESULT w_widget::add_checkBox(int pID, 
	_In_z_ std::wstring pText, 
	int pX, int pY, 
	int pWidth, int pHeight, 
	bool pChecked, 
	int pTextOffsetX, int pTextOffsetY,
	UINT pHotkey, 
	bool pIsDefault, 
	W_COLOR pColor, W_COLOR pMouseOverColor, W_COLOR pPressedColor, W_COLOR pFocusedColor, 
	_Out_opt_ w_checkBox** pCreatedControl)
{
	auto _checkBox = new (std::nothrow) w_checkBox(this);

	if (pCreatedControl)
	{
		*pCreatedControl = _checkBox;
	}

	if (!_checkBox) return E_OUTOFMEMORY;

	auto hr = add_control(_checkBox);
	if (FAILED(hr)) return hr;

	// Set the ID and list index
	_checkBox->set_ID(pID);
	_checkBox->set_text(pText);
	_checkBox->set_location(pX, pY);
	_checkBox->set_size(pWidth, pHeight);
	_checkBox->set_checked(pChecked);
	_checkBox->set_original_text_offset_x(pTextOffsetX);
	_checkBox->set_original_text_offset_y(pTextOffsetY);
	_checkBox->set_hotKey(pHotkey);

	_checkBox->set_checkBox_color(pColor);
	_checkBox->set_label_color(pColor);

	_checkBox->set_label_mouse_over_color(pMouseOverColor);
	_checkBox->set_checkBox_mouse_over_color(pMouseOverColor);

	_checkBox->set_label_pressed_color(pPressedColor);
	_checkBox->set_checkBox_pressed_color(pPressedColor);

	_checkBox->set_label_focused_color(pFocusedColor);
	_checkBox->set_checkBox_focused_color(pFocusedColor);

	_checkBox->is_default = pIsDefault;


	return S_OK;
}

_Use_decl_annotations_
HRESULT w_widget::add_radioButton(int pID,
	UINT pButtonGroup,
	std::wstring pText,
	int pX, int pY,
	int pWidth, int pHeight,
	bool pChecked,
	int pTextOffsetX, int pTextOffsetY,
	UINT pHotkey,
	bool pIsDefault,
	W_COLOR pColor, W_COLOR pMouseOverColor, W_COLOR pPressedColor, W_COLOR pFocusedColor,
	w_radioButton** pCreatedControl)
{
	auto _radio_button = new (std::nothrow) w_radioButton(this);

	if (pCreatedControl)
	{
		*pCreatedControl = _radio_button;
	}
	if (!_radio_button) return E_OUTOFMEMORY;

	auto hr = add_control(_radio_button);
	if (FAILED(hr)) return hr;

	// Set the ID and list index
	_radio_button->set_ID(pID);
	_radio_button->set_text(pText);
	_radio_button->set_button_group(pButtonGroup);
	_radio_button->set_location(pX, pY);
	_radio_button->set_size(pWidth, pHeight);
	_radio_button->set_checked(pChecked);
	_radio_button->set_original_text_offset_x(pTextOffsetX);
	_radio_button->set_original_text_offset_y(pTextOffsetY);
	_radio_button->set_hotKey(pHotkey);

	_radio_button->set_checkBox_color(pColor);
	_radio_button->set_label_color(pColor);

	_radio_button->set_label_mouse_over_color(pMouseOverColor);
	_radio_button->set_checkBox_mouse_over_color(pMouseOverColor);

	_radio_button->set_label_pressed_color(pPressedColor);
	_radio_button->set_checkBox_pressed_color(pPressedColor);

	_radio_button->set_label_focused_color(pFocusedColor);
	_radio_button->set_checkBox_focused_color(pFocusedColor);

	_radio_button->is_default = pIsDefault;

	return S_OK;
}

_Use_decl_annotations_
DX_EXP HRESULT w_widget::add_comboBox(int pID,
	int pX, int pY,
	int pWidth, int pHeight,
	int pTextOffsetX, int pTextOffsetY,
	UINT pHotkey,
	bool pIsDefault,
	w_comboBox** pCreatedControl)
{
	return add_comboBox(pID,
		pX, pY,
		pWidth, pHeight,
		pTextOffsetX, pTextOffsetY,
		pHotkey,
		pIsDefault,
		W_COLOR(200, 200, 200, 150),
		W_COLOR(255, 255, 0, 255),
		W_COLOR(255, 255, 255, 200),
		W_COLOR(255, 255, 0, 170),
		W_COLOR(255, 255, 255, 150),
		W_COLOR(255, 255, 0, 255),
		W_COLOR(150, 150, 150, 255),
		W_COLOR(255, 255, 255, 200),
		W_COLOR(255, 255, 255, 255),
		W_COLOR(255, 0, 0, 155),
		W_COLOR(255, 255, 255, 255),
		pCreatedControl);
}

_Use_decl_annotations_
HRESULT w_widget::add_comboBox(int pID,
	int pX, int pY,
	int pWidth, int pHeight,
	int pTextOffsetX, int pTextOffsetY,
	UINT pHotkey,
	bool pIsDefault,
	W_COLOR pColor, W_COLOR pMouseOverColor, W_COLOR pPressedColor, W_COLOR pFocusedColor,
	W_COLOR pButtonColor, W_COLOR pButtonMouseOverColor, W_COLOR pButtonPressedColor, W_COLOR pButtonFocusedColor,
	W_COLOR pDropDownColor,
	W_COLOR pSelectionColor, W_COLOR pSelectionFontColor,
	w_comboBox** pCreatedControl)
{
	auto _comboBox = new (std::nothrow) w_comboBox(this);

	if (pCreatedControl)
	{
		*pCreatedControl = _comboBox;
	}
	if (!_comboBox) return E_OUTOFMEMORY;

	auto _hr = add_control(_comboBox);
	if (FAILED(_hr)) return _hr;

	// Set the ID and list index
	_comboBox->set_ID(pID);
	_comboBox->set_location(pX, pY);
	_comboBox->set_size(pWidth, pHeight);
	_comboBox->set_original_text_offset_x(pTextOffsetX);
	_comboBox->set_original_text_offset_y(pTextOffsetY);
	_comboBox->set_hotKey(pHotkey);
	_comboBox->is_default = pIsDefault;
	_comboBox->set_font_size(this->_sprite_batch->get_debug_font_size());

	//set comboBox colors
	_comboBox->set_comboBox_color(pColor);
	_comboBox->set_comboBox_mouse_over_color(pMouseOverColor);
	_comboBox->set_comboBox_pressed_color(pPressedColor);
	_comboBox->set_label_focused_color(pFocusedColor);

	//set comboBox button color
	_comboBox->set_comboBox_button_color(pButtonColor);
	_comboBox->set_comboBox_button_mouse_over_color(pButtonMouseOverColor);
	_comboBox->set_comboBox_button_pressed_color(pButtonPressedColor);
	_comboBox->set_comboBox_button_focused_color(pButtonFocusedColor);

	//set comboBox drop down color
	_comboBox->set_comboBox_button_color(pDropDownColor);

	//set comboBox selection color
	_comboBox->set_comboBox_selected_color(pSelectionColor);
	_comboBox->set_comboBox_selection_font_color(pSelectionFontColor);

	return S_OK;
}

_Use_decl_annotations_
HRESULT w_widget::add_slider(int pID,
	int pX, int pY,
	int pWidth, int pHeight,
	int pMin, int pMax,
	int pValue,
	bool pIsDefault,
	W_COLOR pMainColor, W_COLOR pMainMouseOverColor, W_COLOR pMainPressedColor, W_COLOR pMainFocusedColor,
	W_COLOR pButtonColor, W_COLOR pButtonMouseOverColor, W_COLOR pButtonPressedColor, W_COLOR pButtonFocusedColor,
	_Out_opt_ w_slider** pCreatedControl)
{
	auto _slider = new (std::nothrow) w_slider(this);

	if (pCreatedControl)
	{
		*pCreatedControl = _slider;
	}
	if (!_slider) return E_OUTOFMEMORY;

	auto _hr = add_control(_slider);
	if (FAILED(_hr)) return _hr;

	// Set the ID and list index
	_slider->set_ID(pID);
	_slider->set_location(pX, pY);
	_slider->set_size(pWidth, pHeight);
	_slider->set_range(pMin, pMax);
	_slider->set_value(pValue);
	
	_slider->set_main_color(pMainColor);
	_slider->set_main_mouse_over_color(pMainMouseOverColor);
	_slider->set_main_pressed_color(pMainPressedColor);
	_slider->set_main_focused_color(pMainFocusedColor);
	
	_slider->set_button_color(pButtonColor);
	_slider->set_button_mouse_over_color(pButtonMouseOverColor);
	_slider->set_button_pressed_color(pButtonPressedColor);
	_slider->set_button_focused_color(pButtonFocusedColor);

	_slider->is_default = pIsDefault;
	_slider->update_rects();

	return S_OK;
}

_Use_decl_annotations_
HRESULT w_widget::add_listBox(int pID, 
	int pX, int pY, 
	int pWidth, int pHeight, 
	DWORD pStyle, 
	w_listBox** pCreatedControl)
{
	auto _listBox = new (std::nothrow) w_listBox(this);

	if (pCreatedControl)
	{
		*pCreatedControl = _listBox;
	}

	if (!_listBox) return E_OUTOFMEMORY;

	auto hr = add_control(_listBox);
	if (FAILED(hr)) return hr;

	// Set the ID and position
	_listBox->set_ID(pID);
	_listBox->set_location(pX, pY);
	_listBox->set_size(pWidth, pHeight);
	_listBox->set_style(pStyle);

	return S_OK;
}

//_Use_decl_annotations_
//HRESULT w_widget::add_editBox(int pID, LPCWSTR pText, int pX, int pY, int pWidth, int pHeight, bool pIsDefault, w_editBox** pCreatedControl)
//{
//	HRESULT hr = S_OK;
//
//	auto pEditBox = new (std::nothrow) w_editBox(this);
//
//	if (pCreatedControl)
//	{
//		*pCreatedControl = pEditBox;
//	}
//
//	if (!pEditBox) return E_OUTOFMEMORY;
//
//	hr = add_control(pEditBox);
//	if (FAILED(hr)) return hr;
//
//	// Set the ID and position
//	pEditBox->set_ID(pID);
//	pEditBox->set_location(pX, pY);
//	pEditBox->set_size(pWidth, pHeight);
//	pEditBox->is_default = pIsDefault;
//
//	if (pText)
//	{
//		pEditBox->set_text(pText);
//	}
//
//	return S_OK;
//}
//

_Use_decl_annotations_
HRESULT w_widget::add_image_to_resource_manager(_In_z_ const wchar_t* pPath)
{
	return this->_sprite_batch->add_image_from_file(pPath);
}

_Use_decl_annotations_
bool w_widget::on_msg_proc(HWND pHWnd, UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	bool bHandled = false;

	// For invisible dialog, do not handle anything.
	if (!this->visible) return false;

	// If automation command-line switch is on, enable this dialog's keyboard input
	// upon any key press or mouse click.
	if (/*DXUTGetAutomation() &&*/
		(WM_LBUTTONDOWN == pMsg || WM_LBUTTONDBLCLK == pMsg || WM_KEYDOWN == pMsg))
	{
		this->_widgets_resource_manager->enable_keyboard_input_for_all_widgets();
	}

	// If caption is enable, check for clicks in the caption area.
	if (this->caption_enable)
	{
		if (pMsg == WM_LBUTTONDOWN || pMsg == WM_LBUTTONDBLCLK)
		{
			POINT mousePoint =
			{
				short(LOWORD(pLParam)), short(HIWORD(pLParam))
			};

			if (mousePoint.x >= this->_x && mousePoint.x < this->_x + this->_width &&
				mousePoint.y >= this->_y && mousePoint.y < this->_y + this->_caption_height)
			{
				this->drag = true;
				SetCapture(w_widgets_resource_manager::get_HWND());
				return true;
			}
		}
		else if (pMsg == WM_LBUTTONUP && this->drag)
		{
			POINT mousePoint =
			{
				short(LOWORD(pLParam)), short(HIWORD(pLParam))
			};

			if (mousePoint.x >= this->_x && mousePoint.x < this->_x + this->_width &&
				mousePoint.y >= this->_y && mousePoint.y < this->_y + this->_caption_height)
			{
				ReleaseCapture();
				this->drag = false;
				this->minimized = !this->minimized;
				return true;
			}
		}
	}

	// If the dialog is minimized, don't send any messages to controls.
	if (this->minimized) return false;

	// If a control is in focus, it belongs to this dialog, and it's enabled, then give
	// it the first chance at handling the message.
	if (s_pControlFocus && s_pControlFocus->parent_widget == this && s_pControlFocus->get_enabled())
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
		POINT pt =
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
		if (s_pControlFocus && s_pControlFocus->parent_widget == this && s_pControlFocus->get_enabled())
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
		if (s_pControlFocus && s_pControlFocus->parent_widget == this && s_pControlFocus->get_enabled())
		{
			for (auto it = this->controls.cbegin(); it != this->controls.cend(); ++it)
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
		if (pMsg == WM_KEYDOWN && (!s_pControlFocus ||
			(s_pControlFocus->get_type() != W_GUI_CONTROL_EDITBOX
				&& s_pControlFocus->get_type() != W_GUI_CONTROL_IMEEDITBOX)))
		{
			for (auto it = this->controls.begin(); it != this->controls.end(); ++it)
			{
				if ((*it)->get_hotKey() == pWParam)
				{
					(*it)->on_hotKey();
					return true;
				}
			}
		}

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
		// If not accepting mouse input, return false to indicate the message should still 
		// be handled by the application (usually to move the camera).
		if (!this->mouse_input) return false;

		POINT mousePoint =
		{
			short(LOWORD(pLParam)), short(HIWORD(pLParam))
		};
		mousePoint.x -= this->_x;
		mousePoint.y -= this->_y;

		// If caption is enabled, offset the Y coordinate by the negative of its height.
		if (this->caption_enable)
		{
			mousePoint.y -= this->_caption_height;
		}

		//If a control is in focus, it belongs to this dialog, and it's enabled, then give it the first chance at handling the message.
		if (s_pControlFocus &&
			s_pControlFocus->parent_widget == this &&
			s_pControlFocus->get_enabled())
		{
			if (s_pControlFocus->handle_mouse(pMsg, mousePoint, pWParam, pLParam))
			{
				return true;
			}
		}

		// Not yet handled, see if the mouse is over any controls
		auto pControl = get_control_at_point(mousePoint);
		if (pControl && pControl->get_enabled())
		{
			bHandled = pControl->handle_mouse(pMsg, mousePoint, pWParam, pLParam);
			if (bHandled)
			{
				return true;
			}
		}
		else
		{
			// Mouse not over any controls in this dialog, if there was a control which had focus it just lost it
			if (pMsg == WM_LBUTTONDOWN && s_pControlFocus && s_pControlFocus->parent_widget == this)
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
			on_mouse_move(mousePoint);
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
		if ((HWND) pLParam != this->_widgets_resource_manager->get_HWND())
		{
			this->drag = false;
		}
	}
	}

	return false;
}

void w_widget::on_mouse_up(_In_ const POINT& pPoint)
{
	W_UNUSED(pPoint);
	s_pControlPressed = nullptr;
	this->control_mouse_over = nullptr;
	this->_is_mouse_over_widget = false;
}

void w_widget::on_mouse_move(_In_ const POINT& pPoint)
{
	// Figure out which control the mouse is over now
	auto pControl = get_control_at_point(pPoint);

	this->_is_mouse_over_widget = false;
	if (pPoint.x >= 0 && pPoint.x <= this->_width &&
		pPoint.y >= 0 && pPoint.y <= this->_height)
	{
		this->_is_mouse_over_widget = true;
	}
	
	// If the mouse is still over the same control, nothing needs to be done
	if (pControl == this->control_mouse_over)
	{
		return;
	}
	// Handle mouse leaving the old control
	if (this->control_mouse_over)
	{
		this->control_mouse_over->on_mouse_leave();
	}
	// Handle mouse entering the new control
	this->control_mouse_over = pControl;
	if (pControl)
	{
		this->control_mouse_over->on_mouse_enter();
	}
}

HRESULT w_widget::initialize_control(_In_ w_control* pControl)
{
	if (!pControl)
	{
		return E_INVALIDARG;
	}
	pControl->index = static_cast<UINT>(this->controls.size());

	// Look for a default Element entries
	for (auto it = this->default_elements.begin(); it != this->default_elements.end(); ++it)
	{
		if ((*it)->control_type == pControl->get_type())
		{
			pControl->set_element((*it)->index_element, &(*it)->element);
		}
	}

	auto _hr = pControl->on_initialize(this->_gDevice);

	return _hr;
}

HRESULT w_widget::add_control(_In_ w_control* pControl)
{
	auto _hr = initialize_control(pControl);
	V(_hr, L"initializing control", _super::name, 2);

	// Add to the list
	this->controls.push_back(pControl);

	return S_OK;
}

void w_widget::clear_radioButton_group(_In_ UINT pButtonGroup)
{
	// Find all radio buttons with the given group number
	for (auto it = this->controls.cbegin(); it != this->controls.cend(); ++it)
	{
		////////////////////////////////if ((*it)->get_type() == W_GUI_CONTROL_RADIOBUTTON)
		////////////////////////////////{
		////////////////////////////////	auto pRadioButton = (w_radioButton*) *it;

		////////////////////////////////	if (pRadioButton->get_button_group() == pButtonGroup)
		////////////////////////////////	{
		////////////////////////////////		pRadioButton->set_checked(false, false);
		////////////////////////////////	}
		////////////////////////////////}
	}
}

void w_widget::clear_comboBox(_In_ int pID)
{
	////////////////////////////////auto _comboBox = reinterpret_cast<w_comboBox*>(get_control(pID, W_GUI_CONTROL_COMBOBOX));
	////////////////////////////////if (!_comboBox)	return;
	////////////////////////////////_comboBox->remove_all_items();
}

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

_Use_decl_annotations_
HRESULT w_widget::draw_rect(const RECT* pRect, DWORD pColor)
{
	W_UNUSED(pRect);
	W_UNUSED(pColor);
	return E_FAIL;
}

_Use_decl_annotations_
HRESULT w_widget::draw_sprite(w_element* pElement, const RECT* pRectDest, float pDepth)
{
	// No need to draw fully transparent layers
	if (pElement->texture_color.current_color_state.w == 0) return S_OK;

	RECT _rectangle_texture = pElement->rect_texture;

	RECT rcScreen = *pRectDest;
	OffsetRect(&rcScreen, this->_x, this->_y);

	// If caption is enabled, offset the Y position by its height.
	if (this->caption_enable)
	{
		OffsetRect(&rcScreen, 0, this->_caption_height);
	}
	auto _texture_node = get_texture(pElement->index_texture);
	if (!_texture_node) return E_FAIL;

	float fBBWidth = (float)this->_widgets_resource_manager->backBuffer_width;
	float fBBHeight = (float)this->_widgets_resource_manager->backBuffer_height;
	float fTexWidth = (float) _texture_node->dwWidth;
	float fTexHeight = (float) _texture_node->dwHeight;

	float fRectLeft = rcScreen.left / fBBWidth;
	float fRectTop = 1.0f - rcScreen.top / fBBHeight;
	float fRectRight = rcScreen.right / fBBWidth;
	float fRectBottom = 1.0f - rcScreen.bottom / fBBHeight;

	fRectLeft = fRectLeft * 2.0f - 1.0f;
	fRectTop = fRectTop * 2.0f - 1.0f;
	fRectRight = fRectRight * 2.0f - 1.0f;
	fRectBottom = fRectBottom * 2.0f - 1.0f;

	float fTexLeft = _rectangle_texture.left / fTexWidth;
	float fTexTop = _rectangle_texture.top / fTexHeight;
	float fTexRight = _rectangle_texture.right / fTexWidth;
	float fTexBottom = _rectangle_texture.bottom / fTexHeight;

	// Add 6 sprite vertices
	w_sprite_vertex _sprite_vertex;

	// tri1
	_sprite_vertex.pos = DirectX::XMFLOAT3(fRectLeft, fRectTop, pDepth);
	_sprite_vertex.tex = DirectX::XMFLOAT2(fTexLeft, fTexTop);
	_sprite_vertex.color = pElement->texture_color.current_color_state;
	this->_widgets_resource_manager->m_SpriteVertices.push_back(_sprite_vertex);

	_sprite_vertex.pos = DirectX::XMFLOAT3(fRectRight, fRectTop, pDepth);
	_sprite_vertex.tex = DirectX::XMFLOAT2(fTexRight, fTexTop);
	_sprite_vertex.color = pElement->texture_color.current_color_state;
	this->_widgets_resource_manager->m_SpriteVertices.push_back(_sprite_vertex);

	_sprite_vertex.pos = DirectX::XMFLOAT3(fRectLeft, fRectBottom, pDepth);
	_sprite_vertex.tex = DirectX::XMFLOAT2(fTexLeft, fTexBottom);
	_sprite_vertex.color = pElement->texture_color.current_color_state;
	this->_widgets_resource_manager->m_SpriteVertices.push_back(_sprite_vertex);

	// tri2
	_sprite_vertex.pos = DirectX::XMFLOAT3(fRectRight, fRectTop, pDepth);
	_sprite_vertex.tex = DirectX::XMFLOAT2(fTexRight, fTexTop);
	_sprite_vertex.color = pElement->texture_color.current_color_state;
	this->_widgets_resource_manager->m_SpriteVertices.push_back(_sprite_vertex);

	_sprite_vertex.pos = DirectX::XMFLOAT3(fRectRight, fRectBottom, pDepth);
	_sprite_vertex.tex = DirectX::XMFLOAT2(fTexRight, fTexBottom);
	_sprite_vertex.color = pElement->texture_color.current_color_state;
	this->_widgets_resource_manager->m_SpriteVertices.push_back(_sprite_vertex);

	_sprite_vertex.pos = DirectX::XMFLOAT3(fRectLeft, fRectBottom, pDepth);
	_sprite_vertex.tex = DirectX::XMFLOAT2(fTexLeft, fTexBottom);
	_sprite_vertex.color = pElement->texture_color.current_color_state;
	this->_widgets_resource_manager->m_SpriteVertices.push_back(_sprite_vertex);

	// Why are we drawing the sprite every time?  This is very inefficient, but the sprite workaround doesn't have support for sorting now, so we have to
	// draw a sprite every time to keep the order correct between sprites and text.
	this->_widgets_resource_manager->end_sprites(this->_gDevice->device.Get(), this->_gDevice->context.Get());

	return S_OK;
}

HRESULT w_widget::draw_image(_In_z_ const wchar_t* pIconName,
	const DirectX::XMFLOAT2 pPosition,
	const DirectX::XMFLOAT2 pScale, const DirectX::XMFLOAT2 pScaleCenter,
	const float pRotationAngle, const DirectX::XMFLOAT2 pRotationCenter,
	const DirectX::XMFLOAT2 pTranslation,
	D2D1_GAUSSIANBLUR_PROP pEffectParam, const float pEffectValue)
{
	this->_sprite_batch->begin();
	this->_sprite_batch->set_gaussian_blur_value(pEffectParam, pEffectValue);
	this->_sprite_batch->draw_image(pIconName, pPosition, pScale, pScaleCenter, pRotationAngle, pRotationCenter, pTranslation);
	this->_sprite_batch->end();

	return S_OK;
}

HRESULT w_widget::draw_image(_In_z_ const wchar_t* pIconName,
	const DirectX::XMFLOAT2 pPosition, const D2D1_RECT_F pRectangle,
	const DirectX::XMFLOAT2 pScale, const DirectX::XMFLOAT2 pScaleCenter,
	const float pRotationAngle, const DirectX::XMFLOAT2 pRotationCenter,
	const DirectX::XMFLOAT2 pTranslation,
	D2D1_GAUSSIANBLUR_PROP pEffectParam, const float pEffectValue)
{
	this->_sprite_batch->begin();
	this->_sprite_batch->set_gaussian_blur_value(pEffectParam, pEffectValue);
	this->_sprite_batch->draw_image(pIconName, pPosition, pRectangle, pScale, pScaleCenter, pRotationAngle, pRotationCenter, pTranslation);
	this->_sprite_batch->end();

	return S_OK;
}

_Use_decl_annotations_
HRESULT w_widget::calc_textRect(LPCWSTR pText, w_element* pElement, const RECT* pRectDest, int pCount)
{
	auto pFontNode = get_font(pElement->index_font);
	if (!pFontNode)
	{
		return E_FAIL;
	}

	W_UNUSED(pText);
	W_UNUSED(pRectDest);
	W_UNUSED(pCount);

	return S_OK;
}

_Use_decl_annotations_
HRESULT w_widget::draw_text(_In_z_ std::wstring pText, DirectX::XMFLOAT2 pPosition, _In_ ID2D1SolidColorBrush* pBrush, bool bShadow)
{
	// If caption is enabled, offset the Y position by its height.
	/*if (this->caption_enable)
	{
	OffsetRect(&rcScreen, 0, this->caption_height);
	}*/
	//float fBBWidth = (float)this->_widgets_resource_manager->backBuffer_width;
	//float fBBHeight = (float)this->_widgets_resource_manager->backBuffer_height;

	if (this->_sprite_batch != nullptr)
	{
		this->_sprite_batch->begin();
		this->_sprite_batch->draw_string(pText, pPosition, pBrush);
		this->_sprite_batch->end();
	}

	//if (pShadow)
	//{
	//	RECT rcShadow = rcScreen;
	//	OffsetRect(&rcShadow, 1, 1);

	//	DirectX::XMFLOAT4 vShadowColor(0, 0, 0, 1.0f);
	//	this->_widgets_resource_manager->draw_text(
	//		pDevice,
	//		pContext,
	//		pText,
	//		rcShadow,
	//		vShadowColor,
	//		fBBWidth,
	//		fBBHeight,
	//		pCenter);
	//}

	//DirectX::XMFLOAT4 _font_color(pElement->font_color.current_state.x, pElement->font_color.current_state.y, pElement->font_color.current_state.z, 1.0f);
	//this->_widgets_resource_manager->draw_text(
	//	pDevice,
	//	pContext,
	//	pText,
	//	rcScreen, 
	//	_font_color,
	//	fBBWidth, 
	//	fBBHeight, 
	//	pCenter);

	return S_OK;
}

void w_widget::clear_focus()
{
	if (s_pControlFocus)
	{
		s_pControlFocus->on_focus_out();
		s_pControlFocus = nullptr;
	}

	ReleaseCapture();
}

void w_widget::focus_default_control()
{
	// Check for default control in this dialog
	for (auto it = this->controls.cbegin(); it != this->controls.cend(); ++it)
	{
		if ((*it)->is_default)
		{
			// Remove focus from the current control
			clear_focus();

			// Give focus to the default control
			s_pControlFocus = *it;
			s_pControlFocus->on_focus_in();
			return;
		}
	}
}

bool w_widget::on_cycle_focus(_In_ bool pForward)
{
	w_control* pControl = nullptr;
	w_widget* pWidget = nullptr; // pDialog and pLastDialog are used to track wrapping of
	w_widget* pLastWidget;    // focus from first control to last or vice versa.

	if (!s_pControlFocus)
	{
		// If s_pControlFocus is nullptr, we focus the first control of first dialog in
		// the case that bForward is true, and focus the last control of last dialog when
		// bForward is false.
		//
		if (pForward)
		{
			// Search for the first control from the start of the dialog
			// array.
			for (auto it = this->_widgets_resource_manager->widgets.cbegin(); it != this->_widgets_resource_manager->widgets.cend(); ++it)
			{
				pWidget = pLastWidget = *it;
				if (pWidget && !pWidget->controls.empty())
				{
					pControl = pWidget->controls[0];
					break;
				}
			}

			if (!pWidget || !pControl)
			{
				// No dialog has been registered yet or no controls have been
				// added to the dialogs. Cannot proceed.
				return true;
			}
		}
		else
		{
			// Search for the first control from the end of the dialog
			// array.
			for (auto it = this->_widgets_resource_manager->widgets.crbegin(); it != this->_widgets_resource_manager->widgets.crend(); ++it)
			{
				pWidget = pLastWidget = *it;
				if (pWidget && !pWidget->controls.empty())
				{
					pControl = pWidget->controls[pWidget->controls.size() - 1];
					break;
				}
			}

			if (!pWidget || !pControl)
			{
				// No dialog has been registered yet or no controls have been
				// added to the dialogs. Cannot proceed.
				return true;
			}
		}
	}
	else if (s_pControlFocus->parent_widget != this)
	{
		// If a control belonging to another dialog has focus, let that other dialog handle this event by returning false.
		return false;
	}
	else
	{
		// Focused control belongs to this dialog. Cycle to the next/previous control.
		assert(pControl != 0);
		_Analysis_assume_(pControl != 0);
		pLastWidget = s_pControlFocus->parent_widget;
		pControl = (pForward) ? get_next_control(s_pControlFocus) : get_prev_control(s_pControlFocus);
		pWidget = pControl->parent_widget;
	}

	assert(pControl != 0);
	_Analysis_assume_(pControl != 0);

	for (int i = 0; i < 0xffff; i++)
	{
		// If we just wrapped from last control to first or vice versa,
		// set the focused control to nullptr. This state, where no control
		// has focus, allows the camera to work.
		int nLastDialogIndex = -1;
		auto fit = std::find(this->_widgets_resource_manager->widgets.cbegin(), this->_widgets_resource_manager->widgets.cend(), pLastWidget);
		if (fit != this->_widgets_resource_manager->widgets.cend())
		{
			nLastDialogIndex = int(fit - this->_widgets_resource_manager->widgets.begin());
		}

		int nDialogIndex = -1;
		fit = std::find(this->_widgets_resource_manager->widgets.cbegin(), this->_widgets_resource_manager->widgets.cend(), pWidget);
		if (fit != this->_widgets_resource_manager->widgets.cend())
		{
			nDialogIndex = int(fit - this->_widgets_resource_manager->widgets.begin());
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

		// If the dialog accepts keybord input and the control can have focus then
		// move focus
		if (pControl->parent_widget->keyboard_input && pControl->can_have_focus())
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
		pWidget = pControl->parent_widget;
	}

	// If we reached this point, the chain of dialogs didn't form a complete loop
	V(S_FALSE, L"multiple dialogs are improperly chained together", _super::name, 2);
	return false;
}

void w_widget::initialize_default_elements()
{
	set_font(0, L"Arial", 14, FW_NORMAL);

	w_element Element;
	RECT rcTexture;

	//-------------------------------------
	// Element for the caption
	//-------------------------------------
	this->caption_element.set_font(0);
	SetRect(&rcTexture, 17, 269, 241, 287);
	this->caption_element.set_texture(0, &rcTexture);
	this->caption_element.texture_color.color_states[W_GUI_STATE_NORMAL] = RGBA_TO_DWORD_COLOR(255, 255, 255, 255);
	//this->caption_element.font_color.color_states[W_GUI_STATE_NORMAL] = ARGB_TO_DWORD_COLOR(255, 255, 255, 255);
	this->caption_element.set_font(0, RGBA_TO_DWORD_COLOR(255, 255, 255, 255), DT_LEFT | DT_VCENTER);
	// Pre-blend as we don't need to transition the state
	this->caption_element.texture_color.blend(W_GUI_STATE_NORMAL, 10.0f);
	this->caption_element.font_color.blend(W_GUI_STATE_NORMAL, 10.0f);

	//-------------------------------------
	// CDXUTStatic
	//-------------------------------------
	Element.set_font(0);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_LABEL, 0, &Element);

	//-------------------------------------
	// CDXUTButton - Button
	//-------------------------------------
	set_default_element(W_GUI_CONTROL_IMAGE, 0, &Element);
	//-------------------------------------
	// CDXUTButton - TAB
	//-------------------------------------
	set_default_element(W_GUI_CONTROL_TAB, 0, &Element);

	//-------------------------------------
	// CDXUTButton - Button
	//-------------------------------------
	SetRect(&rcTexture, 0, 0, 136, 54);
	Element.set_texture(0, &rcTexture);
	Element.set_font(0);
	//Element.texture_color.color_states[W_GUI_STATE_NORMAL] = ARGB_TO_D3DCOLOR(255, 255, 255, 255);
	//Element.texture_color.color_states[W_GUI_STATE_PRESSED] = ARGB_TO_D3DCOLOR(200, 255, 255, 255);
	//Element.font_color.color_states[W_GUI_STATE_MOUSEOVER] = ARGB_TO_D3DCOLOR(255, 0, 0, 0);

	//// Assign the Element
	set_default_element(W_GUI_CONTROL_BUTTON, 0, &Element);

	//-------------------------------------
	// CDXUTButton - Fill layer
	//-------------------------------------
	SetRect(&rcTexture, 136, 0, 252, 54);
	Element.set_texture(0, &rcTexture, RGBA_TO_DWORD_COLOR(255, 255, 255, 0));
	//Element.texture_color.color_states[W_GUI_STATE_MOUSEOVER] = RGBA_TO_DWORD_COLOR(255, 255, 255, 160);
	//Element.texture_color.color_states[W_GUI_STATE_PRESSED] = RGBA_TO_DWORD_COLOR(0, 0, 0, 60);
	//Element.texture_color.color_states[W_GUI_STATE_FOCUS] = RGBA_TO_DWORD_COLOR(255, 255, 255, 30);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_BUTTON, 1, &Element);

	//-------------------------------------
	// CDXUTCheckBox - Box
	//-------------------------------------
	SetRect(&rcTexture, 0, 54, 27, 81);
	Element.set_texture(0, &rcTexture);
	//Element.set_font(0, RGBA_TO_DWORD_COLOR(255, 255, 255, 255), DT_LEFT | DT_VCENTER);
	//Element.font_color.color_states[W_GUI_STATE_DISABLED] = RGBA_TO_DWORD_COLOR(200, 200, 200, 200);
	//Element.texture_color.color_states[W_GUI_STATE_NORMAL] = RGBA_TO_DWORD_COLOR(255, 255, 255, 150);
	//Element.texture_color.color_states[W_GUI_STATE_FOCUS] = RGBA_TO_DWORD_COLOR(255, 255, 255, 200);
	//Element.texture_color.color_states[W_GUI_STATE_PRESSED] = RGBA_TO_DWORD_COLOR(255, 255, 255, 255);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_CHECKBOX, 0, &Element);

	//-------------------------------------
	// CDXUTCheckBox - Check
	//-------------------------------------
	SetRect(&rcTexture, 27, 54, 54, 81);
	Element.set_texture(0, &rcTexture);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_CHECKBOX, 1, &Element);

	//-------------------------------------
	// CDXUTRadioButton - Box
	//-------------------------------------
	SetRect(&rcTexture, 54, 54, 81, 81);
	Element.set_texture(0, &rcTexture);
	Element.set_font(0, RGBA_TO_DWORD_COLOR(255, 255, 255, 255), DT_LEFT | DT_VCENTER);
	//Element.font_color.color_states[W_GUI_STATE_DISABLED] = RGBA_TO_DWORD_COLOR(200, 200, 200, 200);
	//Element.texture_color.color_states[W_GUI_STATE_NORMAL] = RGBA_TO_DWORD_COLOR(255, 255, 255, 150);
	//Element.texture_color.color_states[W_GUI_STATE_FOCUS] = RGBA_TO_DWORD_COLOR(255, 255, 255, 200);
	//Element.texture_color.color_states[W_GUI_STATE_PRESSED] = RGBA_TO_DWORD_COLOR(255, 255, 255, 255);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_RADIOBUTTON, 0, &Element);

	//-------------------------------------
	// CDXUTRadioButton - Check
	//-------------------------------------
	SetRect(&rcTexture, 81, 54, 108, 81);
	Element.set_texture(0, &rcTexture);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_RADIOBUTTON, 1, &Element);

	//-------------------------------------
	// CDXUTComboBox - Main
	//-------------------------------------
	SetRect(&rcTexture, 7, 81, 247, 123);
	Element.set_texture(0, &rcTexture);
	Element.set_font(0);
	//Element.texture_color.color_states[W_GUI_STATE_NORMAL] = RGBA_TO_DWORD_COLOR(200, 200, 200, 150);
	//Element.texture_color.color_states[W_GUI_STATE_FOCUS] = RGBA_TO_DWORD_COLOR(230, 230, 230, 170);
	//Element.texture_color.color_states[W_GUI_STATE_DISABLED] = RGBA_TO_DWORD_COLOR(200, 200, 200, 70);
	//Element.font_color.color_states[W_GUI_STATE_MOUSEOVER] = RGBA_TO_DWORD_COLOR(0, 0, 0, 255);
	//Element.font_color.color_states[W_GUI_STATE_PRESSED] = RGBA_TO_DWORD_COLOR(0, 0, 0, 255);
	//Element.font_color.color_states[W_GUI_STATE_DISABLED] = RGBA_TO_DWORD_COLOR(200, 200, 200, 200);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_COMBOBOX, 0, &Element);

	//-------------------------------------
	// CDXUTComboBox - Button
	//-------------------------------------
	SetRect(&rcTexture, 98, 189, 151, 238);
	Element.set_texture(0, &rcTexture);
	//Element.texture_color.color_states[W_GUI_STATE_NORMAL] = RGBA_TO_DWORD_COLOR(255, 255, 255, 150);
	//Element.texture_color.color_states[W_GUI_STATE_PRESSED] = RGBA_TO_DWORD_COLOR(150, 150, 150, 255);
	//Element.texture_color.color_states[W_GUI_STATE_FOCUS] = RGBA_TO_DWORD_COLOR(255, 255, 255, 200);
	//Element.texture_color.color_states[W_GUI_STATE_DISABLED] = RGBA_TO_DWORD_COLOR(255, 255, 255, 70);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_COMBOBOX, 1, &Element);

	//-------------------------------------
	// CDXUTComboBox - Dropdown
	//-------------------------------------
	SetRect(&rcTexture, 13, 123, 241, 160);
	Element.set_texture(0, &rcTexture);
	//Element.set_font(0, RGBA_TO_DWORD_COLOR(0, 0, 0, 255), DT_LEFT | DT_TOP);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_COMBOBOX, 2, &Element);

	//-------------------------------------
	// CDXUTComboBox - Selection
	//-------------------------------------
	SetRect(&rcTexture, 12, 163, 239, 183);
	Element.set_texture(0, &rcTexture);
	//Element.set_font(0, RGBA_TO_DWORD_COLOR(255, 255, 255, 255), DT_LEFT | DT_TOP);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_COMBOBOX, 3, &Element);

	//-------------------------------------
	// CDXUTSlider - Track
	//-------------------------------------
	SetRect(&rcTexture, 1, 187, 93, 228);
	Element.set_texture(0, &rcTexture);
	//Element.texture_color.color_states[W_GUI_STATE_NORMAL] = RGBA_TO_DWORD_COLOR(255, 255, 255, 150);
	//Element.texture_color.color_states[W_GUI_STATE_FOCUS] = RGBA_TO_DWORD_COLOR(255, 255, 255, 200);
	//Element.texture_color.color_states[W_GUI_STATE_DISABLED] = RGBA_TO_DWORD_COLOR(255, 255, 255, 70);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_SLIDER, 0, &Element);

	//-------------------------------------
	// CDXUTSlider - Button
	//-------------------------------------
	SetRect(&rcTexture, 151, 193, 192, 234);
	Element.set_texture(0, &rcTexture);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_SLIDER, 1, &Element);

	//-------------------------------------
	// CDXUTScrollBar - Track
	//-------------------------------------
	int nScrollBarStartX = 196;
	int nScrollBarStartY = 191;
	SetRect(&rcTexture, nScrollBarStartX + 0, nScrollBarStartY + 21, nScrollBarStartX + 22, nScrollBarStartY + 32);
	Element.set_texture(0, &rcTexture);
	//Element.texture_color.color_states[W_GUI_STATE_DISABLED] = RGBA_TO_DWORD_COLOR(200, 200, 200, 255);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_SCROLLBAR, 0, &Element);

	//-------------------------------------
	// CDXUTScrollBar - Up Arrow
	//-------------------------------------
	SetRect(&rcTexture, nScrollBarStartX + 0, nScrollBarStartY + 1, nScrollBarStartX + 22, nScrollBarStartY + 21);
	Element.set_texture(0, &rcTexture);
	//Element.texture_color.color_states[W_GUI_STATE_DISABLED] = RGBA_TO_DWORD_COLOR(200, 200, 200, 255);


	// Assign the Element
	set_default_element(W_GUI_CONTROL_SCROLLBAR, 1, &Element);

	//-------------------------------------
	// CDXUTScrollBar - Down Arrow
	//-------------------------------------
	SetRect(&rcTexture, nScrollBarStartX + 0, nScrollBarStartY + 32, nScrollBarStartX + 22, nScrollBarStartY + 53);
	Element.set_texture(0, &rcTexture);
	//Element.texture_color.color_states[W_GUI_STATE_DISABLED] = RGBA_TO_DWORD_COLOR(200, 200, 200, 255);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_SCROLLBAR, 2, &Element);

	//-------------------------------------
	// CDXUTScrollBar - Button
	//-------------------------------------
	SetRect(&rcTexture, 220, 192, 238, 234);
	Element.set_texture(0, &rcTexture);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_SCROLLBAR, 3, &Element);


	//-------------------------------------
	// CDXUTEditBox
	//-------------------------------------
	// Element assignment:
	//   0 - text area
	//   1 - top left border
	//   2 - top border
	//   3 - top right border
	//   4 - left border
	//   5 - right border
	//   6 - lower left border
	//   7 - lower border
	//   8 - lower right border

	Element.set_font(0, RGBA_TO_DWORD_COLOR(0, 0, 0, 255), DT_LEFT | DT_TOP);

	// Assign the style
	SetRect(&rcTexture, 14, 90, 241, 113);
	Element.set_texture(0, &rcTexture);
	set_default_element(W_GUI_CONTROL_EDITBOX, 0, &Element);
	SetRect(&rcTexture, 8, 82, 14, 90);
	Element.set_texture(0, &rcTexture);
	set_default_element(W_GUI_CONTROL_EDITBOX, 1, &Element);
	SetRect(&rcTexture, 14, 82, 241, 90);
	Element.set_texture(0, &rcTexture);
	set_default_element(W_GUI_CONTROL_EDITBOX, 2, &Element);
	SetRect(&rcTexture, 241, 82, 246, 90);
	Element.set_texture(0, &rcTexture);
	set_default_element(W_GUI_CONTROL_EDITBOX, 3, &Element);
	SetRect(&rcTexture, 8, 90, 14, 113);
	Element.set_texture(0, &rcTexture);
	set_default_element(W_GUI_CONTROL_EDITBOX, 4, &Element);
	SetRect(&rcTexture, 241, 90, 246, 113);
	Element.set_texture(0, &rcTexture);
	set_default_element(W_GUI_CONTROL_EDITBOX, 5, &Element);
	SetRect(&rcTexture, 8, 113, 14, 121);
	Element.set_texture(0, &rcTexture);
	set_default_element(W_GUI_CONTROL_EDITBOX, 6, &Element);
	SetRect(&rcTexture, 14, 113, 241, 121);
	Element.set_texture(0, &rcTexture);
	set_default_element(W_GUI_CONTROL_EDITBOX, 7, &Element);
	SetRect(&rcTexture, 241, 113, 246, 121);
	Element.set_texture(0, &rcTexture);
	set_default_element(W_GUI_CONTROL_EDITBOX, 8, &Element);

	//-------------------------------------
	// CDXUTListBox - Main
	//-------------------------------------
	SetRect(&rcTexture, 13, 123, 241, 160);
	Element.set_texture(0, &rcTexture);
	Element.set_font(0, RGBA_TO_DWORD_COLOR(0, 0, 0, 255), DT_LEFT | DT_TOP);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_LISTBOX, 0, &Element);

	//-------------------------------------
	// CDXUTListBox - Selection
	//-------------------------------------

	SetRect(&rcTexture, 16, 166, 240, 183);
	Element.set_texture(0, &rcTexture);
	Element.set_font(0, RGBA_TO_DWORD_COLOR(255, 255, 255, 255), DT_LEFT | DT_TOP);

	// Assign the Element
	set_default_element(W_GUI_CONTROL_LISTBOX, 1, &Element);
}

#pragma region Getters

w_font_node* w_widget::get_font(_In_ UINT pIndex) const
{
	if (!this->_widgets_resource_manager) return nullptr;
	return this->_widgets_resource_manager->get_font_node(this->fonts[pIndex]);
}

w_texture_node* w_widget::get_texture(_In_ UINT pIndex) const
{
	if (!this->_widgets_resource_manager) return nullptr;
	return this->_widgets_resource_manager->get_texture_node(this->textures[pIndex]);
}

w_control* w_widget::get_control_at_point(_In_ const POINT& pPoint) const
{
	// Search through all child controls for the first one which
	// contains the mouse point
	for (auto it = this->controls.cbegin(); it != this->controls.cend(); ++it)
	{
		if (!*it)
		{
			continue;
		}

		// We only return the current control if it is visible
		// and enabled.  Because GetControlAtPoint() is used to do mouse
		// hittest, it makes sense to perform this filtering.
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
	for (auto it = this->controls.cbegin(); it != this->controls.cend(); ++it)
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
	for (auto it = this->controls.cbegin(); it != this->controls.cend(); ++it)
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
	int index = pControl->index + 1;

	auto pWidget = pControl->parent_widget;

	/*
	Cycle through dialogs in the loop to find the next control. Note
	that if only one control exists in all looped dialogs it will
	be the returned 'next' control.
	*/
	while (index >= (int) pWidget->controls.size())
	{
		pWidget = pWidget->_next_widget;
		index = 0;
	}

	return pWidget->controls[index];
}

w_control* w_widget::get_prev_control(_In_ w_control* pControl)
{
	int index = pControl->index - 1;

	auto _widget = pControl->parent_widget;

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
			_widget = pControl->parent_widget;
		}
		index = int(_widget->controls.size()) - 1;
	}

	return _widget->controls[index];
}

_Use_decl_annotations_
w_element* w_widget::get_default_element(UINT pControlType, UINT pIndexElement) const
{
	for (auto it = this->default_elements.cbegin(); it != this->default_elements.cend(); ++it)
	{
		if ((*it)->control_type == pControlType &&
			(*it)->index_element == pIndexElement)
		{
			return &(*it)->element;
		}
	}

	return nullptr;
}

#pragma endregion

#pragma region Setters

_Use_decl_annotations_
void w_widget::set_callBack(_In_ W_CALLBACK_GUI_EVENT pCallback, _In_opt_ void* pUserContext)
{
	// If this assert triggers, you need to call w_widget::Initialize() first.  This change
	// was made so that the DXUT's GUI could become seperate and optional from DXUT's core.  The 
	// creation and interfacing with w_widget_resource_manager is now the responsibility 
	// of the application if it wishes to use DXUT's GUI.
	assert(this->_widgets_resource_manager && L"To fix call w_widget_resource_manager::initialize() first.  See comments for details.");

	this->callBack_event = pCallback;
	this->callBack_event_user_context = pUserContext;
}

_Use_decl_annotations_
HRESULT w_widget::set_font(UINT pIndex, LPCWSTR pFontName, LONG pHeight, LONG pWeight)
{
	// If this assert triggers, you need to call CDXUTDialog::Init() first.  This change
	// was made so that the DXUT's GUI could become seperate and optional from DXUT's core.  The 
	// creation and interfacing with CDXUTDialogResourceManager is now the responsibility 
	// of the application if it wishes to use DXUT's GUI.
	assert(this->_widgets_resource_manager && L"To fix call CDXUTDialog::Init() first.  See comments for details.");
	_Analysis_assume_(this->_widgets_resource_manager);

	// Make sure the list is at least as large as the index being set
	for (size_t i = this->fonts.size(); i <= pIndex; i++)
	{
		this->fonts.push_back(-1);
	}

	int iFont = this->_widgets_resource_manager->add_font(pFontName, pHeight, pWeight);
	this->fonts[pIndex] = iFont;

	return S_OK;
}

_Use_decl_annotations_
HRESULT w_widget::set_texture(UINT pIndex, LPCWSTR pFilename)
{
	// If this assert triggers, you need to call CDXUTDialog::Init() first.  This change
	// was made so that the DXUT's GUI could become seperate and optional from DXUT's core.  The 
	// creation and interfacing with CDXUTDialogResourceManager is now the responsibility 
	// of the application if it wishes to use DXUT's GUI.
	assert(this->_widgets_resource_manager && L"To fix this, call CDXUTDialog::Init() first.  See comments for details.");
	_Analysis_assume_(this->_widgets_resource_manager);

	// Make sure the list is at least as large as the index being set
	for (size_t i = this->textures.size(); i <= pIndex; i++)
	{
		this->textures.push_back(-1);
	}

	int iTexture = this->_widgets_resource_manager->add_texture(pFilename);

	this->textures[pIndex] = iTexture;

	return S_OK;
}

_Use_decl_annotations_
HRESULT w_widget::set_texture(UINT pIndex, LPCWSTR pResourceName, HMODULE pHResourceModule)
{
	// If this assert triggers, you need to call CDXUTDialog::Init() first.  This change
	// was made so that the DXUT's GUI could become seperate and optional from DXUT's core.  The 
	// creation and interfacing with CDXUTDialogResourceManager is now the responsibility 
	// of the application if it wishes to use DXUT's GUI.
	assert(this->_widgets_resource_manager && L"To fix this, call CDXUTDialog::Init() first.  See comments for details.");
	_Analysis_assume_(this->_widgets_resource_manager);

	// Make sure the list is at least as large as the index being set
	for (size_t i = this->textures.size(); i <= pIndex; i++)
	{
		this->textures.push_back(-1);
	}

	int iTexture = this->_widgets_resource_manager->add_texture(pResourceName, pHResourceModule);

	this->textures[pIndex] = iTexture;

	return S_OK;
}

void w_widget::set_control_enabled(_In_ int pID, _In_ bool pEnabled)
{
	auto pControl = get_control(pID);
	if (!pControl)
		return;

	pControl->set_enabled(pEnabled);
}

_Use_decl_annotations_
HRESULT w_widget::set_default_element(UINT nControlType, UINT iElement, w_element* pElement)
{
	// If this Element type already exist in the list, simply update the stored Element
	for (auto it = this->default_elements.begin(); it != this->default_elements.end(); ++it)
	{
		if ((*it)->control_type == nControlType &&
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
	_new_holder->control_type = nControlType;
	_new_holder->index_element = iElement;
	_new_holder->element = *pElement;

	this->default_elements.push_back(_new_holder);

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

_Use_decl_annotations_
void w_widget::set_background_colors(DWORD pColorTopLeft, DWORD pColorTopRight, DWORD pColorBottomLeft, DWORD pColorBottomRight)
{
	this->color_top_left = pColorTopLeft;
	this->color_top_right = pColorTopRight;
	this->color_bottom_left = pColorBottomLeft;
	this->color_bottom_right = pColorBottomRight;
}


#pragma endregion
