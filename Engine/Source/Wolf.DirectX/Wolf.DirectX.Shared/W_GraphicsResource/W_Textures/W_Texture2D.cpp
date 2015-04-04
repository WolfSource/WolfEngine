#include "Wolf.DirectX.Shared.PCH.h"
#include "W_Texture2D.h"

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

HRESULT W_Texture2D::CreateColorBar(_In_ ID3D11Device1* pDevice, UINT width, UINT height,
	DXGI_FORMAT format, D3D11_USAGE usage, D3D11_BIND_FLAG bind, D3D11_CPU_ACCESS_FLAG cpuAccessFlags)
{
	auto size = width * height;
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
	
	for (size_t i = 0, k = 0; i < width; ++i)
	{
		if (i != 0 && i % 90 == 0) k++;
		for (size_t j = 0; j < height; ++j)
		{
			data[j * width + i] = colorData[k];
		}
	}

	#pragma endregion

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = data;
	initData.SysMemPitch = static_cast<UINT>(width * 4);
	initData.SysMemSlicePitch = static_cast<UINT>(size * 4);

	auto hr = Create(pDevice, width, height, &initData, format, usage, bind, cpuAccessFlags);

	//Release array
	delete[] data;
	return hr;
}

HRESULT W_Texture2D::Create(_In_ ID3D11Device1* pDevice, UINT width, UINT height,
	ColorF color, DXGI_FORMAT format, D3D11_USAGE usage, 
	D3D11_BIND_FLAG bind, D3D11_CPU_ACCESS_FLAG cpuAccessFlags)
{
	UINT r = color.r * 255;
	UINT g = color.g * 255;
	UINT b = color.b * 255;
	UINT a = color.a * 255;

	//Fill SUBRESOURCE DATA
	auto size = width * height;
	auto data = new UINT32[size];

	if (r == g == b == a)
	{
		//Same rgba color for all pixesl
		memset(data, r, size * sizeof(UINT));
	}
	else
	{
		for (size_t i = 0; i < width; ++i)
		{
			for (size_t j = 0; j < height; ++j)
			{
				data[j * width + i] = ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8) + (a & 0xff);//colorData[k];
			}
		}
	}

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = data;
	initData.SysMemPitch = static_cast<UINT>(width * 4);
	initData.SysMemSlicePitch = static_cast<UINT>(size * 4);

	auto hr = Create(pDevice, width, height, &initData, format, usage, bind, cpuAccessFlags);
	
	//Release data
	delete [] data;
	return hr;
}

HRESULT W_Texture2D::Create(_In_ ID3D11Device1* pDevice, UINT width, UINT height,
	D3D11_SUBRESOURCE_DATA* initialData, DXGI_FORMAT format, D3D11_USAGE usage,
	D3D11_BIND_FLAG bindFlag, D3D11_CPU_ACCESS_FLAG cpuAccessFlags)
{
	//Currently we use just one texture
	UINT arraySize = 1;

	//set D3D11_CPU_ACCESS_READ flag for CPUAccessFlags
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

	desc.Height = height;
	desc.Width = width;
	desc.MipLevels = 1;
	desc.ArraySize = arraySize;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = usage;
	desc.BindFlags = bindFlag;
	desc.CPUAccessFlags = usage == D3D11_USAGE_DEFAULT ? 0 : cpuAccessFlags;//We can not set CPU access flag in D3D11_USAGE_DEFAULT usage mode
	desc.MiscFlags = 0;

	HRESULT hr = pDevice->CreateTexture2D(&desc, initialData, &this->texture2D);
	OnFailed(hr, "CreateTexture2D", this->Name, false, true);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = format;
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