#include "Wolf.DirectX.Shared.PCH.h"
#include "W_GraphicsDeviceManager.h"
#include "W_GraphicsResource\W_Textures\W_Texture2D.h"

#if defined(__WINRT)
#include "windows.ui.xaml.media.dxinterop.h"
#endif

//#include "GraphicsResource\Textures\Texture2D.h"
//#include "GraphicsResource\Textures\ScreenGrab.h"

using namespace std;
using namespace Wolf::System;
using namespace Wolf::Graphics;
using namespace DirectX;
using namespace Microsoft::WRL;

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
const unsigned int bytesPerPixel = 4;

W_GraphicsDeviceManager::W_GraphicsDeviceManager()
{
	this->Name = typeid(this).name();
	this->backColor[0] = 0.149f; this->backColor[1] = 0.149f; this->backColor[2] = 0.149f; this->backColor[3] = 1.0f;
}

W_GraphicsDeviceManager::~W_GraphicsDeviceManager()
{
	Release();
}

#if defined(__WIN32)

void W_GraphicsDeviceManager::Initialize()
{
	auto hr = CreateDXGIFactory(__uuidof(IDXGIFactory2), (void**)(&this->dxgiFactory));
	OnFailed(hr, "Error on creating dxgi factory", this->Name, true, true);

	auto size = this->graphicsDevices.size();
	if (size != 0)
	{
		//Release all graphics devices and then clear them
		for (size_t i = 0; i < size; ++i)
		{
			this->graphicsDevices.at(i)->Release();
			SHARED_RELEASE(this->graphicsDevices.at(i));
		}
		this->graphicsDevices.clear();
	}

	//Iterate for all adaptors
	Logger.Write("++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
	Logger.Write("++++++++++++++++ Graphics devices Info +++++++++++++++++");
	
	IDXGIAdapter* _adapter = nullptr;
	for (int i = 0; DXGI_ERROR_NOT_FOUND != this->dxgiFactory->EnumAdapters(i, &_adapter); ++i)
	{
		DXGI_ADAPTER_DESC adapterDesc = { 0 };
		auto hr = _adapter->GetDesc(&adapterDesc);

		if (hr != S_OK)
		{
			OnFailed(S_FALSE, "getting adaptors", this->Name, true, true);
		}

		Logger.Write("++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
		Logger.Write("DeviceId : " + std::to_string(adapterDesc.DeviceId));
		Logger.Write("DedicatedSystemMemory : " + std::to_string(adapterDesc.DedicatedSystemMemory));
		Logger.Write("DedicatedVideoMemory : " + std::to_string(adapterDesc.DedicatedVideoMemory));
		Logger.Write("VendorId : " + std::to_string(adapterDesc.VendorId));
		Logger.Write("SubSysId : " + std::to_string(adapterDesc.SubSysId));
		Logger.Write("++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

		/*
			Acording to MSDN, starting with Windows 8, an adapter called the "Microsoft Basic Render Driver"
			is always present.This adapter has a VendorId of 0x1414 and a DeviceID of 0x8c.
			We skip it.
		*/
		if (adapterDesc.DeviceId == 140)
		{
			Logger.Write("Microsoft Basic Render Driver skipped");
			Logger.Write("++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
		}
		else
		{
			auto gDevice = make_shared<W_GraphicsDevice>();
			gDevice->adaptor = _adapter;

#pragma region Get number of dxgi outputs for this graphic adaptor

			IDXGIOutput* dxgiOutput = nullptr;
			for (int i = 0; DXGI_ERROR_NOT_FOUND != _adapter->EnumOutputs(i, &dxgiOutput); ++i)
			{
				// get the description
				DXGI_OUTPUT_DESC outputDesc = { 0 };
				HRESULT hr = dxgiOutput->GetDesc(&outputDesc);
				if (hr != S_OK)
				{
					OnFailed(S_FALSE, "getting dxgi outputs", this->Name, true, true);
				}
				gDevice->dxgiOutputs.push_back(dxgiOutput);
			}
			dxgiOutput = nullptr;

#pragma endregion

			//Create a d3dDevice & d3dContext for each graphics device associated to graphics adaptor, 

#pragma region Create device3D and context3D

			hr = S_FALSE;
			UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
			//For debugging
			creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

			D3D_FEATURE_LEVEL featureLevels[] =
			{
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3,
				D3D_FEATURE_LEVEL_9_2,
				D3D_FEATURE_LEVEL_9_1
			};

			ComPtr<ID3D11Device> device = nullptr;
			ComPtr<ID3D11DeviceContext> context = nullptr;

			D3D_FEATURE_LEVEL _featureLevel;
			hr = D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				creationFlags,
				featureLevels,
				ARRAYSIZE(featureLevels),
				D3D11_SDK_VERSION,
				&device,
				&_featureLevel,
				&context);
			OnFailed(hr, "creating graphics device", this->Name, true, true);

			//Log for feature level
			switch (_featureLevel)
			{
			case D3D_FEATURE_LEVEL_11_1:
				Logger.Write("Device created with D3D_FEATURE_LEVEL_11_1 feature level");
				break;
			case D3D_FEATURE_LEVEL_11_0:
				Logger.Write("Device created with D3D_FEATURE_LEVEL_11_0 feature level");
				break;
			case D3D_FEATURE_LEVEL_10_1:
				Logger.Write("Device created with D3D_FEATURE_LEVEL_10_1 feature level");
				break;
			case D3D_FEATURE_LEVEL_10_0:
				Logger.Write("Device created with D3D_FEATURE_LEVEL_10_0 feature level");
				break;
			case D3D_FEATURE_LEVEL_9_3:
				Logger.Write("Device created with D3D_FEATURE_LEVEL_9_3 feature level");
				break;
			case D3D_FEATURE_LEVEL_9_2:
				Logger.Write("Device created with D3D_FEATURE_LEVEL_9_2 feature level");
				break;
			case D3D_FEATURE_LEVEL_9_1:
				Logger.Write("Device created with D3D_FEATURE_LEVEL_9_1 feature level");
				break;
			}

			gDevice->featureLevel = _featureLevel;

			ComPtr<IDXGIDevice1> dxgiDevice = nullptr;
			{
				hr = device.As(&dxgiDevice);//reinterpret_cast<void**>(&dxgiDevice)
				OnFailed(hr, "getting IDXGIDevice", this->Name, true);
			}

#if defined(WINRT)
			hr = dxgiDevice->SetMaximumFrameLatency(3);
			OnFailed(hr, "Set maximum frame latency", false);
#endif
			hr = device.As(&gDevice->device);
			OnFailed(hr, "getting device1", this->Name, true, true);
			if (gDevice->device == nullptr)
			{
				auto msg = "Graphics Device1 is null";
				Logger.Write(msg);
				OnFailed(S_FALSE, msg, this->Name, true, true);
			}

			hr = context.As(&gDevice->context);
			OnFailed(hr, "getting context1", this->Name, true, true);
			if (gDevice->context == nullptr)
			{
				auto msg = "Context1 is null";
				Logger.Write(msg);
				OnFailed(S_FALSE, msg, this->Name, true, true);
			}

			COM_RELEASE(dxgiDevice);
			COM_RELEASE(context);
			COM_RELEASE(device);

#pragma endregion

#pragma region Create device2D and context2D

			D2D1_FACTORY_OPTIONS factory2DOptions;
			ZeroMemory(&factory2DOptions, sizeof(D2D1_FACTORY_OPTIONS));

#ifdef _DEBUG
			factory2DOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

			//Create Direct2D factory
			hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), &factory2DOptions, &gDevice->factory2D);//reinterpret_cast<void**>(&this->graphicsDevice->d2dFactory)
			OnFailed(hr, "creating Direct2D factory", this->Name, true, true);

			//Create DirectWrite factory
			hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &gDevice->writeFactory);//reinterpret_cast<IUnknown**>(&this->graphicsDevice->writeFactory)
			OnFailed(hr, "creating write factory", this->Name, true, true);

			//Create image factory
			hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&gDevice->imageFactory));
			OnFailed(hr, "creating image factory", this->Name, true, true);

			//Get IDXGIDevice
			dxgiDevice = nullptr;
			{
				hr = gDevice->device.As(&dxgiDevice);//reinterpret_cast<void**>(&dxgiDevice)
				OnFailed(hr, "getting IDXGIDevice", this->Name, true, true);

				//Create 2D Device from 2D factory
				hr = gDevice->factory2D->CreateDevice(dxgiDevice.Get(), &gDevice->device2D);
				OnFailed(hr, "getting device2D", this->Name, true, true);

				//Create 2D context from 2D device
				hr = gDevice->device2D->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &gDevice->context2D);
				OnFailed(hr, "creating context2D", this->Name, true, true);

				COM_RELEASE(dxgiDevice);
			}

			//Check out how to access Direct3D resources directly without Direct2D
			gDevice->factory2D.Get()->QueryInterface(IID_PPV_ARGS(&gDevice->d2DMultithread));

#pragma endregion

			this->graphicsDevices.push_back(gDevice);
		}
	}
	_adapter = nullptr;

	if (this->graphicsDevices.size() == 0)
	{
		OnFailed(S_FALSE, "getting graphics device. No device available", this->Name, true, false);
	}

#pragma region Create TextureSampler

	//Load default sampler
	const D3D11_TEXTURE_ADDRESS_MODE AddressUVW[3] =
	{
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP
	};
	W_Texture2D::CreateSampler(
		this->graphicsDevices.at(0),
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		AddressUVW);

#pragma endregion

}

#endif

void W_GraphicsDeviceManager::OnDeviceLost()
{
	Logger.Write("Device is going to reseting");
	{
		Initialize();
		InitializeOutputWindows(this->windowsInfo);
	}
	Logger.Write("Device reset succeeded");
}

void W_GraphicsDeviceManager::InitializeOutputWindows(map<int, vector<W_WindowInfo>> pOutputWindowsInfo)
{
	HRESULT hr;

	//We need to store it for the first time, in order to reseting device in the future
	if (this->windowsInfo.size() == 0)
	{
		this->windowsInfo = pOutputWindowsInfo;
	}

	for (pair<int, vector<W_WindowInfo>> p : pOutputWindowsInfo)
	{
		//Get graphics devices & hwnds
		auto gDevice = this->graphicsDevices.at(p.first);
		auto windowInfo = p.second;

		W_OutputWindow window;
		for (size_t i = 0; i < windowInfo.size(); ++i)
		{
			ZeroMemory(&window, sizeof(W_OutputWindow));

			auto wInfo = windowInfo.at(i);
			
			window.index = i;
			window.hWnd = wInfo.hWnd;
			window.width = wInfo.width;
			window.height = wInfo.height;
			window.aspectRatio = static_cast<float>(window.width) / static_cast<float>(window.height);

			if (CreateSwapChainForWindow(gDevice, window) == S_OK)
			{
				gDevice->outputWindows.push_back(window);
			}
		}
	}
}

void W_GraphicsDeviceManager::OnWindowResize(UINT pIndex)
{
	if (pIndex >= this->graphicsDevices.size()) return;

	auto gDevice = this->graphicsDevices.at(pIndex);
	for (size_t j = 0; j < gDevice->outputWindows.size(); ++j)
	{
		auto window = gDevice->outputWindows.at(j);
		CreateSwapChainForWindow(gDevice, window);
	}
}

HRESULT W_GraphicsDeviceManager::CreateSwapChainForWindow(std::shared_ptr<W_GraphicsDevice> pGDevice, W_OutputWindow& pWindow)
{
	HRESULT hr = S_FALSE;

	auto indexStr = std::to_string(pWindow.index);

	if (pWindow.swapChain.Get() != nullptr)
	{
#pragma region Resize swap chain

		Logger.Write("SwapChain for window " + indexStr + " is going to resizing");

		// If the swap chain already exists, resize it.
		hr = pWindow.swapChain->ResizeBuffers(
			2, // Double-buffered swap chain.
			pWindow.width,
			pWindow.height,
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0);
		OnFailed(hr, "resizing swap chain ", this->Name);

		Logger.Write("SwapChain resized");

#pragma endregion
	}
	else
	{
#pragma region Create swap chain

		//Create a new swap chain using the same adapter as the existing Direct3D device.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2; // Use double-buffering to minimize latency.
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		//Set the size of swap chain
		swapChainDesc.Width = pWindow.width;
		swapChainDesc.Height = pWindow.height;

		hr = this->dxgiFactory->CreateSwapChainForHwnd(
			pGDevice->device.Get(),
			pWindow.hWnd,
			&swapChainDesc,
			nullptr, //Todo: Full screen desc.
			nullptr,
			&pWindow.swapChain);
		OnFailed(hr, "creating swap chain for #" + indexStr + " handle of window");

		//window->swapChain->SetFullscreenState(TRUE, NULL);// switch to windowed mode

		auto device = pGDevice->device;
		{
#pragma region Create back buffer, depth-stencil & view port

			// Create a render target view of the swap chain back buffer.
			//hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&this->backBuffer));
			hr = pWindow.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pWindow.backBuffer);
			OnFailed(hr, "getting back buffer", this->Name, true, true);

			hr = device->CreateRenderTargetView(pWindow.backBuffer.Get(), nullptr, &pWindow.renderTargetView);
			OnFailed(hr, "creating render target view", this->Name, true, true);

			// Create a depth stencil view.
			CD3D11_TEXTURE2D_DESC depthStencilDesc(
				DXGI_FORMAT_D24_UNORM_S8_UINT,
				pWindow.width,
				pWindow.height,
				1,
				1,
				D3D11_BIND_DEPTH_STENCIL);

			ComPtr<ID3D11Texture2D> depthStencil;
			hr = device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil);
			OnFailed(hr, "creating depth stencil texture 2D", this->Name, true, true);

			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
			hr = device->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &pWindow.depthStencilView);
			OnFailed(hr, "creating depth stencil view", this->Name, true, true);

			// Set the rendering viewport to target the entire window.
			pWindow.viewPort = CD3D11_VIEWPORT(
				0.0f,
				0.0f,
				pWindow.width,
				pWindow.height);


#pragma endregion

#pragma region Create Direct2D Target

			auto context2D = pGDevice->context2D;
			D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
				DEFAULT_DPI,
				DEFAULT_DPI);

			ComPtr<IDXGISurface> dxgiSurface;
			hr = pWindow.swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiSurface));
			OnFailed(hr, "Creating dxgi surface", this->Name, true, true);

			ComPtr<ID2D1Bitmap1> d2dTarget;
			hr = pGDevice->context2D->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &bitmapProperties, &d2dTarget);
			OnFailed(hr, "Create bitmap from Dxgi surface", this->Name, true, true);

			context2D->SetTarget(d2dTarget.Get());
			// Grayscale text anti-aliasing is recommended for all Metro style apps.
			context2D->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

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

		device = nullptr;

		// Disable the ALT-ENTER shortcut for entering full-screen mode
		//hr = dxgiFactory->MakeWindowAssociation(window->hWnd, DXGI_MWA_NO_ALT_ENTER);
		//OnFailed(hr, "disabling Alt-Enter for full screen", this->Name, false, true);

		//hr = dxgiFactory->RegisterOcclusionStatusWindow(hwnd, WM_USER, &this->pdwCookie);
		//OnFailed(hr, "Register occlusion status window", false);

#pragma endregion
	}

	return hr;
}

void W_GraphicsDeviceManager::BeginRender()
{
	for (size_t i = 0; i < this->graphicsDevices.size(); ++i)
	{
		auto gDevice = this->graphicsDevices.at(i);

		for (size_t j = 0; j < gDevice->outputWindows.size(); ++j)
		{
			auto window = gDevice->outputWindows.at(j);

			auto context = gDevice->context;
			{
				auto _depthStencilView = window.depthStencilView.Get();
				auto _renderTargetView = window.renderTargetView;

				context->RSSetViewports(1, &window.viewPort);
				context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView);
				context->ClearRenderTargetView(_renderTargetView.Get(), this->backColor);
				context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

				_depthStencilView = nullptr;
				_renderTargetView = nullptr;
				context = nullptr;
			}
		}
	}
}

void W_GraphicsDeviceManager::EndRender()
{
	DXGI_PRESENT_PARAMETERS parameters;
	ZeroMemory(&parameters, sizeof(DXGI_PRESENT_PARAMETERS));

	for (size_t i = 0; i < this->graphicsDevices.size(); ++i)
	{
		auto gDevice = this->graphicsDevices.at(i);

		for (size_t j = 0; j < gDevice->outputWindows.size(); ++j)
		{
			auto window = gDevice->outputWindows.at(j);

			auto context = gDevice->context;
			{
				//Present swap chain
				auto hr = window.swapChain->Present1(1, 0, &parameters);
				//Discard views
				context->DiscardView(window.renderTargetView.Get());
				context->DiscardView(window.depthStencilView.Get());

				if (hr == DXGI_ERROR_DEVICE_REMOVED)
				{
					OnDeviceLost();
				}
				else
				{
					OnFailed(hr, "Error on presenting at the process of end render", this->Name, false, true);
				}

				context = nullptr;
			}
		}
	}
}

ULONG W_GraphicsDeviceManager::Release()
{
	if (this->IsReleased()) return 0;

	for (size_t i = 0; i < this->graphicsDevices.size(); ++i)
	{
		this->graphicsDevices.at(i)->Release();
		SHARED_RELEASE(this->graphicsDevices.at(i));
	}

	return W_Object::Release();
}
