/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_list_box.h
	Description		 : The list_box user control class.
	Comment          :
*/

#ifndef __W_LIST_BOX_H__
#define __W_LIST_BOX_H__

#include "w_list_widget.h"
#include "w_image.h"
#include "w_label.h"

namespace wolf
{
	namespace gui
	{
		struct w_list_box_item
		{
			std::wstring					text;
			std::wstring					icon_path;
			void*							data;
			//bool							selected;
		};

		class w_list_box : public w_list_widget
		{
		public:
			DX_EXP							w_list_box(_In_opt_ w_widget* pParent = nullptr);
			DX_EXP virtual					~w_list_box();
	
			virtual void					render(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ float pElapsedTime) override;
			
			DX_EXP HRESULT					add_item(_In_z_ const std::wstring& pText, _In_z_ const std::wstring& pIconPath = L"", _In_ bool pIsAbsolutePath = false, _In_opt_  void* pData = nullptr);

			DX_EXP HRESULT					insert_item(_In_ int pIndex, _In_z_ const std::wstring& pText, _In_z_ const std::wstring& pIconPath = L"", _In_ bool pIsAbsolutePath = false, _In_opt_  void* pData = nullptr);

			DX_EXP virtual void				remove_item(_In_ const size_t pIndex) override;
			DX_EXP virtual void				remove_all_items() override;


#pragma region Getters

			DX_EXP int						get_text_margin_x() const											{ return this->text_margin_x; }
			DX_EXP int						get_text_margin_y() const											{ return this->text_margin_y; }
			DX_EXP int						get_icon_margin_x() const											{ return this->icon_margin_x; }
			DX_EXP int						get_icon_margin_y() const											{ return this->icon_margin_y; }
			DX_EXP int						get_icon_height_offset() const										{ return this->icon_height_offset; }

			DX_EXP UINT						get_icon_width() const												{ return this->icon_width; }
			DX_EXP UINT						get_icon_height() const												{ return this->icon_height; }
			DX_EXP bool						get_icon_use_default_size() const									{ return this->use_default_icon_size; }

			DX_EXP w_list_box_item*			get_list_box_item(_In_ int pIndex) const;
			DX_EXP w_list_box_item*			get_list_box_selected_item(_In_ int pPreviousSelected = -1) const	{ return get_list_box_item(get_selected_index(pPreviousSelected)); }

			DX_EXP w_color					get_label_color() const												{ return this->list_box_text_color; }
			DX_EXP w_color					get_label_selected_color() const									{ return this->list_box_text_selected_color; }
			DX_EXP w_color					get_label_disabled_color() const									{ return this->list_box_text_disabled_color; }

#pragma endregion

#pragma region Setters

			DX_EXP void						set_text_margin(_In_ const int pX, _In_ const int pY)				{ this->text_margin_x = pX; this->text_margin_y = pY;}
			DX_EXP void						set_icon_margin(_In_ const int pX, _In_ const int pY)				{ this->icon_margin_x = pX; this->icon_margin_y = pY; }
			DX_EXP void						set_icon_size(_In_ const UINT pWidth, _In_ const UINT pHeight)		{ this->icon_width = pWidth; this->icon_height = pHeight; }
			DX_EXP void						set_icon_use_default_size(_In_ const bool pValue)					{ this->use_default_icon_size = pValue; }
			DX_EXP void						set_icon_height_offset(_In_ const int pValue)						{ this->icon_height_offset = pValue; }
			DX_EXP void						set_label_color(_In_ w_color pColor)								{ this->list_box_text_color = pColor; }
			DX_EXP void						set_label_selected_color(_In_ w_color pColor)						{ this->list_box_text_selected_color = pColor; }
			DX_EXP void						set_label_disabled_color(_In_ w_color pColor)						{ this->list_box_text_disabled_color = pColor; }

#pragma endregion

		protected:
			
			w_color															list_box_text_color;
			w_color															list_box_text_selected_color;
			w_color															list_box_text_disabled_color;

			int																text_margin_x;
			int																text_margin_y;
			int																icon_margin_x;
			int																icon_margin_y;
			int																icon_height_offset;

			UINT															icon_width;
			UINT															icon_height;
			bool															use_default_icon_size;

			std::vector<w_list_box_item*>									list_items;

		private:
			typedef w_list_widget											_super;
		};
	}
}

#endif
