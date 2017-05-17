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

#include "w_graphics_device_manager.h"
#include "w_graphics/w_shader.h"

//#include <w_time_span.h>

//#include "w_element.h"
//#include "w_control.h"
//#include "w_label.h"
//#include "w_image.h"
//#include "w_button.h"
//#include "w_check_box.h"
//#include "w_radio_button.h"
//#include "w_combo_box.h"
//#include "w_slider.h"
//#include "w_list_box.h"
//#include "w_tree_list.h"
//#include "w_list_widget.h"
//#include "w_tab.h"
//#include "w_line_shape.h"
//#include "w_rounded_rectangle_shape.h"
//#include "w_ellipse_shape.h"
//#include "w_graphics/w_sprite_batch.h"

namespace wolf
{
	namespace gui
	{		
		//typedef void (*W_CALLBACK_GUI_EVENT)(_In_ UINT pEvent, _In_ int pControlID, _In_ w_control* pControl, _In_opt_ void* pUserContext);
		
		class w_widget : public system::w_object
		{
		public:
			W_EXP w_widget(UINT pParentWindowWidth, UINT pParentWindowHeight);
			W_EXP ~w_widget();

            W_EXP HRESULT load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
                               _In_z_ const std::wstring& pControlTextureFilePath = L"");
            
			//W_EXP static void WINAPI clear_focus();


//			W_EXP HRESULT render(_In_ const float pElapsedTime);

//			/*
//				Add label to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pText = Text,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pColor = Color of label,
//				pIsDefault = Is default control,
//				pCreatedControl = Output reference created label 
//			*/
//			DX_EXP HRESULT add_label(_In_ int pID,
//				_In_z_ std::wstring pText,
//				_In_ int pX, _In_ int pY,
//				_In_ int pWidth, _In_ int pHeight,
//				_In_ bool pIsDefault = false,
//				_Out_opt_ w_label** pCreatedControl = nullptr);
//
//			/*
//				Add image to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pPath = path of image file,
//				pAbsolutePath = if false means, the image is related to Content Path, otherwise it has absolute path,
//				pRotationX = X parameter of image's rotation
//				pRotationY = Y parameter of image's rotation
//				pUseDefaultSize = Use absolute size of image
//				pWidth = Width of image,
//				pHeight = Height of image,
//				pIsDefault = Is default control,
//				pCreatedControl = Output reference created image 
//			*/
//			DX_EXP HRESULT add_image(_In_ int pID,
//				_In_ int pX, _In_ int pY,
//				_In_z_ const std::wstring pPath,
//				_In_ bool pAbsolutePath = false,
//				_In_ float pRotationX = 0.0f, _In_ float pRotationY = 0.0f,
//				_In_ bool pUseDefaultSize = true,
//				_In_ UINT pWidth = 0, _In_ UINT pHeight = 0,
//				_In_ bool pIsDefault = false,
//				_Out_opt_ w_image** pCreatedControl = nullptr);
//
//			/*
//				Add button to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pText = Text,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pIconPath = Related path (to Content folder) of icon,
//				pIconAbsolutePath = if false means, the image is related to Content Path, otherwise it has absolute path,
//				pTextOffsetX = Offset of text in pixel,
//				pTextOffsetY = Offset of text in pixel,
//				pIconOffsetX = Offset of icon in pixel,
//				pIconOffsetY = Offset of icon in pixel,
//				pIconUseDefaultSize = Use default size of icon,
//				pIconWidth = Width of icon,
//				pIconHeight = Height of icon,
//				pHotkey = Shortkey of button,
//				pIsDefault = Is default control,
//				pCreatedControl = Output reference created button control
//			*/
//			DX_EXP HRESULT add_button(_In_ int pID,
//				_In_z_ std::wstring pText,
//				_In_ int pX, _In_ int pY,
//				_In_ UINT pWidth, _In_ UINT pHeight,
//				_In_z_ std::wstring pIconPath = L"",
//				_In_ bool pIconAbsolutePath = false,
//				_In_ int pTextOffsetX = -27, _In_ int pTextOffsetY = 10,
//				_In_ int pIconOffsetX = 10, _In_ int pIconOffsetY = 10,
//				_In_ bool pIconUseDefaultSize = false,
//				_In_ UINT pIconWidth = 32, _In_ UINT pIconHeight = 32,
//				_In_ UINT pHotkey = 0,
//				_In_ bool pIsDefault = false,
//				_Out_opt_ w_button** pCreatedControl = nullptr);
//
//			/*
//				Add button to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pText = Text,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pIconPath = Path of icon,
//				pIconAbsolutePath = if false means, the image is related to Content Path, otherwise it has absolute path,
//				pTextOffsetX = X parameter offset of text in pixel,
//				pTextOffsetY = Y parameter offset of text in pixel,
//				pIconOffsetX = X parameter offset of icon in pixel,
//				pIconOffsetY = Y parameter offset of icon in pixel,
//				pIconUseDefaultSize = Use default size of icon,
//				pIconWidth = Width of icon,
//				pIconHeight = Height of icon,
//				pColor = Color of button,
//				W_COLOR = Color of button when mouse is over on it,
//				W_COLOR pPressedColor = Color of button when pressed,
//				W_COLOR pFocusedColor = Color of button when focused,
//				W_COLOR pDisabledColor = Color of button when disabled,
//				W_COLOR pTextColor = Color of button's text,
//				W_COLOR pTextMouseOverColor = Color of button's text when mouse is over on it,
//				W_COLOR pTextPressedColor = Color of button's text when button pressed,
//				W_COLOR pTextFocusedColor = Color of button's text when button focused,
//				W_COLOR pTextDisabledColor = Color of button's text when button disabled,
//				pEffectMinValue = Minimum range of guassian effect
//				pEffectMaxValue = Maximum range of guassian effect,
//				pHotkey = Shortkey of button,
//				pIsDefault = Is default control,
//				pCreatedControl = Output reference created button control
//			*/
//			DX_EXP HRESULT add_button(_In_ int pID,
//				_In_z_ std::wstring pText,
//				_In_ int pX, _In_ int pY,
//				_In_ UINT pWidth, _In_ UINT pHeight,
//				_In_z_ std::wstring pIconPath,
//				_In_ bool pIconAbsolutePath,
//				_In_ int pTextOffsetX, _In_ int pTextOffsetY,
//				_In_ int pIconOffsetX, _In_ int pIconOffsetY,
//				_In_ bool pIconUseDefaultSize,
//				_In_ UINT pIconWidth, _In_ UINT pIconHeight,
//				_In_ UINT pHotkey,
//				_In_ bool pIsDefault,
//				_In_ w_color pColor, _In_ w_color pMouseOverColor, _In_ w_color pPressedColor, _In_ w_color pFocusedColor, _In_ w_color pDisabledColor,
//				_In_ w_color pTextColor, _In_ w_color pTextMouseOverColor, _In_ w_color pTextPressedColor, _In_ w_color pTextFocusedColor, _In_ w_color pTextDisabledColor,
//				_In_ float pEffectMinValue, _In_ float pEffectMaxValue,
//				_Out_opt_ w_button** pCreatedControl = nullptr);
//			
//			/*
//				Add checkBox to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pText = Text,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pChecked = is checked or not,
//				pTextOffsetX = Offset of text in pixel,
//				pTextOffsetY = Offset of text in pixel,
//				pColor = color of button,
//				pMouseOverColor = Color of checkBox when mouse is over on it,
//				pPressedColor = Color of checkBox when checkBox pressed,
//				pFocusedColor = Color of checkBox when checkBox focussed,
//				pHotkey = Shortkey of button,
//				pIsDefault = Is default control,
//				pCreatedControl = Output reference created check_box control
//			*/
//			DX_EXP HRESULT add_check_box(_In_ int pID,
//				_In_z_ std::wstring pText,
//				_In_ int pX, _In_ int pY,
//				_In_ UINT pWidth, _In_ UINT pHeight,
//				_In_ bool pChecked = false,
//				_In_ int pTextOffsetX = -56,
//				_In_ int pTextOffsetY = -6,
//				_In_ UINT pHotkey = 0,
//				_In_ bool pIsDefault = false,
//				_In_ w_color pColor = w_color(255, 255, 255, 255),
//				_In_ w_color pMouseOverColor = w_color(255, 255, 0, 255),
//				_In_ w_color pPressedColor = w_color(255, 255, 255, 200),
//				_In_ w_color pFocusedColor = w_color(255, 255, 0, 255),
//				_In_ w_color pDisabledColor = w_color(50, 50, 50, 150),
//				_Out_opt_ w_check_box** pCreatedControl = nullptr);
//
//			/*
//				Add radio button to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pText = Text,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pChecked = is checked or not,
//				pTextOffsetX = Offset of text in pixel,
//				pTextOffsetY = Offset of text in pixel,
//				pHotkey = Shortkey of button,
//				pIsDefault = Is default control,
//				pColor = color of button,
//				pMouseOverColor = Color of radioButton when mouse is over on it,
//				pPressedColor = Color of radioButton when radio_button pressed,
//				pFocusedColor = Color of radioButton when radio_button focussed,
//				pCreatedControl = Output reference created check_box control
//			*/
//			DX_EXP HRESULT add_radio_button(_In_ int pID,
//				UINT pButtonGroup,
//				_In_z_ std::wstring pText,
//				_In_ int pX, _In_ int pY,
//				_In_ UINT pWidth, _In_ UINT pHeight,
//				_In_ bool pChecked = false,
//				_In_ int pTextOffsetX = -56, _In_ int pTextOffsetY = -6,
//				UINT pHotkey = 0,
//				bool pIsDefault = false,
//				_In_ w_color pColor = w_color(255, 255, 255, 255),
//				_In_ w_color pMouseOverColor = w_color(255, 255, 0, 255),
//				_In_ w_color pPressedColor = w_color(255, 255, 255, 200),
//				_In_ w_color pFocusedColor = w_color(255, 255, 0, 255),
//				_In_ w_color pDisabledColor = w_color(50, 50, 50, 150),
//				_Out_opt_ w_radio_button** pCreatedControl = nullptr);
//	
//
//			/*
//				Add comboBox to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pChecked = is checked or not,
//				pTextOffsetX = Offset of text in pixel,
//				pTextOffsetY = Offset of text in pixel,
//				pHotkey = Shortkey of button,
//				pIsDefault = Is default control,
//				pCreatedControl = Output reference created combo_box control
//			*/
//			DX_EXP HRESULT add_combo_box(_In_ int pID,
//				_In_ int pX, _In_ int pY,
//				_In_ UINT pWidth, _In_ UINT pHeight,
//				_In_ int pTextOffsetX = -130,
//				_In_ int pTextOffsetY = -7,
//				_In_ UINT pHotkey = 0,
//				_In_ bool pIsDefault = false,
//				_Out_opt_ w_combo_box** pCreatedControl = nullptr);
//
//			/*
//				Add comboBox to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pChecked = is checked or not,
//				pTextOffsetX = Offset of text in pixel,
//				pTextOffsetY = Offset of text in pixel,
//				pHotkey = Shortkey of button,
//				pIsDefault = Is default control,
//				pColor = Color of comboBox,
//				pMouseOverColor = Color of comboBox when mouse is over on it,
//				pPressedColor = Color of comboBox when checkBox pressed,
//				pFocusedColor =  Color of comboBox when comboBox focussed,
//				pDisabledColor =  Color of comboBox when comboBox disabled,
//				pButtonColor = Color of comboBox's button,
//				pButtonMouseOverColor = Color of comboBox's button when mouse is over on it,
//				pButtonPressedColor =  Color of comboBox's button when button pressed,
//				pButtonFocusedColor =  Color of comboBox's button when button focussed,
//				pButtonDisabledColor = Color of comboBox's button when button disabled,
//				pDropDownColor = Color of comboBox's drop down,
//				pSelectionColor = Color of selected item,
//				pSelectionFontColor = Color of selected item's font,
//				pCreatedControl = Output reference created comboBox control
//			*/
//			DX_EXP HRESULT add_combo_box(_In_ int pID,
//				_In_ int pX, _In_ int pY,
//				_In_ UINT pWidth, _In_ UINT pHeight,
//				_In_ int pTextOffsetX = -130, _In_ int pTextOffsetY = -7,
//				_In_ UINT pHotkey = 0,
//				_In_ bool pIsDefault = false,
//				_In_ w_color pColor = w_color(200, 200, 200, 255),
//				_In_ w_color pMouseOverColor = w_color(255, 255, 0, 255),
//				_In_ w_color pPressedColor = w_color(255, 255, 255, 200),
//				_In_ w_color pFocusedColor = w_color(255, 255, 0, 170),
//				_In_ w_color pDisabledColor = w_color(50, 50, 50, 150),
//				_In_ w_color pButtonColor = w_color(255, 255, 255, 255),
//				_In_ w_color pButtonMouseOverColor = w_color(255, 255, 0, 255),
//				_In_ w_color pButtonPressedColor = w_color(150, 150, 150, 255),
//				_In_ w_color pButtonFocusedColor = w_color(255, 255, 255, 200),
//				_In_ w_color pButtonDisabledColor = w_color(50, 50, 50, 150),
//				_In_ w_color pDropDownColor = w_color(255, 255, 255, 255),
//				_In_ w_color pSelectionColor = w_color(255, 0, 0, 155),
//				_In_ w_color pSelectionFontColor = w_color(255, 255, 255, 255),
//				_Out_opt_ w_combo_box** pCreatedControl = nullptr);
//
//			/*
//				Add slider to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pMin = Minimum value of slider,
//				pMax = Maximum value of slider,
//				pValue = value of slider,
//				pIsDefault = Is default control,
//				pMainColor = Main color of slider,
//				pMainMouseOverColor = Main color of slider when mouse is over on it,
//				pMainPressedColor = Main color of slider when slider pressed,
//				pMainFocusedColor =  Main color of slider when slider focussed,
//				pMainDisabledColor = Main color of slider when slider disabled,
//				pButtonColor = Color of slider's button,
//				pButtonMouseOverColor = Color of slider's button when mouse is over on it,
//				pButtonPressedColor =  Color of slider's button when button pressed,
//				pButtonFocusedColor =  Color of slider's button when button focussed,
//				pButtonDisabledColor =  Color of slider's button when button disabled,
//				pCreatedControl = Output reference created slider control
//			*/
//			DX_EXP HRESULT add_slider(_In_ int pID,
//				_In_ int pX, _In_ int pY,
//				_In_ UINT pWidth, _In_ UINT pHeight,
//				_In_ int pMin = 0, _In_ int pMax = 100,
//				_In_ int pValue = 50,
//				_In_ UINT pHotkey = 0,
//				_In_ bool pIsDefault = false,
//				_In_ w_color pMainColor = w_color(255, 255, 255, 150),
//				_In_ w_color pMainMouseOverColor = w_color(255, 255, 0, 255),
//				_In_ w_color pMainPressedColor = w_color(255, 255, 255, 255),
//				_In_ w_color pMainFocusedColor = w_color(255, 255, 0, 255),
//				_In_ w_color pMainDisabledColor = w_color(50, 50, 50, 150),
//				_In_ w_color pButtonColor = w_color(255, 255, 255, 150),
//				_In_ w_color pButtonMouseOverColor = w_color(255, 255, 0, 255),
//				_In_ w_color pButtonPressedColor = w_color(255, 255, 255, 255),
//				_In_ w_color pButtonFocusedColor = w_color(255, 255, 0, 255),
//				_In_ w_color pButtonDisabledColor = w_color(50, 50, 50, 150),
//				_Out_opt_ w_slider** pCreatedControl = nullptr);
//
//			/*
//				Add list_box to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pMultiSelection = Enable/Disable multi selection,
//				pCreatedControl = Output reference created listbox control
//			*/
//			DX_EXP HRESULT add_list_box(_In_ int pID,
//				_In_ int pX, _In_ int pY,
//				_In_ UINT pWidth, _In_ UINT pHeight,
//				_In_ bool pMultiSelection = false,
//				_Out_opt_ w_list_box** pCreatedControl = nullptr);
//
//			/*
//				Add tree_list to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pMultiSelection = Enable/Disable multi selection,
//				pCreatedControl = Output reference created listbox control
//			*/
//			DX_EXP HRESULT add_tree_list(_In_ int pID,
//				_In_ int pX, _In_ int pY,
//				_In_ UINT pWidth, _In_ UINT pHeight,
//				_In_ bool pMultiSelection = false,
//				_Out_opt_ w_tree_list** pCreatedControl = nullptr);
//
//			/*
//				Add list_widget to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pMultiSelection = Enable/Disable multi selection,
//				pCreatedControl = Output reference created listbox control
//			*/
//			DX_EXP HRESULT add_list_widget(_In_ int pID,
//				_In_ int pX, _In_ int pY,
//				_In_ UINT pWidth, _In_ UINT pHeight,
//				_In_ bool pMultiSelection = false,
//				_Out_opt_ w_list_widget** pCreatedControl = nullptr);
//
//			/*
//				Add tab to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pTabButtonWidth = Width of each button, 
//				pTabButtonHeight = Height of each button,
//				pArrowButtonTextOffsetX = Arrow button text offset x, 
//				pArrowButtonTextOffsetY = Arrow button text offset y,
//				pCreatedControl = Output reference created tab control
//			*/
//			DX_EXP HRESULT add_tab(_In_ int pID,
//				_In_ int pX, _In_ int pY,
//				_In_ UINT pWidth, _In_ UINT pHeight,
//				_In_ UINT pTabButtonWidth, _In_ UINT pTabButtonHeight,
//				_In_ int pArrowButtonTextOffsetX, _In_ int pArrowButtonTextOffsetY,
//				_Out_opt_ w_tab** pCreatedControl = nullptr);
//
//			/*
//				Add line shape to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pIsDefault = Is default control,
//				pFillColor = Color that used to fill the shape,
//				pMouseOverColor = Color that used when mouse is over the shape,
//				pDisabledColor = Color that used when shape is disable,
//				pCreatedControl = Output reference created shape
//			*/
//			DX_EXP HRESULT add_line_shape(_In_ int pID,
//				_In_ int pStartX, _In_ int pStartY,
//				_In_ int pStopX, _In_ int pStopY,
//				_In_ UINT pStrokeWidth,
//				_In_ bool pIsDefault = false,
//				_In_ w_color pFillColor = w_color(255, 255, 255, 255),
//				_In_ w_color pDisabledColor = w_color(50, 50, 50, 150),
//				_Out_opt_ w_line_shape** pCreatedControl = nullptr);
//			/*
//				Add rounded rectangle shape to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,,
//				pX = X parameter of location,
//				pY = Y parameter of location,
//				pWidth = Width of control,
//				pHeight = Height of control,
//				pIsDefault = Is default control,
//				pFillColor = Color that used to fill the shape,
//				pBorderColor = Color of border,
//				pMouseOverColor = Color that used when mouse is over the shape,
//				pDisabledColor = Color that used when shape is disable,
//				pCreatedControl = Output reference created shape
//			*/
//			DX_EXP HRESULT add_rounded_rectangle_shape(_In_ int pID,
//				_In_ int pX, _In_ int pY,
//				_In_ UINT pWidth, _In_ UINT pHeight,
//				_In_ float pRadiusX, _In_ float pRadiusY,
//				_In_ float pStrokeWidth,
//				_In_ bool pIsDefault = false,
//				_In_ w_color pFillColor = w_color(128, 57, 123, 255),
//				_In_ w_color pBorderColor = w_color(255, 255, 255, 255),
//				_In_ w_color pMouseOverColor = w_color(169, 75, 162, 255),
//				_In_ w_color pDisabledColor = w_color(50, 50, 50, 150),
//				_Out_opt_ w_rounded_rectangle_shape** pCreatedControl = nullptr);
//
//			/*
//				Add ellipse shape to widget
//
//				pID = Unique ID of control, please do not use INT_MAX and INT_MIN, 2147483647 and -2147483648 are reserved,
//				pCenterX = X parameter of start point,
//				pCenterY = Y parameter of start point,
//				pRadiusX = X parameter of stop point,
//				pRadiusY = Y parameter of stop point,
//				pStrokeWidth = stroke width of ellipse,
//				pIsDefault = Is default control,
//				pFillColor = Color that used to fill the shape,
//				pBorderColor = Color of border,
//				pMouseOverColor = Color that used when mouse is over the shape,
//				pDisabledColor = Color that used when shape is disable,
//				pCreatedControl = Output reference created shape
//			*/
//			DX_EXP HRESULT add_ellipse_shape(_In_ int pID,
//				_In_ float pCenterX, _In_ float pCenterY,
//				_In_ float pRadiusX, _In_ float pRadiusY,
//				_In_ UINT pStrokeWidth,
//				_In_ bool pIsDefault = false,
//				_In_ w_color pFillColor = w_color(128, 57, 123, 255),
//				_In_ w_color pBorderColor = w_color(255, 255, 255, 255),
//				_In_ w_color pMouseOverColor = w_color(169, 75, 162, 255),
//				_In_ w_color pDisabledColor = w_color(50, 50, 50, 150),
//				_Out_opt_ w_ellipse_shape** pCreatedControl = nullptr);
//
//			// Windows message handler
//			DX_EXP bool on_msg_proc(_In_ HWND pHWND, _In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam);
//
//			DX_EXP HRESULT add_image_to_resource_manager(_In_z_ const wchar_t* pPath);
//
//			HRESULT add_control(_In_ wolf::gui::w_control* pControl);
//			HRESULT initialize_control(_In_ wolf::gui::w_control* pControl);
//
//			void clear_radioButton_group(_In_ UINT pGroup);
//			void clear_comboBox(_In_ int ID);
//
//			// Access the default display Elements used when adding new controls
//			HRESULT set_default_element(_In_ UINT nControlType, _In_ UINT iElement, _In_ w_element* pElement);
//
//			// Methods called by controls
//			void send_event(_In_ UINT nEvent, _In_ bool bTriggeredByUser, _In_ w_control* pControl);
//			void request_focus(_In_ w_control* pControl);
//
//			// Render helpers
//			HRESULT draw_sprite(_In_ w_element* pElement, _In_ const RECT* prcDest, _In_ float fDepth);
//			
//			HRESULT draw_image(_In_z_ const wchar_t* pIconName,
//				const DirectX::XMFLOAT2 pPosition,
//				const DirectX::XMFLOAT2 pScale = DirectX::XMFLOAT2(1.0f, 1.0f), const DirectX::XMFLOAT2 pScaleCenter = DirectX::XMFLOAT2(0, 0),
//				const float pRotationAngle = 0, const DirectX::XMFLOAT2 pRotationCenter = DirectX::XMFLOAT2(0, 0),
//				const DirectX::XMFLOAT2 pTranslation = DirectX::XMFLOAT2(0, 0));
//
//			HRESULT draw_image(_In_z_ const wchar_t* pIconName, 
//				const DirectX::XMFLOAT2 pPosition, const D2D1_RECT_F pRectangle,
//				const DirectX::XMFLOAT2 pScale = DirectX::XMFLOAT2(1.0f, 1.0f), const DirectX::XMFLOAT2 pScaleCenter = DirectX::XMFLOAT2(0, 0),
//				const float pRotationAngle = 0, const DirectX::XMFLOAT2 pRotationCenter = DirectX::XMFLOAT2(0, 0), 
//				const DirectX::XMFLOAT2 pTranslation = DirectX::XMFLOAT2(0, 0));
//
//			HRESULT draw_text(_In_z_ std::wstring pText, DirectX::XMFLOAT2 pPosition, _In_ ID2D1SolidColorBrush* pBrush);
//			HRESULT draw_shape(_In_ wolf::graphics::direct2D::Isprite_batch_drawable* pShape);
//
//			void remove_control(_In_ int ID);
//			void remove_all_controls();
//
//			// Sets the callback used to notify the app of control events
//			void enable_nonUser_events(_In_ bool pValue)															{ this->non_user_events = pValue; }
//			void enable_keyboard_input(_In_ bool pValue)															{ this->keyboard_input = pValue; }
//			void enable_mouse_input(_In_ bool pValue)																{ this->mouse_input = pValue; }
//			
//			// Device state notification
//			void refresh();
//			void focus_default_control();
//			void apply_render_UI(_In_ ID3D11DeviceContext1* pContext);
//			void apply_render_UI_Untex(_In_ ID3D11DeviceContext1* pContext);

			//release the resources
			ULONG release() override;

#pragma region Getters

//			static w_control* WINAPI			get_next_control(_In_ w_control* pControl);
//			static w_control* WINAPI			get_prev_control(_In_ w_control* pControl);
//
//			bool get_is_keyboard_input_enabled() const																{ return this->keyboard_input; }
//
//			//get hwnd associated  to this widget
//			DX_EXP HWND							get_hwnd() const													{ return this->_hwnd; }
//			//get name of this widget
//			DX_EXP const char*					get_name() const													{ return this->_name; }
//			//get width
//			DX_EXP int							get_width() const													{ return this->_width; }
//			//get height
//			DX_EXP int							get_height() const													{ return this->_height; }
//			//get wheather this widget is dragable
//			DX_EXP bool							get_is_dragable() const												{ return this->_draggable; }
//			//get wheather this widget is visible
//			DX_EXP bool							get_is_visible() const												{ return this->_visible; }
//			//get is widget minimized
//			DX_EXP bool							get_is_minimized() const											{ return this->_minimized; }
//			//get the position of widget
//			DX_EXP void							get_position(_Out_ int& pX, _Out_ int& pY)      					{ pX = this->_x; pY = this->_y; }
//			//get background color
//			DX_EXP void							get_background_colors(
//				_Out_ w_color& pColorTopLeft,
//				_Out_ w_color& pColorTopRight,
//				_Out_ w_color& pColorBottomLeft,
//				_Out_ w_color& pColorBottomRight)
//			{ 
//				pColorTopLeft = this->_background_color_top_left;
//				pColorTopRight = this->_background_color_top_right;
//				pColorBottomLeft = this->_background_color_bottom_left;
//				pColorBottomRight = this->_background_color_bottom_right;
//			}
//			//get background color when mouse over widget
//			DX_EXP void							get_active_background_colors(
//				_Out_ w_color& pColorTopLeft,
//				_Out_ w_color& pColorTopRight,
//				_Out_ w_color& pColorBottomLeft,
//				_Out_ w_color& pColorBottomRight)
//			{
//				pColorTopLeft = this->_active_background_color_bottom_left;
//				pColorTopRight = this->_active_background_color_top_right;
//				pColorBottomLeft = this->_active_background_color_bottom_left;
//				pColorBottomRight = this->_active_background_color_bottom_right;
//			}
//
//			//get caption is enabled
//			DX_EXP bool							get_is_caption_enabled() const										 { return this->_is_caption_enabled; }
//			//get caption																							 
//			DX_EXP const wchar_t*				get_caption() const													 { return this->_caption; }
//			//get caption background color																			 
//			DX_EXP void							get_caption_background_color(_Out_ w_color& pColor) const			 { pColor = this->_caption_background_color; }
//			//get caption background color																			 
//			DX_EXP void							get_caption_active_background_color(_Out_ w_color& pColor) const	{ pColor = this->_caption_active_background_color; }
//			//get is dragging																						 
//			DX_EXP bool							get_is_dragging() const												 { return this->_draggable ? this->_is_dragging : false; }
//			//get caption height																					 
//			DX_EXP UINT							get_caption_height() const											 { return this->_caption_height; }
//			//get caption margin left																				 
//			DX_EXP int							get_caption_margin_left() const										 { return this->_caption_margin_left; }
//			//get caption margin top																				 
//			DX_EXP int							get_caption_margin_top() const										 { return this->_caption_margin_top; }
//			//get whether control is enable																			 
//			DX_EXP  bool						get_enabled() const													 { return this->_enabled; }
//			//get z index order																						 
//			DX_EXP  float						get_z_order() const													 { return this->_z_order; }
//																													 
//			//get parent window width																				 
//			DX_EXP int							get_parent_window_width() const										 { return this->_parent_window_width; }
//			//get parent window height																				 
//			DX_EXP int							get_parent_window_height() const									 { return this->_parent_window_height; }
//			//get control is enabled																				 
//			DX_EXP bool							get_control_enabled(_In_ int pID) const;
//			//get default element
//			DX_EXP w_element*					get_default_element(_In_ UINT pControlType, _In_ UINT pElement) const;
//			//get base control class by ID
//			DX_EXP w_control*					get_control(_In_ int pID) const;
//			//get base control class by ID and control type
//			DX_EXP w_control*					get_control(_In_ int pID, _In_ UINT pControlType) const;
//			//get base control at point
//			DX_EXP w_control*					get_control_at_point(_In_ const POINT& pPoint) const;
//			//get font
//			DX_EXP w_font_node*					get_font(_In_ UINT pIndex) const;
//			//get texture by index
//			DX_EXP w_texture_node*				get_texture(_In_ UINT pIndex) const;
//			//get background buffer width
//			DX_EXP UINT							get_background_buffer_width() const;
//			//get background buffer height
//			DX_EXP UINT							get_background_buffer_height() const;
//
//
//			//get label control by ID
//			DX_EXP w_label*						get_label(_In_ const int pID) const									{ return reinterpret_cast<w_label*>(get_control(pID, W_GUI_CONTROL_LABEL)); }
//			//get image control by ID
//			DX_EXP w_image*						get_image(_In_ const int pID) const									{ return reinterpret_cast<w_image*>(get_control(pID, W_GUI_CONTROL_IMAGE)); }
//			//get button control by ID
//			DX_EXP w_button*					get_button(_In_ const int pID) const								{ return reinterpret_cast<w_button*>(get_control(pID, W_GUI_CONTROL_BUTTON)); }
//			//get checkbox control by ID
//			DX_EXP w_check_box*					get_checkBox(_In_ const int pID) const								{ return reinterpret_cast<w_check_box*>(get_control(pID, W_GUI_CONTROL_CHECKBOX)); }
//			//get radioButton control by ID
//			DX_EXP w_radio_button*				get_radio_button(_In_ const int pID) const							{ return reinterpret_cast<w_radio_button*>(get_control(pID, W_GUI_CONTROL_RADIOBUTTON)); }
//			//get comboBox control by ID
//			DX_EXP w_combo_box*					get_combo_box(_In_ const int pID) const								{ return reinterpret_cast<w_combo_box*>(get_control(pID, W_GUI_CONTROL_COMBOBOX)); }
//			//get slider control by ID
//			DX_EXP w_slider*					get_slider(_In_ const int pID) const								{ return reinterpret_cast<w_slider*>(get_control(pID, W_GUI_CONTROL_SLIDER)); }
//			//get list_widget control by ID
//			DX_EXP w_list_widget*				get_list_widget(_In_ const int ID) const							{ return reinterpret_cast<w_list_widget*>(get_control(ID, W_GUI_CONTROL_LISTWIDGET)); }
//			//get list_box control by ID
//			DX_EXP w_list_box*					get_list_box(_In_ const int ID) const								{ return reinterpret_cast<w_list_box*>(get_control(ID, W_GUI_CONTROL_LISTBOX)); }
//			//get widget resource manager
//			DX_EXP w_widgets_resource_manager*	get_manager() const													{ return this->_widgets_resource_manager; }
//			//get all controls id
//			DX_EXP void							get_all_controls_id(_Inout_ std::vector<int>& pIDs);
			
#pragma endregion

#pragma region Setters

//			static void WINAPI set_refresh_time(_In_ double pValue)													{ s_time_refresh.from_seconds(pValue); }
//
//			//set the name of widget
//			DX_EXP  void	set_name(_In_z_ const char* pValue)														{ this->_name = pValue; }
//			//set the width of widget
//			DX_EXP  void	set_width(_In_ UINT pValue)																{ this->_width = pValue; }
//			//set the height of widget
//			DX_EXP  void	set_height(_In_ UINT pValue)															{ this->_height = pValue; }
//			//enable/disable dragging widget 
//			DX_EXP  void	set_draggable(_In_ bool pValue)															{ this->_draggable = pValue; }
//			//enable/disable visibility of widget 
//			DX_EXP  void	set_visibility(_In_ bool pValue)														{ this->_visible = pValue; }
//			//set whether widget is minimized
//			DX_EXP	void	set_minimized(_In_ bool bMinimized)														{ this->_minimized = bMinimized; }
//			//set the position of widget
//			DX_EXP	void	set_position(_In_ int pX, _In_ int pY)													{ this->_x = pX; this->_y = pY; }
//			//set background color, one color for all corners
//			DX_EXP	void	set_background_colors(_In_ w_color pColorForAllCorners)									{ set_background_colors(pColorForAllCorners, pColorForAllCorners, pColorForAllCorners, pColorForAllCorners); }
//			//set background color
//			DX_EXP void		set_background_colors(_In_ w_color pColorTopLeft, _In_ w_color pColorTopRight, _In_ w_color pColorBottomLeft, _In_ w_color pColorBottomRight);
//			//The background color of widget when mouse is over on it 
//			DX_EXP	void	set_mouse_over_background_colors(_In_ w_color pColorForAllCorners)						{ set_active_background_colors(pColorForAllCorners, pColorForAllCorners, pColorForAllCorners, pColorForAllCorners); }
//			//The background color of widget when mouse is over on it 
//			DX_EXP void		set_active_background_colors(_In_ w_color pColorTopLeft, _In_ w_color pColorTopRight, _In_ w_color pColorBottomLeft, _In_ w_color pColorBottomRight);
//			//set enabled
//			DX_EXP  void	set_enabled(_In_ bool pValue);
//			//set z index order
//			DX_EXP  void	set_z_order(_In_ float pValue)															{ this->_z_order = pValue; }
//			//set whether caption is enable			
//			DX_EXP	void	set_enable_caption(bool pValue)															{ this->_is_caption_enabled = pValue; }
//			//set caption of widget
//			DX_EXP	void	set_caption(_In_ const wchar_t* pValue)													{ wcscpy_s(this->_caption, sizeof(this->_caption) / sizeof(this->_caption[0]), pValue); }
//			//set caption background color
//			DX_EXP	void	set_caption_background_color(_In_ w_color pValue)										{ this->_caption_background_color = pValue; }
//			//set the background color of caption, when mouse is over on widget
//			DX_EXP	void	set_caption_active_background_color(_In_ w_color pValue)								{ this->_caption_active_background_color = pValue; }
//			//set the caption height
//			DX_EXP  void	set_caption_height(UINT pValue)															{ this->_caption_height = pValue; }
//			//set the caption margin left
//			DX_EXP  void	set_caption_margin_left(_In_ int pValue)												{ this->_caption_margin_left = pValue; }
//			//set the caption margin top
//			DX_EXP  void	set_caption_margin_top(_In_ int pValue)													{ this->_caption_margin_top = pValue; }
//			//set shader resource
//			DX_EXP	HRESULT set_shader_resource();
//			
//
//			//set graphical user interface callback
//			DX_EXP	void	set_call_back(_In_ W_CALLBACK_GUI_EVENT pCallback, _In_opt_ void* pUserContext = nullptr);
//			//remove graphical user interface callback
//			DX_EXP	void	remove_call_back(_In_ W_CALLBACK_GUI_EVENT pCallback);
//
//			//ToDo: deprecated. do not use this function
//			DX_EXP HRESULT	set_font(_In_ UINT pIndex, _In_z_ LPCWSTR pStrFaceName, _In_ LONG pHeight, _In_ LONG pWeight);
//			//set texture by index and file name
//			DX_EXP HRESULT	set_texture(_In_ UINT pIndex, _In_z_ LPCWSTR pFileName);
//			//set texture
//			DX_EXP HRESULT	set_texture(_In_ UINT pIndex, _In_z_ LPCWSTR pStrResourceName, _In_ HMODULE pResourceModule);
//			//set whether control is enabled
//			DX_EXP void		set_control_enabled(_In_ int pID, _In_ bool pEnabled);
#pragma endregion

#pragma region Events
//
//			 std::function<void(const POINT&)> on_mouse_move_callback;
//			 std::function<void(const POINT&)> on_mouse_enter_callback;
//			 std::function<void(const POINT&)> on_mouse_leave_callback;

#pragma endregion

//			bool                                                        non_user_events;
//			bool                                                        keyboard_input;
//			bool                                                        mouse_input;

		private:
			typedef system::w_object                                    _super;

            std::shared_ptr<wolf::graphics::w_graphics_device>			_gDevice;
            wolf::graphics::w_shader*                                   _shader;
            wolf::graphics::w_texture*                                  _texture;
        
            
//			// Initialize default Elements
//			void _initialize();
//
//			// Windows message handlers
//			void on_mouse_move(_In_ const POINT& pPoint);
//			void on_mouse_up(_In_ const POINT& pPoint);
//
//			void set_next_widget(_In_ w_widget* pNextWidget);
//
//			// Control events
//			bool on_cycle_focus(_In_ bool pForward);
//

//
//			w_widgets_resource_manager*									_widgets_resource_manager;
//
//			HWND														_hwnd;
//			UINT														_parent_window_width;
//			UINT														_parent_window_height;
//			const char*													_name;
//			UINT														_width;
//			UINT														_height;
//			bool														_draggable;
//			bool														_is_dragging;
//			bool														_visible;
//			bool														_minimized;
//			int															_x;
//			int															_y;
//			bool														_enabled;
//			float														_z_order;
//			
//			w_color														_background_color_top_left;
//			w_color														_background_color_top_right;
//			w_color														_background_color_bottom_left;
//			w_color														_background_color_bottom_right;
//
//			w_color														_active_background_color_top_left;
//			w_color														_active_background_color_top_right;
//			w_color														_active_background_color_bottom_left;
//			w_color														_active_background_color_bottom_right;
//
//			bool														_is_caption_enabled;
//			wchar_t														_caption[256];
//			w_color														_caption_background_color;
//			w_color														_caption_active_background_color;
//			UINT														_caption_height;
//			int															_caption_margin_left;
//			int															_caption_margin_top;
//			POINT														_global_mouse_point;
//
//			static system::w_time_span									s_time_refresh;
//			static w_control*											s_pControlFocus;        // The control which has focus
//			static w_control*											s_pControlPressed;      // The control currently pressed
//
//			system::w_time_span time_last_refresh;
//
//			bool														_redraw;
//			ID3D11Buffer*												_vertex_buffer;
//
//			ID2D1SolidColorBrush*										_brush;
//
//			bool														_is_double_click;
//			bool														_is_mouse_over_widget;
//			w_control*													_control_mouse_over;           // The control which is hovered over
//
//			bool														_is_double_clicked;
//			
//			std::vector<W_CALLBACK_GUI_EVENT>							_call_back_events;
//			std::vector<void*>											_call_back_events_user_contexts;
//
//			//std::vector<w_control*>										_controls;
//			//std::vector<w_element_holder*>								_default_elements;
//
//			// Element for the caption
//			w_element													_caption_element;
//
//			w_widget*													_next_widget;
//			w_widget*													_prev_widget;
		};
	}
}

#endif
