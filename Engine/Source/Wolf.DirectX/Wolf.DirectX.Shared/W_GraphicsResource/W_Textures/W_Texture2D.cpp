#include "Wolf.DirectX.Shared.PCH.h"
#include "W_Texture2D.h"
#include "ScreenGrab.h"

using namespace std;
using namespace D2D1;
using namespace Microsoft::WRL;
using namespace Wolf::Graphics;

W_Texture2D::W_Texture2D()
{
	this->Name = typeid(this).name();
}

W_Texture2D::~W_Texture2D()
{
	Release();
}

HRESULT W_Texture2D::CreateColorBar(_In_ ID3D11Device1* pDevice, UINT pWidth, UINT pHeight, D3D11_USAGE pUsage,
	D3D11_CPU_ACCESS_FLAG pCpuAccessFlags, DXGI_FORMAT pFormat, D3D11_BIND_FLAG pBind)
{
	auto size = pWidth * pHeight;
	auto data = new UINT32[size];

	#pragma region Create colorbar

	const UINT32  colorData[8] =
	{
		0xFFFFFFFF,//White
		0x00FFFF00,//Yellow
		0xFF00FFFF,//Cyan
		0x000FF000,//Green
		0xFFFF00FF,//Pink
		0x00FF0000,//Red
		0xFF0000FF,//Blue
		0x00000000,//Black
	};
	
	for (size_t i = 0, k = 0; i < pWidth; ++i)
	{
		if (i != 0 && i % 90 == 0) k++;
		for (size_t j = 0; j < pHeight; ++j)
		{
			data[j * pWidth + i] = colorData[k];
		}
	}

	#pragma endregion

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = data;
	initData.SysMemPitch = static_cast<UINT>(pWidth * 4);
	initData.SysMemSlicePitch = static_cast<UINT>(size * 4);

	auto hr = Create(pDevice, pWidth, pHeight, pUsage, pCpuAccessFlags, pFormat, pBind, &initData);

	//Release array
	delete[] data;
	return hr;
}

HRESULT W_Texture2D::Create(_In_ ID3D11Device1* pDevice, UINT pWidth, UINT pHeight, ColorF pColor, D3D11_USAGE pUsage, 
	D3D11_CPU_ACCESS_FLAG pCpuAccessFlags, DXGI_FORMAT pFormat, D3D11_BIND_FLAG pBind)
{
	UINT r = pColor.r * 255;
	UINT g = pColor.g * 255;
	UINT b = pColor.b * 255;
	UINT a = pColor.a * 255;

	//Fill SUBRESOURCE DATA
	auto size = pWidth * pHeight;
	auto data = new UINT32[size];

	if (r == g == b == a)
	{
		//Same rgba color for all pixesl
		memset(data, r, size * sizeof(UINT));
	}
	else
	{
		for (size_t i = 0; i < pWidth; ++i)
		{
			for (size_t j = 0; j < pHeight; ++j)
			{
				data[j * pWidth + i] = ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8) + (a & 0xff);//colorData[k];
			}
		}
	}

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = data;
	initData.SysMemPitch = static_cast<UINT>(pWidth * 4);
	initData.SysMemSlicePitch = static_cast<UINT>(size * 4);

	auto hr = Create(pDevice, pWidth, pHeight, pUsage, pCpuAccessFlags, pFormat, pBind, &initData);
	
	//Release data
	delete [] data;
	return hr;
}

HRESULT W_Texture2D::Create(_In_ ID3D11Device1* pDevice, UINT pWidth, UINT pHeight, D3D11_USAGE pUsage, 
	D3D11_CPU_ACCESS_FLAG pCpuAccessFlags, DXGI_FORMAT pFormat, D3D11_BIND_FLAG pBindFlag, D3D11_SUBRESOURCE_DATA* pIinitialData)
{
	//Currently we use just one texture
	UINT arraySize = 1;

	//set D3D11_CPU_ACCESS_READ flag for CPUAccessFlags
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

	desc.Height = pHeight;
	desc.Width = pWidth;
	desc.MipLevels = 1;
	desc.ArraySize = arraySize;
	desc.Format = pFormat;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = pUsage;
	desc.BindFlags = pBindFlag;
	desc.CPUAccessFlags = pUsage == D3D11_USAGE_DEFAULT ? 0 : pCpuAccessFlags;//We can not set CPU access flag in D3D11_USAGE_DEFAULT usage mode
	desc.MiscFlags = 0;

	HRESULT hr = pDevice->CreateTexture2D(&desc, pIinitialData, &this->texture2D);
	OnFailed(hr, "CreateTexture2D", this->Name, false, true);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = pFormat;
	if (arraySize > 1)
	{
		SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
		SRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
		SRVDesc.Texture2DArray.ArraySize = static_cast<UINT>(arraySize);
	}
	else
	{
		SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = desc.MipLevels;
	}

	hr = pDevice->CreateShaderResourceView(this->texture2D.Get(), &SRVDesc, &this->srv);
	OnFailed(hr, "Create texture resource view from texture2D", this->Name, false, true);
	if (FAILED(hr)) { COM_RELEASE(this->texture2D); }

	return hr;
}

void W_Texture2D::LoadResource(_In_ ComPtr<ID3D11Resource> resource, _In_ ComPtr<ID3D11ShaderResourceView> shaderResourceView)
{
	if (shaderResourceView == nullptr || resource == nullptr) return;

	//Copy to local srv
	this->srv = shaderResourceView;
	//copy resource to texture
	auto hr = resource.As(&this->texture2D);
	OnFailed(hr, "copying resource to texture2D", this->Name, false, true);
	this->texture2D->GetDesc(&this->description);
}

ID3D11Resource* W_Texture2D::GetResource()
{
	ID3D11Resource* resource = nullptr;
	this->srv->GetResource(&resource);
	return resource;
}

ULONG W_Texture2D::Release()
{
	if (IsReleased()) return 0;

	COM_RELEASE(this->srv);
	COM_RELEASE(this->texture2D);
	
	return W_Object::Release();
}

HRESULT W_Texture2D::CaptureScreen(_In_ ID3D11DeviceContext* pContext, _In_ ID3D11Resource* pSource, _In_z_ LPCWSTR pFileName)
{
	return DirectX::SaveDDSTextureToFile(pContext, pSource, pFileName);
}

HRESULT W_Texture2D::LoadTexture(_In_ ID3D11Device1* pD3dDevice,
	_Inout_ W_Texture2D* pTexture,
	wstring pPath,
	size_t pMaxSize,
	DXGI_FORMAT pFormat,
	UINT pBindFlags)
{
	using namespace std;
	using namespace System::IO;

	std::unique_ptr<uint8_t[]> data;
	size_t dataSize;
	int fileState;

	auto _path = GetContentDirectory() + pPath;
	auto hr = System::IO::ReadBinaryFile(_path, data, &dataSize, &fileState);
	if (FAILED(hr))
	{
		string msg = "";
		switch (fileState)
		{
		case -1:
			msg = "Could not find the texture : ";
			break;
		case -2:
			msg = "Texture is corrupted : ";
			break;
		}


		OnFailed(S_FALSE, msg + string(pPath.begin(), pPath.end()), "Texture2D", false, false);

		return hr;
	}

	Microsoft::WRL::ComPtr<ID3D11Resource> resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;

	CreateDDSTextureFromMemory(
		pD3dDevice,
		data.get(),
		dataSize,
		&resource,
		&srv,
		pMaxSize,
		pFormat,
		pBindFlags);

	pTexture->LoadResource(resource, srv);

	return S_OK;
}

//Create a sampler
void W_Texture2D::CreateSampler(const std::shared_ptr<W_GraphicsDevice>& pGDevice,
	const D3D11_FILTER pFilter,
	const D3D11_TEXTURE_ADDRESS_MODE pAddress[3])
{
	// Once the texture view is created, create a sampler.  This defines how the color
	// for a particular texture coordinate is determined using the relevant texture data.
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	samplerDesc.Filter = pFilter;

	// The sampler does not use anisotropic filtering, so this parameter is ignored.
	samplerDesc.MaxAnisotropy = 0;

	// Specify how texture coordinates outside of the range 0..1 are resolved.
	samplerDesc.AddressU = pAddress[0];
	samplerDesc.AddressV = pAddress[1];
	samplerDesc.AddressW = pAddress[2];

	// Use no special MIP clamping or bias.
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Don't use a comparison function.
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	// Border address mode is not used, so this parameter is ignored.
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;

	ID3D11SamplerState* _sampler = nullptr;
	{
		auto hr = pGDevice->device->CreateSamplerState(&samplerDesc, &_sampler);
		OnFailed(hr, "Error on create sampler for texture2D", "Texture2D", false, true);

		pGDevice->samplers.push_back(_sampler);
	}
}