#include "w_directX_pch.h"
#include "w_sprite_font.h"

using namespace std;
using namespace D2D1;
using namespace wolf::graphics;

w_sprite_font::w_sprite_font(w_sprite_font_style pFontStyle) : _font_style(pFontStyle), _right_to_left(false)
{
	_super::set_class_name(typeid(this).name());
	std::memset(&this->_font_metrics, 0, sizeof(_font_metrics));
}

w_sprite_font::~w_sprite_font()
{
	release();
}

HRESULT w_sprite_font::load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
	auto _hr = pGDevice->factory_2D->CreatePathGeometry(&this->_path_geometry);
	V(_hr, L"creating path geometry", this->name, 2, false, true);

	_hr = pGDevice->write_factory->CreateTextFormat(
		this->_font_style.NAME == nullptr ? L"Times New Roman" : this->_font_style.NAME,
		nullptr,
		this->_font_style.WEIGHT,
		this->_font_style.STYLE,
		this->_font_style.STRETCH,
		this->_font_style.SIZE,
		this->_font_style.LOCAL,
		&this->_text_format);
	V(_hr, L"Create text format", this->name, 2, false, true);

	_hr = this->_text_format->SetTextAlignment(this->_font_style.TEXT_ALIGNMENT);
	V(_hr, L"Setting text alignment", this->name, 2, false, true);

	_hr = this->_text_format->SetParagraphAlignment(this->_font_style.PARAGRAPH_ALIGNMENT);
	V(_hr, L"Setting paragraph alignment", this->name, 2, false, true);

	if (this->_brush == nullptr)
	{
		_hr = set_color(pGDevice->context_2D.Get(), w_color(255, 255, 255, 255));
	}

	std::wstring _text = L"Wolf";
	_hr = pGDevice->write_factory->CreateTextLayout(
		_text.c_str(),
		static_cast<UINT32>(_text.size()),
		this->_text_format,
		4096.0f,
		4096.0f,
		&this->_text_layout);
	V(_hr, L"create text layout", this->name, 2, false, true);

	_hr = create_font_face_and_metrics();

	return _hr;
}

HRESULT w_sprite_font::create_font_face_and_metrics()
{
	IDWriteFontCollection* _collection;
	UINT32 _index;
	BOOL _exists;

	auto _size = std::char_traits<wchar_t>::length(this->_font_style.NAME);
	auto _font_family_name = const_cast<WCHAR*>(this->_font_style.NAME);

	this->_text_layout->GetFontCollection(0, &_collection);
	_collection->FindFamilyName(_font_family_name, &_index, &_exists);

	IDWriteFontFamily* _font_family;
	_collection->GetFontFamily(_index, &_font_family);

	IDWriteFont* _font;
	if (_font_family)
	{
		_font_family->GetFirstMatchingFont(this->_font_style.WEIGHT, this->_font_style.STRETCH, this->_font_style.STYLE, &_font);

		_font->GetMetrics(&this->_font_metrics);

		//create font face
		auto _hr = _font->CreateFontFace(&this->_font_face);
		V(_hr, L"create font face", this->name, 2, false, true);

		COM_RELEASE(_font);
		COM_RELEASE(_font_family);
		COM_RELEASE(_collection);

		return _hr;
	}
	return S_FALSE;
}

ULONG w_sprite_font::release()
{
	if (_super::get_is_released()) return 0;
	
	COM_RELEASE(this->_brush);
	COM_RELEASE(this->_text_format);
	COM_RELEASE(this->_text_layout);
	COM_RELEASE(this->_path_geometry);
	
	return _super::release();
}

#pragma region Getters

//Get color of font
w_color	w_sprite_font::get_color() const
{
	if (this->_brush)
	{
		auto _color = this->_brush->GetColor();
		return w_color(
			static_cast<unsigned char>(_color.r * 255.000f), 
			static_cast<unsigned char>(_color.g * 255.000f), 
			static_cast<unsigned char>(_color.b * 255.000f), 
			static_cast<unsigned char>(_color.a * 255.000f));
	}
	return w_color();
}

ID2D1PathGeometry1* w_sprite_font::get_text_bounding_box(_In_z_ const wchar_t* pTextShouldBeCalculated, _Inout_ D2D1_RECT_F& pBoundingBox) const
{
	std::memset(&pBoundingBox, -1, sizeof(pBoundingBox));

	auto _length = std::wcslen(pTextShouldBeCalculated);

	auto _glyph_indices = new UINT16[_length];
	std::memset(_glyph_indices, 0, sizeof(UINT16) * _length);

	// Open sink
	ID2D1GeometrySink* _geometry_sink = nullptr;
	auto _hr = this->_path_geometry->Open(&_geometry_sink);
	if (FAILED(_hr))
	{
		V(S_FALSE, L"open geometry sink failed for get_text_bounding_box", this->name, 3, false, true);
		return nullptr;
	}

	// Get glyph run outline
	_hr = _font_face->GetGlyphRunOutline(
		get_font_size(), // font size
		_glyph_indices,
		NULL,
		NULL,
		_length,//text length
		FALSE,
		get_right_to_left(), //right to left
		_geometry_sink);

	delete[] _glyph_indices;

	if (FAILED(_hr))
	{
		V(S_FALSE, L"get glyph run outline failed", this->name, 3, false, true);
		return nullptr;
	}

	// Close sink
	_geometry_sink->Close();
	COM_RELEASE(_geometry_sink);

	this->_path_geometry->GetBounds(D2D1::IdentityMatrix(), &pBoundingBox);
	return this->_path_geometry;
}

IDWriteFontFace* w_sprite_font::get_font_face() const
{
	return this->_font_face;
}

DWRITE_FONT_METRICS w_sprite_font::get_font_metrics() const
{
	return this->_font_metrics;

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

#pragma endregion

#pragma region Setters

HRESULT w_sprite_font::set_color(_In_ ID2D1DeviceContext* pContext, _In_ w_color pColor)
{
	D2D1_COLOR_F _color;
	_color.r = pColor.r / 255.000f;
	_color.g = pColor.g / 255.000f;
	_color.b = pColor.b / 255.000f;
	_color.a = pColor.a / 255.000f;

	auto _hr = pContext->CreateSolidColorBrush(_color, &this->_brush);
	V(_hr, L"Setting color of font", this->name, 2, false, true);
	return _hr;
}
#pragma endregion