#include "w_directX_pch.h"
#include "w_renderable_model.h"
#include <w_vertex_declaration.h>

using namespace wolf::graphics::directX;
using namespace wolf::content_pipeline;

w_renderable_model::w_renderable_model(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, w_model* pModelData) :
	_gDevice(pGDevice),
	_view_projection(DirectX::XMMatrixIdentity())
{
	this->_base_model = new w_model();
	if (pModelData)
	{
		//copy memory
		std::memcpy(this->_base_model, pModelData, sizeof(w_model));
	}
	this->_shader = new w_shader();
}

w_renderable_model::~w_renderable_model()
{

}

HRESULT w_renderable_model::load()
{
	if (this->_base_model == nullptr) return S_FALSE;

	//load meshes
	std::vector<w_model::w_mesh*> _meshes;
	this->_base_model->get_meshes(_meshes);

	byte _error = 0;
	for (size_t i = 0; i < _meshes.size(); ++i)
	{
		if (_create_buffers(_meshes[i]) == S_FALSE)
		{
			_error += 1;
		}
	}

	//load global shaders for rendering all submeshes
	auto _device = this->_gDevice->device.Get();
	auto _hr = w_shader::load_shader(
		_device, 
		L"Shaders\\static_model_vs.cso", 
		w_shader_type::VERTEX_SHADER, 
		w_vertex_declaration::POSITION_NORMAL_UV_TANGENT_BINORMAL, 
		this->_shader);
	if (_hr == S_FALSE)
	{
		_error += 1;
		V(_hr, L"loading vertex shader", this->name, 3, false, true);
	}
	else
	{
		//if we already load vertex shader, then load pixel shader
		_hr = w_shader::load_shader(
			_device, 
			L"Shaders\\model_ps.cso", 
			w_shader_type::PIXEL_SHADER, 
			w_vertex_declaration::UNKNOWN, 
			this->_shader);
		if (_hr == S_FALSE)
		{
			_error += 1;
			V(_hr, L"loading pixel shader", this->name, 3, false, true);
		}
	}

	this->_model_cBuffer.load(_device);

	return _error > 0 ? S_FALSE : S_OK;
}

HRESULT w_renderable_model::_create_buffers(_In_ w_model::w_mesh* pMesh)
{
	if (pMesh == nullptr) return S_FALSE;

	_release_vertex_buffers();
	_release_index_buffers();

#pragma region create vertex buffer

	auto _size = pMesh->vertices.size();
	std::vector<vertex_declaration_structs::vertex_position_normal_uv_tangent_binormal> _vertex_data;
	_vertex_data.resize(_size);
	for (size_t i = 0; i < _size; ++i)
	{
		_vertex_data[i].position.x = pMesh->vertices[i].position.x;
		_vertex_data[i].position.y = pMesh->vertices[i].position.y;
		_vertex_data[i].position.z = pMesh->vertices[i].position.z;

		_vertex_data[i].normal = pMesh->vertices[i].normal;

		_vertex_data[i].uv = pMesh->vertices[i].uv;

		_vertex_data[i].tangent = pMesh->vertices[i].tangent;

		_vertex_data[i].binormal = pMesh->vertices[i].binormal;
	}

	D3D11_BUFFER_DESC _v_buffer_desc;
	std::memset(&_v_buffer_desc, 0, sizeof(_v_buffer_desc));
	_v_buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex_declaration_structs::vertex_position_normal_uv_tangent_binormal) * _size);
	_v_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	_v_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	_v_buffer_desc.CPUAccessFlags = 0;
	_v_buffer_desc.MiscFlags = 0;
	_v_buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA _v_buffer_data;
	std::memset(&_v_buffer_data, 0, sizeof(_v_buffer_data));
	_v_buffer_data.pSysMem = _vertex_data.data();

	ID3D11Buffer* __vertex_buffer;
	auto _hr = this->_gDevice->device->CreateBuffer(&_v_buffer_desc, &_v_buffer_data, &__vertex_buffer);
	V(_hr, L"creating vertex buffer", this->name, 3, true, true);

	this->_vertex_buffers.push_back(__vertex_buffer);
	_vertex_data.clear();

#pragma endregion

#pragma region create index buffer

	D3D11_BUFFER_DESC _index_buffer_desc;
	std::memset(&_index_buffer_desc, 0, sizeof(_index_buffer_desc));

	_index_buffer_desc.ByteWidth = sizeof(unsigned short) * pMesh->indices.size();
	_index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	_index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	_index_buffer_desc.CPUAccessFlags = 0;
	_index_buffer_desc.MiscFlags = 0;
	_index_buffer_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA _index_buffer_data;
	std::memset(&_index_buffer_data, 0, sizeof(_index_buffer_data));
	_index_buffer_data.pSysMem = pMesh->indices.data();

	ID3D11Buffer* __index_buffer;
	_hr = this->_gDevice->device->CreateBuffer(&_index_buffer_desc, &_index_buffer_data, &__index_buffer);
	V(_hr, L"creating index buffer", this->name, 3, true, true);

	this->_index_buffers.push_back(__index_buffer);

#pragma endregion

	return S_OK;
}

void w_renderable_model::render(const wolf::system::w_game_time& pGameTime)
{
	auto _context = this->_gDevice->context.Get();

	auto _transform = this->_base_model->get_transform();
	DirectX::XMMATRIX _transform_matrix(
		_transform.transform[0][0], _transform.transform[0][1], _transform.transform[0][2], _transform.transform[0][3],
		_transform.transform[1][0], _transform.transform[1][1], _transform.transform[1][2], _transform.transform[1][3],
		_transform.transform[2][0], _transform.transform[2][1], _transform.transform[2][2], _transform.transform[2][3],
		_transform.transform[3][0], _transform.transform[3][1], _transform.transform[3][2], _transform.transform[3][3]);

	this->_model_cBuffer.data.wvp = DirectX::XMMatrixTranspose(_transform_matrix * this->_view_projection);
	this->_model_cBuffer.update(_context);
	this->_shader->set_constant_buffer(_context, 0, 1, this->_model_cBuffer.get_buffer());
	this->_shader->apply(_context);

	//render all sub meshes
	std::vector<w_model::w_mesh*> _meshes;
	this->_base_model->get_meshes(_meshes);
	for (size_t i = 0; i < _meshes.size(); ++i)
	{
		_render_mesh(_meshes[i], i);
	}
}

void w_renderable_model::_render_mesh(wolf::content_pipeline::w_model::w_mesh* pMesh, size_t pIndex)
{
	if (pMesh == nullptr) return;

	auto _context = this->_gDevice->context.Get();
	const UINT _offset = 0;
	const UINT _stride = sizeof(vertex_declaration_structs::vertex_position_normal_uv_tangent_binormal);

	_context->IASetVertexBuffers(0, 1, &this->_vertex_buffers[pIndex], &_stride, &_offset);
	_context->IASetIndexBuffer(this->_index_buffers[pIndex], DXGI_FORMAT_R16_UINT, 0);
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_context->DrawIndexed(pMesh->indices.size(), 0, 0);
}

void w_renderable_model::_release_vertex_buffers()
{
	if (this->_vertex_buffers.size() == 0) return;
	
	std::for_each(this->_vertex_buffers.begin(), this->_vertex_buffers.end(),[](ID3D11Buffer* pIter)
	{
		COM_RELEASE(pIter);
	});
}

void w_renderable_model::_release_index_buffers()
{
	if (this->_index_buffers.size() == 0) return;

	std::for_each(this->_index_buffers.begin(), this->_index_buffers.end(), [](ID3D11Buffer* pIter)
	{
		COM_RELEASE(pIter);
	});
}

ULONG w_renderable_model::release()
{
	if (this->is_released()) return 0;
	
	_release_vertex_buffers();
	_release_index_buffers();

	return _super::release();
}


#pragma region Setters

void w_renderable_model::set_view_projection(DirectX::XMMATRIX pValue)
{
	this->_view_projection = pValue;
	//this->_view_projection = DirectX::XMMATRIX(
	//	pValue[0][0], pValue[0][1], pValue[0][2], pValue[0][3],
	//	pValue[1][0], pValue[1][1], pValue[1][2], pValue[1][3],
	//	pValue[2][0], pValue[2][1], pValue[2][2], pValue[2][3],
	//	pValue[3][0], pValue[3][1], pValue[3][2], pValue[3][3]);
}

#pragma endregion
