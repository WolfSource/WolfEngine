/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_editBox.h
	Description		 : The editBox user control class.This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_EDITBOX_H__
#define __W_EDITBOX_H__

#include "w_control.h"
#include "w_wchar_buffer.h"
#include <w_time_span.h>

#define W_EVENT_EDITBOX_STRING              0x0601
// EVENT_EDITBOX_CHANGE is sent when the listbox content changes due to user input.
#define W_EVENT_EDITBOX_CHANGE              0x0602


namespace wolf
{
	namespace gui
	{
		class w_editBox : public w_control
		{
		public:
			w_editBox(_In_opt_ w_dialog* pParent = nullptr);
			virtual ~w_editBox();

			virtual bool handle_keyboard(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual bool handle_mouse(_In_ UINT pMsg, _In_ const POINT& pPoint, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual bool on_msg_proc(_In_ UINT pMsg, _In_ WPARAM pWParam, _In_ LPARAM pLParam) override;
			virtual void update_rects() override;
			virtual bool can_have_focus() override;

			virtual void render(_In_ ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext, _In_ float pElapsedTime) override;
			virtual void on_focus_in() override;

			void clear_text();
			void parse_float_array(_In_reads_(nCount) float* pNumbers, _In_ int pCount);


#pragma region Getters

			LPCWSTR get_text() const { return this->m_Buffer.get_buffer(); }
			size_t  get_text_length() const { return this->m_Buffer.get_text_size(); }  // Returns text length in chars excluding nul.
			HRESULT get_text_copy(_Out_writes_(bufferCount) LPWSTR strDest, _In_ UINT bufferCount) const;

#pragma endregion

#pragma region Setters

			void set_text(_In_z_ LPCWSTR pText, _In_ bool pSelected = false);
			void set_text_color(_In_ DWORD Color) override { this->m_TextColor = Color; }  // Text color
			void set_selected_text_color(_In_ DWORD Color) { this->m_SelTextColor = Color; }  // Selected text color
			void set_selected_backColor(_In_ DWORD Color) { this->m_SelBkColor = Color; }  // Selected background color
			void set_caret_color(_In_ DWORD Color) { this->m_CaretColor = Color; }  // Caret color
			void set_border_width(_In_ int nBorder) { this->m_nBorder = nBorder; update_rects(); }  // Border of the window
			void set_spacing(_In_ int nSpacing) { this->m_nSpacing = nSpacing; update_rects(); }
			void set_text_float_array(_In_reads_(nCount) const float* pNumbers, _In_ int nCount);

#pragma endregion

		protected:
			//PlaceCaret: Set the caret to a character position, and adjust the scrolling if necessary.
			void place_caret(_In_ int nCP);
			void delete_selection_text();
			void reset_caret_blink();
			void copy_to_clipboard();
			void paste_from_clipboard();

			w_wchar_buffer				m_Buffer;		// Buffer to hold text
			int							m_nBorder;      // Border of the window
			int							m_nSpacing;     // Spacing between the text and the edge of border
			RECT						m_rcText;       // Bounding rectangle for the text
			RECT						m_rcRender[9];  // Convenient rectangles for rendering elements
			double						m_dfBlink;		// Caret blink time in milliseconds
			wolf::system::w_time_span	m_dfLastBlink;  // Last timestamp of caret blink
			bool						m_bCaretOn;     // Flag to indicate whether caret is currently visible
			int							m_nCaret;       // Caret position, in characters
			bool						m_bInsertMode;  // If true, control is in insert mode. Else, overwrite mode.
			int							m_nSelStart;    // Starting position of the selection. The caret marks the end.
			int							m_nFirstVisible;// First visible character in the edit control
			DWORD						m_TextColor;    // Text color
			DWORD						m_SelTextColor; // Selected text color
			DWORD						m_SelBkColor;   // Selected background color
			DWORD						m_CaretColor;   // Caret color
			bool						m_bMouseDrag;   // True to indicate drag in progress

			static bool					s_bHideCaret;   // If true, we don't render the caret.

		private:
			typedef w_control _super;
		};
	}
}

#endif
