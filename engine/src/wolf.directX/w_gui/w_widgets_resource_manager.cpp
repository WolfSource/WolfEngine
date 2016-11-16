#include "w_directX_pch.h"
#include "w_widgets_resource_manager.h"
#include "w_graphics/w_textures/DDSTextureLoader.h"
#include <d3dcompiler.h>
#include <w_graphics/w_textures/w_texture_2D.h>
#include <w_graphics/w_textures/ScreenGrab.h>

using namespace wolf::gui;

w_widgets_resource_manager::w_widgets_resource_manager() :
	m_pDepthStencilStateUI11(nullptr),
	m_pRasterizerStateUI11(nullptr),
	m_pBlendStateUI11(nullptr),
	m_pSamplerStateUI11(nullptr),
	m_pDepthStencilStateStored11(nullptr),
	m_pRasterizerStateStored11(nullptr),
	m_pBlendStateStored11(nullptr),
	m_pSamplerStateStored11(nullptr),
	m_pSpriteBuffer11(nullptr),
	m_SpriteBufferBytes11(0)
{
	_super::set_class_name(typeid(this).name());
}

w_widgets_resource_manager::~w_widgets_resource_manager()
{
	for (auto it = this->font_cache.begin(); it != this->font_cache.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	this->font_cache.clear();

	for (auto it = this->texture_cache.begin(); it != this->texture_cache.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	this->texture_cache.clear();
}

_Use_decl_annotations_
HRESULT w_widgets_resource_manager::on_device_created(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext)
{
	// Create Shaders
	this->_shader = std::make_unique<wolf::graphics::w_shader>();
	auto _hr = _shader->load_shader(pDevice, L"Shaders/gui/gui_vs.cso", w_shader_type::VERTEX_SHADER, wolf::content_pipeline::w_vertex_declaration::POSITION_COLOR_UV, _shader.get());
	V(_hr, L"creating vertex shader", _super::name, 3, true, true);
	
	size_t _pixel_shader_index = 0;
	_hr = _shader->load_shader(pDevice, L"Shaders/gui/gui_ps.cso", w_shader_type::PIXEL_SHADER, wolf::content_pipeline::w_vertex_declaration::POSITION_COLOR_UV, _shader.get(), &_pixel_shader_index);
	V(_hr, L"creating pixel shader", _super::name, 3, true, true);

	_hr = _shader->load_shader(pDevice, L"Shaders/gui/gui_ps_no_texture.cso", w_shader_type::PIXEL_SHADER, wolf::content_pipeline::w_vertex_declaration::POSITION_COLOR_UV, _shader.get(), &_pixel_shader_index);
	V(_hr, L"creating pixel shader with no texture", _super::name, 3, true, true);

	// States
	D3D11_DEPTH_STENCIL_DESC _depth_stencil_desc;
	std::memset(&_depth_stencil_desc, 0, sizeof(_depth_stencil_desc));

	_depth_stencil_desc.DepthEnable = FALSE;
	_depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	_depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	_depth_stencil_desc.StencilEnable = FALSE;
	_hr = pDevice->CreateDepthStencilState(&_depth_stencil_desc, &m_pDepthStencilStateUI11);
	V(_hr, L"creating depth stencil description", _super::name, 3, true, true);

	D3D11_RASTERIZER_DESC _rasterizer_desc;
	_rasterizer_desc.AntialiasedLineEnable = FALSE;
	_rasterizer_desc.CullMode = D3D11_CULL_BACK;
	_rasterizer_desc.DepthBias = 0;
	_rasterizer_desc.DepthBiasClamp = 0.0f;
	_rasterizer_desc.DepthClipEnable = TRUE;
	_rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	_rasterizer_desc.FrontCounterClockwise = FALSE;
	_rasterizer_desc.MultisampleEnable = TRUE;
	_rasterizer_desc.ScissorEnable = FALSE;
	_rasterizer_desc.SlopeScaledDepthBias = 0.0f;
	_hr = pDevice->CreateRasterizerState(&_rasterizer_desc, &m_pRasterizerStateUI11);
	V(_hr, L"creating rasterizer description", _super::name, 3, true, true);

	D3D11_BLEND_DESC _blend_state_desc;
	std::memset(&_blend_state_desc, 0, sizeof(_blend_state_desc));

	_blend_state_desc.RenderTarget[0].BlendEnable = TRUE;
	_blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	_blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	_blend_state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	_blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	_blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	_blend_state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	_blend_state_desc.RenderTarget[0].RenderTargetWriteMask = 0x0F;

	_hr = pDevice->CreateBlendState(&_blend_state_desc, &this->m_pBlendStateUI11);
	V(_hr, L"creating blend state description", _super::name, 3, true, true);

	D3D11_SAMPLER_DESC _sampler_state_desc;
	std::memset(&_sampler_state_desc, 0, sizeof(_sampler_state_desc));

	_sampler_state_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	_sampler_state_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	_sampler_state_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	_sampler_state_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	_sampler_state_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	_sampler_state_desc.MaxAnisotropy = 16;
	_sampler_state_desc.MinLOD = 0;
	_sampler_state_desc.MaxLOD = D3D11_FLOAT32_MAX;
	if (pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_9_3)
	{
		_sampler_state_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		_sampler_state_desc.MaxAnisotropy = 0;
	}
	_hr = pDevice->CreateSamplerState(&_sampler_state_desc, &m_pSamplerStateUI11);
	V(_hr, L"creating sample state description", _super::name, 3, true, true);

//	this->_font = std::make_unique<w_font>(pDevice, _shader->get_input_layout());

	return S_OK;
}

_Use_decl_annotations_
HRESULT w_widgets_resource_manager::on_swapChain_resized(const UINT pWidth, const UINT pHeight)
{
	this->back_buffer_width = pWidth;
	this->back_buffer_height = pHeight;

	return S_OK;
}

void w_widgets_resource_manager::on_releasing_swapChain()
{
}

void w_widgets_resource_manager::on_destroy_device()
{
	// Release the resources but don't clear the cache, as these will need to be recreated if the device is recreated
	for (auto it = this->texture_cache.begin(); it != this->texture_cache.end(); ++it)
	{
		COM_RELEASE((*it)->shader_resource_view);
		COM_RELEASE((*it)->texture_2d);
	}

	COM_RELEASE(m_pSpriteBuffer11);
	m_SpriteBufferBytes11 = 0;

	//release states
	COM_RELEASE(m_pDepthStencilStateUI11);
	COM_RELEASE(m_pRasterizerStateUI11);
	COM_RELEASE(m_pBlendStateUI11);
	COM_RELEASE(m_pSamplerStateUI11);

	COM_RELEASE(m_pDepthStencilStateStored11);
	COM_RELEASE(m_pRasterizerStateStored11);
	COM_RELEASE(m_pBlendStateStored11);
	COM_RELEASE(m_pSamplerStateStored11);

	//release font texture
	UNIQUE_RELEASE(this->_shader);
}

void w_widgets_resource_manager::store_D3D_state(_In_ ID3D11DeviceContext1* pContext)
{
	pContext->OMGetDepthStencilState(&m_pDepthStencilStateStored11, &m_StencilRefStored11);
	pContext->RSGetState(&m_pRasterizerStateStored11);
	pContext->OMGetBlendState(&m_pBlendStateStored11, m_BlendFactorStored11, &m_SampleMaskStored11);
	pContext->PSGetSamplers(0, 1, &m_pSamplerStateStored11);
}

void w_widgets_resource_manager::restore_D3D_state(_In_ ID3D11DeviceContext1* pContext)
{
	pContext->OMSetDepthStencilState(m_pDepthStencilStateStored11, m_StencilRefStored11);
	pContext->RSSetState(m_pRasterizerStateStored11);
	pContext->OMSetBlendState(m_pBlendStateStored11, m_BlendFactorStored11, m_SampleMaskStored11);
	pContext->PSSetSamplers(0, 1, &m_pSamplerStateStored11);

	COM_RELEASE(m_pDepthStencilStateStored11);
	COM_RELEASE(m_pRasterizerStateStored11);
	COM_RELEASE(m_pBlendStateStored11);
	COM_RELEASE(m_pSamplerStateStored11);
}

void w_widgets_resource_manager::apply_render_UI(_In_ ID3D11DeviceContext1* pContext)
{
	// Shaders
	pContext->VSSetShader(this->_shader->get_vertex_shader(), nullptr, 0);
	pContext->HSSetShader(nullptr, nullptr, 0);
	pContext->DSSetShader(nullptr, nullptr, 0);
	pContext->GSSetShader(nullptr, nullptr, 0); 
	//Render gui with pixel shader
	pContext->PSSetShader(this->_shader->get_pixel_shader(0), nullptr, 0);

	// States
	pContext->OMSetDepthStencilState(m_pDepthStencilStateUI11, 0);
	pContext->RSSetState(m_pRasterizerStateUI11);
	float BlendFactor[4] = { 0, 0, 0, 0 };
	pContext->OMSetBlendState(m_pBlendStateUI11, BlendFactor, 0xFFFFFFFF);
	pContext->PSSetSamplers(0, 1, &m_pSamplerStateUI11);
}

void w_widgets_resource_manager::apply_render_UI_Untex(_In_ ID3D11DeviceContext1* pContext)
{
	// Shaders
	pContext->VSSetShader(this->_shader->get_vertex_shader(), nullptr, 0);
	pContext->HSSetShader(nullptr, nullptr, 0);
	pContext->DSSetShader(nullptr, nullptr, 0);
	pContext->GSSetShader(nullptr, nullptr, 0);
	//Pixel shader untexture
	pContext->PSSetShader(this->_shader->get_pixel_shader(1), nullptr, 0);

	// States
	pContext->OMSetDepthStencilState(m_pDepthStencilStateUI11, 0);
	pContext->RSSetState(m_pRasterizerStateUI11);
	float BlendFactor[4] = { 0, 0, 0, 0 };
	pContext->OMSetBlendState(m_pBlendStateUI11, BlendFactor, 0xFFFFFFFF);
	pContext->PSSetSamplers(0, 1, &m_pSamplerStateUI11);
}

void w_widgets_resource_manager::begin_sprites()
{
	m_SpriteVertices.clear();
}

_Use_decl_annotations_
void w_widgets_resource_manager::end_sprites(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext)
{
	// ensure our buffer size can hold our sprites
	UINT SpriteDataBytes = static_cast<UINT>(m_SpriteVertices.size() * sizeof(wolf::content_pipeline::vertex_declaration_structs::vertex_color_uv));
	if (m_SpriteBufferBytes11 < SpriteDataBytes)
	{
		COM_RELEASE(m_pSpriteBuffer11);
		m_SpriteBufferBytes11 = SpriteDataBytes;

		D3D11_BUFFER_DESC _buffer_desc;
		_buffer_desc.ByteWidth = m_SpriteBufferBytes11;
		_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		_buffer_desc.MiscFlags = 0;

		if (FAILED(pDevice->CreateBuffer(&_buffer_desc, nullptr, &m_pSpriteBuffer11)))
		{
			m_pSpriteBuffer11 = nullptr;
			m_SpriteBufferBytes11 = 0;
			V(S_FALSE, L"create buffer", _super::name, 3, true, true);
			return;
		}
	}

	// Copy the sprites over
	D3D11_BOX _dest_region;
	_dest_region.left = 0;
	_dest_region.right = SpriteDataBytes;
	_dest_region.top = 0;
	_dest_region.bottom = 1;
	_dest_region.front = 0;
	_dest_region.back = 1;

	D3D11_MAPPED_SUBRESOURCE _mapped_resource;
	if (S_OK == pContext->Map(m_pSpriteBuffer11, 0, D3D11_MAP_WRITE_DISCARD, 0, &_mapped_resource))
	{
		memcpy(_mapped_resource.pData, (const void*)&m_SpriteVertices[0], SpriteDataBytes);
		pContext->Unmap(m_pSpriteBuffer11, 0);
	}

	// Draw
	UINT Stride = sizeof(wolf::content_pipeline::vertex_declaration_structs::vertex_color_uv);
	UINT Offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pSpriteBuffer11, &Stride, &Offset);
	pContext->IASetInputLayout(this->_shader->get_input_layout());
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pContext->Draw(static_cast<UINT>(m_SpriteVertices.size()), 0);

	m_SpriteVertices.clear();
}

bool w_widgets_resource_manager::register_widget(_In_ w_widget* pWidget)
{
	// Check that the dialog isn't already registered.
	for (auto it = this->widgets.cbegin(); it != this->widgets.cend(); ++it)
	{
		if (*it == pWidget) return true;
	}

	// Add to the list.
	this->widgets.push_back(pWidget);

	// Set up next and prev pointers.
	if (this->widgets.size() > 1)
	{
		this->widgets[this->widgets.size() - 2]->set_next_widget(pWidget);
	}
	this->widgets[this->widgets.size() - 1]->set_next_widget(this->widgets[0]);

	return true;
}

void w_widgets_resource_manager::unregister_widget(_In_ w_widget* pWidget)
{
	// Search for the dialog in the list.
	for (size_t i = 0; i < this->widgets.size(); ++i)
	{
		if (this->widgets[i] == pWidget)
		{
			this->widgets.erase(this->widgets.begin() + i);
			if (!this->widgets.empty())
			{
				int l, r;

				if (0 == i)
				{
					l = int(this->widgets.size() - 1);
				}
				else
				{
					l = int(i) - 1;
				}
				if (this->widgets.size() == i)
				{
					r = 0;
				}
				else
				{
					r = int(i);
				}
				this->widgets[l]->set_next_widget(this->widgets[r]);
			}
			return;
		}
	}
}

void w_widgets_resource_manager::enable_keyboard_input_for_all_widgets()
{
	// Enable keyboard input for all registered dialogs
	for (auto it = this->widgets.begin(); it != this->widgets.end(); ++it)
	{
		(*it)->enable_keyboard_input(true);
	}
}

_Use_decl_annotations_
int w_widgets_resource_manager::add_font(LPCWSTR strFaceName, LONG height, LONG weight)
{
	// See if this font already exists
	for (size_t i = 0; i < this->font_cache.size(); ++i)
	{
		auto pFontNode = this->font_cache[i];
		size_t nLen = 0;
		nLen = wcsnlen(strFaceName, MAX_PATH);
		if (0 == _wcsnicmp(pFontNode->name, strFaceName, nLen) &&
			pFontNode->height == height &&
			pFontNode->weight == weight)
		{
			return static_cast<int>(i);
		}
	}

	// Add a new font and try to create it
	auto pNewFontNode = new (std::nothrow) w_font_node;
	if (!pNewFontNode) return -1;

	ZeroMemory(pNewFontNode, sizeof(pNewFontNode));

	wcscpy_s(pNewFontNode->name, MAX_PATH, strFaceName);
	pNewFontNode->height = height;
	pNewFontNode->weight = weight;
	this->font_cache.push_back(pNewFontNode);

	int iFont = (int)this->font_cache.size() - 1;

	// If a device is available, try to create immediately
	return iFont;
}

int w_widgets_resource_manager::add_texture(_In_z_ LPCWSTR pFilename)
{
	// See if this texture already exists
	for (size_t i = 0; i < this->texture_cache.size(); ++i)
	{
		auto pTextureNode = this->texture_cache[i];
		size_t nLen = 0;
		nLen = wcsnlen(pFilename, MAX_PATH);
		// Sources must match
		if (pTextureNode->is_loaded_from_source &&  _wcsnicmp(pTextureNode->file_name, pFilename, nLen) == 0)
		{
			return static_cast<int>(i);
		}
	}

	// Add a new texture and try to create it
	auto pNewTextureNode = new (std::nothrow) w_texture_node;
	if (!pNewTextureNode) return -1;

	ZeroMemory(pNewTextureNode, sizeof(pNewTextureNode));
	pNewTextureNode->is_loaded_from_source = true;
	wcscpy_s(pNewTextureNode->file_name, MAX_PATH, pFilename);

	this->texture_cache.push_back(pNewTextureNode);

	int iTexture = int(this->texture_cache.size()) - 1;

	// If a device is available, try to create immediately

	return iTexture;
}

HRESULT w_widgets_resource_manager::update_texture_cache(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext, UINT pIndex)
{
	//we alredy have a texture in this slot
	if (pIndex < this->texture_cache.size())
	{
		auto _texture_node = this->texture_cache.at(pIndex);
		if (_texture_node != nullptr && _texture_node->texture_2d != nullptr && _texture_node->shader_resource_view != nullptr)
		{
			return S_OK;
		}
	}
	return create_texture(pDevice, pContext, pIndex);
}

_Use_decl_annotations_
int w_widgets_resource_manager::add_texture(LPCWSTR pResourceName, HMODULE pHResourceModule)
{
	// See if this texture already exists
	for (size_t i = 0; i < this->texture_cache.size(); i++)
	{
		auto pTextureNode = this->texture_cache[i];
		// Sources must match
		if (!pTextureNode->is_loaded_from_source && pTextureNode->resource_module == pHResourceModule) // Module handles must match
		{
			if (IS_INTRESOURCE(pResourceName))
			{
				// Integer-based ID
				if ((INT_PTR)pResourceName == pTextureNode->resource_id) return static_cast<int>(i);
			}
			else
			{
				// String-based ID
				size_t nLen = 0;
				nLen = wcsnlen(pResourceName, MAX_PATH);
				if (0 == _wcsnicmp(pTextureNode->file_name, pResourceName, nLen)) return static_cast<int>(i);
			}
		}
	}

	// Add a new texture and try to create it
	auto _texture_node = (w_texture_node*)malloc(sizeof(w_texture_node));
	if (!_texture_node) return -1;

	_texture_node->texture_2d = nullptr;
	_texture_node->shader_resource_view = nullptr;
	_texture_node->resource_module = pHResourceModule;

	if (IS_INTRESOURCE(pResourceName))
	{
		_texture_node->resource_id = (int)(size_t)pResourceName;
	}
	else
	{
		_texture_node->resource_id = 0;
		wcscpy_s(_texture_node->file_name, MAX_PATH, pResourceName);
	}

	this->texture_cache.push_back(_texture_node);

	int iTexture = int(this->texture_cache.size()) - 1;

	// If a device is available, try to create immediately

	return iTexture;
}

HRESULT w_widgets_resource_manager::create_texture(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext, _In_ UINT iTexture)
{
	HRESULT _hr = S_OK;

	auto _texture_node = this->texture_cache[iTexture];

	std::unique_ptr<wolf::graphics::w_texture_2D> _w_texture;
	_hr = wolf::graphics::w_texture_2D::load_texture_2D_from_file(pDevice, _w_texture, L"Textures\\GUI\\Controls.dds");
	V(_hr, L"locading GUI\Controls.dds asset", _super::name, 3, true, false);

	_texture_node->texture_2d = _w_texture->get_texture_2D();
	
	// Store dimensions
	D3D11_TEXTURE2D_DESC desc;
	_texture_node->texture_2d->GetDesc(&desc);
	_texture_node->width = 256; //always use 256 for base controls
	_texture_node->height = 256; //always use 256 for base controls
	
	// Create resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC _SRV_Desc;
	_SRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	_SRV_Desc.Format = desc.Format;
	_SRV_Desc.Texture2D.MipLevels = 1;
	_SRV_Desc.Texture2D.MostDetailedMip = 0;
	_hr = pDevice->CreateShaderResourceView(_texture_node->texture_2d, &_SRV_Desc, &_texture_node->shader_resource_view);
	if (FAILED(_hr)) return _hr;

	return _hr;
}



