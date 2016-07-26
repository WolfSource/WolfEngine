#include "w_directX_pch.h"
#include "w_graphics_device_manager.h"
#include "w_graphics\w_textures\w_texture_2D.h"

#if defined(__WINRT)
#include "windows.ui.xaml.media.dxinterop.h"
#endif

using namespace std;
using namespace wolf::system;
using namespace wolf::graphics;
using namespace DirectX;
using namespace Microsoft::WRL;

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
const unsigned int bytesPerPixel = 4;

std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>>	w_graphics_device_manager::samplers;
std::unique_ptr<concurrency::accelerator_view>  w_graphics_device_manager::cAmpAcc;

#if  defined(_DEBUG) && defined(__DX_DIAGNOSTIC__)
	ComPtr<IDXGraphicsAnalysis> w_graphics_device_manager::graphics_diagnostic;
#endif

#pragma region w_graphics_device_manager

w_graphics_device_manager::w_graphics_device_manager(bool pUse_Wrap_Mode) : _use_warp_mode(pUse_Wrap_Mode), _cpp_AMP_Device_Type(CPP_AMP_DEVICE_TYPE::GPU)
{
	_super::set_class_name(typeid(this).name());

	this->_backColor[0] = 0.149f; this->_backColor[1] = 0.149f; this->_backColor[2] = 0.149f; this->_backColor[3] = 1.0f;

	// Enable the D3D12 debug layer.
#if defined(__DX12__)

#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif

#endif

	//list all information of graphics devices
	_enumerate_devices();
}

w_graphics_device_manager::~w_graphics_device_manager()
{
	release();
}

void w_graphics_device_manager::initialize()
{
	//If there is no assiociated graphics device
	if (this->_graphics_devices.size() == 0)
	{
		V(S_FALSE, L"getting graphics device. No device available", this->name, 3, true, false);
	}

	_create_device();

#pragma region Create TextureSampler

	//Load default sampler
	const D3D11_TEXTURE_ADDRESS_MODE _AddressUVW[3] =
	{
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP
	};
	w_texture_2D::create_sampler(
		this->_graphics_devices.at(0),
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		_AddressUVW);

#pragma endregion

#pragma region Cpp Amp

	if (this->_cpp_AMP_Device_Type == CPP_AMP_DEVICE_TYPE::CPU)
	{
		//Create C++ Amp Accelerator from Windows CPU
		auto _cpuAcc = concurrency::accelerator(concurrency::accelerator::cpu_accelerator);
		cAmpAcc = std::make_unique<concurrency::accelerator_view>(_cpuAcc.default_view);
	}
	else if (this->_cpp_AMP_Device_Type == CPP_AMP_DEVICE_TYPE::GPU_REF)
	{
		//Create C++ Amp Accelerator from GPU reference
		auto _refAcc = concurrency::accelerator(concurrency::accelerator::direct3d_ref);
		cAmpAcc = std::make_unique<concurrency::accelerator_view>(_refAcc.default_view);
	}
	else if (this->_cpp_AMP_Device_Type == CPP_AMP_DEVICE_TYPE::GPU_WARP)
	{
		//Create C++ Amp Accelerator from Windows Advanced Rasterization Platform(WARP)
		auto _warpAcc = concurrency::accelerator(concurrency::accelerator::direct3d_warp);
		cAmpAcc = std::make_unique<concurrency::accelerator_view>(_warpAcc.default_view);
	}
	else if(this->_cpp_AMP_Device_Type == CPP_AMP_DEVICE_TYPE::GPU)
	{
		//Create C++ Amp Accelerator from first DirectX device
		auto _d3dDevice = this->_graphics_devices.at(0)->device.Get();
		cAmpAcc = std::make_unique<concurrency::accelerator_view>(concurrency::direct3d::create_accelerator_view(reinterpret_cast<IUnknown *>(_d3dDevice)));
	}
	else
	{
		//Default
		auto _defaultAcc = concurrency::accelerator(concurrency::accelerator::default_accelerator);
		cAmpAcc = std::make_unique<concurrency::accelerator_view>(_defaultAcc.default_view);
	}

	logger.write(L"C++ AMP created with following device : " + cAmpAcc->accelerator.description);

#pragma endregion

#if defined(_DEBUG) && defined(__DX_DIAGNOSTIC__)
	auto _hr = DXGIGetDebugInterface1(0, __uuidof(graphics_diagnostic), reinterpret_cast<void**>(graphics_diagnostic.GetAddressOf()));
	if (FAILED(_hr))
	{
		logger.error(L"Application could not use graphics diagnostic debug interface, capturing frame during running application might abort application");
	}
#endif
}

void w_graphics_device_manager::_enumerate_devices()
{
	auto _hr = S_FALSE;

#if defined(__DX12__)

	ComPtr<IDXGIFactory4> _dxgiFactory;
	_hr = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
	V(_hr, L"Error on creating dxgi factory4", this->name, 3, true, true);

#elif defined(__DX11_X__)

	ComPtr<IDXGIFactory1> _dxgiFactory;

	//Obtain DXGI factory from device
	_hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**) (&_dxgiFactory));
	V(_hr, L"Error on creating dxgi factory1", this->name, 3, true, true);

	_hr = _dxgiFactory.Get()->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(this->_dxgi_factory.GetAddressOf()));
	V(_hr, L"Error on creating dxgi factory2, Wolf engine is optimized for DirectX 11.1 or later version which is not supported on this device", this->name, 3, true, true);

#endif

	//release local dxgi factory
	COMPTR_RELEASE(_dxgiFactory);

	logger.write(L"++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
	logger.write(L"++++++++++++++++ Graphics devices Info +++++++++++++++++");

	//Iterate for all adaptors
	ComPtr<IDXGIAdapter> _adapter;

	for (int i = 0; this->_dxgi_factory->EnumAdapters(i, &_adapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC _adapter_desc = { 0 };
		auto _hr = _adapter->GetDesc(&_adapter_desc);
		if (_hr != S_OK)
		{
			V(S_FALSE, L"getting adaptors", this->name, 3, true, true);
		}

		logger.write(L"++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
		logger.write(L"DeviceId : " + std::to_wstring(_adapter_desc.DeviceId));
		logger.write(L"DedicatedSystemMemory : " + std::to_wstring(_adapter_desc.DedicatedSystemMemory));
		logger.write(L"DedicatedVideoMemory : " + std::to_wstring(_adapter_desc.DedicatedVideoMemory));
		logger.write(L"VendorId : " + std::to_wstring(_adapter_desc.VendorId));
		logger.write(L"SubSysId : " + std::to_wstring(_adapter_desc.SubSysId));
		logger.write(L"++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

		/*
		Acording to MSDN, starting with Windows 8, an adapter called the "Microsoft Basic Render Driver"
		is always present.This adapter has a VendorId of 0x1414 and a DeviceID of 0x8c.
		Wolf will skip it.
		*/
		if (_adapter_desc.DeviceId == 140)
		{
			logger.write(L"Microsoft Basic Render Driver skipped");
			logger.write(L"++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
		}
		else
		{
			auto _gDevice = make_shared<w_graphics_device>();
			_gDevice->adaptor = _adapter;

			// Get number of dxgi outputs descrption for this graphic adaptor. How many outputs to monitors do we have for this adaptor
			ComPtr<IDXGIOutput> _dxgi_output;
			for (int i = 0; DXGI_ERROR_NOT_FOUND != _adapter->EnumOutputs(i, &_dxgi_output); ++i)
			{
				// get the description
				DXGI_OUTPUT_DESC _output_desc = { 0 };
				auto _hr = _dxgi_output->GetDesc(&_output_desc);
				if (_hr != S_OK)
				{
					V(S_FALSE, L"getting dxgi outputs", this->name, 3, true, true);
				}
				_gDevice->dxgi_outputs_desc.push_back(_output_desc);
			}

			this->_graphics_devices.push_back(_gDevice);

			COMPTR_RELEASE(_dxgi_output);
		}
	}
}

void w_graphics_device_manager::_create_device()
{
	auto _hr = CoInitialize(NULL);
	V(_hr, L"CoInitialize", _super::name, 3, true, false);

#if defined(__DX12__)

	//minimum feature level
	D3D_FEATURE_LEVEL _feature_level = D3D_FEATURE_LEVEL_11_1;
	_hr = D3D12CreateDevice(
		_gDevice->adaptor.Get(),
		_feature_level,
		IID_PPV_ARGS(&_gDevice->device)
		);

	_gDevice->featureLevel = _feature_level;

#elif defined(__DX11_X__)

	//now create a d3dDevice & d3dContext for default graphics adaptor 

	//enable debug layer for directX 11.1
	UINT _creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	//For debugging
	//_creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	_creationFlags |= D3D11_CREATE_DEVICE_DEBUGGABLE;//Debugging shaders
#endif

	if (this->_graphics_devices.size() == 0)
	{
		V(S_FALSE, L"Unexpected error, no graphics device detected", this->name, 3, true, false);
	}

	auto _gDevice = this->_graphics_devices.at(0);

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;

	D3D_FEATURE_LEVEL _feature_level;
	D3D_FEATURE_LEVEL _feature_levels [] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	wstring _str = L"";
	if (this->_use_warp_mode)
	{
		_hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_WARP,
			nullptr,
			_creationFlags,
			_feature_levels,
			W_ARRAY_SIZE(_feature_levels),
			D3D11_SDK_VERSION,
			&_device,
			&_feature_level,
			&_context);

		V(_hr, L"creating graphics device", this->name, 3, true, true);
		_str = L"warp device";
	}
	else
	{
		/*
			Try to create a Hardware device, if the initialization fails, fall back to the WARP device.
			for more information about wrap mode read https://msdn.microsoft.com/en-us/library/gg615082.aspx
		*/
		D3D_DRIVER_TYPE  _driver_types [] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
		};

		int _driver_type_index = 0;
		for (_driver_type_index = 0; _driver_type_index < W_ARRAY_SIZE(_driver_types); ++_driver_type_index)
		{
			_hr = D3D11CreateDevice(
				nullptr,
				_driver_types[_driver_type_index],
				nullptr,
				_creationFlags,
				_feature_levels,
				W_ARRAY_SIZE(_feature_levels),
				D3D11_SDK_VERSION,
				&_device,
				&_feature_level,
				&_context);

			if (_hr == S_OK)
			{
				break;
			}
		}
		V(_hr, L"creating graphics device", this->name, 3, true, true);

		_str = _driver_type_index == 0 ? L"hardware device" : L"warp device";
	}

	//Log for feature level
	switch (_feature_level)
	{
	case D3D_FEATURE_LEVEL_11_1:
		logger.write(L"Device created with D3D_FEATURE_LEVEL_11_1 feature level on" + _str);
		break;
	case D3D_FEATURE_LEVEL_11_0:
		logger.write(L"Device created with D3D_FEATURE_LEVEL_11_0 feature level on" + _str);
		break;
	case D3D_FEATURE_LEVEL_10_1:
		logger.write(L"Device created with D3D_FEATURE_LEVEL_10_1 feature level on" + _str);
		break;
	case D3D_FEATURE_LEVEL_10_0:
		logger.write(L"Device created with D3D_FEATURE_LEVEL_10_0 feature level on" + _str);
		break;
	case D3D_FEATURE_LEVEL_9_3:
		logger.write(L"Device created with D3D_FEATURE_LEVEL_9_3 feature level on" + _str);
		break;
	case D3D_FEATURE_LEVEL_9_2:
		logger.write(L"Device created with D3D_FEATURE_LEVEL_9_2 feature level on" + _str);
		break;
	case D3D_FEATURE_LEVEL_9_1:
		logger.write(L"Device created with D3D_FEATURE_LEVEL_9_1 feature level on" + _str);
		break;
	}

	_gDevice->feature_level = _feature_level;

#endif

	ComPtr<IDXGIDevice1> _dxgiDevice;
	_hr = _device.As(&_dxgiDevice);//reinterpret_cast<void**>(&dxgiDevice)
	V(_hr, L"getting IDXGIDevice", this->name, 3, true);

#if defined(WINRT)
	hr = _dxgiDevice->SetMaximumFrameLatency(3);
	OnFailed(hr, L"Set maximum frame latency", false);
#endif

	//Get 3D device & 3D context
	_hr = _device.As(&_gDevice->device);
	V(_hr, L"getting device1", this->name, 3, true, true);
	if (_gDevice->device == nullptr)
	{
		auto _msg = L"Graphics Device1 is null";
		V(S_FALSE, _msg, this->name, 3, true, true);
	}

	_hr = _context.As(&_gDevice->context);
	V(_hr, L"getting context1", this->name, 3, true, true);
	if (_gDevice->context == nullptr)
	{
		auto _msg = L"Context1 is null";
		V(S_FALSE, _msg, this->name, 3, true, true);
	}

#pragma region Create States

	_gDevice->create_blend_state(
		TRUE, 
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, 
		D3D11_BLEND_ONE, D3D11_BLEND_ZERO, 
		D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_ADD, 
		0x0f, &_gDevice->_alpha_blend);

	//_gDevice->create_blend_state(D3D11_BLEND_ONE, D3D11_BLEND_ZERO, &_gDevice->_opaque);
	//_gDevice->create_blend_state(D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, &_gDevice->_alpha_blend);
	//_gDevice->create_blend_state(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE, &_gDevice->_additive);
	//_gDevice->create_blend_state(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, &_gDevice->_non_pre_multiplied);

#pragma endregion

#pragma region Create device2D and context2D

	D2D1_FACTORY_OPTIONS _factory2DOptions;
	ZeroMemory(&_factory2DOptions, sizeof(D2D1_FACTORY_OPTIONS));

#ifdef _DEBUG
	_factory2DOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	//Create Direct2D factory
	_hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), &_factory2DOptions, &_gDevice->factory_2D);//reinterpret_cast<void**>(&this->graphicsDevice->d2dFactory)
	V(_hr, L"creating Direct2D factory", this->name, 3, true, true);

	//Create DirectWrite factory
	_hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &_gDevice->write_factory);//reinterpret_cast<IUnknown**>(&this->graphicsDevice->writeFactory)
	V(_hr, L"creating write factory", this->name, 3, true, true);

	//Create image factory
	_hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_gDevice->image_factory));
	V(_hr, L"creating image factory", this->name, 3, true, true);

	//Get IDXGIDevice
	//_dxgiDevice = nullptr;
	//{
	//	_hr = _gDevice->device.As(&_dxgiDevice);//reinterpret_cast<void**>(&dxgiDevice)
	//	V(_hr, L"getting IDXGIDevice", this->name, 3, true, true);

		//Create 2D Device from 2D factory
	_hr = _gDevice->factory_2D->CreateDevice(_dxgiDevice.Get(), &_gDevice->device_2D);
	V(_hr, L"getting device2D", this->name, 3, true, true);

	//Create 2D context from 2D device
	_hr = _gDevice->device_2D->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &_gDevice->context_2D);
	V(_hr, L"creating context2D", this->name, 3, true, true);

	COMPTR_RELEASE(_dxgiDevice);


	//Check out how to access Direct3D resources directly without Direct2D
	_hr = _gDevice->factory_2D.Get()->QueryInterface(IID_PPV_ARGS(&_gDevice->d2D_multithread));
	V(_hr, L"Not supported Direct2D Multithreaded", this->name, 3, true, true);


#pragma endregion

#ifdef _DEBUG
	_gDevice->device.Get()->QueryInterface(__uuidof(ID3D11Debug), (void**) &_gDevice->d3d_debug_layer);
#endif

	COMPTR_RELEASE(_dxgiDevice);
	COMPTR_RELEASE(_context);
	COMPTR_RELEASE(_device);
}

void w_graphics_device_manager::on_device_lost()
{
	logger.write(L"Device is going to reset");
	
	initialize();
	initialize_output_windows(this->_windowsInfo);
	
	logger.write(L"Device reseted successfully");
}

void w_graphics_device_manager::initialize_output_windows(map<int, vector<W_WindowInfo>> pOutputWindowsInfo)
{
	//We need to store it for the first time, in order to reseting device in the future
	if (this->_windowsInfo.size() == 0)
	{
		this->_windowsInfo = pOutputWindowsInfo;
	}

	for (pair<int, vector<W_WindowInfo>> p : pOutputWindowsInfo)
	{
		//Get graphics devices & hwnds
		auto _gDevice = this->_graphics_devices.at(p.first);
		auto _windowInfo = p.second;

		w_output_window _window;
		for (size_t i = 0; i < _windowInfo.size(); ++i)
		{
			ZeroMemory(&_window, sizeof(w_output_window));

			auto wInfo = _windowInfo.at(i);
			
			_window.index = static_cast<int>(i);
			_window.hWnd = wInfo.hWnd;
			_window.width = wInfo.width;
			_window.height = wInfo.height;
			_window.aspectRatio = static_cast<float>(_window.width) / static_cast<float>(_window.height);

			if (_create_swapChain_for_window(_gDevice, _window) == S_OK)
			{
				_gDevice->output_windows.push_back(_window);
			}
		}
	}
}

void w_graphics_device_manager::on_window_resized(UINT pIndex)
{
	if (pIndex >= this->_graphics_devices.size()) return;

	auto _gDevice = this->_graphics_devices.at(pIndex);
	auto _size = _gDevice->output_windows.size();
	for (size_t j = 0; j < _size; ++j)
	{
		auto _window = _gDevice->output_windows.at(j);
		_create_swapChain_for_window(_gDevice, _window);
	}
}

HRESULT w_graphics_device_manager::_create_swapChain_for_window(std::shared_ptr<w_graphics_device> pGDevice, w_output_window& pWindow)
{
	HRESULT _hr = S_FALSE;

	auto _indexStr = std::to_wstring(pWindow.index);

	if (pWindow.swapChain.Get() != nullptr)
	{
#pragma region Resize swap chain

		logger.write(L"SwapChain for window " + _indexStr + L" is going to resizing");

		// If the swap chain already exists, resize it.
		_hr = pWindow.swapChain->ResizeBuffers(
			2, // Double-buffered swap chain.
			pWindow.width,
			pWindow.height,
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0);
		V(_hr, L"resizing swap chain ", this->name);

		logger.write(L"SwapChain resized");

#pragma endregion
	}
	else
	{
#pragma region Create swap chain

		//Create a new swap chain using the same adapter as the existing Direct3D device.
		DXGI_SWAP_CHAIN_DESC1 _swapChainDesc = { 0 };
		_swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
		_swapChainDesc.Stereo = false;
		_swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
		_swapChainDesc.SampleDesc.Quality = 0;
		_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		_swapChainDesc.BufferCount = 2; // Use double-buffering to minimize latency.
		_swapChainDesc.Scaling = DXGI_SCALING_NONE;
		_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
		_swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		
		//Set the size of swap chain
		_swapChainDesc.Width = pWindow.width;
		_swapChainDesc.Height = pWindow.height;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC _fullScreenDesc;
		ZeroMemory(&_fullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));

		_fullScreenDesc.RefreshRate.Denominator = 1;
		_fullScreenDesc.RefreshRate.Numerator = 60;
		_fullScreenDesc.Windowed = true;

		_hr = this->_dxgi_factory->CreateSwapChainForHwnd(
			pGDevice->device.Get(),
			pWindow.hWnd,
			&_swapChainDesc,
			&_fullScreenDesc,
			nullptr,
			&pWindow.swapChain);
		V(_hr, L"creating swap chain for #" + _indexStr + L" handle of window");

		//window->swapChain->SetFullscreenState(TRUE, NULL);// switch to windowed mode

		auto _device = pGDevice->device;
		{
#pragma region Create back buffer, depth-stencil & view port

			// Create a render target view of the swap chain back buffer.
			//hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&this->backBuffer));
			_hr = pWindow.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pWindow.backBuffer);
			V(_hr, L"getting back buffer", this->name, 3, true, true);

			_hr = _device->CreateRenderTargetView(pWindow.backBuffer.Get(), nullptr, &pWindow.renderTargetView);
			V(_hr, L"creating render target view", this->name, 3, true, true);

			// Create a depth stencil view.
			CD3D11_TEXTURE2D_DESC _depthStencilDesc(
				DXGI_FORMAT_D24_UNORM_S8_UINT,
				pWindow.width,
				pWindow.height,
				1,
				1,
				D3D11_BIND_DEPTH_STENCIL);

			ComPtr<ID3D11Texture2D> _depthStencil;
			_hr = _device->CreateTexture2D(&_depthStencilDesc, nullptr, &_depthStencil);
			V(_hr, L"creating depth stencil texture 2D", this->name, 3, true, true);

			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
			_hr = _device->CreateDepthStencilView(_depthStencil.Get(), &depthStencilViewDesc, &pWindow.depthStencilView);
			V(_hr, L"creating depth stencil view", this->name, 3, true, true);

			// Set the rendering viewport to target the entire window.
			pWindow.viewPort = CD3D11_VIEWPORT(
				0.0f,
				0.0f,
				static_cast<float>(pWindow.width),
				static_cast<float>(pWindow.height));
			
#pragma endregion

#pragma region Create Direct2D Target

			auto _context2D = pGDevice->context_2D;

			D2D1_BITMAP_PROPERTIES1 _bitmapProperties = D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
				DEFAULT_DPI,
				DEFAULT_DPI);

			ComPtr<IDXGISurface> _dxgiSurface;
			_hr = pWindow.swapChain->GetBuffer(0, IID_PPV_ARGS(&_dxgiSurface));
			V(_hr, L"Creating dxgi surface", this->name, 3, true, true);

			_hr = pGDevice->context_2D->CreateBitmapFromDxgiSurface(_dxgiSurface.Get(), &_bitmapProperties, &pGDevice->target_2D);
			V(_hr, L"Create bitmap from Dxgi surface", this->name, 3, true, true);

			_context2D->SetTarget(pGDevice->target_2D.Get());
			//Grayscale text anti-aliasing is recommended for all Metro style apps.
			_context2D->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

			//D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();
			//// Set the DPI to be the default system DPI to allow direct mapping
			//// between image pixels and desktop pixels in different system DPI settings
			//renderTargetProperties.dpiX = DEFAULT_DPI;
			//renderTargetProperties.dpiY = DEFAULT_DPI;

			//D2D1_SIZE_U _2DRenderSize = D2D1::SizeU(width, height);
			//hr = d2dFactory->CreateHwndRenderTarget(
			//	renderTargetProperties,
			//	D2D1::HwndRenderTargetProperties(this->hwnd, _2DRenderSize),
			//	&this->graphicsDevice->renderD2DTarget);

#pragma endregion

		}

		_device = nullptr;

		// Disable the ALT-ENTER shortcut for entering full-screen mode
		//hr = dxgiFactory->MakeWindowAssociation(window->hWnd, DXGI_MWA_NO_ALT_ENTER);
		//OnFailed(hr, "disabling Alt-Enter for full screen", this->Name, false, true);

		//hr = dxgiFactory->RegisterOcclusionStatusWindow(hwnd, WM_USER, &this->pdwCookie);
		//OnFailed(hr, "Register occlusion status window", false);

#pragma endregion
	}

	return _hr;
}

void w_graphics_device_manager::begin_render()
{
	//In DX11.1 mode, we have only one graphics device
	for (size_t i = 0; i < this->_graphics_devices.size(); ++i)
	{
		auto _gDevice = this->_graphics_devices.at(i);

		for (size_t j = 0; j < _gDevice->output_windows.size(); ++j)
		{
			auto _window = _gDevice->output_windows.at(j);

			auto _context = _gDevice->context;
			auto _depthStencilView = _window.depthStencilView.Get();
			auto _renderTargetView = _window.renderTargetView;

			_context->RSSetViewports(1, &_window.viewPort);
			_context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView);
			_context->ClearRenderTargetView(_renderTargetView.Get(), this->_backColor);
			_context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			_depthStencilView = nullptr;
			_renderTargetView = nullptr;
		}
	}
}

void w_graphics_device_manager::end_render()
{
	DXGI_PRESENT_PARAMETERS _parameters;
	ZeroMemory(&_parameters, sizeof(DXGI_PRESENT_PARAMETERS));

	for (size_t i = 0; i < this->_graphics_devices.size(); ++i)
	{
		auto _gDevice = this->_graphics_devices.at(i);

		for (size_t j = 0; j < _gDevice->output_windows.size(); ++j)
		{
			auto _window = _gDevice->output_windows.at(j);

			auto _context = _gDevice->context;
			{
				//Present swap chain
				auto _hr = _window.swapChain->Present1(1, 0, &_parameters);

				//Discard views
				_context->DiscardView(_window.renderTargetView.Get());
				_context->DiscardView(_window.depthStencilView.Get());
				
				if (_hr == DXGI_ERROR_DEVICE_REMOVED)
				{
					on_device_lost();
				}
				else
				{
					V(_hr, L"Error on presenting at the process of end render", this->name, 3, false, true);
				}

				_context = nullptr;
			}
		}
	}
}

ULONG w_graphics_device_manager::release()
{
	if (_super::is_released()) return 0;

	COMPTR_RELEASE(this->_dxgi_factory);

	//Release samplers
	std::for_each(samplers.begin(), samplers.end(),
		[](Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)->void
	{
		COMPTR_RELEASE(sampler);
	});
	samplers.clear();

	while (this->_graphics_devices.size() > 0)
	{
		auto _gDevice = this->_graphics_devices.front();

#ifdef _DEBUG
		_gDevice->release();
		if (_gDevice->d3d_debug_layer)
		{
			_gDevice->d3d_debug_layer->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		}
#endif
		SHARED_RELEASE(_gDevice);

		this->_graphics_devices.pop_back();
	}

#if defined(_DEBUG) && defined(__DX_DIAGNOSTIC__)
	COMPTR_RELEASE(graphics_diagnostic)
#endif

	return _super::release();
}

#pragma endregion

#pragma region Getters

const DirectX::XMFLOAT2 w_graphics_device_manager::get_dpi() const
{
	auto _gDevice = get_graphics_device();
	DirectX::XMFLOAT2 dpi;
	if (_gDevice && _gDevice->factory_2D)
	{
		_gDevice->factory_2D->GetDesktopDpi(&dpi.x, &dpi.y);
	}

	return dpi;
}

const DirectX::XMFLOAT2 w_graphics_device_manager::get_pixels_to_inches(float pX, float pY) const
{
	DirectX::XMFLOAT2 _inches;
	auto _dpi = get_dpi();
	if (_dpi.x != 0 && _dpi.y != 0)
	{
		_inches.x = pX / _dpi.x; _inches.y = pY / _dpi.y;
		return _inches;
	}
	return _inches;
}

#pragma endregion

#pragma region w_graphics_device

void w_graphics_device::create_blend_state(
	BOOL pBlendEnable, 
	D3D11_BLEND pSrcBlend, D3D11_BLEND pDestBlend, 
	D3D11_BLEND pSrcBlendAlpha, D3D11_BLEND pDestBlendAlpha, 
	D3D11_BLEND_OP pBlendOp, D3D11_BLEND_OP pBlendOpAlpha,
	UINT8 pRenderTargetWriteMask,
	_Out_ ID3D11BlendState1** pBlendstate)
{
	D3D11_BLEND_DESC1 _blend_desc;
	std::memset(&_blend_desc, 0, sizeof(_blend_desc));

	_blend_desc.RenderTarget[0].BlendEnable = pBlendEnable;
	_blend_desc.RenderTarget[0].SrcBlend = pSrcBlend;
	_blend_desc.RenderTarget[0].DestBlend = pDestBlend;
	_blend_desc.RenderTarget[0].BlendOp = pBlendOp;
	_blend_desc.RenderTarget[0].SrcBlendAlpha = pSrcBlendAlpha;
	_blend_desc.RenderTarget[0].DestBlendAlpha = pDestBlendAlpha;
	_blend_desc.RenderTarget[0].BlendOpAlpha = pBlendOpAlpha;
	_blend_desc.RenderTarget[0].RenderTargetWriteMask = pRenderTargetWriteMask;

	auto _hr = this->device->CreateBlendState1(&_blend_desc, pBlendstate);
	V(_hr, L"creating blend state", L"w_graphics_device", 3, true, true);
}

void w_graphics_device::create_depth_stencil_state(bool pEnable, bool pWriteEnable, _Out_ ID3D11DepthStencilState** pDepthStencilState)
{
	D3D11_DEPTH_STENCIL_DESC _depth_desc;
	std::memset(&_depth_desc, 0, sizeof(_depth_desc));

	_depth_desc.DepthEnable = pEnable;
	_depth_desc.DepthWriteMask = pWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	_depth_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	_depth_desc.StencilEnable = false;
	_depth_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	_depth_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	_depth_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	_depth_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	_depth_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	_depth_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	_depth_desc.BackFace = _depth_desc.FrontFace;

	auto _hr = device->CreateDepthStencilState(&_depth_desc, pDepthStencilState);
	V(_hr, L"creating depth state", L"w_graphics_device", 3, true, true);
}

void w_graphics_device::create_rasterizer_state(D3D11_CULL_MODE pCullMode, D3D11_FILL_MODE pFillMode, _Out_ ID3D11RasterizerState** pRasterizerState)
{
	D3D11_RASTERIZER_DESC _rasterizer_desc;
	std::memset(&_rasterizer_desc, 0, sizeof(_rasterizer_desc));

	_rasterizer_desc.CullMode = pCullMode;
	_rasterizer_desc.FillMode = pFillMode;
	_rasterizer_desc.DepthClipEnable = true;
	_rasterizer_desc.MultisampleEnable = true;
	auto _hr = this->device->CreateRasterizerState(&_rasterizer_desc, pRasterizerState);
	V(_hr, L"creating rasterizer state", L"w_graphics_device", 3, true, true);
}

void w_graphics_device::create_sampler_state(D3D11_FILTER pFilter, D3D11_TEXTURE_ADDRESS_MODE pAddressMode, _Out_ ID3D11SamplerState** pSamplerState)
{
	D3D11_SAMPLER_DESC _sampler_desc;
	std::memset(&_sampler_desc, 0, sizeof(_sampler_desc));

	_sampler_desc.Filter = pFilter;
	_sampler_desc.AddressU = pAddressMode;
	_sampler_desc.AddressV = pAddressMode;
	_sampler_desc.AddressW = pAddressMode;
	_sampler_desc.MaxAnisotropy = (this->device->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1) ? 16 : 2;
	_sampler_desc.MaxLOD = FLT_MAX;
	_sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	auto _hr = this->device->CreateSamplerState(&_sampler_desc, pSamplerState);
	V(_hr, L"creating sampler state", L"w_graphics_device", 3, true, true);
}

void w_graphics_device::enable_alpha_blend(std::initializer_list<float> pBlendFactor, UINT pSampleMask)
{
	auto _size = pBlendFactor.size();
	assert(_size != 0, "pBlendFactor must have 4 float number");
	std::vector<float> _blend_factor(pBlendFactor.begin(), pBlendFactor.end());

	this->context->OMSetBlendState(this->_alpha_blend.Get(), _blend_factor.data(), pSampleMask);
}

#pragma endregion