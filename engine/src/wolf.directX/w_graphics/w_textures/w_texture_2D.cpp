#include "w_directX_pch.h"
#include "w_texture_2D.h"
#include "ScreenGrab.h"
#include <w_io.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

HRESULT w_texture_2D::create_color_bar(_In_ ID3D11Device1* pDevice, _In_ UINT pWidth, _In_ UINT pHeight, _In_ D3D11_USAGE pUsage,
	_In_ UINT pCpuAccessFlags, _In_ DXGI_FORMAT pFormat, _In_ UINT pBindFlags)
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

	D3D11_SUBRESOURCE_DATA _init_data;
	std::memset(&_init_data, 0, sizeof(_init_data));

	_init_data.pSysMem = (void*)data;
	_init_data.SysMemPitch = static_cast<UINT>(memPitch);
	_init_data.SysMemSlicePitch = static_cast<UINT>(slicePitch);

	auto hr = create(pDevice, pWidth, pHeight, pUsage, pCpuAccessFlags, pFormat, pBindFlags, &_init_data);

	//Release array
	delete[] data;
	return hr;
}

HRESULT w_texture_2D::create(_In_ ID3D11Device1* pDevice, _In_ UINT pWidth, _In_ UINT pHeight, _In_ ColorF pColor, _In_ D3D11_USAGE pUsage,
	_In_ UINT pCpuAccessFlags, _In_ DXGI_FORMAT pFormat, _In_ UINT pBindFlags, _In_ GPGPU_TYPE pGPGPU_Type)
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
		_hr = pDevice->CreateTexture2D(&_desc, NULL, &this->_texture_buffer);
		V(_hr, L"create texture_2D", this->name, 2, false, true);

		_create_shader_resource_view(pDevice, pFormat, _arraySize, _mipLevels);

		//Fill texture with C++AMP 
		auto _amp_acc_view = w_graphics_device_manager::get_camp_accelerator_view(CPP_AMP_DEVICE_TYPE::GPU, pDevice);

		// Create Amp textures
		auto _amp_texture = make_texture<int, 2>(_amp_acc_view, this->_texture_buffer.Get());
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
					_data.at(j * pWidth + i) = w_color::to_hex(r, g, b, a);
				}
			}
		}
		
		concurrency::graphics::copy(_data.begin(), _data.end(), _amp_texture);

		_data.clear();
	}

	return _hr;
}

HRESULT w_texture_2D::create(_In_ ID3D11Device1* pDevice, _In_ UINT pWidth, _In_ UINT pHeight, _In_ D3D11_USAGE pUsage,
	_In_ UINT pCpuAccessFlags, _In_ DXGI_FORMAT pFormat, _In_ UINT pBindFlag, _In_ D3D11_SUBRESOURCE_DATA* pIinitialData)
{
	//Currently we use just one texture & one mipmap level
	UINT _arraySize = 1;
	UINT _mipMapLevel = 1;

	//set D3D11_CPU_ACCESS_READ flag for CPUAccessFlags
	D3D11_TEXTURE2D_DESC _texture_desc;
	std::memset(&_texture_desc, 0, sizeof(_texture_desc));

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

	HRESULT _hr = pDevice->CreateTexture2D(&_texture_desc, pIinitialData, &this->_texture_buffer);
	V(_hr, L"createTexture2D", this->name, 2, false, true);
	this->_texture_buffer->GetDesc(&this->_description);

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

	D3D11_SUBRESOURCE_DATA _init_data;
	std::memset(&_init_data, 0, sizeof(_init_data));
	
	_init_data.pSysMem = (void*)_data;
	_init_data.SysMemPitch = static_cast<UINT>(_memPitch);
	_init_data.SysMemSlicePitch = static_cast<UINT>(_slicePitch);

	auto _hr = create(pDevice, pWidth, pHeight, pUsage, pCpuAccessFlags, pFormat, pBind, &_init_data);

	free(_data);
	return _hr;
}

HRESULT w_texture_2D::_create_shader_resource_view(_In_ ID3D11Device1* pDevice, DXGI_FORMAT pFormat, UINT pArraySize, UINT pMipMapLevels)
{
	HRESULT _hr = S_FALSE;

	//Currently, only one dimention & one mipMap
	UINT _mipLevels = 1;

	D3D11_SHADER_RESOURCE_VIEW_DESC _srv_desc;
	std::memset(&_srv_desc, 0, sizeof(_srv_desc));

	_srv_desc.Format = pFormat;
	if (pArraySize > 1)
	{
		_srv_desc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
		_srv_desc.Texture2DArray.MipLevels = _mipLevels;
		_srv_desc.Texture2DArray.ArraySize = static_cast<UINT>(pArraySize);
	}
	else
	{
		_srv_desc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		_srv_desc.Texture2D.MipLevels = _mipLevels;
	}

	_hr = pDevice->CreateShaderResourceView(this->_texture_buffer.Get(), &_srv_desc, &this->_srv);
	V(_hr, L"create texture resource view from texture2D", this->name, 2, false, true);
	if (_hr == S_FALSE) 
	{ 
		COMPTR_RELEASE(this->_texture_buffer);
	}

	return _hr;

}

void w_texture_2D::load_resource(_In_ ComPtr<ID3D11Resource> resource, _In_ ComPtr<ID3D11ShaderResourceView> shaderResourceView)
{
	if (shaderResourceView == nullptr || resource == nullptr) return;

	//Copy to local srv
	this->_srv = shaderResourceView;
	//copy resource to texture
	auto hr = resource.As(&this->_texture_buffer);
	V(hr, L"copying resource to texture2D", this->name, 2, false, true);
	this->_texture_buffer->GetDesc(&this->_description);
}

ULONG w_texture_2D::release()
{
	if (_super::is_released()) return 0;

	COMPTR_RELEASE(this->_srv);
	COMPTR_RELEASE(this->_texture_buffer);
	
	return _super::release();
}

#pragma region Getters

ID3D11Texture2D* w_texture_2D::get_texture_2D() const
{
	return this->_texture_buffer.Get();
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

D3D11_TEXTURE2D_DESC w_texture_2D::get_description() const
{
	return this->_description;
}

#pragma endregion

#pragma region static methods

HRESULT w_texture_2D::capture_screen(_In_ ID3D11DeviceContext* pContext, _In_ ID3D11Resource* pSource, _In_z_ LPCWSTR pFileName)
{
	return DirectX::SaveDDSTextureToFile(pContext, pSource, pFileName);
}

HRESULT w_texture_2D::load_texture_2D_from_file(_In_ ID3D11Device1* pDevice,
	_Inout_ std::unique_ptr<w_texture_2D>& pTexture,
	_In_ wstring pPath,
	_In_ bool pIsAbsolutePath,
	_In_ bool pEnableTextureMemoryUsage2ChannelEncoding,
	_In_ glm::i32vec3 pRGBWeights,
	_In_ size_t pMaxSize,
	_In_ DXGI_FORMAT pFormat,
	_In_ UINT pBindFlags)
{
	using namespace std;
	using namespace system::io;

	auto _path = ( pIsAbsolutePath ? L"" : get_content_directoryW() ) + pPath;
	auto _ext = wolf::system::io::get_file_extentionW(_path.c_str());
	std::transform(_ext.begin(), _ext.end(), _ext.begin(), ::tolower);

	if (_ext == L".dds")
	{
		std::unique_ptr<uint8_t[]> data;
		size_t dataSize;
		int _file_status = -1;
		auto _hr = system::io::read_binary_file(_path.c_str(), data, &dataSize, _file_status);
		if (_hr == S_FALSE)
		{
			wstring msg = L"";
			if (_file_status == -1)
			{
				msg = L"Could not find the texture file: ";
			}
			else
			{
				msg = L"Texture file is corrupted: ";
			}
			V(S_FALSE, msg + pPath, "Texture2D", 2, false, false);

			return _hr;
		}

		Microsoft::WRL::ComPtr<ID3D11Resource> _resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _srv = nullptr;

		DirectX::CreateDDSTextureFromMemory(
			pDevice,
			data.get(),
			dataSize,
			&_resource,
			&_srv,
			pMaxSize,
			pFormat,
			pBindFlags);

		if (!pTexture)
		{
			pTexture = std::make_unique<w_texture_2D>();
		}
		pTexture->load_resource(_resource, _srv);
	}
	else if (_ext == L".jpg" || _ext == L".bmp" || _ext == L".png" || _ext == L".psd" || _ext == L".tga")
	{
		if (!wolf::system::io::get_is_file(_path.c_str()))
		{
			V(S_FALSE, L"could not find the file: " + pPath, "Texture2D", 2, false, false);
			return S_FALSE;
		}
		else
		{
			auto __path = wolf::system::convert::to_utf8(_path);

			int _width, _height, _comp;
			unsigned char* _rgba = stbi_load(__path.c_str(), &_width, &_height, &_comp, STBI_rgb_alpha);
			if (_rgba)
			{
				if (!pTexture)
				{
					pTexture = std::make_unique<w_texture_2D>();
				}

				const size_t _mem_pitch = _width * 4; // 4 bytes of ARGB
				const size_t _slice_pitch = _mem_pitch * _height;

				D3D11_SUBRESOURCE_DATA _data;
				std::memset(&_data, 0, sizeof(_data));

				_data.pSysMem = (void*)_rgba;
				_data.SysMemPitch = static_cast<UINT>(_mem_pitch);
				_data.SysMemSlicePitch = static_cast<UINT>(_slice_pitch);

				pTexture->create(pDevice,
					_width, _height,
					D3D11_USAGE::D3D11_USAGE_DEFAULT,
					0,
					pFormat,
					pBindFlags,
					&_data);
				stbi_image_free(_rgba);
			}
			else
			{
				V(S_FALSE, L"Texture file is corrupted: " + pPath, "Texture2D", 2, false, false);
				return S_FALSE;
			}
		}
	}
	else if (_ext == L".gif")
	{
		throw std::runtime_error("gif not implemented yet");
	}
	else
	{
		auto _str_path = std::string(_path.begin(), _path.end());
		throw std::runtime_error(_str_path + "not supported");
	}
	

	//enable encoding
	if (pEnableTextureMemoryUsage2ChannelEncoding)
	{
		w_texture_2D* _encoded_texture = nullptr;
		encode_texture_2D(pDevice, pRGBWeights, pTexture, &_encoded_texture);

		if (_encoded_texture)
		{
			//release old texture and swap with the new encoded one
			UNIQUE_RELEASE(pTexture);
			pTexture.reset(_encoded_texture);
		}

	}

	return S_OK;
}

//Create a sampler
void w_texture_2D::create_sampler(const std::shared_ptr<w_graphics_device>& pGDevice,
	const D3D11_FILTER pFilter,
	const D3D11_TEXTURE_ADDRESS_MODE pAddress[3])
{
	// Once the texture view is created, create a sampler.  This defines how the color
	// for a particular texture coordinate is determined using the relevant texture data.
	D3D11_SAMPLER_DESC _sampler_desc;
	std::memset(&_sampler_desc, 0, sizeof(_sampler_desc));

	_sampler_desc.Filter = pFilter;

	// The sampler does not use anisotropic filtering, so this parameter is ignored.
	_sampler_desc.MaxAnisotropy = 0;

	// Specify how texture coordinates outside of the range 0..1 are resolved.
	_sampler_desc.AddressU = pAddress[0];
	_sampler_desc.AddressV = pAddress[1];
	_sampler_desc.AddressW = pAddress[2];

	// Use no special MIP clamping or bias.
	_sampler_desc.MipLODBias = 0.0f;
	_sampler_desc.MinLOD = 0;
	_sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	// Don't use a comparison function.
	_sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	// Border address mode is not used, so this parameter is ignored.
	_sampler_desc.BorderColor[0] = 0.0f;
	_sampler_desc.BorderColor[1] = 0.0f;
	_sampler_desc.BorderColor[2] = 0.0f;
	_sampler_desc.BorderColor[3] = 0.0f;

	ID3D11SamplerState* _sampler = nullptr;
	{
		auto _hr = pGDevice->device->CreateSamplerState(&_sampler_desc, &_sampler);
		V(_hr, L"Error on create sampler for texture2D", "Texture2D", 2, false, true);

		w_graphics_device_manager::samplers.push_back(_sampler);
	}
}

HRESULT w_texture_2D::encode_texture_2D(_In_ ID3D11Device1* pDevice, 
	_In_ glm::i32vec3 pRGBWeights,
	_Inout_ std::unique_ptr<w_texture_2D>& pSrcTexture,
	_Inout_ w_texture_2D** pEncodedTexture)
{
	if (!pSrcTexture) return S_FALSE;
	
	auto _src_desc = pSrcTexture->get_description();

	//first create texture 2D with cpu access texture
	D3D11_TEXTURE2D_DESC _desc;
	std::memset(&_desc, 0, sizeof(_desc));

	_desc.Height = _src_desc.Width;
	_desc.Width = _src_desc.Height;
	_desc.MipLevels = 1;
	_desc.ArraySize = 1;
	_desc.Format = _src_desc.Format;
	_desc.SampleDesc.Count = 1;
	_desc.SampleDesc.Quality = 0;
	_desc.Usage = D3D11_USAGE_STAGING;
	_desc.BindFlags = 0;
	_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	_desc.MiscFlags = 0;

	ID3D11Texture2D* _texture_2d = nullptr;
	auto _hr = pDevice->CreateTexture2D(&_desc, NULL, &_texture_2d);
	if (_hr == S_FALSE)
	{
		V(S_FALSE, "creating texture 2D for reading from CPU while encoding texture 2d", "w_texture_2d", 3, false, true);
		return S_FALSE;
	}

#pragma region Read source texture pixels

	const size_t _mem_pitch = _desc.Width * 4; // 4 bytes of ARGB
	const size_t _slice_pitch = _mem_pitch * _desc.Height;

	auto _pixels = new UINT[_slice_pitch];

	//copy to source texture 2d to the pixels
	ID3D11DeviceContext* _context = nullptr;
	pDevice->GetImmediateContext(&_context);
	if (!_context)
	{
		V(S_FALSE, "getting immediate context", "w_texture_2d", 3, false, true);
		return S_FALSE;
	}
	_context->CopyResource(_texture_2d, pSrcTexture->get_resource());
	D3D11_MAPPED_SUBRESOURCE _map_sub;
	std::memset(&_map_sub, 0, sizeof(_map_sub));
	_context->Map(_texture_2d, 0, D3D11_MAP_READ, 0, &_map_sub);
	{
		auto _data = static_cast<UINT*>(_map_sub.pData);
		if (!_data)
		{
			_context->Unmap(_texture_2d, 0);
			V(S_FALSE, "E_POINTER on getting mapped pixels", "w_texture_2d", 3, false, true);
			return S_FALSE;
		}
		
		std::memcpy(_pixels, _data, _slice_pitch);

		_context->Unmap(_texture_2d, 0);
		_data = nullptr;
	}

#pragma endregion

	auto _encoded_pixels = new UINT[_slice_pitch];
	glm::vec3 _2_channel_encoding_base_a;
	glm::vec3 _2_channel_encoding_base_b;
	_encode_texture(_pixels, _desc, pRGBWeights, _2_channel_encoding_base_a, _2_channel_encoding_base_b, &_encoded_pixels);

	if (pEncodedTexture == nullptr || *pEncodedTexture == nullptr)
	{
		*pEncodedTexture = new w_texture_2D();
	}
	D3D11_SUBRESOURCE_DATA _init_data;
	std::memset(&_init_data, 0, sizeof(_init_data));

	_init_data.pSysMem = (void*)_encoded_pixels;
	_init_data.SysMemPitch = static_cast<UINT>(_mem_pitch);
	_init_data.SysMemSlicePitch = static_cast<UINT>(_slice_pitch);

	//create encoded texture
	(*pEncodedTexture)->create(pDevice,
		_src_desc.Width,
		_src_desc.Height,
		_src_desc.Usage,
		_src_desc.CPUAccessFlags,
		_src_desc.Format,
		_src_desc.BindFlags,
		&_init_data);
	(*pEncodedTexture)->set_encode_bases(_2_channel_encoding_base_a, _2_channel_encoding_base_b);

	//release memory
	COM_RELEASE(_texture_2d);
	delete[] _pixels;
	delete[] _encoded_pixels;
	_context = nullptr;
	
	return S_OK;
}

void w_texture_2D::_encode_texture(_In_ UINT* pSrc, _In_ D3D11_TEXTURE2D_DESC pTextureDescription,
	_In_ glm::i32vec3 pRGBWeights, _Inout_ glm::vec3& pBaseA, _Inout_ glm::vec3& pBaseB, _Inout_ UINT** pEncodedPixels)
{
	auto n = _estimate_image(pSrc, pTextureDescription, pRGBWeights);
	_stamp_color_probe(pSrc, pTextureDescription);
	_encode_pixels(pSrc, n, pTextureDescription, pBaseA, pBaseB, pEncodedPixels);
}

HRESULT w_texture_2D::decode_texture_2D(_In_ ID3D11Device1* pDevice,
	_In_ glm::vec3 pBaseA,
	_In_ glm::vec3 pBaseB,
	_In_ w_texture_2D* pEncodedTexture,
	_Inout_ w_texture_2D** pDecodedTexture)
{
	if (!pEncodedTexture) return S_FALSE;
	
	auto _src_desc = pEncodedTexture->get_description();

	const size_t _mem_pitch = _src_desc.Width * 4; // 4 bytes of ARGB
	const size_t _slice_pitch = _mem_pitch * _src_desc.Height;

	//first create a texture 2D with cpu access texture
	D3D11_TEXTURE2D_DESC _desc;
	std::memset(&_desc, 0, sizeof(_desc));

	_desc.Height = _src_desc.Width;
	_desc.Width = _src_desc.Height;
	_desc.MipLevels = 1;
	_desc.ArraySize = 1;
	_desc.Format = _src_desc.Format;
	_desc.SampleDesc.Count = 1;
	_desc.SampleDesc.Quality = 0;
	_desc.Usage = D3D11_USAGE_STAGING;
	_desc.BindFlags = 0;
	_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	_desc.MiscFlags = 0;

	//read pixels of encoded texture
	ID3D11Texture2D* _texture_2d = nullptr;
	auto _hr = pDevice->CreateTexture2D(&_desc, NULL, &_texture_2d);
	if (_hr == S_FALSE)
	{
		V(S_FALSE, "creating texture 2D for reading from CPU while encoding texture 2d", "w_texture_2d", 3, false, true);
		return S_FALSE;
	}

	//get context
	ID3D11DeviceContext* _context = nullptr;
	pDevice->GetImmediateContext(&_context);
	if (!_context)
	{
		V(S_FALSE, "getting immediate context", "w_texture_2d", 3, false, true);
		return S_FALSE;
	}

	//copy encoded texture 2d to the pixels array
	auto _pixels = new UINT[_slice_pitch];
	
	_context->CopyResource(_texture_2d, pEncodedTexture->get_resource());
	
	D3D11_MAPPED_SUBRESOURCE _map_sub;
	std::memset(&_map_sub, 0, sizeof(_map_sub));
	
	_context->Map(_texture_2d, 0, D3D11_MAP_READ, 0, &_map_sub);
	{
		auto _data = static_cast<UINT*>(_map_sub.pData);
		if (!_data)
		{
			_context->Unmap(_texture_2d, 0);
			V(S_FALSE, "E_POINTER on getting mapped pixels", "", 3, false, true);
			return S_FALSE;
		}
		std::memcpy(_pixels, _data, _slice_pitch);
		_context->Unmap(_texture_2d, 0);
		_data = nullptr;
	}

	for (size_t i = 0; i < _desc.Width; ++i)
	{
		for (size_t j = 0; j < _desc.Height; ++j)
		{
			auto _index = j * _desc.Width + i;
			auto _pixel = w_color::from_hex(_pixels[_index]);

			float _hue = _pixel.r / 255.f;
			float _lum = _pixel.g / 255.f;

			// Decode linear luminance - apply gamma 2.0.
			_lum *= _lum;

			// Decode hue color.
			auto _color = pBaseA + (pBaseB - pBaseA) * _hue;

			// Compute luminance of interpolated hue.
			float _clum = 0.2126f * _color.x + 0.7152f * _color.y + 0.0722f * _color.z;

			// Adjust luminance to match desired value.
			_color *= _lum / (_clum + 0.00000001f);

			// Return to original sRGB space.
			//	All sRGB-linear conversions use approximage gamma 2.0,
			//	so all "linear" operations were carried out in not exactly linear space.
			//	Using exact simplifications to gamma 2.0 everywhere make sure that we
			//	return to correct original sRGB space.
			_color.x = sqrt(_color.x);
			_color.y = sqrt(_color.y);
			_color.z = sqrt(_color.z);

			//Output color
			_color *= 255;

			_color.x = int(floor(_color.x + 0.5f));
			_color.y = int(floor(_color.y + 0.5f));
			_color.z = int(floor(_color.z + 0.5f));

			if (_color.x < 0) _color.x = 0;
			if (_color.x > 255) _color.x = 255;

			if (_color.y < 0) _color.y = 0;
			if (_color.y > 255) _color.y = 255;

			if (_color.z < 0) _color.z = 0;
			if (_color.z > 255) _color.z = 255;

			_pixels[_index] = w_color::to_hex(_color.x, _color.y, _color.z, 255);
		}
	}

	//now create decode texture
	if (pDecodedTexture == nullptr || *pDecodedTexture == nullptr)
	{
		*pDecodedTexture = new w_texture_2D();
	}
	D3D11_SUBRESOURCE_DATA _init_data;
	std::memset(&_init_data, 0, sizeof(_init_data));

	_init_data.pSysMem = (void*)_pixels;
	_init_data.SysMemPitch = static_cast<UINT>(_mem_pitch);
	_init_data.SysMemSlicePitch = static_cast<UINT>(_slice_pitch);

	//create decoded texture
	(*pDecodedTexture)->create(pDevice,
		_src_desc.Width,
		_src_desc.Height,
		_src_desc.Usage,
		_src_desc.CPUAccessFlags,
		_src_desc.Format,
		_src_desc.BindFlags,
		&_init_data);
	(*pDecodedTexture)->set_encode_bases(pBaseA, pBaseB);

	//release memory
	COM_RELEASE(_texture_2d);
	delete[] _pixels;
	_context = nullptr;

	return S_OK;
}

int w_texture_2D::compare(_In_ ID3D11Device1* pDevice,
	_In_ w_texture_2D* pTextureA,
	_In_ w_texture_2D* pTextureB,
	_In_ UINT pEpsilon)
{
	auto _a_desc = pTextureA->get_description();
	auto _b_desc = pTextureB->get_description();

	if (_a_desc.Width != _b_desc.Width ||
		_a_desc.Height != _b_desc.Height || 
		_a_desc.Format != _b_desc.Format)
	{
		logger.write("size and format of two texture must be equal");
		return -1;
	}

	//get pixels of pTextureA
	D3D11_TEXTURE2D_DESC _desc;
	std::memset(&_desc, 0, sizeof(_desc));

	_desc.Height = _a_desc.Width;
	_desc.Width = _a_desc.Height;
	_desc.MipLevels = 1;
	_desc.ArraySize = 1;
	_desc.Format = _a_desc.Format;
	_desc.SampleDesc.Count = 1;
	_desc.SampleDesc.Quality = 0;
	_desc.Usage = D3D11_USAGE_STAGING;
	_desc.BindFlags = 0;
	_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	_desc.MiscFlags = 0;

	//read pixels of encoded texture
	ID3D11Texture2D* _texture_2d = nullptr;
	auto _hr = pDevice->CreateTexture2D(&_desc, NULL, &_texture_2d);
	if (_hr == S_FALSE)
	{
		V(S_FALSE, "creating texture 2D for reading from CPU while encoding texture 2d", "w_texture_2d", 3, false, true);
		return S_FALSE;
	}

	//get context
	ID3D11DeviceContext* _context = nullptr;
	pDevice->GetImmediateContext(&_context);
	if (!_context)
	{
		V(S_FALSE, "getting immediate context", "w_texture_2d", 3, false, true);
		return S_FALSE;
	}

	const size_t _mem_pitch = _desc.Width * 4; // 4 bytes of ARGB
	const size_t _slice_pitch = _mem_pitch * _desc.Height;

	//copy pTextureA to the pixels array
	auto _a_pixels = new UINT[_slice_pitch];

	_context->CopyResource(_texture_2d, pTextureA->get_resource());

	D3D11_MAPPED_SUBRESOURCE _map_sub;
	std::memset(&_map_sub, 0, sizeof(_map_sub));

	_context->Map(_texture_2d, 0, D3D11_MAP_READ, 0, &_map_sub);
	{
		auto _data = static_cast<UINT*>(_map_sub.pData);
		if (!_data)
		{
			_context->Unmap(_texture_2d, 0);
			V(S_FALSE, "E_POINTER on getting mapped pixels", "", 3, false, true);
			return S_FALSE;
		}
		std::memcpy(_a_pixels, _data, _slice_pitch);
		_context->Unmap(_texture_2d, 0);
		_data = nullptr;
	}

	//get pixels of pTextureB
	auto _b_pixels = new UINT[_slice_pitch];

	_context->CopyResource(_texture_2d, pTextureB->get_resource());

	std::memset(&_map_sub, 0, sizeof(_map_sub));
	_context->Map(_texture_2d, 0, D3D11_MAP_READ, 0, &_map_sub);
	{
		auto _data = static_cast<UINT*>(_map_sub.pData);
		if (!_data)
		{
			_context->Unmap(_texture_2d, 0);
			V(S_FALSE, "E_POINTER on getting mapped pixels", "", 3, false, true);
			return S_FALSE;
		}
		std::memcpy(_b_pixels, _data, _slice_pitch);
		_context->Unmap(_texture_2d, 0);
		_data = nullptr;
	}

	//let's compare two pixels
	int _is_equal = 1;
	for (size_t i = 0; i < _slice_pitch; i++)
	{
		if (std::abs((int)_a_pixels[i] - (int)_b_pixels[i]) > pEpsilon)
		{
			_is_equal = 0;
			break;
		}
	}


	COM_RELEASE(_texture_2d);
	delete[] _a_pixels;
	delete[] _b_pixels;
	_context = nullptr;

	return _is_equal;
}

glm::vec3 w_texture_2D::_estimate_image(_In_ UINT* pSrc, _In_ D3D11_TEXTURE2D_DESC pTextureDescription, _In_ glm::i32vec3 pRGBWeights)
{
	// Accumulators.
	__int64 _rr = 0;
	__int64 _gg = 0;
	__int64 _bb = 0;
	__int64 _rg = 0;
	__int64 _rb = 0;
	__int64 _gb = 0;

	// Process pixels.
	for (size_t i = 0; i < pTextureDescription.Width; ++i)
	{
		for (size_t j = 0; j < pTextureDescription.Height; ++j)
		{
			auto _argb = pSrc[j * pTextureDescription.Width + i];

			// Extract ARGB
			auto pixel = w_color::from_hex(_argb);

			// Apply weights and gamma 2.0.
			pixel.r *= pixel.r * pRGBWeights.r;
			pixel.g *= pixel.g * pRGBWeights.g;
			pixel.b *= pixel.b * pRGBWeights.b;

			// Accumulate.
			_rr += __int64(pixel.r) * pixel.r;
			_gg += __int64(pixel.g) * pixel.g;
			_bb += __int64(pixel.b) * pixel.b;
			_rg += __int64(pixel.r) * pixel.g;
			_rb += __int64(pixel.r) * pixel.b;
			_gb += __int64(pixel.g) * pixel.b;

		}
	}


	// Compute averagesfrom the accumulators.
	int pix = pTextureDescription.Width * pTextureDescription.Height;
	float rr = float(_rr) / pix;
	float gg = float(_gg) / pix;
	float bb = float(_bb) / pix;
	float rg = float(_rg) / pix;
	float rb = float(_rb) / pix;
	float gb = float(_gb) / pix;


	// Brute force normal fitting.
	auto best_n = glm::vec3(0.0f, 0.0f, 0.0f);
	float best_E = 1e18;

	for (int z = -100; z <= 100; z++)
	{
		for (int y = -100; y <= 100; y++)
		{
			for (int x = -100; x <= 100; x++)
			{
				if (!(x | y | z)) continue;

				glm::vec3 n(x, y, z);
				n = glm::normalize(n);

				// Compute total sum of squared error values.
				float E =
					n.x * n.x * rr +
					n.y * n.y * gg +
					n.z * n.z * bb +
					2 * n.x * n.y * rg +
					2 * n.x * n.z * rb +
					2 * n.y * n.z * gb;

				if (E < best_E)
				{
					// Apply weights to normal to negate their influence.
					n.x *= pRGBWeights.r;
					n.y *= pRGBWeights.g;
					n.z *= pRGBWeights.b;

					n = glm::normalize(n);

					// Update best normal.
					best_E = E;
					best_n = n;
				}
			}
		}
	}

	return best_n;
}

void w_texture_2D::_stamp_color_probe(_Inout_ UINT* pSrc, _In_ D3D11_TEXTURE2D_DESC pTextureDescription)
{
	if (pTextureDescription.Width <64 || pTextureDescription.Height <64) return;

	for (int y = 0; y < 64; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			auto _p = glm::vec2(x - 31.5f, y - 31.5f);
			auto _length = _p.length();

			_p *= 1.f / 31;
			
			float _a = (1 - _length) * 27;
			float _blk = 1 - (1 - _length) * 15;

			_blk *= 2;
			if (_blk < 0) _blk = 0;
			if (_blk > 1) _blk = 1;
			
			if (_a > 1) _a = 1;
			if (_a <= 0) continue;
			
			_a = 1 - _a;
			_blk = 1 - _blk;

			float _hue = atan2(_p.x, -_p.y) * (1.f / 2 / glm::pi<float>()) * 3;
			float _s = (1 - _length);
			if (_s < 0) _s = 0;

			_p = glm::normalize(_p);

			float r = _xcos(_hue);
			float g = _xcos(_hue - 1);
			float b = _xcos(_hue - 2);
			float m = std::max(r, g);

			m = std::max(m, b) / 255.f;
			r /= m;
			g /= m;
			b /= m;
			r += (255 - r) * _s;
			g += (255 - g) * _s;
			b += (255 - b) * _s;
			r *= _blk;
			g *= _blk;
			b *= _blk;

			auto _pixel_index = (y + pTextureDescription.Height - 64) * pTextureDescription.Width + x + pTextureDescription.Width - 64;
			
			auto _argb = pSrc[_pixel_index];
			auto _pixel = w_color::from_hex(_argb);

			r += (_pixel.r - _pixel.r) * _a;
			g += (_pixel.r - _pixel.g) * _a;
			b += (_pixel.r - _pixel.b) * _a;

			pSrc[_pixel_index] = w_color::to_hex( r, g, b, _pixel.a);
		}
	}
}

void w_texture_2D::_encode_pixels(_In_ UINT* pSrc, _In_ glm::vec3 n, _In_ D3D11_TEXTURE2D_DESC pTextureDescription,
	_Inout_ glm::vec3& pBaseA, _Inout_ glm::vec3& pBaseB, _Inout_ UINT** pEncodedPixels)
{
	// Normalize the normal and find base colors.
	n = glm::normalize(n);

	_find_components(n, pBaseA, pBaseB);
	// Recompute normal in case it was degenerate.
	n = glm::normalize(glm::cross(pBaseA, pBaseB));

	auto _to_scale = glm::vec3(0.2126f, 0.7152f, 0.0722f);

	// Find 2D coordinate frame on the plane.
	//	frame_x is aligned with base_a.
	//	frame_y is perpendicular to frame_x and the normal.
	auto _frame_x = glm::normalize(pBaseA);
	auto _frame_y = glm::normalize(glm::cross(_frame_x, n));

	// fa, fb - base colors as 2D coordinates in plane coordinate frame.
	auto _fa = glm::vec2(glm::dot(_frame_x, pBaseA), glm::dot(_frame_y, pBaseA));
	auto _fb = glm::vec2(glm::dot(_frame_x, pBaseB), glm::dot(_frame_y, pBaseB));

	// Process.
	for (size_t i = 0; i < pTextureDescription.Width; ++i)
	{
		for (size_t j = 0; j < pTextureDescription.Height; ++j)
		{
			auto _index = j * pTextureDescription.Width + i;
			auto _pixel = w_color::from_hex(pSrc[_index]);

			auto _c = glm::vec3(_pixel.r, _pixel.g, _pixel.b);
			_c *= 1.f / 255;
			// Apply gamma 2.0.
			_c *= _c;

			// Compute color XY coordinates in 2D frame.
			auto _cf = glm::vec2(glm::dot(_frame_x, _c), glm::dot(_frame_y, _c));

			// "cf" was direction vector; rotate it by 90 deg to make it 2D line normal. -> (y, -x)
			auto _temp = _cf.x;
			_cf.x = _cf.y;
			_cf.y = -_temp;

			// Compute relative distances of base colors to the line.
			float _da = glm::dot(_fa, _cf);
			float _db = glm::dot(_fb, _cf);

			// Compute hue (blend factor between base colors).
			float _hue = -_da / (_db - _da + 0.00000001f);
			if (_hue < 0) _hue = 0;
			if (_hue > 1) _hue = 1;

			// Compute real luminance and move to gamma 2.0 space.
			float _lum = glm::dot(_c, _to_scale);
			_lum = sqrt(_lum);

			// Encode data into 2 channels.
			//	R - hue
			//	G - luminace
			int _h = int(floor(_hue * 255 + 0.5f));
			int _l = int(floor(_lum * 255 + 0.5f));

			if (_h < 0) _h = 0;
			if (_h > 255) _h = 255;

			if (_l < 0) _l = 0;
			if (_l > 255) _l = 255;

			auto _ptr = *pEncodedPixels;
			_ptr[_index] = w_color::to_hex(_h, _l, 0, 255);//rgba
		}
	}
}

float w_texture_2D::_xcos(float f)
{
	f = fmod(f + 4.5f, 3) - 1.5f;
	if (f < -1) return 0;
	if (f > 1) return 0;

	f = 2 * fabs(f);
	if (f < 1) return 1;

	f = 2 - f;
	return (3 - 2 * f) * f * f;
}

//Computes two points where plane with normal "n" crosses unit cube silhouette. Returned as "ca" and "cb".
void w_texture_2D::_find_components(_In_ const glm::vec3& n, _Inout_ glm::vec3& ca, _Inout_ glm::vec3& cb)
{
	// PTS - unit cube silhouette, as seen from origin.
	static const glm::vec3 PTS[] = {
		glm::vec3(1, 0, 0),
		glm::vec3(1, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(1, 0, 1)
	};

	// Initialize to safe base colors (white and green) in case we get degenerate normal.
	ca = glm::vec3(1, 1, 1);
	cb = glm::vec3(0, 1, 0);

	// Search for intersections.
	glm::vec3 tmp;
	for (int i = 0; i < 6; i++)
	{
		if (_cross_test(PTS[i], PTS[(i + 1) % 6], n, tmp))
		{
			cb = ca, ca = tmp;
		}
	}
}

/*
	Segment-plane intersection.
	p1, p2	- segment endpoints (p1-inclusive, p2-exclusive)
	n		- plane normal (plane always crosses origin)

	Returns true if intersection is found and returned via "out".
*/
bool w_texture_2D::_cross_test(_Inout_ const glm::vec3& p1, _Inout_ const glm::vec3& p2, _Inout_ const glm::vec3& n, _Inout_ glm::vec3& out)
{
	float d1 = glm::dot(p1, n);
	float d2 = glm::dot(p2, n);

	if ((d1 <= 0 && d2 > 0) || (d1 >= 0 && d2 < 0))
	{
		float p = -d1 / (d2 - d1);
		out = p1 + (p2 - p1)*p;

		float m = max(out.x, out.y);
		m = max(m, out.z);
		out *= 1.f / m;

		return true;
	}

	return false;
}

#pragma endregion