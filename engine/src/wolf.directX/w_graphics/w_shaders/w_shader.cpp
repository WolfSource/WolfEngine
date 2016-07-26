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

HRESULT w_shader::create_vertex_shader(_In_ ID3D11Device1* pDevice, const void* pBytes, SIZE_T pLength, wolf::content_pipeline::w_vertex_declaration pVDeclaration)
{
	auto _hr = pDevice->CreateVertexShader(pBytes, pLength, nullptr, &this->_vertex_shader);
	V(_hr, L"create vertex shader", this->name, 3, false);

	_hr = _choose_input_layout(pDevice, pVDeclaration, pBytes, pLength);
	V(_hr, L"create vertex declaration for vertex shader", this->name, 3, false);

	return _hr;
}

HRESULT w_shader::create_pixel_shader(_In_ ID3D11Device1* pDevice, const void* bytes, SIZE_T Length)
{
	auto hr = pDevice->CreatePixelShader(bytes, Length, nullptr, &this->_pixel_shader);
	V(hr, L"create pixel shader", this->name, 3, false);

	return hr;
}

HRESULT w_shader::load_shader(_In_ ID3D11Device1* pDevice, std::wstring pPath, w_shader_type pShaderType, wolf::content_pipeline::w_vertex_declaration pVDeclaration, _Inout_ w_shader* pShader)
{
	auto _path = wolf::system::io::get_content_directory() + pPath;
	if (pShader == nullptr)
	{
		logger.write(L"Shader is null when loading shader from following path: " + _path);
		return S_FALSE;
	}

	std::unique_ptr<uint8_t[]> data;
	size_t dataSize;
	std::tuple<int, boost::filesystem::perms> fileState;

	//read binary compiled shader object
	auto _hr = wolf::system::io::read_binary_file(_path, data, &dataSize, &fileState);
	V(_hr, L"could not read binary data", L"Shader.h", 3, false);
	if (FAILED(_hr)) return _hr;

	//create shader
	switch (pShaderType)
	{
	case w_shader_type::VERTEX_SHADER:
		_hr = pShader->create_vertex_shader(pDevice, data.get(), dataSize, pVDeclaration);
		break;
	case w_shader_type::PIXEL_SHADER:
		_hr = pShader->create_pixel_shader(pDevice, data.get(), dataSize);
		break;
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
		V(S_FALSE, _msg, L"Shader.h", 0, true);
		_msg.clear();
	}

	return _hr;
}

void w_shader::set_vertex_shader(_In_ ID3D11VertexShader* pVS)
{
	this->_vertex_shader = pVS;
}

void w_shader::set_pixel_shader(_In_ ID3D11PixelShader* pPS)
{
	this->_pixel_shader = pPS;
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

void w_shader::set_srv(_In_ ID3D11DeviceContext1* pContext, UINT StartSLot, UINT NumViews, _In_ ID3D11ShaderResourceView* srv)
{
	if (w_graphics_device_manager::samplers.size() != 0)
	{
		auto _sampler = w_graphics_device_manager::samplers.at(0);
		pContext->PSSetShaderResources(StartSLot, NumViews, &srv);
		pContext->PSSetSamplers(StartSLot, NumViews, _sampler.GetAddressOf());
	}
}

void w_shader::apply(_In_ ID3D11DeviceContext1* pContext)
{
	pContext->IASetInputLayout(this->_input_layout.Get());
	pContext->VSSetShader(this->_vertex_shader.Get(), nullptr, 0);
	pContext->PSSetShader(this->_pixel_shader.Get(), nullptr, 0);
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
		D3D11_INPUT_ELEMENT_DESC _vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0								, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, _vertexDesc, W_ARRAY_SIZE(_vertexDesc), pShaderData, pShaderDataLength);
		V(_hr, L"Create Input Layout", this->name, 3, false, true);
	}
	else if (pVertexTypes == content_pipeline::POSITION_UV)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0								, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT		, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, vertexDesc, W_ARRAY_SIZE(vertexDesc), pShaderData, pShaderDataLength);
		V(_hr, L"Create Input Layout", this->name, 3, false, true);
	}
	else if (pVertexTypes == content_pipeline::POSITION_COLOR)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0								, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, vertexDesc, W_ARRAY_SIZE(vertexDesc), pShaderData, pShaderDataLength);
		V(_hr, L"Create Input Layout", this->name, 3, false, true);
	}
	else if (pVertexTypes == content_pipeline::POSITION_NORMAL_COLOR)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0								, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, vertexDesc, W_ARRAY_SIZE(vertexDesc), pShaderData, pShaderDataLength);
		V(_hr, L"Create Input Layout", this->name, 3, false, true);
	}
	else if (pVertexTypes == content_pipeline::POSITION_NORMAL_UV)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0								, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT		, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, vertexDesc, W_ARRAY_SIZE(vertexDesc), pShaderData, pShaderDataLength);
		V(_hr, L"Create Input Layout", this->name, 3, false, true);
	}
	else if (pVertexTypes == content_pipeline::POSITION_NORMAL_UV_TANGENT_BINORMAL)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, 0								, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT		, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT"	, 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT	, 0, D3D11_APPEND_ALIGNED_ELEMENT	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_hr = _create_input_layout(pDevice, vertexDesc, W_ARRAY_SIZE(vertexDesc), pShaderData, pShaderDataLength);
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

ULONG w_shader::release()
{
	if (_super::is_released()) return 0;
	
	COMPTR_RELEASE(this->_input_layout);
	COMPTR_RELEASE(this->_vertex_shader);
	COMPTR_RELEASE(this->_pixel_shader);
	
	return _super::release();
}
