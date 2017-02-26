/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_wchar_buffer.h
	Description		 : The button user control class.This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_CHAR_BUFFER_H__
#define __W_CHAR_BUFFER_H__

#include <w_gui/w_font.h>
#include <usp10.h>

namespace wolf
{
	namespace gui
	{
		class w_wchar_buffer
		{
		public:
			w_wchar_buffer(_In_ int pInitialSize = 1);
			~w_wchar_buffer();

#pragma region Getters

			size_t get_buffer_size() const							{ return m_nBufferSize; }
			int get_text_size() const								{ return (int) wcslen(m_pwszBuffer); }
			const wchar_t* get_buffer() const						{ return m_pwszBuffer; }
			w_font_node* get_font_node() const						{ return m_pFontNode; }

#pragma endregion

#pragma region Setters

			bool set_buffer_size(_In_ int pSize);
			bool set_text(_In_z_ LPCWSTR pText);
			void set_font_node(_In_opt_ w_font_node* pFontNode)		{ m_pFontNode = pFontNode; }

#pragma endregion
			
			void clear();

			// Inserts the char at specified index. If nIndex == -1, insert to the end.
			bool insert_char(_In_ int nIndex, _In_ WCHAR wChar);
			// Removes the char at specified index. If nIndex == -1, remove the last char.
			bool remove_char(_In_ int nIndex);
			/*
				Inserts the first nCount characters of the string pStr at specified index.  
				If nCount == -1, the entire string is inserted. If nIndex == -1, insert to the end.
			*/
			bool insert_string(_In_ int nIndex, _In_z_ const WCHAR* pStr, _In_ int nCount = -1);
			
			// Uniscribe
			bool CP_to_X(_In_ int pCP, _In_ bool pTrail, _Out_ int* pX);
			bool X_to_CP(_In_ int pX, _Out_ int* pCP, _Out_ int* pTrail);
			void get_prior_item_pos(_In_ int pCP, _Out_ int* pPrior);
			void get_next_item_pos(_In_ int pCP, _Out_ int* pPrior);

			const wchar_t& operator[](_In_ int p) const
			{
				return m_pwszBuffer[p];
			}
			wchar_t& operator[](_In_ int n);

		private:
			HRESULT analyse();// Uniscribe -- Analyse() analyses the string in the buffer

			wchar_t* m_pwszBuffer;// Buffer to hold text
			int m_nBufferSize;// Size of the buffer allocated, in characters
			w_font_node* m_pFontNode;// Font node for the font that this buffer uses
			bool m_bAnalyseRequired;// True if the string has changed since last analysis.
			SCRIPT_STRING_ANALYSIS m_Analysis;// Analysis for the current string
		};
	}
}

#endif
