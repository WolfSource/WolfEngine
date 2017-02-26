#include "w_directX_pch.h"
#include "w_wchar_buffer.h"

// DXUT_MAX_EDITBOXLENGTH is the maximum string length allowed in edit boxes,
// including the nul terminator.
// 
// Uniscribe does not support strings having bigger-than-16-bits length.
// This means that the string must be less than 65536 characters long,
// including the nul terminator.
#define W_GUI_MAX_EDITBOXLENGTH 0xFFFF

using namespace wolf::gui;

w_wchar_buffer::w_wchar_buffer(_In_ int pInitialSize)
{
	m_nBufferSize = 0;
	m_pwszBuffer = nullptr;
	m_bAnalyseRequired = true;
	m_Analysis = nullptr;
	m_pFontNode = nullptr;

	if (pInitialSize > 0)
	{
		set_buffer_size(pInitialSize);
	}
}

w_wchar_buffer::~w_wchar_buffer()
{
	delete [] m_pwszBuffer;
	if (m_Analysis)
	{
		(void) ScriptStringFree(&m_Analysis);
	}
}

HRESULT w_wchar_buffer::analyse()
{
	if (m_Analysis)
	{
		(void) ScriptStringFree(&m_Analysis);
	}

	SCRIPT_CONTROL ScriptControl; // For uniscribe
	SCRIPT_STATE ScriptState;   // For uniscribe
	ZeroMemory(&ScriptControl, sizeof(ScriptControl));
	ZeroMemory(&ScriptState, sizeof(ScriptState));

#pragma warning(push)
#pragma warning(disable : 4616 6309 6387 )
	HRESULT hr = ScriptApplyDigitSubstitution(nullptr, &ScriptControl, &ScriptState);
	if (FAILED(hr))
		return hr;
#pragma warning(pop)

	if (!m_pFontNode) return E_FAIL;

	HDC hDC = nullptr;
	hr = ScriptStringAnalyse(hDC,
		m_pwszBuffer,
		(int) wcslen(m_pwszBuffer) + 1,  // nul is also analyzed.
		(int) wcslen(m_pwszBuffer) * 3 / 2 + 16,
		-1,
		SSA_BREAK | SSA_GLYPHS | SSA_FALLBACK | SSA_LINK,
		0,
		&ScriptControl,
		&ScriptState,
		nullptr,
		nullptr,
		nullptr,
		&m_Analysis);
	if (SUCCEEDED(hr))
	{
		m_bAnalyseRequired = false;  // Analysis is up-to-date
	}

	return hr;
}

WCHAR& w_wchar_buffer::operator[](_In_ int n)  // No param checking
{
	// This version of operator[] is called only
	// if we are asking for write access, so
	// re-analysis is required.
	m_bAnalyseRequired = true;
	return m_pwszBuffer[n];
}

void w_wchar_buffer::clear()
{
	*m_pwszBuffer = L'\0';
	m_bAnalyseRequired = true;
}

bool w_wchar_buffer::insert_char(_In_ int nIndex, _In_ WCHAR wChar)
{
	assert(nIndex >= 0);

	if (nIndex < 0 || nIndex >(int)wcslen(m_pwszBuffer))
		return false;  // invalid index

					   // Check for maximum length allowed
	if (get_text_size() + 1 >= W_GUI_MAX_EDITBOXLENGTH) return false;

	if ((int) wcslen(m_pwszBuffer) + 1 >= m_nBufferSize)
	{
		if (!set_buffer_size(-1))
		{
			return false;  // out of memory
		}
	}

	assert(m_nBufferSize >= 2);

	// Shift the characters after the index, start by copying the null terminator
	WCHAR* dest = m_pwszBuffer + wcslen(m_pwszBuffer) + 1;
	WCHAR* stop = m_pwszBuffer + nIndex;
	WCHAR* src = dest - 1;

	while (dest > stop)
	{
		*dest-- = *src--;
	}

	// Set new character
	m_pwszBuffer[nIndex] = wChar;
	m_bAnalyseRequired = true;

	return true;
}

bool w_wchar_buffer::remove_char(_In_ int nIndex)
{
	if (!wcslen(m_pwszBuffer) || nIndex < 0 || nIndex >= (int) wcslen(m_pwszBuffer)) return false;// Invalid index

	MoveMemory(m_pwszBuffer + nIndex, m_pwszBuffer + nIndex + 1, sizeof(WCHAR) * (wcslen(m_pwszBuffer) - nIndex));
	m_bAnalyseRequired = true;
	return true;
}

_Use_decl_annotations_
bool w_wchar_buffer::insert_string(int nIndex, const WCHAR* pStr, int nCount)
{
	assert(nIndex >= 0);
	if (nIndex < 0 || nIndex >(int)wcslen(m_pwszBuffer)) return false; // invalid index 
	
	if (-1 == nCount) nCount = (int) wcslen(pStr);

	// Check for maximum length allowed
	if (get_text_size() + nCount >= W_GUI_MAX_EDITBOXLENGTH) return false;

	if ((int) wcslen(m_pwszBuffer) + nCount >= m_nBufferSize)
	{
		if (!set_buffer_size((int) wcslen(m_pwszBuffer) + nCount + 1)) return false;// out of memory
	}
	MoveMemory(m_pwszBuffer + nIndex + nCount, m_pwszBuffer + nIndex, sizeof(WCHAR) * (wcslen(m_pwszBuffer) - nIndex + 1));
	memcpy(m_pwszBuffer + nIndex, pStr, nCount * sizeof(WCHAR));
	m_bAnalyseRequired = true;

	return true;
}

bool w_wchar_buffer::set_text(_In_z_ LPCWSTR wszText)
{
	assert(wszText);

	size_t nRequired = wcslen(wszText) + 1;

	// Check for maximum length allowed
	if (nRequired >= W_GUI_MAX_EDITBOXLENGTH) return false;

	while (get_buffer_size() < nRequired)
	{
		if (!set_buffer_size(-1)) break;
	}
	// Check again in case out of memory occurred inside while loop.
	if (get_buffer_size() >= nRequired)
	{
		wcscpy_s(m_pwszBuffer, get_buffer_size(), wszText);
		m_bAnalyseRequired = true;
		return true;
	}
	else
	{
		return false;
	}
}

_Use_decl_annotations_
bool w_wchar_buffer::CP_to_X(int nCP, bool bTrail, int* pX)
{
	assert(pX);
	*pX = 0;  // Default

	HRESULT _hr = S_OK;
	if (m_bAnalyseRequired)
	{
		_hr = analyse();
	}

	if (SUCCEEDED(_hr))
	{
		_hr = ScriptStringCPtoX(m_Analysis, nCP, bTrail, pX);
	}
	if (FAILED(_hr))
	{
		*pX = 0;
		return false;
	}

	return true;
}

_Use_decl_annotations_
bool w_wchar_buffer::X_to_CP(int pX, int* pCP, int* pTrail)
{
	assert(pCP && pTrail);
	*pCP = 0;
	*pTrail = FALSE;

	HRESULT _hr = S_OK;
	if (m_bAnalyseRequired)
	{
		_hr = analyse();
	}

	if (SUCCEEDED(_hr))
	{
		_hr = ScriptStringXtoCP(m_Analysis, pX, pCP, pTrail);
		if (FAILED(_hr))
		{
			*pCP = 0;
			*pTrail = FALSE;
			return false;
		}
	}

	// If the coordinate falls outside the text region, we
	// can get character positions that don't exist.  We must
	// filter them here and convert them to those that do exist.
	if (*pCP == -1 && *pTrail == TRUE)
	{
		*pCP = 0;
		*pTrail = FALSE;
	}
	else if (*pCP > (int) wcslen(m_pwszBuffer) && *pTrail == FALSE)
	{
		*pCP = (int) wcslen(m_pwszBuffer);
		*pTrail = TRUE;
	}

	if (FAILED(_hr))
	{
		*pCP = 0;
		*pTrail = FALSE;
		return false;
	}
	return true;
}

_Use_decl_annotations_
void w_wchar_buffer::get_prior_item_pos(int pCP, int* pPrior)
{
	*pPrior = pCP;  // Default is the char itself

	if (m_bAnalyseRequired)
	{
		if (FAILED(analyse())) return;
	}

	const SCRIPT_LOGATTR* pLogAttr = ScriptString_pLogAttr(m_Analysis);
	if (!pLogAttr) return;

	if (!ScriptString_pcOutChars(m_Analysis))
		return;
	int nInitial = *ScriptString_pcOutChars(m_Analysis);
	if (pCP - 1 < nInitial)
	{
		nInitial = pCP - 1;
	}
	for (int i = nInitial; i > 0; --i)
	{
		if (pLogAttr[i].fWordStop ||       // Either the fWordStop flag is set
			(!pLogAttr[i].fWhiteSpace &&  // Or the previous char is whitespace but this isn't.
				pLogAttr[i - 1].fWhiteSpace))
		{
			*pPrior = i;
			return;
		}
	}
	// We have reached index 0.  0 is always a break point, so simply return it.
	*pPrior = 0;
}

_Use_decl_annotations_
void w_wchar_buffer::get_next_item_pos(int pCP, int* pPrior)
{
	*pPrior = pCP;  // Default is the char itself

	HRESULT _hr = S_OK;
	if (m_bAnalyseRequired)
		_hr = analyse();
	if (FAILED(_hr)) return;

	const SCRIPT_LOGATTR* pLogAttr = ScriptString_pLogAttr(m_Analysis);
	if (!pLogAttr) return;

	if (!ScriptString_pcOutChars(m_Analysis)) return;
	int nInitial = *ScriptString_pcOutChars(m_Analysis);
	if (pCP + 1 < nInitial)
	{
		nInitial = pCP + 1;
	}

	int i = nInitial;
	int limit = *ScriptString_pcOutChars(m_Analysis);
	while (limit > 0 && i < limit - 1)
	{
		if (pLogAttr[i].fWordStop)      // Either the fWordStop flag is set
		{
			*pPrior = i;
			return;
		}
		else if (pLogAttr[i].fWhiteSpace &&  // Or this whitespace but the next char isn't.
			!pLogAttr[i + 1].fWhiteSpace)
		{
			*pPrior = i + 1;  // The next char is a word stop
			return;
		}

		++i;
		limit = *ScriptString_pcOutChars(m_Analysis);
	}
	// We have reached the end. It's always a word stop, so simply return it.
	*pPrior = *ScriptString_pcOutChars(m_Analysis) - 1;
}

bool w_wchar_buffer::set_buffer_size(_In_ int nNewSize)
{
	// If the current size is already the maximum allowed,
	// we can't possibly allocate more.
	if (m_nBufferSize >= W_GUI_MAX_EDITBOXLENGTH) return false;

	int nAllocateSize = (nNewSize == -1 || nNewSize < m_nBufferSize * 2) ? (m_nBufferSize ? m_nBufferSize * 2 : 256) : nNewSize * 2;

	// Cap the buffer size at the maximum allowed.
	if (nAllocateSize > W_GUI_MAX_EDITBOXLENGTH)
	{
		nAllocateSize = W_GUI_MAX_EDITBOXLENGTH;
	}

	auto pTempBuffer = new (std::nothrow) WCHAR[nAllocateSize];
	if (!pTempBuffer) return false;

	ZeroMemory(pTempBuffer, sizeof(WCHAR) * nAllocateSize);

	if (m_pwszBuffer)
	{
		memcpy(pTempBuffer, m_pwszBuffer, m_nBufferSize * sizeof(WCHAR));
		delete [] m_pwszBuffer;
	}

	m_pwszBuffer = pTempBuffer;
	m_nBufferSize = nAllocateSize;
	return true;
}
