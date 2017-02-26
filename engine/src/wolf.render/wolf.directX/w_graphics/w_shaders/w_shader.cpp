#include "w_directX_pch.h"
#include "w_shader.h"

using namespace DirectX;
using namespace wolf::graphics;

w_shader::w_shader()
{
	_super::set_class_name(typeid(this).name());
}

w_shader::~w_shader()
{
	release();
}

HRESULT w_shader::create_vertex_shader(_In_ ID3D11Device1* pDevice, const void* pBytes, size_t pLength, wolf::content_pipeline::w_vertex_declaration pVDeclaration)
{
	auto _hr = pDevice->CreateVertexShader(pBytes, pLength, nullptr, &this->_vertex_shader);
	V(_hr, L"create vertex shader", this->name, 3, false);

	_hr = _choose_input_layout(pDevice, pVDeclaration, pBytes, pLength);
	V(_hr, L"create vertex declaration for vertex shader", this->name, 3, false);

	return _hr;
}

HRESULT w_shader::create_pixel_shader(_In_ ID3D11Device1* pDevice, const void* bytes, size_t Length, size_t& pIndexCreated)
{
	ID3D11PixelShader* _ps;
	auto hr = pDevice->CreatePixelShader(bytes, Length, nullptr, &_ps);
	V(hr, L"create pixel shader", this->name, 3, false);

	pIndexCreated = this->_pixel_shaders.size();
	this->_pixel_shaders.push_back(_ps);

	return hr;
}

HRESULT w_shader::load_shader(_In_ ID3D11Device1* pDevice, std::wstring pPath, w_shader_type pShaderType, wolf::content_pipeline::w_vertex_declaration pVDeclaration, _Inout_ w_shader* pShader, _Out_ size_t* pPixelShaderCreatedIndex)
{
	auto _path = wolf::system::io::get_content_directoryW() + pPath;
	if (pShader == nullptr)
	{
		logger.write(L"Shader is null when loading shader from following path: " + _path);
		return S_FALSE;
	}

	std::unique_ptr<uint8_t[]> data;
	size_t dataSize;
	int _file_status = -1;

	//read binary compiled shader object
	auto _hr = wolf::system::io::read_binary_file(_path.c_str(), data, &dataSize, _file_status);
	V(_hr, L"could not read binary data " + (_file_status == -1) ? L"file not found" : L"file corrupted", "Shader.h", 3, false);
	if (FAILED(_hr)) return _hr;

	//create shader
	switch (pShaderType)
	{
	case w_shader_type::VERTEX_SHADER:
		_hr = pShader->create_vertex_shader(pDevice, data.get(), dataSize, pVDeclaration);
		break;
	case w_shader_type::PIXEL_SHADER:
	{
		if (pPixelShaderCreatedIndex)
		{
			_hr = pShader->create_pixel_shader(pDevice, data.get(), dataSize, *pPixelShaderCreatedIndex);
		}
		else
		{
			size_t _pixel_shader_index = 0;
			_hr = pShader->create_pixel_shader(pDevice, data.get(), dataSize, _pixel_shader_index);
		}
		break;
	}
	case w_shader_type::HULL_SHADER:
		//Not implemented yet
		break;
	case w_shader_type::DOMAIN_SHADER:
		//Not implemented yet
		break;
	case w_shader_type::GEOMETRY_SHADER:
		//Not implemented yet
		break;
	case w_shader_type::COMPUTE_SHADER:
		//Not implemented yet
		break;
	default:
		std::wstring _msg = L"Unknown shader type";
		MessageBox(NULL, _msg.c_str(), L"Error", MB_OK);
		V(S_FALSE, _msg, "Shader.h", 0, true);
		_msg.clear();
	}

	return _hr;
}

void w_shader::set_vertex_shader(_In_ ID3D11VertexShader* pVS)
{
	this->_vertex_shader = pVS;
}

void w_shader::set_pixel_shader(_In_ ID3D11PixelShader* pPS, size_t& pIndex)
{
	pIndex = this->_pixel_shaders.size();
	this->_pixel_shaders.push_back(pPS);
}

void w_shader::set_constant_buffer(_In_ ID3D11DeviceContext1* pContext, UINT startSlot, UINT numBuffer, _In_ ID3D11Buffer* CBuffer)
{
	pContext->VSSetConstantBuffers(startSlot, numBuffer, &CBuffer);
	pContext->PSSetConstantBuffers(startSlot, numBuffer, &CBuffer);
	pContext->HSSetConstantBuffers(startSlot, numBuffer, &CBuffer);
}

void w_shader::set_input_layout(ID3D11InputLayout* pValue)
{
	this->_input_layout = pValue;
}

void w_shader::set_srv(_In_ ID3D11DeviceContext1* pContext, UINT pStartSLot, UINT pNumViews, _In_ ID3D11ShaderResourceView* pSRV)
{
	if (w_graphics_device_manager::samplers.size() != 0)
	{
		auto _sampler = w_graphics_device_manager::samplers.at(0);
		pContext->PSSetShaderResources(pStartSLot, pNumViews, &pSRV);
		pContext->PSSetSamplers(pStartSLot, pNumViews, _sampler.GetAddressOf());
	}
}

void w_shader::apply(_In_ ID3D11DeviceContext1* pContext, size_t pPixelShaderIndex)
{
	pContext->IASetInputLayout(this->_input_layout.Get());
	pContext->VSSetShader(this->_vertex_shader.Get(), nullptr, 0);
	if (pPixelShaderIndex < this->_pixel_shaders.size())
	{
		pContext->PSSetShader(this->_pixel_shaders.at(pPixelShaderIndex), nullptr, 0);
	}
}

HRESULT w_shader::_create_input_layout(_In_ ID3D11Device1* pDevice, const D3D11_INPUT_ELEMENT_DESC* pElementDesc, const UINT pElementDescLength,
	const void* pShaderData, const SIZE_T pShaderDataLength)
{
	auto hr = pDevice->CreateInputLayout(
		pElementDesc,
		pElementDescLength,
		pShaderData,
		pShaderDataLength,
		&this->_input_layout);
	return hr;
}

HRESULT w_shader::_choose_input_layout(_In_ ID3D11Device1* pDevice, wolf::content_pipeline::w_vertex_declaration pVertexTypes, const void* pShaderData, SIZE_T pShaderDataLength)
{
	HRESULT _hr = S_OK;
	if (pVertexTypes == content_pipeline::POSITION)
	{
		D3D11_INPUT_ELEMENT_DESC _layout [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0								, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, _layout, W_ARRAY_SIZE(_layout), pShaderData, pShaderDataLength);
		V(_hr, L"Create Input Layout", this->name, 3, false, true);
	}
	else if (pVertexTypes == content_pipeline::POSITION_UV)
	{
		const D3D11_INPUT_ELEMENT_DESC _layout [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0								, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT		, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, _layout, W_ARRAY_SIZE(_layout), pShaderData, pShaderDataLength);
		V(_hr, L"Create Input Layout", this->name, 3, false, true);
	}
	else if (pVertexTypes == content_pipeline::POSITION_COLOR)
	{
		const D3D11_INPUT_ELEMENT_DESC _layout [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0								, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, _layout, W_ARRAY_SIZE(_layout), pShaderData, pShaderDataLength);
		V(_hr, L"Create Input Layout", this->name, 3, false, true);
	}
	else if (pVertexTypes == content_pipeline::POSITION_COLOR_UV)
	{
		const D3D11_INPUT_ELEMENT_DESC _layout [] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0							 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, _layout, W_ARRAY_SIZE(_layout), pShaderData, pShaderDataLength);
		V(_hr, L"Create Input Layout", this->name, 3, false, true);
	}
	else if (pVertexTypes == content_pipeline::POSITION_NORMAL_COLOR)
	{
		const D3D11_INPUT_ELEMENT_DESC _layout [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0								, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, _layout, W_ARRAY_SIZE(_layout), pShaderData, pShaderDataLength);
		V(_hr, L"Create Input Layout", this->name, 3, false, true);
	}
	else if (pVertexTypes == content_pipeline::POSITION_NORMAL_UV)
	{
		const D3D11_INPUT_ELEMENT_DESC _layout [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0								, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT		, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, _layout, W_ARRAY_SIZE(_layout), pShaderData, pShaderDataLength);
		V(_hr, L"Create Input Layout", this->name, 3, false, true);
	}
	else if (pVertexTypes == content_pipeline::POSITION_NORMAL_UV_TANGENT_BINORMAL)
	{
		const D3D11_INPUT_ELEMENT_DESC _layout [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0								, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT		, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT"	, 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, _layout, W_ARRAY_SIZE(_layout), pShaderData, pShaderDataLength);
		V(_hr, L"Create Input Layout", this->name, 3, false, true);
	}
	/*else if (pVertexTypes == Position_Normal_Tangent_Color_Texture)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, vertexDesc, W_ARRAY_SIZE(vertexDesc), pShaderData, pShaderDataLength);
		V(_hr, L"Create Input Layout", this->name, 3, false, true);
	}*/
	else
	{
		//Unknown vertex format
		_hr = S_FALSE;
		std::wstring _msg = L"Unknown vertex format";
		MessageBox(NULL, _msg.c_str(), L"Error", MB_OK);
		V(_hr, _msg, this->name, 3, false);
	}
	return _hr;
}

void w_shader::release_pixel_shaders()
{
	for (size_t i = 0; i < this->_pixel_shaders.size(); ++i)
	{
		auto _ps = this->_pixel_shaders.at(i);
		if (_ps)
		{
			_ps->Release();
		}
	}
	this->_pixel_shaders.clear();
}

ULONG w_shader::release()
{
	if (_super::get_is_released()) return 0;
	
	COMPTR_RELEASE(this->_input_layout);
	COMPTR_RELEASE(this->_vertex_shader);

	release_pixel_shaders();

	return _super::release();
}
