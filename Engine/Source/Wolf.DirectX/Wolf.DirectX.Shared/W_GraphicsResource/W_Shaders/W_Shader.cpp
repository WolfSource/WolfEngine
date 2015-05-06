#include "Wolf.DirectX.Shared.PCH.h"
#include "W_Shader.h"

using namespace DirectX;
using namespace Wolf::Graphics;

W_Shader::W_Shader(const std::shared_ptr<W_GraphicsDevice>& pGDevice) : gDevice(pGDevice)
{
	this->Name = typeid(this).name();
}

W_Shader::~W_Shader()
{
	Release();
}

HRESULT W_Shader::CreateVertexShader(const void* bytes, SIZE_T Length, W_VertexDeclaration VDeclaration)
{
	auto hr = this->gDevice->device->CreateVertexShader(bytes, Length, nullptr, &this->vShader);
	OnFailed(hr, "create vertex shader", this->Name, false);

	hr = ChooseInputLayout(VDeclaration, bytes, Length);
	OnFailed(hr, "create vertex declaration for vertex shader", this->Name, false);

	return hr;
}

HRESULT W_Shader::CreatePixelShader(const void* bytes, SIZE_T Length)
{
	auto hr = this->gDevice->device->CreatePixelShader(bytes, Length, nullptr, &this->pShader);
	OnFailed(hr, "create pixel shader", this->Name, false);

	return hr;
}

HRESULT W_Shader::LoadShader(std::wstring path, W_ShaderType shaderType, W_VertexDeclaration vDeclaration, _Inout_ W_Shader* shader)
{
	std::unique_ptr<uint8_t[]> data;
	size_t dataSize;
	int fileState;

	auto _path = Wolf::System::IO::GetContentDirectory() + path;

	//ToDo://Check path before loading
	auto hr = Wolf::System::IO::ReadBinaryFile(_path, data, &dataSize, &fileState);
	OnFailed(hr, "Could not read binary data", "Shader.h", false);
	if (FAILED(hr)) return hr;

	switch (shaderType)
	{
	case W_ShaderType::VertexShader:
		hr = shader->CreateVertexShader(data.get(), dataSize, vDeclaration);
		break;
	case W_ShaderType::PixelShader:
		hr = shader->CreatePixelShader(data.get(), dataSize);
		break;
	case W_ShaderType::HullShader:
		//Not implemented yet
		break;
	case W_ShaderType::DomainShader:
		//Not implemented yet
		break;
	case W_ShaderType::GeometryShader:
		//Not implemented yet
		break;
	case W_ShaderType::ComputeShader:
		//Not implemented yet
		break;
	default:
		std::wstring msg = L"Unknown shader type";
		MessageBox(NULL, msg.c_str(), L"Error", MB_OK);
		OnFailedW(S_FALSE, msg, "Shader.h", true);
		msg.clear();
	}

	return hr;
}

void W_Shader::SetVertexShader(_In_ ID3D11VertexShader* VS)
{
	this->vShader = VS;
}

void W_Shader::SetPixelShader(_In_ ID3D11PixelShader* PS)
{
	this->pShader = PS;
}

void W_Shader::SetConstantBuffer(UINT startSlot, UINT numBuffer, _In_ ID3D11Buffer* CBuffer)
{
	auto context = this->gDevice->context;
	{
		context->VSSetConstantBuffers(startSlot, numBuffer, &CBuffer);
		context->PSSetConstantBuffers(startSlot, numBuffer, &CBuffer);
		context->HSSetConstantBuffers(startSlot, numBuffer, &CBuffer);
	}
	context = nullptr;
}

void W_Shader::SetInputLayout(ID3D11InputLayout* value)
{
	this->inputLayout = value;
}

void W_Shader::SetSRV(UINT StartSLot, UINT NumViews, _In_ ID3D11ShaderResourceView* srv)
{
	if (this->gDevice->samplers.size() != 0)
	{
		auto context = this->gDevice->context;
		{
			auto sampler = this->gDevice->samplers.at(0);
			context->PSSetShaderResources(StartSLot, NumViews, &srv);
			context->PSSetSamplers(StartSLot, NumViews, sampler.GetAddressOf());
		}
		context = nullptr;
	}
}

void W_Shader::Apply()
{
	auto context = this->gDevice->context;
	{
		context->IASetInputLayout(this->inputLayout.Get());
		context->VSSetShader(this->vShader.Get(), nullptr, 0);
		context->PSSetShader(this->pShader.Get(), nullptr, 0);
	}
	context = nullptr;
}

HRESULT W_Shader::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* elementDesc, const UINT elementDescLength,
	const void* shaderData, const SIZE_T shaderDataLength)
{
	auto hr = this->gDevice->device->CreateInputLayout(
		elementDesc,
		elementDescLength,
		shaderData,
		shaderDataLength,
		&this->inputLayout);
	return hr;
}

HRESULT W_Shader::ChooseInputLayout(W_VertexDeclaration vertexTypes, const void* shaderData, SIZE_T shaderDataLength)
{
	HRESULT hr = S_OK;
	if (vertexTypes == Position)
	{
		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength);
	}
	else if (vertexTypes == PositionTexture)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength);
	}
	else if (vertexTypes == PositionColor)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength);
	}
	else if (vertexTypes == PositionNormalColor)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength);
		OnFailed(hr, "Create Input Layout", this->Name, false);
	}
	else if (vertexTypes == PositionNormalTexture)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0,	0,									D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength);
		OnFailed(hr, "Create Input Layout", this->Name, false);
	}
	else if (vertexTypes == PositionNormalTextureTangent)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength);
	}
	else if (vertexTypes == PositionNormalTextureTangentBinormal)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength);
	}
	else if (vertexTypes == PositionNormalTangentColorTexture)
	{
		const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), shaderData, shaderDataLength);
	}

	else
	{
		//Unknown vertex format
		hr = S_FALSE;
		std::wstring msg = L"Unknown vertex format";
		MessageBox(NULL, msg.c_str(), L"Error", MB_OK);
		OnFailedW(hr, msg, this->Name, false);
	}
	return hr;
}

ULONG W_Shader::Release()
{
	if (IsReleased()) return 0;
	
	COM_RELEASE(inputLayout);
	COM_RELEASE(vShader);
	COM_RELEASE(pShader);

	this->gDevice = nullptr;

	return W_Object::Release();
}
