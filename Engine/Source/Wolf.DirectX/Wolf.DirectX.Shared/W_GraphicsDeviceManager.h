/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_GraphicsDeviceManager.h
	Description		 : The manager for graphics devices
	Comment          :
*/

#pragma once

#if defined(__WINRT)
#include <agile.h>
#endif

#include <wrl\client.h>
#include <algorithm>
#include <memory>
#include <vector>
#include <map>
#include <W_Object.h>
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <dwrite_1.h>
#include <dxgi1_2.h>
#include <Wincodec.h> // for IWICImagingFactory2
#include <W_Window.h>

#define DEFAULT_DPI 96

namespace Wolf
{
	namespace Graphics
	{
		//Output window which handles all direct 3d resources for output renderer
		struct W_OutputWindow
		{
		public:
			//Add refrence
			ULONG AddRef() { return 1; }
			//Release resources
			ULONG Release()
			{
				if (this->isReleased) return 0;
				this->isReleased = true;

				this->hWnd = NULL;

				COM_RELEASE(this->backBuffer);
				COM_RELEASE(this->depthStencilView);
				COM_RELEASE(this->renderTargetView);
				COM_RELEASE(this->swapChain);

				return 1;
			}

			UINT																index;
			bool																isReleased;
			HWND																hWnd;
			UINT																width;
			UINT																height;
			DWORD																pdwCookie;
			float																aspectRatio;
			CD3D11_VIEWPORT														viewPort;
			Microsoft::WRL::ComPtr<IDXGISwapChain1>								swapChain;
			Microsoft::WRL::ComPtr<ID3D11Texture2D>								backBuffer;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>						renderTargetView;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>						depthStencilView;
		};

		//Contains direct3D device and context which performs primitive-based rendering
		struct W_GraphicsDevice
		{
		public:
			//Get the first and the primary window which was created with this device
			W_OutputWindow* MainWindow()
			{
				if (this->outputWindows.size() > 0)
				{
					auto window = this->outputWindows.at(0);
					return &window;
				}
				return nullptr;
			}

			//Release all resources
			ULONG Release()
			{
				if (this->isReleased) return 0;
				this->isReleased = true;

				//Release samplers
				std::for_each(this->samplers.begin(), this->samplers.end(),
					[](Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)->void
				{
					COM_RELEASE(sampler);
				});
				this->samplers.clear();

				COM_RELEASE(this->d2DMultithread);
				COM_RELEASE(this->imageFactory);
				COM_RELEASE(this->factory2D);
				COM_RELEASE(this->writeFactory);
				COM_RELEASE(this->context2D);
				COM_RELEASE(this->device2D);

				for (size_t i = 0; i < this->outputWindows.size(); ++i)
				{
					this->outputWindows.at(i).Release();
					this->dxgiOutputs.at(i)->Release();
				}
				this->outputWindows.clear();
				this->dxgiOutputs.clear();

				COM_RELEASE(this->context);
				COM_RELEASE(this->device);

				COM_RELEASE(this->adaptor);
				COM_RELEASE(this->adaptor);

				return 1;
			}

			D3D_FEATURE_LEVEL													featureLevel;

			Microsoft::WRL::ComPtr<IDXGIAdapter>								adaptor;
			std::vector<IDXGIOutput*>											dxgiOutputs;
			std::vector<W_OutputWindow>											outputWindows;

			Microsoft::WRL::ComPtr<ID3D11Device1>								device;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext1>						context;

			Microsoft::WRL::ComPtr<ID2D1Device>									device2D;
			Microsoft::WRL::ComPtr<ID2D1DeviceContext>							context2D;
			Microsoft::WRL::ComPtr<ID2D1Factory1>								factory2D;
			Microsoft::WRL::ComPtr<ID2D1Multithread>							d2DMultithread;

			Microsoft::WRL::ComPtr<IDWriteFactory>								writeFactory;
			Microsoft::WRL::ComPtr<IWICImagingFactory2>							imageFactory;

			std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>>				samplers;

		private:
			bool isReleased;
		};

		//Handles the configuration and management of the graphics device.
		class W_GraphicsDeviceManager : public System::W_Object
		{
		public:
			API W_GraphicsDeviceManager();
			API virtual ~W_GraphicsDeviceManager();

			//Initialize graphics devices
			API virtual void Initialize();
			//Initialize the output windows
			API virtual void InitializeOutputWindows(std::map<int, std::vector<W_WindowInfo>> pOutputWindowsInfo);
			//Called when corresponding window resized
			API virtual void OnWindowResize(UINT pIndex);
			//Called when any graphics device lost
			API virtual void OnDeviceLost();
			//Begin render on all graphics devices
			API virtual void BeginRender();
			//End render on all graphics devices
			API virtual void EndRender();
			//Release all resources
			API ULONG Release() override;

#pragma region Getters

			//Get the main graphics device, this is first and the primary device.
			std::shared_ptr<W_GraphicsDevice> GDevice() const					{ return this->graphicsDevices.size() > 0 ? this->graphicsDevices.at(0) : nullptr; }
			//Returns number of available graphics devices
			const ULONG GetNumberOfGraphicsDevices() const						{ return this->graphicsDevices.size(); }
			//Get deafult device window width
			const UINT GetWindowWidth() const									{ return this->windowsInfo.size() == 0 || this->windowsInfo.at(0).size() == 0 ? 0 : this->windowsInfo.at(0).at(0).width; }
			//Get deafult device window height
			const UINT GetWindowHeight() const									{ return this->windowsInfo.size() == 0 || this->windowsInfo.at(0).size() == 0 ? 0 : this->windowsInfo.at(0).at(0).height; }

#pragma endregion

		protected:
			float																backColor[4];
			std::vector<std::shared_ptr<W_GraphicsDevice>>						graphicsDevices;

		private:
			HRESULT CreateSwapChainForWindow(std::shared_ptr<W_GraphicsDevice> pGDevice, W_OutputWindow& pOutputWindow);

			std::map<int, std::vector<W_WindowInfo>>							windowsInfo;
			Microsoft::WRL::ComPtr<IDXGIFactory2>								dxgiFactory;
		};
	}
}