/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_tab.h
	Description		 : The tab control contains of multiple widgets.

	Comment          :
*/

#ifndef __W_TAB__
#define __W_TAB__

#include "w_control.h"
#include "w_button.h"

namespace wolf
{
	namespace gui
	{
		class w_tab : public w_control
		{
		public:
			w_tab(_In_opt_ w_widget* pParent);

			virtual HRESULT on_initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice) override;
			void add_tab();
			void remove_all_tabs();
			virtual void	render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;

#pragma region Getter


#pragma endregion

#pragma region Setter


#pragma endregion

		protected:
			
			std::vector<w_button*> buttons;
			std::vector<w_widget*> widgets;

		private:
			typedef w_control _super;
		};
	}
}

#endif
