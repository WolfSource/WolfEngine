#include "w_directX_pch.h"
#include "w_mesh.h"
#include "w_graphics\w_textures\w_render_target_2D.h"

using namespace std;
using namespace D2D1;
using namespace DirectX;
using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::content_pipeline;
using namespace wolf::content_pipeline::vertex_declaration_structs;

w_mesh::w_mesh() : 
	_stride(sizeof(vertex_position_normal_uv)), 
	_offset(0), 
	_use_identity_matrix(false), 
	_is_sprite(false), 
	_texture_2D_width(512), 
	_texture_2D_height(512)
{
	_super::set_class_name(typeid(this).name());

	this->_texture_2D = make_unique<w_texture_2D>();
	this->_shader = make_unique<w_shader>();
	this->_position = XMFLOAT3(0, 0, 0);
	this->_rotation = XMFLOAT3(0, 0, 0);
	this->_scale = XMFLOAT3(1, 1, 1);
}

w_mesh::~w_mesh()
{
	release();
}

HRESULT w_mesh::load(_In_ ID3D11Device1* pDevice, std::wstring pVertexShaderPath, std::wstring pPixelShaderPath, std::wstring pTexture2DPath, bool pLoadDeafultTexture2D,
	const vertex_position_normal_uv* const pVertices, const UINT pVerticesSize, const unsigned short* const pIndices, const UINT pIndicesSize)
{
	this->_indices_size = pIndicesSize;

#pragma region Create Vertex Buffer

	D3D11_BUFFER_DESC _vBufferDesc;
	ZeroMemory(&_vBufferDesc, sizeof(D3D11_BUFFER_DESC));
	_vBufferDesc.ByteWidth = sizeof(vertex_position_normal_uv) * pVerticesSize;
	_vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	_vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	_vBufferDesc.CPUAccessFlags = 0;
	_vBufferDesc.MiscFlags = 0;
	_vBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA _vBufferData;
	ZeroMemory(&_vBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	_vBufferData.pSysMem = pVertices;

	auto _hr = pDevice->CreateBuffer(&_vBufferDesc, &_vBufferData, &this->_vertex_buffer);
	V(_hr, L"creating vertex buffer", this->name, 3, true, true);

#pragma endregion

#pragma region Create Index Buffer

	D3D11_BUFFER_DESC _indexBufferDesc;
	ZeroMemory(&_indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	_indexBufferDesc.ByteWidth = sizeof(unsigned short) * this->_indices_size;
	_indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	_indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	_indexBufferDesc.CPUAccessFlags = 0;
	_indexBufferDesc.MiscFlags = 0;
	_indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA _indexBufferData;
	ZeroMemory(&_indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	_indexBufferData.pSysMem = pIndices;

	_hr = pDevice->CreateBuffer(&_indexBufferDesc, &_indexBufferData, &this->_index_buffer);
	V(_hr, L"creating index buffer", this->name, 3, true, true);

#pragma endregion

	//Load constant buffer for each section
	this->_wvp_cBuffer.load(pDevice);
	if (this->_is_sprite)
	{
		this->_sprite_obj_cBuffer.load(pDevice);
		//Set the initial constants
		this->_sprite_obj_cBuffer.data.CurrentFrame = 0;
		this->_sprite_obj_cBuffer.data.SpritesPerRow = 1;
		this->_sprite_obj_cBuffer.data.SpritesPerCol = 1;
	}
	else
	{
		this->_quad_cBuffer.load(pDevice);
	}

	//If we need to load default texture load it, on the other hand load it from storage
	if (pTexture2DPath == L"")
	{
		if (pLoadDeafultTexture2D)
		{
			create_texture_2D(pDevice, this->_texture_2D_width, this->_texture_2D_height, D2D1::ColorF::CornflowerBlue);
		}
	}
	else
	{
		load_texture_2D(pDevice, pTexture2DPath);
	}

	//Load shaders
	_hr = w_shader::load_shader(pDevice, pVertexShaderPath, w_shader_type::VERTEX_SHADER, w_vertex_declaration::POSITION_NORMAL_UV, this->_shader.get());
	V(_hr, L"loading vertex shader", this->name, 3, true, true);

	_hr = w_shader::load_shader(pDevice, pPixelShaderPath, w_shader_type::PIXEL_SHADER, w_vertex_declaration::DONTCARE, this->_shader.get());
	V(_hr, L"loading pixel shader", this->name, 3, true, true);

	//Create world matrix
	auto posVect = XMLoadFloat3(&this->_position);
	auto rotVect = XMLoadFloat3(&this->_rotation);

	this->_world = XMMatrixScaling(_scale.x, _scale.y, _scale.z) *
		XMMatrixRotationRollPitchYawFromVector(rotVect) *
		XMMatrixTranslationFromVector(posVect);

	return _hr;
}

HRESULT w_mesh::create_texture_2D(_In_ ID3D11Device1* pDevice,
		_In_ UINT pWidth,
		_In_ UINT pHeight,
		_In_ D2D1::ColorF pColor,
		_In_ D3D11_USAGE pUsage,
		_In_ UINT pCpuAccessFlags,
		_In_ DXGI_FORMAT pFormat ,
		_In_ UINT pBindFlag)
{
	auto hr = this->_texture_2D->create(
		pDevice, 
		pWidth, 
		pHeight,
		pColor,
		pUsage, 
		pCpuAccessFlags,
		pFormat,
		pBindFlag);

	V(hr, L"Creating default texture", this->name, 3, true, true);
	return hr;
}

HRESULT w_mesh::create_texture_2D(_In_ ID3D11Device1* pDevice,
	_In_ UINT pWidth,
	_In_ UINT pHeight,
	_In_ D3D11_USAGE pUsage,
	_In_ UINT pCpuAccessFlags,
	_In_ DXGI_FORMAT pFormat,
	_In_ UINT pBindFlag,
	_In_ D3D11_SUBRESOURCE_DATA* pInitialData)
{
	auto hr = this->_texture_2D->create(
		pDevice,
		pWidth,
		pHeight,
		pUsage,
		pCpuAccessFlags,
		pFormat,
		pBindFlag,
		pInitialData);

	V(hr, L"Creating default texture", this->name, 3, true, true);
	return hr;
}

HRESULT w_mesh::load_texture_2D(_In_ ID3D11Device1* pDevice, wstring pPath)
{
	auto _hr = w_texture_2D::load_texture_2D(pDevice, this->_texture_2D.get(), pPath);
	V(_hr, L"loading texture", this->name, 3, false, true);
	return _hr;
}

void w_mesh::render(_In_ ID3D11DeviceContext1* pContext)
{
	auto _posVect = XMLoadFloat3(&this->_position);
	auto _rotVect = XMLoadFloat3(&this->_rotation);
	this->_world = XMMatrixScaling(_scale.x, _scale.y, _scale.z) *
		XMMatrixRotationRollPitchYawFromVector(_rotVect) *
		XMMatrixTranslationFromVector(_posVect);

	auto WVP = XMMatrixTranspose(this->_world * this->_view_projection);
	if (this->_is_sprite)
	{
		this->_sprite_obj_cBuffer.data.WorldViewProjection = this->_use_identity_matrix ? XMMatrixIdentity() : WVP;
		this->_sprite_obj_cBuffer.update(pContext);
		this->_shader->set_constant_buffer(pContext, 0, 1, this->_sprite_obj_cBuffer.get_buffer());
	}
	else
	{
		this->_wvp_cBuffer.data.world_view_projection = this->_use_identity_matrix ? XMMatrixIdentity() : WVP;
		this->_wvp_cBuffer.update(pContext);
		this->_shader->set_constant_buffer(pContext, 0, 1, this->_wvp_cBuffer.get_buffer());

		this->_quad_cBuffer.update(pContext);
		this->_shader->set_constant_buffer(pContext, 1, 1, this->_quad_cBuffer.get_buffer());
	}

	this->_shader->set_srv(pContext, 0, 1, this->_texture_2D->get_srv());
	this->_shader->set_srv(pContext, 1, 1, nullptr);

	this->_shader->apply(pContext);
	{
		pContext->IASetVertexBuffers(0, 1, this->_vertex_buffer.GetAddressOf(), &this->_stride, &this->_offset);
		pContext->IASetIndexBuffer(this->_index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pContext->DrawIndexed(this->_indices_size, 0, 0);
	}
}

void w_mesh::render(_In_ ID3D11DeviceContext1* pContext, std::vector<ID3D11ShaderResourceView*> pSRVs)
{
	auto posVect = XMLoadFloat3(&this->_position);
	auto rotVect = XMLoadFloat3(&this->_rotation);
	this->_world = XMMatrixScaling(this->_scale.x, this->_scale.y, this->_scale.z) *
		XMMatrixRotationRollPitchYawFromVector(rotVect) *
		XMMatrixTranslationFromVector(posVect);

	auto WVP = XMMatrixTranspose(this->_world * this->_view_projection);
	if (this->_is_sprite)
	{
		this->_sprite_obj_cBuffer.data.WorldViewProjection = _use_identity_matrix ? XMMatrixIdentity() : WVP;
		this->_sprite_obj_cBuffer.update(pContext);
		this->_shader->set_constant_buffer(pContext, 0, 1, this->_sprite_obj_cBuffer.get_buffer());
	}
	else
	{
		this->_wvp_cBuffer.data.world_view_projection = this->_use_identity_matrix ? XMMatrixIdentity() : WVP;
		this->_wvp_cBuffer.update(pContext);
		this->_shader->set_constant_buffer(pContext, 0, 1, this->_wvp_cBuffer.get_buffer());

		this->_quad_cBuffer.update(pContext);
		this->_shader->set_constant_buffer(pContext, 1, 1, this->_quad_cBuffer.get_buffer());
	}

	//8 srvs we can use for this shader
	if (this->_shader)
	{
		auto size = pSRVs.size();
		if (size > 0)
		{
			for (int i = 0; i < size; ++i)
			{
				//Set shader resource view
				auto srv = pSRVs.at(i);
				this->_shader->set_srv(pContext, i, 1, srv != nullptr ? srv : this->_texture_2D->get_srv());
			}
		}
		else
		{
			this->_shader->set_srv(pContext, 0, 1, this->_texture_2D->get_srv());
		}
	}


	this->_shader->apply(pContext);
	{
		pContext->IASetVertexBuffers(0, 1, this->_vertex_buffer.GetAddressOf(), &this->_stride, &this->_offset);
		pContext->IASetIndexBuffer(this->_index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pContext->DrawIndexed(this->_indices_size, 0, 0);
	}
}

void w_mesh::render_srv_over_default_texture(_In_ ID3D11DeviceContext1* pContext, _In_ ID3D11ShaderResourceView* pSRV, _In_ UINT pSRVSlotIndex)
{
	auto posVect = XMLoadFloat3(&this->_position);
	auto rotVect = XMLoadFloat3(&this->_rotation);
	this->_world = XMMatrixScaling(this->_scale.x, this->_scale.y, this->_scale.z) *
		XMMatrixRotationRollPitchYawFromVector(rotVect) *
		XMMatrixTranslationFromVector(posVect);

	auto WVP = XMMatrixTranspose(this->_world * this->_view_projection);
	if (this->_is_sprite)
	{
		this->_sprite_obj_cBuffer.data.WorldViewProjection = this->_use_identity_matrix ? XMMatrixIdentity() : WVP;
		this->_sprite_obj_cBuffer.update(pContext);
		this->_shader->set_constant_buffer(pContext, 0, 1, this->_sprite_obj_cBuffer.get_buffer());
	}
	else
	{
		this->_wvp_cBuffer.data.world_view_projection = this->_use_identity_matrix ? XMMatrixIdentity() : WVP;
		this->_wvp_cBuffer.update(pContext);
		this->_shader->set_constant_buffer(pContext, 0, 1, this->_wvp_cBuffer.get_buffer());

		this->_quad_cBuffer.update(pContext);
		this->_shader->set_constant_buffer(pContext, 1, 1, this->_quad_cBuffer.get_buffer());
	}

	if (pSRVSlotIndex == 1)
	{
		this->_shader->set_srv(pContext, 0, 1, this->_texture_2D->get_srv());
		this->_shader->set_srv(pContext, 1, 1, pSRV);
	}
	else
	{
		this->_shader->set_srv(pContext, 1, 1, this->_texture_2D->get_srv());
		this->_shader->set_srv(pContext, 0, 1, pSRV);
	}
	
	this->_shader->apply(pContext);
	{
		pContext->IASetVertexBuffers(0, 1, this->_vertex_buffer.GetAddressOf(), &this->_stride, &this->_offset);
		pContext->IASetIndexBuffer(this->_index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pContext->DrawIndexed(this->_indices_size, 0, 0);
	}
}

ULONG w_mesh::release()
{
	if (_super::is_released()) return 0;
	
	COMPTR_RELEASE(this->_vertex_buffer);
	COMPTR_RELEASE(this->_index_buffer);
	UNIQUE_RELEASE(this->_texture_2D);
	UNIQUE_RELEASE(this->_shader);

	//Release shader's constant buffers
	this->_wvp_cBuffer.release();
	this->_quad_cBuffer.release();
	this->_sprite_obj_cBuffer.release();
	
	return _super::release();
}