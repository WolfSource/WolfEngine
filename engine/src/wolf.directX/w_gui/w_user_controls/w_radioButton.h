/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_radioButton.h
	Description		 : The radioButton user control class.This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_RADIOBUTTON_H__
#define __W_RADIOBUTTON_H__

#include "w_checkBox.h"

#define W_EVENT_RADIOBUTTON_CHANGED           0x0301

namespace wolf
{
	namespace gui
	{
		class w_radioButton : public w_checkBox
		{
		public:
			w_radioButton(_In_opt_ w_widget* pParent = nullptr);

			virtual bool handle_keyboard(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
			virtual bool handle_mouse(_In_ UINT uMsg, _In_ const POINT& pt, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
			virtual void on_hotKey() override;

#pragma region Getters

			UINT get_button_group() const												{ return this->button_group; }

#pragma endregion

#pragma region Setters

			void set_checked(_In_ bool pChecked, _In_ bool pClearGroup = true)			{ set_checked_internal(pChecked, pClearGroup, false); }
			void set_button_group(_In_ UINT pButtonGroup)								{ this->button_group = pButtonGroup; }

#pragma endregion

		protected:
			virtual void set_checked_internal(_In_ bool pChecked, _In_ bool pClearGroup, _In_ bool pFromInput);
			UINT button_group;

		private:
			typedef w_checkBox _super;
		};

	}
}

#endif
