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
	_stride(0), 
	_offset(0), 
	_active_pixel_shader(0),
	_use_identity_matrix(false), 
	_vertex_buffer(nullptr),
	_index_buffer(nullptr),
	_index_buffer_format(DXGI_FORMAT_UNKNOWN)
{
	_super::set_class_name(typeid(this).name());

	this->_texture = make_unique<w_texture_2D>();
	this->_shader = make_unique<w_shader>();
	this->_position = XMFLOAT3(0, 0, 0);
	this->_rotation = XMFLOAT3(0, 0, 0);
	this->_scale = XMFLOAT3(1, 1, 1);
}

w_mesh::~w_mesh()
{
	release();
}

void w_mesh::initialize(_In_ ID3D11Device1* pDevice)
{
	//Load constant buffer
	this->_wvp_const_buffer.load(pDevice);
	
	//make world matrix
	auto _pos_vect = XMLoadFloat3(&this->_position);
	auto _rot_vect = XMLoadFloat3(&this->_rotation);

	this->_world = XMMatrixScaling(_scale.x, _scale.y, _scale.z) *
		XMMatrixRotationRollPitchYawFromVector(_rot_vect) *
		XMMatrixTranslationFromVector(_pos_vect);
}

HRESULT w_mesh::create_vertex_shader(_In_ ID3D11Device1* pDevice, _In_z_ const std::wstring pPath)
{
	return w_shader::load_shader(pDevice, pPath, w_shader_type::VERTEX_SHADER, w_vertex_declaration::POSITION_NORMAL_UV, this->_shader.get());
}

HRESULT w_mesh::create_pixel_shader(_In_ ID3D11Device1* pDevice, _In_z_ const std::wstring pPath)
{
	return w_shader::load_shader(pDevice, pPath, w_shader_type::PIXEL_SHADER, w_vertex_declaration::DONTCARE, this->_shader.get());
}

HRESULT w_mesh::create_vertex_buffer(_In_ ID3D11Device1* pDevice,
	_In_ const void* const pVertices,
	_In_ const UINT pVerticesSize,
	_In_ const UINT pStride,
	_In_ D3D11_USAGE pUsageFlag,
	_In_ const UINT pCPUAccessFlags,
	_In_ const UINT pMiscFlags,
	_In_ const UINT pStructureByteStride)
{
	this->_stride = pStride;

	D3D11_BUFFER_DESC _buffer_desc;
	std::memset(&_buffer_desc, 0, sizeof(_buffer_desc));

	_buffer_desc.ByteWidth = this->_stride * pVerticesSize;
	_buffer_desc.Usage = pUsageFlag;
	_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	_buffer_desc.CPUAccessFlags = pCPUAccessFlags;
	_buffer_desc.MiscFlags = pMiscFlags;
	_buffer_desc.StructureByteStride = pStructureByteStride;

	D3D11_SUBRESOURCE_DATA _buffer_data;
	std::memset(&_buffer_data, 0, sizeof(_buffer_data));
	_buffer_data.pSysMem = pVertices;

	return pDevice->CreateBuffer(&_buffer_desc, &_buffer_data, &this->_vertex_buffer);
}

HRESULT w_mesh::create_index_buffer(_In_ ID3D11Device1* pDevice,
	_In_ const unsigned short* const pIndices,
	_In_ const UINT pIndicesSize,
	_In_ D3D11_USAGE pUsageFlag,
	_In_ const UINT pCPUAccessFlags,
	_In_ const UINT pMiscFlags,
	_In_ const UINT pStructureByteStride,
	_In_ const DXGI_FORMAT pFormat)
{
	this->_index_buffer_format = pFormat;
	this->_indices_size = pIndicesSize;

	D3D11_BUFFER_DESC _buffer_desc;
	std::memset(&_buffer_desc, 0, sizeof(_buffer_desc));
	_buffer_desc.ByteWidth = sizeof(unsigned short) * this->_indices_size;
	_buffer_desc.Usage = pUsageFlag;
	_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	_buffer_desc.CPUAccessFlags = pCPUAccessFlags;
	_buffer_desc.MiscFlags = pMiscFlags;
	_buffer_desc.StructureByteStride = pStructureByteStride;

	D3D11_SUBRESOURCE_DATA _buffer_data;
	std::memset(&_buffer_data, 0, sizeof(_buffer_data));
	_buffer_data.pSysMem = pIndices;

	return pDevice->CreateBuffer(&_buffer_desc, &_buffer_data, &this->_index_buffer);
}

HRESULT w_mesh::create_default_texture_2D(_In_ ID3D11Device1* pDevice)
{
	return this->_texture->create_color_bar(pDevice, 256, 256);
}

HRESULT w_mesh::load_texture_2D_from_file(_In_ ID3D11Device1* pDevice, _In_z_ const wstring pPath, _In_ bool pIsAbsolutePath)
{
	HRESULT _hr = S_FALSE;

	if (this->_texture)
	{
		UNIQUE_RELEASE(this->_texture);
		this->_texture.reset(new w_texture_2D());
	}

	//If we need to load default texture load it, on the other hand load it from storage
	if (pPath == L"")
	{
		_hr = create_default_texture_2D(pDevice);
	}
	else
	{
		_hr = w_texture_2D::load_texture_2D_from_file(pDevice, this->_texture, pPath, pIsAbsolutePath);
	}

	return _hr;
}

HRESULT w_mesh::create_texture_2D(_In_ ID3D11Device1* pDevice,
	_In_ UINT pWidth,
	_In_ UINT pHeight,
	_In_ D2D1::ColorF pColor,
	_In_ D3D11_USAGE pUsage,
	_In_ UINT pCpuAccessFlags,
	_In_ DXGI_FORMAT pFormat,
	_In_ UINT pBindFlag)
{
	return this->_texture->create(
		pDevice,
		pWidth,
		pHeight,
		pColor,
		pUsage,
		pCpuAccessFlags,
		pFormat,
		pBindFlag);
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
	return this->_texture->create(
		pDevice,
		pWidth,
		pHeight,
		pUsage,
		pCpuAccessFlags,
		pFormat,
		pBindFlag,
		pInitialData);
}

void w_mesh::_update_world_view_projection(_In_ ID3D11DeviceContext1* pContext)
{
	//make world matrix
	auto _pos_vect = XMLoadFloat3(&this->_position);
	auto _rot_vect = XMLoadFloat3(&this->_rotation);
	this->_world = XMMatrixScaling(_scale.x, _scale.y, _scale.z) *
		XMMatrixRotationRollPitchYawFromVector(_rot_vect) *
		XMMatrixTranslationFromVector(_pos_vect);

	//make world * view * projection matrix
	auto _wvp = XMMatrixTranspose(this->_world * this->_view_projection);
	//update constant buffer of wvp 
	this->_wvp_const_buffer.data.world_view_projection = this->_use_identity_matrix ? XMMatrixIdentity() : _wvp;
	this->_wvp_const_buffer.update(pContext);
	
	//update hlsl constant buffer
	update_shader_constant_buffer(pContext, 0, 1, this->_wvp_const_buffer.get_buffer());
}

void w_mesh::update_shader_constant_buffer(_In_ ID3D11DeviceContext1* pContext, UINT pStartSlot, UINT pNumBuffer, _In_ ID3D11Buffer* pCBuffer)
{
	this->_shader->set_constant_buffer(pContext, pStartSlot, pNumBuffer, pCBuffer);
}

void w_mesh::update_shader_resource_view(_In_ ID3D11DeviceContext1* pContext, UINT pStartSLot, UINT pNumViews, _In_ ID3D11ShaderResourceView* pSRV)
{
	this->_shader->set_srv(pContext, pStartSLot, pNumViews, pSRV);
}

void w_mesh::render(_In_ ID3D11DeviceContext1* pContext, _In_ D3D_PRIMITIVE_TOPOLOGY pPrimitiveTopology)
{
	_update_world_view_projection(pContext);

	//set the first texture
	update_shader_resource_view(pContext, 0, 1, this->_texture->get_srv());

	this->_shader->apply(pContext, this->_active_pixel_shader);
	{
		pContext->IASetVertexBuffers(0, 1, &this->_vertex_buffer, &this->_stride, &this->_offset);
		pContext->IASetIndexBuffer(this->_index_buffer, this->_index_buffer_format, 0);
		pContext->IASetPrimitiveTopology(pPrimitiveTopology);
		pContext->DrawIndexed(this->_indices_size, 0, 0);
	}
}

void w_mesh::render(_In_ ID3D11DeviceContext1* pContext, _In_ std::vector<ID3D11ShaderResourceView*> pShaderResourceViews, _In_ D3D_PRIMITIVE_TOPOLOGY pPrimitiveTopology)
{
	_update_world_view_projection(pContext);

	//set the first texture
	auto _size = pShaderResourceViews.size();
	for (int i = 0; i < pShaderResourceViews.size(); ++i)
	{
		update_shader_resource_view(pContext, i, 1, pShaderResourceViews[i]);
	}

	this->_shader->apply(pContext, this->_active_pixel_shader);
	{
		pContext->IASetVertexBuffers(0, 1, &this->_vertex_buffer, &this->_stride, &this->_offset);
		pContext->IASetIndexBuffer(this->_index_buffer, this->_index_buffer_format, 0);
		pContext->IASetPrimitiveTopology(pPrimitiveTopology);
		pContext->DrawIndexed(this->_indices_size, 0, 0);
	}
}

ULONG w_mesh::release()
{
	if (_super::get_is_released()) return 0;
	
	COM_RELEASE(this->_vertex_buffer);
	COM_RELEASE(this->_index_buffer);
	UNIQUE_RELEASE(this->_texture);
	UNIQUE_RELEASE(this->_shader);

	//Release shader's constant buffers
	this->_wvp_const_buffer.release();
	
	return _super::release();
}

HRESULT w_mesh::update_vertex_buffer_data(_In_ ID3D11DeviceContext1* pContext, _In_ void* pData, _In_ size_t pSizeOfDataInBytes)
{
	D3D11_MAPPED_SUBRESOURCE _mapped_resource;
	auto _hr = pContext->Map(this->_vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &_mapped_resource);
	if (FAILED(_hr)) return _hr;

	memcpy(_mapped_resource.pData, pData, pSizeOfDataInBytes);
	pContext->Unmap(this->_vertex_buffer, 0);

	return S_OK;
}

#pragma region Getters

glm::vec3 w_mesh::get_position() const
{
	return glm::vec3(this->_position.x, this->_position.y, this->_position.z);
}

ID3D11Texture2D* w_mesh::get_texture_2D() const
{
	if (this->_texture)
	{
		return this->_texture->get_texture_2D();
	}
	return nullptr;
}

ID3D11Resource*	w_mesh::get_texture_resource() const
{
	if (this->_texture)
	{
		return this->_texture->get_resource();
	}
	return nullptr;
}

ID3D11ShaderResourceView* w_mesh::get_shader_resource_view() const
{
	if (this->_texture)
	{
		return this->_texture->get_srv();
	}
	return nullptr;
}

D3D11_TEXTURE2D_DESC w_mesh::get_texture_2D_description() const
{
	if (this->_texture)
	{
		return this->_texture->get_description();
	}

	D3D11_TEXTURE2D_DESC _description;
	std::memset(&_description, 0, sizeof(_description));
	return _description;
}

#pragma endregion