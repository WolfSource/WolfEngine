#include "w_directX_pch.h"
#include "w_sprite_font.h"

using namespace std;
using namespace D2D1;
using namespace wolf::graphics;

w_sprite_font::w_sprite_font(w_sprite_font_style pFontStyle) : _fontStyle(pFontStyle), _right_to_left(false)
{
	_super::set_class_name(typeid(this).name());
}

w_sprite_font::~w_sprite_font()
{
	release();
}

HRESULT w_sprite_font::load(_In_ ID2D1DeviceContext* pContext, _In_ IDWriteFactory* pWriteFactory)
{
	auto _hr = S_FALSE;

	_hr = pWriteFactory->CreateTextFormat(
		this->_fontStyle.NAME == nullptr ? L"Times New Roman" : this->_fontStyle.NAME,
		nullptr,
		this->_fontStyle.WEIGHT,
		this->_fontStyle.STYLE,
		this->_fontStyle.STRETCH,
		this->_fontStyle.SIZE,
		this->_fontStyle.LOCAL,
		&this->_text_format);
	V(_hr, L"Create text format", this->name, 2, false, true);

	_hr = this->_text_format->SetTextAlignment(this->_fontStyle.TEXT_ALIGNMENT);
	V(_hr, L"Setting text alignment", this->name, 2, false, true);

	_hr = this->_text_format->SetParagraphAlignment(this->_fontStyle.PARAGRAPH_ALIGNMENT);
	V(_hr, L"Setting paragraph alignment", this->name, 2, false, true);

	if (this->_brush == nullptr)
	{
		_hr = set_color(pContext, ColorF::White);
	}

	std::wstring _text = L"Wolf";
	_hr = pWriteFactory->CreateTextLayout(
		_text.c_str(),
		static_cast<UINT32>(_text.size()),
		this->_text_format.Get(),
		4096.0f,
		4096.0f,
		&this->_text_layout);
	V(_hr, L"create text layout", this->name, 2, false, true);
	
	return _hr;
}

DWRITE_FONT_METRICS w_sprite_font::get_font_metrics() const
{
	DWRITE_FONT_METRICS _metrics;
	std::memset(&_metrics, 0, sizeof(_metrics));

	if (this->_text_layout != nullptr)
	{
		IDWriteFontCollection* _collection;
		UINT32 _index;
		BOOL _exists;

		auto _size = std::char_traits<wchar_t>::length(this->_fontStyle.NAME);
		auto _font_family_name = const_cast<WCHAR*>(this->_fontStyle.NAME);

		this->_text_layout->GetFontCollection(0, &_collection);
		_collection->FindFamilyName(_font_family_name, &_index, &_exists);

		IDWriteFontFamily* _font_family;
		_collection->GetFontFamily(_index, &_font_family);

		IDWriteFont* _font;
		_font_family->GetFirstMatchingFont(this->_fontStyle.WEIGHT, this->_fontStyle.STRETCH, this->_fontStyle.STYLE, &_font);

		_font->GetMetrics(&_metrics);
	}

	return _metrics;
	
	//float _ratio = this->_fontStyle.SIZE / (float) _metrics.designUnitsPerEm;
	//float _size_rate = (_metrics.ascent + _metrics.descent + _metrics.lineGap) * _ratio;

	//int _retval = static_cast<int>(pWindowHeight / _size_rate);
	//
	//_font_family->Release();
	//_collection->Release();
	//_font->Release();
	//
	//return _retval;
}

HRESULT w_sprite_font::set_color(_In_ ID2D1DeviceContext* pContext, ColorF color)
{
	auto _hr = pContext->CreateSolidColorBrush(color, this->_brush.GetAddressOf());
	V(_hr, L"Setting color of font", this->name, 2, false, true);
	return _hr;
}


ULONG w_sprite_font::release()
{
	if (_super::is_released()) return 0;
	
	COMPTR_RELEASE(this->_brush);
	COMPTR_RELEASE(this->_text_format);
	COMPTR_RELEASE(this->_text_layout);
	
	return _super::release();
}