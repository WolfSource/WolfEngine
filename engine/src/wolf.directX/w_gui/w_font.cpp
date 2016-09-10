#include "w_directX_pch.h"
#include "w_font.h"

using namespace DirectX;
using namespace wolf::gui;

w_font::w_font(_In_ ID3D11Device1* pDevice, _In_ ID3D11InputLayout* pInputLayout)
{
	_super::set_class_name(typeid(this).name());

	this->_input_layout = pInputLayout;

	// Init the D3D11 font
	this->_font_texture = std::make_unique<wolf::graphics::w_texture_2D>();
	auto _hr = wolf::graphics::w_texture_2D::load_texture_2D(pDevice, this->_font_texture.get(), L"Textures\\GUI\\Font.dds");
	V(_hr, L"loading GUI\\Font.dds asset", _super::name, 3, true, true);
}

w_font::~w_font()
{
	release();
}

void w_font::begin_text()
{
	this->_font_vertices.clear();
}

_Use_decl_annotations_
void w_font::draw_text(ID3D11Device1* pDevice,
	ID3D11DeviceContext1* pContext,
	LPCWSTR pText, 
	const RECT& pRectScreen, 
	DirectX::XMFLOAT4 pFontColor,
	float pWidth, 
	float pHeight, 
	bool bCenter)
{
	float _char_tex_sizeX = 0.010526315f;
	float _glyph_sizeX = 15.0f / pWidth;
	float _glyph_sizeY = 42.0f / pHeight;

	float _rect_left = pRectScreen.left / pWidth;
	float _rect_top = 1.0f - pRectScreen.top / pHeight;

	_rect_left = _rect_left * 2.0f - 1.0f;
	_rect_top = _rect_top * 2.0f - 1.0f;

	int _num_chars = (int)wcslen(pText);
	if (bCenter) 
	{
		float _rect_right = pRectScreen.right / pWidth;
		_rect_right = _rect_right * 2.0f - 1.0f;

		float fRectBottom = 1.0f - pRectScreen.bottom / pHeight;
		fRectBottom = fRectBottom * 2.0f - 1.0f;
		float fcenterx = ((_rect_right - _rect_left) - (float)_num_chars * _glyph_sizeX) *0.5f;
		float fcentery = ((_rect_top - fRectBottom) - (float)1 * _glyph_sizeY) *0.5f;
		_rect_left += fcenterx;
		_rect_top -= fcentery;
	}
	float _original_left = _rect_left;
	float _tex_top = 0.0f;
	float _tex_bottom = 1.0f;

	float fDepth = 0.5f;
	for (int i = 0; i < _num_chars; i++)
	{
		if (pText[i] == '\n')
		{
			_rect_left = _original_left;
			_rect_top -= _glyph_sizeY;

			continue;
		}
		else if (pText[i] < 32 || pText[i] > 126)
		{
			continue;
		}

		// Add 6 sprite vertices
		w_sprite_vertex _sprite_vertex;
		float _rect_right = _rect_left + _glyph_sizeX;
		float _rect_bottom = _rect_top - _glyph_sizeY;
		float _tex_left = (pText[i] - 32) * _char_tex_sizeX;
		float _tex_right = _tex_left + _char_tex_sizeX;

		// tri1
		_sprite_vertex.pos = XMFLOAT3(_rect_left, _rect_top, fDepth);
		_sprite_vertex.tex = XMFLOAT2(_tex_left, _tex_top);
		_sprite_vertex.color = pFontColor;
		this->_font_vertices.push_back(_sprite_vertex);

		_sprite_vertex.pos = XMFLOAT3(_rect_right, _rect_top, fDepth);
		_sprite_vertex.tex = XMFLOAT2(_tex_right, _tex_top);
		_sprite_vertex.color = pFontColor;
		this->_font_vertices.push_back(_sprite_vertex);

		_sprite_vertex.pos = XMFLOAT3(_rect_left, _rect_bottom, fDepth);
		_sprite_vertex.tex = XMFLOAT2(_tex_left, _tex_bottom);
		_sprite_vertex.color = pFontColor;
		this->_font_vertices.push_back(_sprite_vertex);

		// tri2
		_sprite_vertex.pos = XMFLOAT3(_rect_right, _rect_top, fDepth);
		_sprite_vertex.tex = XMFLOAT2(_tex_right, _tex_top);
		_sprite_vertex.color = pFontColor;
		this->_font_vertices.push_back(_sprite_vertex);

		_sprite_vertex.pos = XMFLOAT3(_rect_right, _rect_bottom, fDepth);
		_sprite_vertex.tex = XMFLOAT2(_tex_right, _tex_bottom);
		_sprite_vertex.color = pFontColor;
		this->_font_vertices.push_back(_sprite_vertex);

		_sprite_vertex.pos = XMFLOAT3(_rect_left, _rect_bottom, fDepth);
		_sprite_vertex.tex = XMFLOAT2(_tex_left, _tex_bottom);
		_sprite_vertex.color = pFontColor;
		this->_font_vertices.push_back(_sprite_vertex);

		_rect_left += _glyph_sizeX;
	}

	// We have to end text after every line so that rendering order between sprites and fonts is preserved
	end_text(pDevice, pContext);
}

_Use_decl_annotations_
void w_font::end_text(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext)
{
	if (this->_font_vertices.empty()) return;

	// ensure our buffer size can hold our sprites
	UINT _font_data_bytes = static_cast<UINT>(this->_font_vertices.size() * sizeof(w_sprite_vertex));
	if (this->_font_buffer_bytes < _font_data_bytes)
	{
		COM_RELEASE(this->_font_buffer);
		this->_font_buffer_bytes = _font_data_bytes;

		D3D11_BUFFER_DESC _buffer_desc;
		_buffer_desc.ByteWidth = this->_font_buffer_bytes;
		_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		_buffer_desc.MiscFlags = 0;

		if (FAILED(pDevice->CreateBuffer(&_buffer_desc, nullptr, &this->_font_buffer)))
		{
			this->_font_buffer = nullptr;
			this->_font_buffer_bytes = 0;
			return;
		}
	}

	// Copy the sprites over
	D3D11_BOX destRegion;
	destRegion.left = 0;
	destRegion.right = _font_data_bytes;
	destRegion.top = 0;
	destRegion.bottom = 1;
	destRegion.front = 0;
	destRegion.back = 1;

	D3D11_MAPPED_SUBRESOURCE _mapped_resource;
	if (pContext->Map(this->_font_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &_mapped_resource) == S_OK)
	{
		memcpy(_mapped_resource.pData, (void*)&this->_font_vertices[0], _font_data_bytes);
		pContext->Unmap(this->_font_buffer, 0);
	}

	ID3D11ShaderResourceView* _old_texture = nullptr;
	pContext->PSGetShaderResources(0, 1, &_old_texture);
	auto _srv = get_srv();
	pContext->PSSetShaderResources(0, 1, &_srv);

	// Draw
	UINT Stride = sizeof(w_sprite_vertex);
	UINT Offset = 0;
	pContext->IASetVertexBuffers(0, 1, &this->_font_buffer, &Stride, &Offset);
	pContext->IASetInputLayout(this->_input_layout);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pContext->Draw(static_cast<UINT>(this->_font_vertices.size()), 0);

	pContext->PSSetShaderResources(0, 1, &_old_texture);
	COM_RELEASE(_old_texture);

	this->_font_vertices.clear();
}

ULONG w_font::release()
{
	if (_super::is_released()) return 0;

	this->_font_buffer_bytes = 0;
	COM_RELEASE(this->_font_buffer);
	UNIQUE_RELEASE(this->_font_texture);

	return _super::release();
}
