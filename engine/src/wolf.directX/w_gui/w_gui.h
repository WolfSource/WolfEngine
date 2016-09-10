#ifndef __W_GUI_H__
#define __W_GUI_H__

#include <map>
#include <w_object.h>
#include "w_widgets_resource_manager.h"
#include <rapidxml.hpp>

namespace wolf
{
	namespace gui
	{
		class w_gui
		{
		public:

			//initialize w_gui
			DX_EXP	static void initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
				wolf::graphics::w_sprite_batch* pSpriteBatch,
				UINT pWidth, 
				UINT pHeight);

			//load gui design path
			DX_EXP  static HRESULT load(HWND pHwnd, const wchar_t* pGuiDesignPath);

			//create a widget with unique name
			DX_EXP	static HRESULT add_widget(_In_ HWND pHwnd,
				_In_ const std::string pWidgetName,
				_Inout_ wolf::gui::w_widget** pWidget = nullptr,
				_In_ UINT pWidth = 400,
				_In_ UINT pHeight = 200,
				_In_ bool pDraggable = false,
				_In_ bool pVisisble = true,
				_In_ bool pMinimized = false,
				_In_ int pX = 10,
				_In_ int pY = 10,
				_In_ w_color pBackgroundColorTopLeft = w_color(216, 238, 249, 255),
				_In_ w_color pBackgroundColorTopRight = w_color(216, 238, 249, 255),
				_In_ w_color pBackgroundColorBottomLeft = w_color(71, 188, 242, 255),
				_In_ w_color pBackgroundColorBottomRight = w_color(71, 188, 242, 255),
				_In_ float pZOrder = 0,
				_In_ bool pEnabled = true,
				_In_ bool pEnableCaption = true,
				_In_ const std::wstring pCaption = L"Caption",
				_In_ w_color pCaptionBackgroundColor = w_color(57, 57, 57, 255),
				_In_ UINT pCaptionHeight = 25,
				_In_ int pCaptionMarginLeft = 5,
				_In_ int pCaptionMarginTop = 0,
				_In_ const std::string pCaptionFontName = "Times New Roman",
				_In_ UINT pCaptionFontSize = 20,
				_In_ UINT pCaptionFontWeight = 400,
				_In_ UINT pCaptionFontStyle = 0,
				_In_ UINT pCaptionFontStretch = 5,
				_In_ UINT pCaptionFontParagraphAlignment = 0,
				_In_ UINT pCaptionFontTextAlignment = 0);

			//remove a widget with unique name
			DX_EXP	static void remove_widget(_In_ const std::string pWidgetName);

				//render all widgets
			DX_EXP	static void render(const wolf::system::w_game_time& pGameTime);

			//render specific widget
			DX_EXP	static void render(const char* pWidgetName, const wolf::system::w_game_time& pGameTime);

			//release all controls
			DX_EXP	static HRESULT release();

#pragma region Getters

			//get pointer to the first widget
			DX_EXP	static void get_first_widget(_Inout_ wolf::gui::w_widget** pWidget);
			//get pointer to the widget by name
			DX_EXP	static void get_widget(_In_z_ const char* pWidgetControlName, _Inout_ wolf::gui::w_widget** pWidget);
			//get control by id
			DX_EXP	static void get_control(_In_z_ const char* pWidgetName, _In_ int pControlID, _Inout_ wolf::gui::w_control** pControl);
			//get all controls id of specific widget
			DX_EXP	static void get_all_controls_id(_In_z_ const char* pWidgetName, _Inout_ std::vector<int>& pControlsID);
				
#pragma endregion

		private:
			static void	_traversing_gui_node(rapidxml::xml_node<char>* pNode, HWND pHwnd, const wchar_t* pGuiDesignPath);

			static HRESULT add_image(_In_ int pID,
				_In_ int pX, _In_ int pY,
				_In_z_ std::wstring pPath,
				_In_ bool pRelativePath,
				_In_ float pGuassianEffectValue,
				_In_ float pScaleX, _In_ float pScaleY,
				_In_ int pWidth, _In_ int pHeight,
				_In_ float pRotationAngle, _In_ int pSetRotationCenterX, _In_ int pSetRotationCenterY,
				_In_ float pTranslationX, _In_ float pTranslationY,
				_In_ bool pIsDefault,
				_In_ float pZOrder,
				_In_ bool pEnabled);

			static HRESULT add_label(_In_ int pID,
				_In_z_ std::wstring pText,
				_In_ UINT pWidth,
				_In_ UINT pHeight,
				_In_ int pX, 
				_In_ int pY,
				_In_ w_color pColor,
				_In_ w_color pMouseOverColor,
				_In_ w_color pPressedColor,
				_In_ w_color pFocusedColor,
				_In_ w_color pDisabledColor,
				_In_ bool pIsDefault,
				_In_ float pZOrder,
				_In_ bool pEnabled,
				_In_ const std::string pFontName,
				_In_ UINT pFontSize,
				_In_ UINT pFontWeight,
				_In_ UINT pFontStyle,
				_In_ UINT pFontStretch,
				_In_ UINT pFontParagraphAlignment,
				_In_ UINT pFontTextAlignment);

			static HRESULT add_button(_In_ int pID,
				_In_z_ std::wstring pText,
				_In_ UINT pWidth, _In_ UINT pHeight,
				_In_ int pX, _In_ int pY,
				_In_z_ std::wstring pIconPath,
				_In_ bool pRelativePath,
				_In_ int pTextOffsetX, _In_ int pTextOffsetY,
				_In_ int pIconOffsetX, _In_ int pIconOffsetY,
				_In_ float pIconScaleX, _In_ float pIconScaleY,
				_In_ UINT pHotkey,
				_In_ bool pIsDefault,
				_In_ w_color pColor, 
				_In_ w_color pMouseOverColor, 
				_In_ w_color pPressedColor, 
				_In_ w_color pFocusedColor,
				_In_ w_color pDisabledColor,
				_In_ w_color pTextColor, 
				_In_ w_color pTextMouseOverColor, 
				_In_ w_color pTextPressedColor, 
				_In_ w_color pTextFocusedColor,
				_In_ w_color pTextDisabledColor,
				_In_ float pEffectMinValue, _In_ float pEffectMaxValue,
				_In_ float pZOrder,
				_In_ bool pEnabled,
				_In_ const std::string pFontName,
				_In_ UINT pFontSize,
				_In_ UINT pFontWeight,
				_In_ UINT pFontStyle,
				_In_ UINT pFontStretch,
				_In_ UINT pFontParagraphAlignment,
				_In_ UINT pFontTextAlignment);

			static HRESULT add_check_box(_In_ int pID,
				_In_z_ std::wstring pText,
				_In_ UINT pWidth, _In_ UINT pHeight,
				_In_ int pX, _In_ int pY,
				_In_ bool pChecked,
				_In_ int pTextOffsetX,
				_In_ int pTextOffsetY,
				_In_ UINT pHotkey,
				_In_ bool pIsDefault,
				_In_ w_color pColor,
				_In_ w_color pMouseOverColor,
				_In_ w_color pPressedColor,
				_In_ w_color pFocusedColor,
				_In_ w_color pDisabledColor,
				_In_ w_color pTextColor,
				_In_ w_color pTextMouseOverColor,
				_In_ w_color pTextPressedColor,
				_In_ w_color pTextFocusedColor,
				_In_ w_color pTextDisabledColor,
				_In_ float pZOrder,
				_In_ bool pEnabled,
				_In_ const std::string pFontName,
				_In_ UINT pFontSize,
				_In_ UINT pFontWeight,
				_In_ UINT pFontStyle,
				_In_ UINT pFontStretch,
				_In_ UINT pFontParagraphAlignment,
				_In_ UINT pFontTextAlignment);

			static HRESULT add_radio_button(int pID,
				UINT pButtonGroup,
				_In_z_ std::wstring pText,
				_In_ UINT pWidth, _In_ UINT pHeight,
				_In_ int pX, _In_ int pY,
				_In_ bool pChecked,
				_In_ int pTextOffsetX, _In_ int pTextOffsetY,
				_In_ UINT pHotkey,
				_In_ bool pIsDefault,
				_In_ w_color pColor,
				_In_ w_color pMouseOverColor,
				_In_ w_color pPressedColor,
				_In_ w_color pFocusedColor,
				_In_ w_color pDisabledColor,
				_In_ w_color pTextColor,
				_In_ w_color pTextMouseOverColor,
				_In_ w_color pTextPressedColor,
				_In_ w_color pTextFocusedColor,
				_In_ w_color pTextDisabledColor,
				_In_ float pZOrder,
				_In_ bool pEnabled,
				_In_ const std::string pFontName,
				_In_ UINT pFontSize,
				_In_ UINT pFontWeight,
				_In_ UINT pFontStyle,
				_In_ UINT pFontStretch,
				_In_ UINT pFontParagraphAlignment,
				_In_ UINT pFontTextAlignment);

			static HRESULT add_combo_box(_In_ int pID,
				_In_ UINT pWidth, _In_ UINT pHeight,
				_In_ int pX, _In_ int pY,
				_In_ int pTextOffsetX, _In_ int pTextOffsetY,
				_In_ UINT pItemHeight,
				_In_ POINT pItemMargin,
				_In_ UINT pHotkey,
				_In_ bool pIsDefault,
				_In_ w_color pColor,
				_In_ w_color pMouseOverColor,
				_In_ w_color pPressedColor,
				_In_ w_color pFocusedColor,
				_In_ w_color pDisabledColor,
				_In_ w_color pButtonColor,
				_In_ w_color pButtonMouseOverColor,
				_In_ w_color pButtonPressedColor,
				_In_ w_color pButtonFocusedColor,
				_In_ w_color pButtonDisabledColor,
				_In_ w_color pDropDownColor,
				_In_ w_color pSelectionColor,
				_In_ w_color pSelectionFontColor,
				_In_ w_color pTextColor,
				_In_ w_color pTextDisabledColor,
				_In_ UINT pDropDownContextHeight,
				_In_ float pZOrder,
				_In_ bool pEnabled,
				_In_ const std::string pFontName,
				_In_ UINT pFontSize,
				_In_ UINT pFontWeight,
				_In_ UINT pFontStyle,
				_In_ UINT pFontStretch,
				_In_ UINT pFontParagraphAlignment,
				_In_ UINT pFontTextAlignment,
				_In_ std::vector<std::wstring>& pItems);

			static HRESULT add_slider(_In_ int pID,
				_In_ UINT pWidth, _In_ UINT pHeight,
				_In_ int pX, _In_ int pY,
				_In_ int pMin, _In_ int pMax,
				_In_ int pValue,
				_In_ UINT pHotkey,
				_In_ bool pIsDefault,
				_In_ w_color pMainColor,
				_In_ w_color pMainMouseOverColor,
				_In_ w_color pMainPressedColor,
				_In_ w_color pMainFocusedColor,
				_In_ w_color pMainDisabledColor,
				_In_ w_color pButtonColor,
				_In_ w_color pButtonMouseOverColor,
				_In_ w_color pButtonPressedColor,
				_In_ w_color pButtonFocusedColor,
				_In_ w_color pButtonDisabledColor,
				_In_ float pZOrder,
				_In_ bool pEnabled,
				_In_ const std::string pFontName,
				_In_ UINT pFontSize,
				_In_ UINT pFontWeight,
				_In_ UINT pFontStyle,
				_In_ UINT pFontStretch,
				_In_ UINT pFontParagraphAlignment,
				_In_ UINT pFontTextAlignment);

			static HRESULT add_list_box(_In_ int pID,
				_In_ UINT pWidth, _In_ UINT pHeight,
				_In_ int pX, _In_ int pY,
				_In_ DWORD pStyle,
				_In_ int pBorderX, _In_ int pBorderY,
				_In_ int pTextMarginX, _In_ int pTextMarginY,
				_In_ int pIconMarginX, _In_ int pIconMarginY,
				_In_ int pSelectedRectangleMarginTop, _In_ int pSelectedRectangleMarginDown,
				_In_ int pItemHeight,
				_In_ UINT pHotkey,
				_In_ bool pIsDefault,
				_In_ w_color pTextColor,
				_In_ w_color pSelectedTextColor,
				_In_ w_color pDisabledTextColor,
				_In_ w_color pColor,
				_In_ w_color pSelectedColor,
				_In_ w_color pDisabledColor,
				_In_ float pZOrder,
				_In_ bool pEnabled,
				_In_ const std::string pFontName,
				_In_ UINT pFontSize,
				_In_ UINT pFontWeight,
				_In_ UINT pFontStyle,
				_In_ UINT pFontStretch,
				_In_ UINT pFontParagraphAlignment,
				_In_ UINT pFontTextAlignment,
				_In_ std::vector<std::tuple<std::wstring, std::wstring, bool>>& pItems);

			static HRESULT add_tab(_In_ int pID,
				_In_ UINT pWidth, _In_ UINT pHeight,
				_In_ int pX, _In_ int pY,
				_In_ UINT pTabButtonWidth, _In_ UINT pTabButtonHeight,
				_In_ int ppArrowButtonTextOffsetX, _In_ int ppArrowButtonTextOffsetY,
				_In_ w_color pButtonColor,
				_In_ w_color pButtonMouseOverColor,
				_In_ w_color pButtonPressedColor,
				_In_ w_color pButtonFocusedColor,
				_In_ w_color pButtonDisabledColor,
				_In_ w_color pTextColor,
				_In_ w_color pTextMouseOverColor,
				_In_ w_color pTextPressedColor,
				_In_ w_color pTextFocusedColor,
				_In_ w_color pTextDisabledColor,
				_In_ float pZOrder,
				_In_ bool pEnabled,
				_In_ std::vector<std::tuple<std::wstring, std::wstring, bool, glm::vec2, glm::vec2, glm::vec2, int, bool>>& pItems);

			static bool															_is_released;
			static std::shared_ptr<wolf::graphics::w_graphics_device>			_gDevice;
			static wolf::graphics::w_sprite_batch*								_sprite_batch;
			static wolf::gui::w_widgets_resource_manager*						_widget_resource_manager;
			static std::map<const std::string, wolf::gui::w_widget*>			_widgets;
			static wolf::gui::w_widget*											_temp_parent_widget_ptr;
			static UINT															_parent_width;
			static UINT															_parent_height;
			static const char*													_trace_class_name;
		};
	}
}

#endif
