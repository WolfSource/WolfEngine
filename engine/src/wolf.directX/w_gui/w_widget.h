/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_widget.h 
	Description		 : The widget class. All gui controls must be assigned to a dialog, which handles input and rendering for the controls. This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_WIDGET_H__
#define __W_WIDGET_H__

#include <Windows.h>
#include <w_object.h>
#include <w_time_span.h>
#include "w_texture_node.h"
#include "w_widgets_resource_manager.h"
#include "w_user_controls/w_element.h"
#include "w_user_controls/w_control.h"
#include "w_user_controls/w_label.h"
#include "w_user_controls/w_image.h"
#include "w_user_controls/w_button.h"
#include "w_user_controls/w_checkBox.h"
#include "w_user_controls/w_radioButton.h"
#include "w_user_controls/w_comboBox.h"
#include "w_user_controls/w_slider.h"
#include "w_user_controls/w_listBox.h"
//#include "w_user_controls/w_editBox.h"
#include "w_graphics/w_sprite_batch.h"

namespace wolf
{
	namespace gui
	{
		typedef void (CALLBACK* W_CALLBACK_GUI_EVENT)(_In_ UINT pEvent, _In_ int pControlID, _In_ w_control* pControl, _In_opt_ void* pUserContext);
		
		class w_widget : public system::w_object
		{
			friend class w_widgets_resource_manager;
		public:
			DX_EXP w_widget(int pParentWindowWidth, int pParentWindowHeight);
			DX_EXP ~w_widget();

			static void WINAPI clear_focus();

			DX_EXP void initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
				_In_ wolf::graphics::w_sprite_batch* pSpriteBatch, 
				_In_ w_widgets_resource_manager* pManager,
				_In_ bool pRegisterDialog = true);

			DX_EXP void initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
				_In_ wolf::graphics::w_sprite_batch* pSpriteBatch,
				_In_ w_widgets_resource_manager* pManager,
				_In_ bool pRegisterDialog, 
				_In_z_ LPCWSTR pControlTextureFilename);

			DX_EXP void initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
				_In_ wolf::graphics::w_sprite_batch* pSpriteBatch,
				_In_ w_widgets_resource_manager* pManager,
				_In_ bool pRegisterDialog,
				_In_z_ LPCWSTR pControlTextureResourceName,
				_In_ HMODULE pControlTextureResourceModule);

			DX_EXP HRESULT on_render(_In_ float pElapsedTime);

			/*
				Add label to widget

				pID = Unique ID of control,
				pText = Text,
				pX = X parameter of location,
				pY = Y parameter of location,
				pWidth = Width of control,
				pHeight = Height of control,
				pColor = Color of label,
				pIsDefault = Is default control,
				pCreatedControl = Output reference created control
			*/
			DX_EXP HRESULT add_label(int pID,
				_In_z_ std::wstring pText,
				int pX, int pY,
				int pWidth, int pHeight,
				bool pIsDefault = false,
				_Out_opt_ w_label** pCreatedControl = nullptr);

			/*
				Add image to widget

				pID = Unique ID of control,
				pX = X parameter of location,
				pY = Y parameter of location,
				pPath = path of image file,
				pRelativePath = if true means, the image is related to Content Path, otherwise it has absolute path,
				pGuassianEffectValue = Guassian effect value for image
				pScaleX = X parameter of image's scale
				pScaleY = Y parameter of image's scale,
				pWidth = Width of image,
				pHeight = Height of image,
				pRotationAngle = rotation,
				pSetRotationCenterX = X parameter of image's center rotation
				pSetRotationCenterY = Y parameter of image's center rotation
				pTranslationX = X parameter of image's translation
				pTranslationY = Y parameter of image's translation
				pIsDefault = Is default control,
				pCreatedControl = Output reference created control
			*/
			DX_EXP HRESULT add_image(_In_ int pID,
				_In_ int pX, _In_ int pY,
				_In_z_ std::wstring pPath,
				_In_ bool pRelativePath = true,
				_In_ float pGuassianEffectValue = 0,
				_In_ float pScaleX = 1, _In_ float pScaleY = 1,
				_In_ int pWidth = -1, _In_ int pHeight = -1,
				_In_ float pRotationAngle = 0, _In_ int pSetRotationCenterX = 0, _In_ int pSetRotationCenterY = 0,
				_In_ float pTranslationX = 0, _In_ float pTranslationY = 0,
				_In_ bool pIsDefault = false,
				_Out_opt_ w_image** pCreatedControl = nullptr);

			/*
				Add button to widget

				pID = Unique ID of control,
				pText = Text,
				pX = X parameter of location,
				pY = Y parameter of location,
				pWidth = Width of control,
				pHeight = Height of control,
				pIconPath = Related path (to Content folder) of icon,
				pRelativePath = if true means, the image is related to Content Path, otherwise it has absolute path,
				pTextOffsetX = Offset of text in pixel,
				pTextOffsetY = Offset of text in pixel,
				pIconOffsetX = Offset of icon in pixel,
				pIconOffsetY = Offset of icon in pixel,
				pIconScaleX = X parameter scale of icon
				pIconScaleY = Y parameter scale of icon,
				pHotkey = Shortkey of button,
				pIsDefault = Is default control,
				pCreatedControl = Output reference created button
			*/
			DX_EXP HRESULT add_button(_In_ int pID,
				_In_z_ std::wstring pText,
				_In_ int pX, _In_ int pY,
				_In_ int pWidth, _In_ int pHeight,
				_In_z_ std::wstring pIconPath = L"",
				_In_ bool pRelativePath = true,
				_In_ int pTextOffsetX = -10, _In_ int pTextOffsetY = -10,
				_In_ int pIconOffsetX = 10, _In_ int pIconOffsetY = 10,
				_In_ float pIconScaleX = 1.0f, _In_ float pIconScaleY = 1.0f,
				_In_ UINT pHotkey = 0,
				_In_ bool pIsDefault = false,
				_Out_opt_ w_button** pCreatedControl = nullptr);

			/*
				Add button to widget

				pID = Unique ID of control,
				pText = Text,
				pX = X parameter of location,
				pY = Y parameter of location,
				pWidth = Width of control,
				pHeight = Height of control,
				pIconPath = Path of icon,
				pRelativePath = if true means, the image is related to Content Path, otherwise it has absolute path,
				pTextOffsetX = X parameter offset of text in pixel,
				pTextOffsetY = Y parameter offset of text in pixel,
				pIconOffsetX = X parameter offset of icon in pixel,
				pIconOffsetY = Y parameter offset of icon in pixel,
				pIconScaleX = X parameter scale of icon
				pIconScaleY = Y parameter scale of icon,
				pColor = Color of button,
				W_COLOR = Color of button when mouse is over on it,
				W_COLOR pPressedColor = Color of button when pressed,
				W_COLOR pFocusedColor = Color of button when focused,
				W_COLOR pTextColor = Color of button's text,
				W_COLOR pTextMouseOverColor = Color of button's text when mouse is over on it,
				W_COLOR pTextPressedColor = Color of button's text when button pressed,
				W_COLOR pTextFocusedColor = Color of button's text when button focused,
				pEffectMinValue = Minimum range of guassian effect
				pEffectMaxValue = Maximum range of guassian effect,
				pHotkey = Shortkey of button,
				pIsDefault = Is default control,
				pCreatedControl = Output reference created button
			*/
			DX_EXP HRESULT add_button(_In_ int pID,
				_In_z_ std::wstring pText,
				_In_ int pX, _In_ int pY,
				_In_ int pWidth, _In_ int pHeight,
				_In_z_ std::wstring pIconPath,
				_In_ bool pRelativePath,
				_In_ int pTextOffsetX, _In_ int pTextOffsetY,
				_In_ int pIconOffsetX, _In_ int pIconOffsetY,
				_In_ float pIconScaleX, _In_ float pIconScaleY,
				_In_ W_COLOR pColor,
				_In_ W_COLOR pMouseOverColor,
				_In_ W_COLOR pPressedColor,
				_In_ W_COLOR pFocusedColor,
				_In_ W_COLOR pTextColor,
				_In_ W_COLOR pTextMouseOverColor,
				_In_ W_COLOR pTextPressedColor,
				_In_ W_COLOR pTextFocusedColor,
				_In_ float pEffectMinValue, _In_ float pEffectMaxValue,
				_In_ UINT pHotkey,
				_In_ bool pIsDefault,
				_Out_opt_ w_button** pCreatedControl = nullptr);
			
			/*
				Add checkBox to widget

				pID = Unique ID of control,
				pText = Text,
				pX = X parameter of location,
				pY = Y parameter of location,
				pWidth = Width of control,
				pHeight = Height of control,
				pChecked = is checked or not,
				pTextOffsetX = Offset of text in pixel,
				pTextOffsetY = Offset of text in pixel,
				pHotkey = Shortkey of button,
				pIsDefault = Is default control,
				pColor = color of button,
				pMouseOverColor = Color of checkBox when mouse is over on it,
				pPressedColor = Color of checkBox when checkBox pressed,
				pFocusedColor = Color of checkBox when checkBox focussed,
				pCreatedControl = Output reference created checkBox
			*/
			DX_EXP HRESULT add_checkBox(int pID,
				_In_z_ std::wstring pText,
				int pX,
				int pY,
				int pWidth,
				int pHeight,
				bool pChecked = false,
				int pTextOffsetX = -56,
				int pTextOffsetY = -6,
				UINT pHotkey = 0,
				bool pIsDefault = false,
				W_COLOR pColor = W_COLOR(255, 255, 255, 255),
				W_COLOR pMouseOverColor = W_COLOR(255, 255, 0, 255),
				W_COLOR pPressedColor = W_COLOR(255, 255, 255, 200),
				W_COLOR pFocusedColor = W_COLOR(255, 255, 0, 255),
				_Out_opt_ w_checkBox** pCreatedControl = nullptr);		

			/*
				Add checkBox to widget

				pID = Unique ID of control,
				pText = Text,
				pX = X parameter of location,
				pY = Y parameter of location,
				pWidth = Width of control,
				pHeight = Height of control,
				pChecked = is checked or not,
				pTextOffsetX = Offset of text in pixel,
				pTextOffsetY = Offset of text in pixel,
				pHotkey = Shortkey of button,
				pIsDefault = Is default control,
				pColor = color of button,
				pMouseOverColor = Color of radioButton when mouse is over on it,
				pPressedColor = Color of radioButton when radioButton pressed,
				pFocusedColor = Color of radioButton when radioButton focussed,
				pCreatedControl = Output reference created checkBox
			*/
			DX_EXP HRESULT add_radioButton(int pID,
				UINT pButtonGroup,
				_In_z_ std::wstring pText,
				int pX, int pY,
				int pWidth, int pHeight,
				bool pChecked = false,
				int pTextOffsetX = -56, int pTextOffsetY = -6,
				UINT pHotkey = 0,
				bool pIsDefault = false,
				W_COLOR pColor = W_COLOR(255, 255, 255, 255),
				W_COLOR pMouseOverColor = W_COLOR(255, 255, 0, 255),
				W_COLOR pPressedColor = W_COLOR(255, 255, 255, 200),
				W_COLOR pFocusedColor = W_COLOR(255, 255, 0, 255),
				_Out_opt_ w_radioButton** pCreatedControl = nullptr);
	

			/*
				Add comboBox to widget

				pID = Unique ID of control,
				pText = Text,
				pX = X parameter of location,
				pY = Y parameter of location,
				pWidth = Width of control,
				pHeight = Height of control,
				pChecked = is checked or not,
				pTextOffsetX = Offset of text in pixel,
				pTextOffsetY = Offset of text in pixel,
				pHotkey = Shortkey of button,
				pIsDefault = Is default control,
				pCreatedControl = Output reference created comboBox
			*/
			DX_EXP HRESULT add_comboBox(int pID,
				int pX,
				int pY,
				int pWidth,
				int pHeight,
				int pTextOffsetX = -130,
				int pTextOffsetY = -7,
				UINT pHotkey = 0,
				bool pIsDefault = false,
				_Out_opt_ w_comboBox** pCreatedControl = nullptr);

			/*
				Add comboBox to widget

				pID = Unique ID of control,
				pText = Text,
				pX = X parameter of location,
				pY = Y parameter of location,
				pWidth = Width of control,
				pHeight = Height of control,
				pChecked = is checked or not,
				pTextOffsetX = Offset of text in pixel,
				pTextOffsetY = Offset of text in pixel,
				pHotkey = Shortkey of button,
				pIsDefault = Is default control,
				pColor = Color of comboBox,
				pMouseOverColor = Color of comboBox when mouse is over on it,
				pPressedColor = Color of comboBox when checkBox pressed,
				pFocusedColor =  Color of comboBox when comboBox focussed,
				pButtonColor = Color of comboBox's button,
				pButtonMouseOverColor = Color of comboBox's button when mouse is over on it,
				pButtonPressedColor =  Color of comboBox's button when button pressed,
				pButtonFocusedColor =  Color of comboBox's button when button focussed,
				pDropDownColor = Color of comboBox's drop down,
				pSelectionColor = Color of selected item,
				pSelectionFontColor = Color of selected item's font,
				pCreatedControl = Output reference created comboBox
			*/
			DX_EXP HRESULT add_comboBox(int pID,
				int pX,
				int pY,
				int pWidth,
				int pHeight,
				int pTextOffsetX = -130,
				int pTextOffsetY = -7,
				UINT pHotkey = 0,
				bool pIsDefault = false,
				W_COLOR pColor = W_COLOR(200, 200, 200, 150),
				W_COLOR pMouseOverColor = W_COLOR(255, 255, 0, 255),
				W_COLOR pPressedColor = W_COLOR(255, 255, 255, 200),
				W_COLOR pFocusedColor = W_COLOR(255, 255, 0, 170),
				W_COLOR pButtonColor = W_COLOR(255, 255, 255, 150),
				W_COLOR pButtonMouseOverColor = W_COLOR(255, 255, 0, 255),
				W_COLOR pButtonPressedColor = W_COLOR(150, 150, 150, 255),
				W_COLOR pButtonFocusedColor = W_COLOR(255, 255, 255, 200),
				W_COLOR pDropDownColor = W_COLOR(255, 255, 255, 255),
				W_COLOR pSelectionColor = W_COLOR(255, 0, 0, 155),
				W_COLOR pSelectionFontColor = W_COLOR(255, 255, 255, 255),
				_Out_opt_ w_comboBox** pCreatedControl = nullptr);

			/*
				Add slider to widget

				pID = Unique ID of control,
				pX = X parameter of location,
				pY = Y parameter of location,
				pWidth = Width of control,
				pHeight = Height of control,
				pMin = Minimum value of slider,
				pMax = Maximum value of slider,
				pValue = value of slider,
				pIsDefault = Is default control,
				pMainColor = Main color of slider,
				pMainMouseOverColor = Main color of slider when mouse is over on it,
				pMainPressedColor = Main color of slider when slider pressed,
				pMainFocusedColor =  Main color of slider when slider focussed,
				pButtonColor = Color of slider's button,
				pButtonMouseOverColor = Color of slider's button when mouse is over on it,
				pButtonPressedColor =  Color of slider's button when button pressed,
				pButtonFocusedColor =  Color of slider's button when button focussed,
				pCreatedControl = Output reference created slider
			*/
			DX_EXP HRESULT add_slider(int pID,
				int pX,
				int pY,
				int pWidth,
				int pHeight,
				int pMin = 0,
				int pMax = 100,
				int pValue = 50,
				bool pIsDefault = false,
				W_COLOR pMainColor = W_COLOR(255, 255, 255, 150),
				W_COLOR pMainMouseOverColor = W_COLOR(255, 255, 0, 255),
				W_COLOR pMainPressedColor = W_COLOR(255, 255, 255, 255),
				W_COLOR pMainFocusedColor = W_COLOR(255, 255, 0, 255),
				W_COLOR pButtonColor = W_COLOR(255, 255, 255, 150),
				W_COLOR pButtonMouseOverColor = W_COLOR(255, 255, 0, 255),
				W_COLOR pButtonPressedColor = W_COLOR(255, 255, 255, 255),
				W_COLOR pButtonFocusedColor = W_COLOR(255, 255, 0, 255),
				_Out_opt_ w_slider** pCreatedControl = nullptr);

			/*
				Add listBox to widget

				pID = Unique ID of control,
				pX = X parameter of location,
				pY = Y parameter of location,
				pWidth = Width of control,
				pHeight = Height of control,
				pStyle = Style of control,
				pCreatedControl = Output reference created listbox
			*/
			DX_EXP HRESULT add_listBox(int pID,
				int pX, int pY,
				int pWidth, int pHeight,
				DWORD pStyle = 0,
				_Out_opt_ w_listBox** pCreatedControl = nullptr);

			/*
			DX_EXP HRESULT add_editBox(_In_ int pID, 
				_In_z_ LPCWSTR pText, 
				_In_ int pX, 
				_In_ int pY, 
				_In_ int pWidth, 
				_In_ int pHeight, 
				_In_ bool pIsDefault = false,
				_Out_opt_ w_editBox** pCreatedControl = nullptr);*/

			// Windows message handler
			DX_EXP bool on_msg_proc(_In_ HWND pHWND, _In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam);

			DX_EXP HRESULT add_image_to_resource_manager(_In_z_ const wchar_t* pPath);

			HRESULT add_control(_In_ wolf::gui::w_control* pControl);
			HRESULT initialize_control(_In_ wolf::gui::w_control* pControl);

			void clear_radioButton_group(_In_ UINT pGroup);
			void clear_comboBox(_In_ int ID);

			// Access the default display Elements used when adding new controls
			HRESULT set_default_element(_In_ UINT nControlType, _In_ UINT iElement, _In_ w_element* pElement);

			// Methods called by controls
			void send_event(_In_ UINT nEvent, _In_ bool bTriggeredByUser, _In_ w_control* pControl);
			void request_focus(_In_ w_control* pControl);

			// Render helpers
			HRESULT draw_rect(_In_ const RECT* pRect, DWORD color);
			HRESULT draw_sprite(_In_ w_element* pElement, _In_ const RECT* prcDest, _In_ float fDepth);
			
			HRESULT draw_image(_In_z_ const wchar_t* pIconName,
				const DirectX::XMFLOAT2 pPosition,
				const DirectX::XMFLOAT2 pScale = DirectX::XMFLOAT2(1.0f, 1.0f), const DirectX::XMFLOAT2 pScaleCenter = DirectX::XMFLOAT2(0, 0),
				const float pRotationAngle = 0, const DirectX::XMFLOAT2 pRotationCenter = DirectX::XMFLOAT2(0, 0),
				const DirectX::XMFLOAT2 pTranslation = DirectX::XMFLOAT2(0, 0),
				D2D1_GAUSSIANBLUR_PROP pEffectParam = D2D1_GAUSSIANBLUR_PROP::D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, const float pEffectValue = 0);

			HRESULT draw_image(_In_z_ const wchar_t* pIconName, 
				const DirectX::XMFLOAT2 pPosition, const D2D1_RECT_F pRectangle,
				const DirectX::XMFLOAT2 pScale = DirectX::XMFLOAT2(1.0f, 1.0f), const DirectX::XMFLOAT2 pScaleCenter = DirectX::XMFLOAT2(0, 0),
				const float pRotationAngle = 0, const DirectX::XMFLOAT2 pRotationCenter = DirectX::XMFLOAT2(0, 0), 
				const DirectX::XMFLOAT2 pTranslation = DirectX::XMFLOAT2(0, 0),
				D2D1_GAUSSIANBLUR_PROP pEffectParam = D2D1_GAUSSIANBLUR_PROP::D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, const float pEffectValue = 0);

			HRESULT calc_textRect(_In_z_ LPCWSTR strText, _In_ w_element* pElement, _In_ const RECT* prcDest, _In_ int nCount = -1);
			HRESULT draw_text(_In_z_ std::wstring pText, DirectX::XMFLOAT2 pPosition, _In_ ID2D1SolidColorBrush* pBrush, bool bShadow = false);

			void enable_caption(_In_ bool bEnable) { this->caption_enable = bEnable; }
			void remove_control(_In_ int ID);
			void remove_all_controls();

			// Sets the callback used to notify the app of control events
			void enable_nonUser_events(_In_ bool bEnable) { this->nonUser_events = bEnable; }
			void enable_keyboard_input(_In_ bool bEnable) { this->keyboard_input = bEnable; }
			void enable_mouse_input(_In_ bool bEnable) { this->mouse_input = bEnable; }
			bool is_keyboard_input_enabled() const { return keyboard_input; }

			// Device state notification
			void refresh();
			void focus_default_control();


#pragma region Getters

			static w_control* WINAPI	get_next_control(_In_ w_control* pControl);
			static w_control* WINAPI	get_prev_control(_In_ w_control* pControl);

			w_label*					get_label(_In_ int pID) const										{ return reinterpret_cast<w_label*>(get_control(pID, W_GUI_CONTROL_LABEL)); }
			w_image*					get_image(_In_ int pID) const										{ return reinterpret_cast<w_image*>(get_control(pID, W_GUI_CONTROL_IMAGE)); }
			w_button*					get_button(_In_ int pID) const										{ return reinterpret_cast<w_button*>(get_control(pID, W_GUI_CONTROL_BUTTON)); }
			w_checkBox*					get_checkBox(_In_ int pID) const									{ return reinterpret_cast<w_checkBox*>(get_control(pID, W_GUI_CONTROL_CHECKBOX)); }
			w_radioButton*				get_radioButton(_In_ int pID) const									{ return reinterpret_cast<w_radioButton*>(get_control(pID, W_GUI_CONTROL_RADIOBUTTON)); }
			w_comboBox*					get_comboBox(_In_ int pID) const									{ return reinterpret_cast<w_comboBox*>(get_control(pID, W_GUI_CONTROL_COMBOBOX)); }
			w_slider*					get_slider(_In_ int pID) const										{ return reinterpret_cast<w_slider*>(get_control(pID, W_GUI_CONTROL_SLIDER)); }
			w_listBox*				    get_listBox(_In_ int ID) const										{ return reinterpret_cast<w_listBox*>(get_control(ID, W_GUI_CONTROL_LISTBOX)); }
			//w_editBox*				   get_editBox(_In_ int ID) const		{ return reinterpret_cast<w_editBox*>(get_control(ID, W_GUI_CONTROL_EDITBOX));			}

			bool						get_minimized() const												{ return this->minimized; }
			int							get_caption_height() const											{ return this->_caption_height; }
			bool						get_visible() const													{ return this->visible; }
			int							get_width() const													{ return this->_width; }
			int							get_height() const													{ return this->_height; }
			int							get_parent_window_width() const										{ return this->_parent_window_width; }
			int							get_parent_window_height() const									{ return this->_parent_window_height; }
			void						get_location(_Out_ POINT& Pt) const									{ Pt.x = this->_x; Pt.y = this->_y; }
			w_widgets_resource_manager* get_manager() const													{ return this->_widgets_resource_manager; }


			bool						get_control_enabled(_In_ int pID) const;
			w_element*					get_default_element(_In_ UINT nControlType, _In_ UINT iElement) const;
			w_control*					get_control(_In_ int ID) const;
			w_control*					get_control(_In_ int ID, _In_ UINT nControlType) const;
			w_control*					get_control_at_point(_In_ const POINT& pt) const;
			w_font_node*				get_font(_In_ UINT pIndex) const;
			w_texture_node*				get_texture(_In_ UINT pIndex) const;

#pragma endregion

#pragma region Setters

			static void WINAPI set_refresh_time(_In_ float pTime) { s_time_refresh.from_seconds((double) pTime); }

			DX_EXP	void	set_location(_In_ int pX, _In_ int pY)												{ this->_x = pX; this->_y = pY; }
			DX_EXP	void	set_minimized(_In_ bool bMinimized)													{ this->minimized = bMinimized; }
			DX_EXP	void	set_background_colors(_In_ DWORD colorAllCorners)									{ set_background_colors(colorAllCorners, colorAllCorners, colorAllCorners, colorAllCorners); }
			DX_EXP	void	set_captionText(_In_ const WCHAR* pText)											{ wcscpy_s(this->caption, sizeof(this->caption) / sizeof(this->caption[0]), pText); }
			DX_EXP	void	set_size(_In_ int pWidth, _In_ int pHeight)											{ this->_width = pWidth; this->_height = pHeight; }
			DX_EXP	void	set_visible(_In_ bool pVisible)														{ this->visible = pVisible; }

			void	set_captionHeight(_In_ int pHeight)														{ this->_caption_height = pHeight; }
			DX_EXP	void	set_callBack(_In_ W_CALLBACK_GUI_EVENT pCallback, _In_opt_ void* pUserContext = nullptr);
			HRESULT set_font(_In_ UINT index, _In_z_ LPCWSTR strFaceName, _In_ LONG height, _In_ LONG weight);
			HRESULT set_texture(_In_ UINT pIndex, _In_z_ LPCWSTR pStr_file_name);
			HRESULT set_texture(_In_ UINT pIndex, _In_z_ LPCWSTR pStr_resource_name, _In_ HMODULE pResource_module);
			DX_EXP void	set_background_colors(_In_ DWORD colorTopLeft, _In_ DWORD colorTopRight, _In_ DWORD colorBottomLeft, _In_ DWORD colorBottomRight);
			void	set_control_enabled(_In_ int pID, _In_ bool pEnabled);

#pragma endregion

			bool nonUser_events;
			bool keyboard_input;
			bool mouse_input;

		private:
			typedef system::w_object _super;

			std::shared_ptr<wolf::graphics::w_graphics_device> _gDevice;
			wolf::graphics::w_sprite_batch* _sprite_batch;

			int _default_control_ID;

			static system::w_time_span s_time_refresh;
			static w_control* s_pControlFocus;        // The control which has focus
			static w_control* s_pControlPressed;      // The control currently pressed

			system::w_time_span time_last_refresh;

			// Initialize default Elements
			void initialize_default_elements();

			// Windows message handlers
			void on_mouse_move(_In_ const POINT& pPoint);
			void on_mouse_up(_In_ const POINT& pPoint);

			void set_next_widget(_In_ w_widget* pNextWidget);

			// Control events
			bool on_cycle_focus(_In_ bool pForward);

			bool		_is_mouse_over_widget;
			w_control* control_mouse_over;           // The control which is hovered over

			bool visible;
			bool caption_enable;
			bool minimized;
			bool drag;
			WCHAR caption[256];

			int _x;
			int _y;
			int _width;
			int _height;
			int _caption_height;

			UINT _parent_window_width;
			UINT _parent_window_height;

			DWORD color_top_left;
			DWORD color_top_right;
			DWORD color_bottom_left;
			DWORD color_bottom_right;

			w_widgets_resource_manager* _widgets_resource_manager;
			W_CALLBACK_GUI_EVENT callBack_event;
			void* callBack_event_user_context;

			// Index into m_TextureCache
			std::vector<int> textures;
			// Index into m_FontCache
			std::vector<int> fonts;

			std::vector<w_control*> controls;
			std::vector<w_element_holder*> default_elements;

			// Element for the caption
			w_element caption_element;

			w_widget* _next_widget;
			w_widget* _prev_widget;
		};
	}
}

#endif
