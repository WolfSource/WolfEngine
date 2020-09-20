#include "w_directX_pch.h"
#include "w_editBox.h"
#include "w_gui/w_dialog_resource_manager.h"

using namespace wolf::gui;

// Static member initialization
bool w_editBox::s_bHideCaret;   // If true, we don't render the caret.

/*
	When scrolling, EDITBOX_SCROLLEXTENT is reciprocal of the amount to scroll.
	If EDITBOX_SCROLLEXTENT = 4, then we scroll 1/4 of the control each time.
*/
#define EDITBOX_SCROLLEXTENT 4

w_editBox::w_editBox(_In_opt_ w_dialog* pParent) : _super(pParent)
{
	_super::type = W_GUI_CONTROL_EDITBOX;
	_super::parent_dialog = pParent;

	this->m_nBorder = 5;  // Default border width
	this->m_nSpacing = 4;  // Default spacing
	this->m_bCaretOn = true;
	this->m_dfBlink = GetCaretBlinkTime() * 0.001f;
	this->m_dfLastBlink = wolf::system::w_time_span::now();
	this->s_bHideCaret = false;
	this->m_nFirstVisible = 0;
	this->m_TextColor = ARGB_TO_D3DCOLOR(255, 16, 16, 16);
	this->m_SelTextColor = ARGB_TO_D3DCOLOR(255, 255, 255, 255);
	this->m_SelBkColor = ARGB_TO_D3DCOLOR(255, 40, 50, 92);
	this->m_CaretColor = ARGB_TO_D3DCOLOR(255, 0, 0, 0);
	this->m_nCaret = m_nSelStart = 0;
	this->m_bInsertMode = true;
	this->m_bMouseDrag = false;
}

w_editBox::~w_editBox()
{

}

void w_editBox::place_caret(_In_ int pCP)
{
	assert(pCP >= 0 && pCP <= this->m_Buffer.get_text_size());
	this->m_nCaret = pCP;

	// Obtain the X offset of the character.
	int nX1st, nX, nX2;
	this->m_Buffer.CP_to_X(m_nFirstVisible, FALSE, &nX1st);  // 1st visible char
	this->m_Buffer.CP_to_X(pCP, FALSE, &nX);  // LEAD
	
	// If nCP is the nul terminator, get the leading edge instead of trailing.
	if (pCP == m_Buffer.get_text_size())
	{
		nX2 = nX;
	}
	else
	{
		m_Buffer.CP_to_X(pCP, TRUE, &nX2);  // TRAIL
	}
	
	// If the left edge of the char is smaller than the left edge of the 1st visible char, we need to scroll left until this char is visible.
	if (nX < nX1st)
	{
		// Simply make the first visible character the char at the new caret position.
		m_nFirstVisible = pCP;
	}
	else
	{
		// If the right of the character is bigger than the offset of the control's
		// right edge, we need to scroll right to this character.
		if (nX2 > nX1st + (m_rcText.right - m_rcText.left))
		{
			// Compute the X of the new left-most pixel
			int nXNewLeft = nX2 - (m_rcText.right - m_rcText.left);

			// Compute the char position of this character
			int nCPNew1st, nNewTrail;
			m_Buffer.X_to_CP(nXNewLeft, &nCPNew1st, &nNewTrail);

			// If this coordinate is not on a character border,
			// start from the next character so that the caret
			// position does not fall outside the text rectangle.
			int nXNew1st;
			m_Buffer.CP_to_X(nCPNew1st, FALSE, &nXNew1st);
			if (nXNew1st < nXNewLeft)
			{
				++nCPNew1st;
			}
			m_nFirstVisible = nCPNew1st;
		}
	}
}

void w_editBox::clear_text()
{
	m_Buffer.clear();
	m_nFirstVisible = 0;
	place_caret(0);
	m_nSelStart = 0;
}

void w_editBox::set_text(_In_z_ LPCWSTR pText, _In_ bool pSelected)
{
	assert(pText);

	m_Buffer.set_text(pText);
	m_nFirstVisible = 0;

	// Move the caret to the end of the text
	place_caret(m_Buffer.get_text_size());
	m_nSelStart = pSelected ? 0 : m_nCaret;
}

_Use_decl_annotations_
HRESULT w_editBox::get_text_copy(LPWSTR pStrDest, UINT pBufferCount) const
{
	assert(pStrDest);

	wcscpy_s(pStrDest, pBufferCount, m_Buffer.get_buffer());

	return S_OK;
}

void w_editBox::delete_selection_text()
{
	int nFirst = std::min(m_nCaret, m_nSelStart);
	int nLast = std::max(m_nCaret, m_nSelStart);
	// Update caret and selection
	place_caret(nFirst);
	m_nSelStart = m_nCaret;
	// Remove the characters
	for (int i = nFirst; i < nLast; ++i)
	{
		m_Buffer.remove_char(nFirst);
	}
}

void w_editBox::update_rects()
{
	w_control::update_rects();

	// Update the text rectangle
	m_rcText = _super::boundingBox;
	// First inflate by m_nBorder to compute render rects
	InflateRect(&m_rcText, -m_nBorder, -m_nBorder);

	// Update the render rectangles
	m_rcRender[0] = m_rcText;
	SetRect(&m_rcRender[1], _super::boundingBox.left, _super::boundingBox.top, m_rcText.left, m_rcText.top);
	SetRect(&m_rcRender[2], m_rcText.left, _super::boundingBox.top, m_rcText.right, m_rcText.top);
	SetRect(&m_rcRender[3], m_rcText.right, _super::boundingBox.top, _super::boundingBox.right, m_rcText.top);
	SetRect(&m_rcRender[4], _super::boundingBox.left, m_rcText.top, m_rcText.left, m_rcText.bottom);
	SetRect(&m_rcRender[5], m_rcText.right, m_rcText.top, _super::boundingBox.right, m_rcText.bottom);
	SetRect(&m_rcRender[6], _super::boundingBox.left, m_rcText.bottom, m_rcText.left, _super::boundingBox.bottom);
	SetRect(&m_rcRender[7], m_rcText.left, m_rcText.bottom, m_rcText.right, _super::boundingBox.bottom);
	SetRect(&m_rcRender[8], m_rcText.right, m_rcText.bottom, _super::boundingBox.right, _super::boundingBox.bottom);

	// Inflate further by m_nSpacing
	InflateRect(&m_rcText, -m_nSpacing, -m_nSpacing);
}

bool w_editBox::can_have_focus()
{
	return _super::visible && _super::enabled;
}

#pragma warning(push)
#pragma warning( disable : 4616 6386 )
void w_editBox::copy_to_clipboard()
{
	// Copy the selection text to the clipboard
	if (m_nCaret != m_nSelStart && OpenClipboard(nullptr))
	{
		EmptyClipboard();

		HGLOBAL hBlock = GlobalAlloc(GMEM_MOVEABLE, sizeof(WCHAR) * (m_Buffer.get_text_size() + 1));
		if (hBlock)
		{
			auto pwszText = reinterpret_cast<WCHAR*>(GlobalLock(hBlock));
			if (pwszText)
			{
				int nFirst = std::min(m_nCaret, m_nSelStart);
				int nLast = std::max(m_nCaret, m_nSelStart);
				if (nLast - nFirst > 0)
				{
					memcpy(pwszText, m_Buffer.get_buffer() + nFirst, (nLast - nFirst) * sizeof(WCHAR));
				}
				pwszText[nLast - nFirst] = L'\0';  // Terminate it
				GlobalUnlock(hBlock);
			}
			SetClipboardData(CF_UNICODETEXT, hBlock);
		}
		CloseClipboard();
		// We must not free the object until CloseClipboard is called.
		if (hBlock)
		{
			GlobalFree(hBlock);
		}
	}
}

void w_editBox::paste_from_clipboard()
{
	delete_selection_text();

	if (OpenClipboard(nullptr))
	{
		HANDLE handle = GetClipboardData(CF_UNICODETEXT);
		if (handle)
		{
			// Convert the ANSI string to Unicode, then
			// insert to our buffer.
			auto pwszText = reinterpret_cast<WCHAR*>(GlobalLock(handle));
			if (pwszText)
			{
				// Copy all characters up to null.
				if (m_Buffer.insert_string(m_nCaret, pwszText))
				{
					place_caret(m_nCaret + (int) wcslen(pwszText));
				}
				m_nSelStart = m_nCaret;
				GlobalUnlock(handle);
			}
		}
		CloseClipboard();
	}
}
#pragma warning(pop)

_Use_decl_annotations_
bool w_editBox::handle_keyboard(UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	W_UNUSED(pLParam);

	if (!_super::enabled || !_super::visible) return false;

	bool bHandled = false;

	switch (pMsg)
	{
	case WM_KEYDOWN:
	{
		switch (pWParam)
		{
		case VK_TAB:
			// We don't process Tab in case keyboard input is enabled and the user
			// wishes to Tab to other controls.
			break;

		case VK_HOME:
			place_caret(0);
			if (GetKeyState(VK_SHIFT) >= 0)
			{
				// Shift is not down. Update selection
				// start along with the caret.
				m_nSelStart = m_nCaret;
			}
			reset_caret_blink();
			bHandled = true;
			break;

		case VK_END:
			place_caret(m_Buffer.get_text_size());
			if (GetKeyState(VK_SHIFT) >= 0)
			{
				// Shift is not down. Update selection
				// start along with the caret.
				m_nSelStart = m_nCaret;
			}
			reset_caret_blink();
			bHandled = true;
			break;

		case VK_INSERT:
			if (GetKeyState(VK_CONTROL) < 0)
			{
				// Control Insert. Copy to clipboard
				copy_to_clipboard();
			}
			else if (GetKeyState(VK_SHIFT) < 0)
			{
				// Shift Insert. Paste from clipboard
				paste_from_clipboard();
			}
			else
			{
				// Toggle caret insert mode
				m_bInsertMode = !m_bInsertMode;
			}
			break;

		case VK_DELETE:
			// Check if there is a text selection.
			if (m_nCaret != m_nSelStart)
			{
				delete_selection_text();
				_super::parent_dialog->send_event(W_EVENT_EDITBOX_CHANGE, true, this);
			}
			else
			{
				// Deleting one character
				if (m_Buffer.remove_char(m_nCaret))
				{
					_super::parent_dialog->send_event(W_EVENT_EDITBOX_CHANGE, true, this);
				}
			}
			reset_caret_blink();
			bHandled = true;
			break;

		case VK_LEFT:
			if (GetKeyState(VK_CONTROL) < 0)
			{
				// Control is down. Move the caret to a new item
				// instead of a character.
				m_Buffer.get_prior_item_pos(m_nCaret, &m_nCaret);
				place_caret(m_nCaret);
			}
			else if (m_nCaret > 0)
			{
				place_caret(m_nCaret - 1);
			}
			if (GetKeyState(VK_SHIFT) >= 0)
			{
				// Shift is not down. Update selection
				// start along with the caret.
				m_nSelStart = m_nCaret;
			}
			reset_caret_blink();
			bHandled = true;
			break;

		case VK_RIGHT:
			if (GetKeyState(VK_CONTROL) < 0)
			{
				// Control is down. Move the caret to a new item
				// instead of a character.
				m_Buffer.get_next_item_pos(m_nCaret, &m_nCaret);
				place_caret(m_nCaret);
			}
			else if (m_nCaret < m_Buffer.get_text_size())
			{
				place_caret(m_nCaret + 1);
			}
			if (GetKeyState(VK_SHIFT) >= 0)
			{
				// Shift is not down. Update selection
				// start along with the caret.
				m_nSelStart = m_nCaret;
			}
			reset_caret_blink();
			bHandled = true;
			break;

		case VK_UP:
		case VK_DOWN:
			// Trap up and down arrows so that the dialog
			// does not switch focus to another control.
			bHandled = true;
			break;

		default:
			bHandled = pWParam != VK_ESCAPE;  // Let the application handle Esc.
		}
	}
	}
	return bHandled;
}

_Use_decl_annotations_
bool w_editBox::handle_mouse(UINT pMsg, const POINT& pPoint, WPARAM pWParam, LPARAM pLParam)
{
	W_UNUSED(pWParam);
	W_UNUSED(pLParam);

	if (!_super::enabled || !_super::visible) return false;

	switch (pMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	{
		if (!_super::has_focus)
		{
			_super::parent_dialog->request_focus(this);
		}

		if (!contains_point(pPoint)) return false;

		m_bMouseDrag = true;
		SetCapture(w_dialog_resource_manager::get_HWND());

		// Determine the character corresponding to the coordinates.
		int nCP, nTrail, nX1st;
		m_Buffer.CP_to_X(m_nFirstVisible, FALSE, &nX1st);  // X offset of the 1st visible char
		if (m_Buffer.X_to_CP(pPoint.x - m_rcText.left + nX1st, &nCP, &nTrail))
		{
			// Cap at the nul character.
			if (nTrail && nCP < m_Buffer.get_text_size())
			{
				place_caret(nCP + 1);
			}
			else
			{
				place_caret(nCP);
			}
			m_nSelStart = m_nCaret;
			reset_caret_blink();
		}
		return true;
	}

	case WM_LBUTTONUP:
		ReleaseCapture();
		m_bMouseDrag = false;
		break;

	case WM_MOUSEMOVE:
		if (m_bMouseDrag)
		{
			// Determine the character corresponding to the coordinates.
			int nCP, nTrail, nX1st;
			m_Buffer.CP_to_X(m_nFirstVisible, FALSE, &nX1st);  // X offset of the 1st visible char
			if (m_Buffer.X_to_CP(pPoint.x - m_rcText.left + nX1st, &nCP, &nTrail))
			{
				// Cap at the nul character.
				if (nTrail && nCP < m_Buffer.get_text_size())
				{
					place_caret(nCP + 1);
				}
				else
				{
					place_caret(nCP);
				}
			}
		}
		break;
	}

	return false;
}

void w_editBox::on_focus_in()
{
	w_control::on_focus_in();

	reset_caret_blink();
}

_Use_decl_annotations_
bool w_editBox::on_msg_proc(UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	W_UNUSED(pLParam);

	if (!_super::enabled || !_super::visible) return false;

	switch (pMsg)
	{
		// Make sure that while editing, the keyup and keydown messages associated with 
		// WM_CHAR messages don't go to any non-focused controls or cameras
	case WM_KEYUP:
	case WM_KEYDOWN:
		return true;

	case WM_CHAR:
	{
		switch ((WCHAR) pWParam)
		{
			// Backspace
		case VK_BACK:
		{
			// If there's a selection, treat this
			// like a delete key.
			if (m_nCaret != m_nSelStart)
			{
				delete_selection_text();
				_super::parent_dialog->send_event(W_EVENT_EDITBOX_CHANGE, true, this);
			}
			else if (m_nCaret > 0)
			{
				// Move the caret, then delete the char.
				place_caret(m_nCaret - 1);
				m_nSelStart = m_nCaret;
				m_Buffer.remove_char(m_nCaret);
				_super::parent_dialog->send_event(W_EVENT_EDITBOX_CHANGE, true, this);
			}
			reset_caret_blink();
			break;
		}

		case 24:        // Ctrl-X Cut
		case VK_CANCEL: // Ctrl-C Copy
		{
			copy_to_clipboard();

			// If the key is Ctrl-X, delete the selection too.
			if ((WCHAR) pWParam == 24)
			{
				delete_selection_text();
				_super::parent_dialog->send_event(W_EVENT_EDITBOX_CHANGE, true, this);
			}

			break;
		}

		// Ctrl-V Paste
		case 22:
		{
			paste_from_clipboard();
			_super::parent_dialog->send_event(W_EVENT_EDITBOX_CHANGE, true, this);
			break;
		}

		// Ctrl-A Select All
		case 1:
			if (m_nSelStart == m_nCaret)
			{
				m_nSelStart = 0;
				place_caret(m_Buffer.get_text_size());
			}
			break;

		case VK_RETURN:
			// Invoke the callback when the user presses Enter.
			_super::parent_dialog->send_event(W_EVENT_EDITBOX_STRING, true, this);
			break;

			// Junk characters we don't want in the string
		case 26:  // Ctrl Z
		case 2:   // Ctrl B
		case 14:  // Ctrl N
		case 19:  // Ctrl S
		case 4:   // Ctrl D
		case 6:   // Ctrl F
		case 7:   // Ctrl G
		case 10:  // Ctrl J
		case 11:  // Ctrl K
		case 12:  // Ctrl L
		case 17:  // Ctrl Q
		case 23:  // Ctrl W
		case 5:   // Ctrl E
		case 18:  // Ctrl R
		case 20:  // Ctrl T
		case 25:  // Ctrl Y
		case 21:  // Ctrl U
		case 9:   // Ctrl I
		case 15:  // Ctrl O
		case 16:  // Ctrl P
		case 27:  // Ctrl [
		case 29:  // Ctrl ]
		case 28:  // Ctrl \ 
			break;

		default:
		{
			// If there's a selection and the user
			// starts to type, the selection should
			// be deleted.
			if (m_nCaret != m_nSelStart)
			{
				delete_selection_text();
			}

			// If we are in overwrite mode and there is already
			// a char at the caret's position, simply replace it.
			// Otherwise, we insert the char as normal.
			if (!m_bInsertMode && m_nCaret < m_Buffer.get_text_size())
			{
				m_Buffer[m_nCaret] = (WCHAR) pWParam;
				place_caret(m_nCaret + 1);
				m_nSelStart = m_nCaret;
			}
			else
			{
				// Insert the char
				if (m_Buffer.insert_char(m_nCaret, (WCHAR) pWParam))
				{
					place_caret(m_nCaret + 1);
					m_nSelStart = m_nCaret;
				}
			}
			reset_caret_blink();
			_super::parent_dialog->send_event(W_EVENT_EDITBOX_CHANGE, true, this);
		}
		}
		return true;
	}
	}
	return false;
}

void w_editBox::render(_In_ ID3D11Device1* pDevice, _In_ ID3D11DeviceContext1* pContext, _In_ float pElapsedTime)
{
	if (_super::visible == false) return;

	int nSelStartX = 0, nCaretX = 0;  // Left and right X cordinates of the selection region

	auto pElement = get_element(0);
	if (pElement)
	{
		m_Buffer.set_font_node(_super::parent_dialog->get_font(pElement->index_font));
		place_caret(m_nCaret);  // Call PlaceCaret now that we have the font info (node),
							   // so that scrolling can be handled.
	}

	// Render the control graphics
	for (int e = 0; e < 9; ++e)
	{
		pElement = _super::elements[e];
		pElement->texture_color.blend(W_GUI_STATE_NORMAL, pElapsedTime);
		_super::parent_dialog->draw_sprite(pDevice, pContext, pElement, &m_rcRender[e], W_GUI_FAR_BUTTON_DEPTH);
	}

	// Compute the X coordinates of the first visible character.
	int nXFirst;
	m_Buffer.CP_to_X(m_nFirstVisible, FALSE, &nXFirst);

	// Compute the X coordinates of the selection rectangle
	m_Buffer.CP_to_X(m_nCaret, FALSE, &nCaretX);
	if (m_nCaret != m_nSelStart)
	{
		m_Buffer.CP_to_X(m_nSelStart, FALSE, &nSelStartX);
	}
	else
	{
		nSelStartX = nCaretX;
	}

	// Render the selection rectangle
	RECT rcSelection;  // Make this available for rendering selected text
	if (m_nCaret != m_nSelStart)
	{
		int nSelLeftX = nCaretX, nSelRightX = nSelStartX;
		// Swap if left is bigger than right
		if (nSelLeftX > nSelRightX)
		{
			int nTemp = nSelLeftX; nSelLeftX = nSelRightX; nSelRightX = nTemp;
		}

		SetRect(&rcSelection, nSelLeftX, m_rcText.top, nSelRightX, m_rcText.bottom);
		OffsetRect(&rcSelection, m_rcText.left - nXFirst, 0);
		IntersectRect(&rcSelection, &m_rcText, &rcSelection);

		_super::parent_dialog->draw_rect(&rcSelection, m_SelBkColor);
	}

	// Render the text
	// Element 0 for text
	_super::elements[0]->font_color.set_current(m_TextColor);
	////////////////////////////_super::parent_dialog->draw_text(pDevice, pContext, m_Buffer.get_buffer() + m_nFirstVisible, this->elements[0], &m_rcText);

	// Render the selected text
	if (m_nCaret != m_nSelStart)
	{
		int nFirstToRender = std::max(m_nFirstVisible, std::min(m_nSelStart, m_nCaret));
		_super::elements[0]->font_color.set_current(m_SelTextColor);
		///////////////////////////////////////////_super::parent_dialog->draw_text(pDevice, pContext, m_Buffer.get_buffer() + nFirstToRender, _super::elements[0], &rcSelection, false);
	}

	// Blink the caret
	auto _now_time_span = wolf::system::w_time_span::now();
	if ((_now_time_span - m_dfLastBlink).get_total_milliseconds() >= m_dfBlink)
	{
		m_bCaretOn = !m_bCaretOn;
		m_dfLastBlink = _now_time_span;
	}

	// Render the caret if this control has the focus
	if (_super::has_focus && m_bCaretOn && !s_bHideCaret)
	{
		// Start the rectangle with insert mode caret
		RECT rcCaret =
		{
			m_rcText.left - nXFirst + nCaretX - 1, m_rcText.top,
			m_rcText.left - nXFirst + nCaretX + 1, m_rcText.bottom
		};

		// If we are in overwrite mode, adjust the caret rectangle
		// to fill the entire character.
		if (!m_bInsertMode)
		{
			// Obtain the right edge X coord of the current character
			int nRightEdgeX;
			m_Buffer.CP_to_X(m_nCaret, TRUE, &nRightEdgeX);
			rcCaret.right = m_rcText.left - nXFirst + nRightEdgeX;
		}

		(_super::parent_dialog)->draw_rect(&rcCaret, m_CaretColor);
	}
}


#define IN_FLOAT_CHARSET( c ) \
    ( (c) == L'-' || (c) == L'.' || ( (c) >= L'0' && (c) <= L'9' ) )

_Use_decl_annotations_
void w_editBox::parse_float_array(float* pNumbers, int pCount)
{
	int nWritten = 0;  // Number of floats written
	const WCHAR* pToken, *pEnd;
	WCHAR wszToken[60];

	pToken = m_Buffer.get_buffer();
	while (nWritten < pCount && *pToken != L'\0')
	{
		// Skip leading spaces
		while (*pToken == L' ')
		{
			++pToken;
		}

		if (*pToken == L'\0') break;

		// Locate the end of number
		pEnd = pToken;
		while (IN_FLOAT_CHARSET(*pEnd))
		{
			++pEnd;
		}

		// Copy the token to our buffer
		int nTokenLen = std::min<int>(sizeof(wszToken) / sizeof(wszToken[0]) - 1, int(pEnd - pToken));
		wcscpy_s(wszToken, nTokenLen, pToken);
		*pNumbers = (float) wcstod(wszToken, nullptr);
		++nWritten;
		++pNumbers;
		pToken = pEnd;
	}
}

_Use_decl_annotations_
void w_editBox::set_text_float_array(const float* pNumbers, int nCount)
{
	WCHAR wszBuffer[512] =
	{
		0
	};
	WCHAR wszTmp[64];

	if (!pNumbers)
		return;

	for (int i = 0; i < nCount; ++i)
	{
		swprintf_s(wszTmp, 64, L"%.4f ", pNumbers[i]);
		wcscat_s(wszBuffer, 512, wszTmp);
	}

	// Don't want the last space
	if (nCount > 0 && wcslen(wszBuffer) > 0)
		wszBuffer[wcslen(wszBuffer) - 1] = 0;

	set_text(wszBuffer);
}

void w_editBox::reset_caret_blink()
{
	m_bCaretOn = true;
	m_dfLastBlink = wolf::system::w_time_span::now();
}

