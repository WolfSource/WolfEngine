#include "w_directX_pch.h"
#include "w_texture_2D.h"
#include "ScreenGrab.h"

using namespace std;
using namespace D2D1;
using namespace Microsoft::WRL;
using namespace wolf::graphics;
using namespace concurrency;
using namespace concurrency::direct3d;
using namespace concurrency::graphics;
using namespace concurrency::graphics::direct3d;

w_texture_2D::w_texture_2D()
{
	_super::set_class_name(typeid(this).name());
}

w_texture_2D::~w_texture_2D()
{
	release();
}

HRESULT w_texture_2D::create_colorBar(_In_ ID3D11Device1* pDevice, UINT pWidth, UINT pHeight, D3D11_USAGE pUsage,
	UINT pCpuAccessFlags, DXGI_FORMAT pFormat, UINT pBindFlags, GPGPU_TYPE pGPGPU_Type)
{
	if (pGPGPU_Type == GPGPU_TYPE::NONE)
	{
		return _create_colorBar(
			pDevice,
			pWidth,
			pHeight,
			pUsage,
			pCpuAccessFlags,
			pFormat,
			pBindFlags);
	}
	else
	{
		if (pGPGPU_Type == GPGPU_TYPE::OPENCL)
		{
			assert(false, "OpenCL is not supported yet");
		}

		//Currently, only one dimention & one mipMap
		UINT _arraySize = 1;
		UINT _mipLevels = 1;

		//Create texture2D description
		D3D11_TEXTURE2D_DESC _desc;
		std::memset(&_desc, 0, sizeof(_desc));
		
		_desc.Height = pHeight;
		_desc.Width = pWidth;
		_desc.MipLevels = _mipLevels;
		_desc.ArraySize = _arraySize;
		_desc.Format = pFormat;
		_desc.SampleDesc.Count = 1;
		_desc.SampleDesc.Quality = 0;
		_desc.Usage = pUsage;
		_desc.BindFlags = pBindFlags;
		_desc.CPUAccessFlags = pUsage == D3D11_USAGE_DEFAULT ? 0 : pCpuAccessFlags;//We can not set CPU access flag in D3D11_USAGE_DEFAULT usage mode
		_desc.MiscFlags = 0;

		//Create texture2D
		HRESULT hr = pDevice->CreateTexture2D(&_desc, NULL, &this->_texture_2D);
		V(hr, L"CreateTexture2D", this->name, 2, false, true);

		_create_shader_resource_view(pDevice, pFormat, _arraySize, _mipLevels);

		//Fill texture with C++AMP 
		auto _amp_acc_view = w_graphics_device_manager::get_camp_accelerator_view(CPP_AMP_DEVICE_TYPE::GPU, pDevice);
		
		// Create Amp textures
		auto _amp_texture = make_texture<unorm4, 2>(_amp_acc_view, this->_texture_2D.Get());
		texture_view<unorm4, 2> amp_texture_view(_amp_texture);

		std::vector<unorm4> _colorData =
		{
			unorm4(1.0f, 1.0f, 1.0f, 1.0f),//White
			unorm4(1.0f, 1.0f, 0.0f, 1.0f),//Yellow
			unorm4(0.0f, 1.0f, 1.0f, 1.0f),//Cyan
			unorm4(0.0f, 1.0f, 0.0f, 1.0f),//Green
			unorm4(1.0f, 0.0f, 1.0f, 1.0f),//Pink
			unorm4(1.0f, 0.0f, 0.0f, 1.0f),//Red
			unorm4(0.0f, 0.0f, 1.0f, 1.0f),//Blue
			unorm4(0.0f, 0.0f, 0.0f, 1.0f),//Blue
		};
		concurrency::array_view<unorm4> array_color(_colorData.size(), _colorData);

		auto _random = 5;
		parallel_for_each(_amp_texture.extent, [amp_texture_view, _random, array_color](index<2> pIdx) restrict(amp)
		{
			auto _y = pIdx[1];

			uint _index = 0;

			if (_y > 630)
			{
				_index = 7;
			}
			else if (_y > 540)
			{
				_index = 6;
			}
			else if (_y > 450)
			{
				_index = 5;
			}
			else if (_y > 360)
			{
				_index = 4;
			}
			else if (_y > 270)
			{
				_index = 3;
			}
			else if (_y > 180)
			{
				_index = 2;
			}
			else if (_y > 90)
			{
				_index = 1;
			}

			amp_texture_view.set(pIdx, array_color[_index]);
		});

		return hr;
	}
}

HRESULT w_texture_2D::_create_colorBar(_In_ ID3D11Device1* pDevice, UINT pWidth, UINT pHeight, D3D11_USAGE pUsage,
	UINT pCpuAccessFlags, DXGI_FORMAT pFormat, UINT pBind)
{
	auto memPitch = pWidth * 4;
	auto slicePitch = pWidth * pHeight * 4;
	auto data = new UINT[slicePitch];//4 byte for argb

#pragma region Create colorbar

	const UINT32 colorData[8] =
	{
		0xFFFFFFFF,//White
		0xFFFFFF00,//Yellow
		0xFF00FFFF,//Cyan
		0xFF00FF00,//Green
		0xFFFF00FF,//Pink
		0xFFFF0000,//Red
		0xFF0000FF,//Blue
		0xFF000000,//Black
	};

	const UINT _div = pWidth / 8;
	for (size_t i = 0, k = 0; i < pWidth; ++i)
	{
		if (i != 0 && i % _div == 0) k++;
		for (size_t j = 0; j < pHeight; ++j)
		{
			data[j * pWidth + i] = colorData[k];
		}
	}

#pragma endregion

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = (void*) data;
	initData.SysMemPitch = static_cast<UINT>(memPitch);
	initData.SysMemSlicePitch = static_cast<UINT>(slicePitch);

	auto hr = create(pDevice, pWidth, pHeight, pUsage, pCpuAccessFlags, pFormat, pBind, &initData);

	//Release array
	delete [] data;
	return hr;
}

HRESULT w_texture_2D::create(_In_ ID3D11Device1* pDevice, UINT pWidth, UINT pHeight, ColorF pColor, D3D11_USAGE pUsage,
	UINT pCpuAccessFlags, DXGI_FORMAT pFormat, UINT pBindFlags, GPGPU_TYPE pGPGPU_Type)
{
	HRESULT _hr = S_OK;

	if (pGPGPU_Type == GPGPU_TYPE::NONE)
	{
		_hr = _create(pDevice, pWidth, pHeight, pColor, pUsage, pCpuAccessFlags, pFormat, pBindFlags);
	}
	else
	{
		if (pGPGPU_Type == GPGPU_TYPE::OPENCL)
		{
			assert(false, "OpenCL is not supported yet");
		}

		//Currently, only one dimention & one mipMap
		UINT _arraySize = 1;
		UINT _mipLevels = 1;

		//Create texture2D description
		D3D11_TEXTURE2D_DESC _desc;
		ZeroMemory(&_desc, sizeof(D3D11_TEXTURE2D_DESC));

		_desc.Height = pHeight;
		_desc.Width = pWidth;
		_desc.MipLevels = _mipLevels;
		_desc.ArraySize = _arraySize;
		_desc.Format = pFormat;
		_desc.SampleDesc.Count = 1;
		_desc.SampleDesc.Quality = 0;
		_desc.Usage = pUsage;
		_desc.BindFlags = pBindFlags;
		_desc.CPUAccessFlags = pUsage == D3D11_USAGE_DEFAULT ? 0 : pCpuAccessFlags;//We can not set CPU access flag in D3D11_USAGE_DEFAULT usage mode
		_desc.MiscFlags = 0;

		//Create texture2D
		_hr = pDevice->CreateTexture2D(&_desc, NULL, &this->_texture_2D);
		V(_hr, L"create texture_2D", this->name, 2, false, true);

		_create_shader_resource_view(pDevice, pFormat, _arraySize, _mipLevels);

		//Fill texture with C++AMP 
		auto _amp_acc_view = w_graphics_device_manager::get_camp_accelerator_view(CPP_AMP_DEVICE_TYPE::GPU, pDevice);

		// Create Amp textures
		auto _amp_texture = make_texture<int, 2>(_amp_acc_view, this->_texture_2D.Get());
		auto _size = _amp_texture.extent.size();
		
		UINT r = static_cast<UINT>(pColor.r * 255);
		UINT g = static_cast<UINT>(pColor.g * 255);
		UINT b = static_cast<UINT>(pColor.b * 255);
		UINT a = static_cast<UINT>(pColor.a * 255);

		std::vector<int> _data(_size);

		if (r == g == b == a)
		{
			//Same rgba color for all pixesl
			std::fill(_data.begin(), _data.end(), r);
		}
		else
		{
			for (size_t i = 0; i < pWidth; ++i)
			{
				for (size_t j = 0; j < pHeight; ++j)
				{
					//Todo: Need alpha 
					_data.at(j * pWidth + i) = ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);//argb
				}
			}
		}
		
		concurrency::graphics::copy(_data.begin(), _data.end(), _amp_texture);

		_data.clear();
	}

	return _hr;
}

HRESULT w_texture_2D::create(_In_ ID3D11Device1* pDevice, UINT pWidth, UINT pHeight, D3D11_USAGE pUsage,
	UINT pCpuAccessFlags, DXGI_FORMAT pFormat, UINT pBindFlag, D3D11_SUBRESOURCE_DATA* pIinitialData)
{
	//Currently we use just one texture & one mipmap level
	UINT _arraySize = 1;
	UINT _mipMapLevel = 1;

	//set D3D11_CPU_ACCESS_READ flag for CPUAccessFlags
	D3D11_TEXTURE2D_DESC _texture_desc;
	ZeroMemory(&_texture_desc, sizeof(D3D11_TEXTURE2D_DESC));

	_texture_desc.Height = pHeight;
	_texture_desc.Width = pWidth;
	_texture_desc.MipLevels = 1;
	_texture_desc.ArraySize = _arraySize;
	_texture_desc.Format = pFormat;
	_texture_desc.SampleDesc.Count = 1;
	_texture_desc.SampleDesc.Quality = 0;
	_texture_desc.Usage = pUsage;
	_texture_desc.BindFlags = pUsage == D3D11_USAGE_STAGING ? 0 : pBindFlag;
	_texture_desc.CPUAccessFlags = pUsage == D3D11_USAGE_DEFAULT ? 0 : pCpuAccessFlags;//We can not set CPU access flag in D3D11_USAGE_DEFAULT usage mode
	_texture_desc.MiscFlags = 0;

	HRESULT _hr = pDevice->CreateTexture2D(&_texture_desc, pIinitialData, &this->_texture_2D);
	V(_hr, L"createTexture2D", this->name, 2, false, true);

	//Can not create shader resource view for staging texture
	if (pUsage != D3D11_USAGE_STAGING)
	{
		_hr = _create_shader_resource_view(pDevice, pFormat, _arraySize, _mipMapLevel);
	}

	return _hr;
}

HRESULT w_texture_2D::_create(_In_ ID3D11Device1* pDevice, UINT pWidth, UINT pHeight, ColorF pColor, D3D11_USAGE pUsage,
	UINT pCpuAccessFlags, DXGI_FORMAT pFormat, UINT pBind)
{
	UINT r = static_cast<UINT>(pColor.r * 255);
	UINT g = static_cast<UINT>(pColor.g * 255);
	UINT b = static_cast<UINT>(pColor.b * 255);
	UINT a = static_cast<UINT>(pColor.a * 255);

	//Fill SUBRESOURCE DATA
	auto _memPitch = pWidth * 4;
	auto _slicePitch = pWidth * pHeight * 4;

	auto _data = static_cast<uint8_t*>(malloc(_slicePitch));

	if (r == g == b == a)
	{
		//Same rgba color for all pixesl
		memset(_data, r, _slicePitch * sizeof(UINT));
	}
	else
	{
		for (size_t i = 0; i < pWidth; ++i)
		{
			for (size_t j = 0; j < pHeight; ++j)
			{
				//Todo: Need alpha 
				_data[j * pWidth + i] = ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);//argb
			}
		}
	}

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = (void*)_data;
	initData.SysMemPitch = static_cast<UINT>(_memPitch);
	initData.SysMemSlicePitch = static_cast<UINT>(_slicePitch);

	auto _hr = create(pDevice, pWidth, pHeight, pUsage, pCpuAccessFlags, pFormat, pBind, &initData);

	free(_data);
	return _hr;
}


HRESULT w_texture_2D::_create_shader_resource_view(_In_ ID3D11Device1* pDevice, DXGI_FORMAT pFormat, UINT pArraySize, UINT pMipMapLevels)
{
	HRESULT _hr = S_FALSE;

	//Currently, only one dimention & one mipMap
	UINT _mipLevels = 1;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = pFormat;
	if (pArraySize > 1)
	{
		SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
		SRVDesc.Texture2DArray.MipLevels = _mipLevels;
		SRVDesc.Texture2DArray.ArraySize = static_cast<UINT>(pArraySize);
	}
	else
	{
		SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = _mipLevels;
	}

	_hr = pDevice->CreateShaderResourceView(this->_texture_2D.Get(), &SRVDesc, &this->_srv);
	V(_hr, L"create texture resource view from texture2D", this->name, 2, false, true);
	if (_hr == S_FALSE) 
	{ 
		COMPTR_RELEASE(this->_texture_2D);
	}

	return _hr;

}

void w_texture_2D::load_resource(_In_ ComPtr<ID3D11Resource> resource, _In_ ComPtr<ID3D11ShaderResourceView> shaderResourceView)
{
	if (shaderResourceView == nullptr || resource == nullptr) return;

	//Copy to local srv
	this->_srv = shaderResourceView;
	//copy resource to texture
	auto hr = resource.As(&this->_texture_2D);
	V(hr, L"copying resource to texture2D", this->name, 2, false, true);
	this->_texture_2D->GetDesc(&this->_description);
}

ID3D11Texture2D* w_texture_2D::get_texture_2D() const
{
	return this->_texture_2D.Get();
}

ID3D11Resource* w_texture_2D::get_resource() const
{
	ID3D11Resource* _resource = nullptr;
	this->_srv->GetResource(&_resource);
	return _resource;
}

ID3D11ShaderResourceView* w_texture_2D::get_srv() const
{ 
	return this->_srv.Get(); 
}

ULONG w_texture_2D::release()
{
	if (_super::is_released()) return 0;

	COMPTR_RELEASE(this->_srv);
	COMPTR_RELEASE(this->_texture_2D);
	
	return _super::release();
}

#pragma region static methods

HRESULT w_texture_2D::capture_screen(_In_ ID3D11DeviceContext* pContext, _In_ ID3D11Resource* pSource, _In_z_ LPCWSTR pFileName)
{
	return DirectX::SaveDDSTextureToFile(pContext, pSource, pFileName);
}

HRESULT w_texture_2D::load_texture_2D(_In_ ID3D11Device1* pD3dDevice,
	_Inout_ w_texture_2D* pTexture,
	wstring pPath,
	bool pIsAbsolutePath,
	size_t pMaxSize,
	DXGI_FORMAT pFormat,
	UINT pBindFlags)
{
	using namespace std;
	using namespace system::io;

	std::unique_ptr<uint8_t[]> data;
	size_t dataSize;
	boost::filesystem::perms _file_permission_status;

	auto _path = ( pIsAbsolutePath ? L"" : get_content_directory() ) + pPath;
	auto hr = system::io::read_binary_file(_path, data, &dataSize, _file_permission_status);
	
	if (hr == S_FALSE)
	{
		wstring msg = L"";
		if (_file_permission_status == boost::filesystem::perms::no_perms)
		{
			msg = L"Could not find the texture file: ";
		}
		else
		{
			msg = L"Texture file is corrupted: ";
		}
		V(S_FALSE, msg + pPath, "Texture2D", 2, false, false);

		return hr;
	}

	Microsoft::WRL::ComPtr<ID3D11Resource> _resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _srv = nullptr;

	DirectX::CreateDDSTextureFromMemory(
		pD3dDevice,
		data.get(),
		dataSize,
		&_resource,
		&_srv,
		pMaxSize,
		pFormat,
		pBindFlags);

	if (pTexture == nullptr)
	{
		pTexture = new w_texture_2D();
	}
	pTexture->load_resource(_resource, _srv);

	return S_OK;
}

//Create a sampler
void w_texture_2D::create_sampler(const std::shared_ptr<w_graphics_device>& pGDevice,
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
		V(hr, L"Error on create sampler for texture2D", "Texture2D", 2, false, true);

		w_graphics_device_manager::samplers.push_back(_sampler);
	}
}

#pragma endregion