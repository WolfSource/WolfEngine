#include "w_render_pch.h"
#include "w_graphics_device_manager.h"
#include <w_logger.h>
#include <w_convert.h>

using namespace std;
using namespace wolf::system;
using namespace wolf::graphics;

#define DEFAULT_WIDTH   800
#define DEFAULT_HEIGHT  600
#define NUM_SAMPLES     VK_SAMPLE_COUNT_1_BIT

static std::once_flag sOnceFlag;

#ifdef __DX12__
ComPtr<IDXGIFactory4>	w_graphics_device::dx_dxgi_factory = nullptr;
#elif   defined(__VULKAN__)
VkInstance w_graphics_device::vk_instance = NULL;
#endif

w_graphics_device::w_graphics_device():
_is_released(false)

#ifdef __VULKAN__
,
vk_queue_family_selected_index(SIZE_MAX),
vk_graphics_queue(nullptr),
vk_present_queue(nullptr),
vk_device(nullptr)

#endif
{
}

w_output_presentation_window w_graphics_device::main_window()
{
    if (this->output_presentation_windows.size() > 0)
    {
        return this->output_presentation_windows.at(0);
    }
    
    //return null window;
	return w_output_presentation_window();
}

ULONG w_graphics_device::release()
{
    //release all resources
    if (this->_is_released) return 0;
    
    this->_is_released = true;
    
#ifdef __DX12__

#elif defined(__VULKAN__)
    //wait for device to become IDLE
    vkDeviceWaitIdle(this->vk_device);

	//release all output presentation windows
    for (size_t i = 0; i < this->output_presentation_windows.size(); ++i)
    {
        auto _output_window = &(this->output_presentation_windows.at(i));
        
        //release the surface
        vkDestroySurfaceKHR(w_graphics_device::vk_instance,
                            _output_window->vk_presentation_surface,
                            nullptr);
    
        //release command pool & buffers
        vkFreeCommandBuffers(this->vk_device,
                             _output_window->vk_command_allocator_pool,
                             static_cast<uint32_t>(_output_window->vk_command_queues.size()),
                             _output_window->vk_command_queues.data());
        
        vkDestroyCommandPool(this->vk_device, _output_window->vk_command_allocator_pool, nullptr);
    
        //release semaphores
        vkDestroySemaphore(this->vk_device,
                           _output_window->vk_image_is_available_semaphore,
                           nullptr);
        vkDestroySemaphore(this->vk_device,
                           _output_window->vk_rendering_done_semaphore,
                           nullptr);
    
        //release both image and view, so no need to call vkDestroyImage
        vkDestroyImageView(this->vk_device,
                           _output_window->vk_depth_buffer_image_view.view,
                           nullptr);
        
        //release memory of image
        vkFreeMemory(this->vk_device,
                    _output_window->vk_depth_buffer_memory,
                    nullptr);
    
        //release all image view of swap chains
        for (size_t i = 0; i < _output_window->vk_swap_chain_image_views.size(); ++i)
        {
            //release both image and view, so no need to call vkDestroyImage
            vkDestroyImageView(this->vk_device,
                               _output_window->vk_swap_chain_image_views[i].view,
                               nullptr);
            //vkDestroyImage(this->vk_device, this->vk_swap_chain_image_views[i].image, nullptr);
        }
        _output_window->vk_swap_chain_image_views.clear();
    
        //release swap chain
        vkDestroySwapchainKHR(this->vk_device,
                              _output_window->vk_swap_chain,
                              nullptr);
    
        this->output_presentation_windows.at(i).release();
    }
    this->output_presentation_windows.clear();
    
    this->vk_queue_family_properties.clear();
    this->vk_queue_family_supports_present.clear();
    
    this->vk_graphics_queue = nullptr;
    this->vk_present_queue = nullptr;
    
    //release vulkan resources
    vkDestroyDevice(this->vk_device, nullptr);

#endif

    return 1;
}

#pragma region w_graphics_device_manager

#ifdef __GNUC__
#pragma GCC visibility push(hidden)
#endif

namespace wolf
{
    namespace graphics
    {
        class w_graphics_device_manager_pimp
        {
        public:
            w_graphics_device_manager_pimp(_In_ w_graphics_device_manager_configs pConfig) : _config(pConfig), _name("w_graphics_device_manager_pimp")
            {
                //set corn_flower_blue for back color
                this->_clear_color.r = 100;
                this->_clear_color.g = 149;
                this->_clear_color.b = 237;
                this->_clear_color.a = 255;
            }
            
            ~w_graphics_device_manager_pimp()
            {
                release();
            }

			void enumerate_devices(_Inout_  std::vector<std::shared_ptr<w_graphics_device>>& pGraphicsDevices)
			{
#ifdef __DX12__

				std::wstring _msg;
				_msg += L"++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n";
				_msg += L"\t\t\t\t\tDirectX API version: 12\r\n";

				UINT _dxgi_factory_flags = 0;
#ifdef _DEBUG
				/*
					Enable the debug layer (requires the Graphics Tools "optional feature").
					NOTE: Enabling the debug layer after device creation will invalidate the active device.
				*/
				ComPtr<ID3D12Debug> _debug_controller;
				if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&_debug_controller))))
				{
					_debug_controller->EnableDebugLayer();
					//Enable additional debug layers.
					_dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
				}
				else
				{
					logger.warning(L"Could not enable the debug layer for DirectX 12");
				}
#endif
				auto _hr = CreateDXGIFactory2(_dxgi_factory_flags, IID_PPV_ARGS(&w_graphics_device::dx_dxgi_factory));
				V(_hr, L"getting dxgi factory", this->_name, 3, true, true);
				
				//create wrap mode device or hardware device?
				if (this->_config.use_wrap_mode)
				{
					auto _gDevice = std::make_shared<w_graphics_device>();
					_gDevice->device_name = "DirectX Wrap mode with D3D_FEATURE_LEVEL_11_0 feature level";
					_gDevice->dx_is_wrap_device = true;
					_msg += L"\r\n\t\t\t\t\tDirectX device with D3D_FEATURE_LEVEL_11_0 feature level created in Wrap mode\r\n";

					ComPtr<IDXGIAdapter> _warp_adapter;
					_hr = w_graphics_device::dx_dxgi_factory->EnumWarpAdapter(IID_PPV_ARGS(&_warp_adapter));
					V(_hr, L"getting wrap adaptor", this->_name, 3, true, true);

					_hr = D3D12CreateDevice(
						_warp_adapter.Get(),
						this->_config.wrap_mode_feature_level,
						IID_PPV_ARGS(&_gDevice->dx_device));
					V(_hr, L"creating wrap device with feature level: " + std::to_wstring(this->_config.wrap_mode_feature_level) +
						L".D3D_FEATURE_LEVEL_9_1 = 0x9100 ... D3D_FEATURE_LEVEL_12_1 = 0xc100", this->_name, 3, true, true);
					
					//add the wrap device graphics devices list
					pGraphicsDevices.push_back(_gDevice);
				}
				else
				{
					//looking for hardware gpus 
					_msg += L"\r\n\t\t\t\t\tGPU(s) founded:\r\n";

					//Iterate for all adaptors
					ComPtr<IDXGIAdapter1> _adapter;
					for (int i = 0; w_graphics_device::dx_dxgi_factory->EnumAdapters1(i, &_adapter) != DXGI_ERROR_NOT_FOUND; ++i)
					{
						//if the feature level not specified in configs, the default feature level is D3D_FEATURE_LEVEL_11_0
						auto _feature_level =  i >= this->_config.harware_feature_levels.size() ? D3D_FEATURE_LEVEL_11_0 : this->_config.harware_feature_levels[i];

						DXGI_ADAPTER_DESC1 _adapter_desc = {};
						auto _hr = _adapter->GetDesc1(&_adapter_desc);
						if (_hr != S_OK)
						{
							logger.write(_msg);
							release();
							V(S_FALSE, L"GPU count. No GPU found.", this->_name, 3, true, true);
						}

						auto _device_name = std::wstring(_adapter_desc.Description);
						_msg += L"\t\t\t\t\t\tDevice Name: " + _device_name + L"\r\n";
						_msg += L"\t\t\t\t\t\tDevice ID: " + std::to_wstring(_adapter_desc.DeviceId) + L"\r\n";
						_msg += L"\t\t\t\t\t\tDevice Vendor: " + std::to_wstring(_adapter_desc.VendorId) + L"\r\n";
						_msg += L"\t\t\t\t\t\tDevice Subsystem ID: " + std::to_wstring(_adapter_desc.SubSysId) + L"\r\n";
						_msg += L"\t\t\t\t\t\tDevice Revision: " + std::to_wstring(_adapter_desc.Revision) + L"\r\n";
						_msg += L"\t\t\t\t\t\tDevice Dedicated Video Memory: " + std::to_wstring(_adapter_desc.DedicatedVideoMemory) + L"\r\n";
						_msg += L"\t\t\t\t\t\tDevice Dedicated System Memory: " + std::to_wstring(_adapter_desc.DedicatedSystemMemory) + L"\r\n";
						_msg += L"\t\t\t\t\t\tDevice Shared System Memory: " + std::to_wstring(_adapter_desc.SharedSystemMemory) + L"\r\n";

						/*
							Acording to MSDN, starting with Windows 8, an adapter called the "Microsoft Basic Render Driver"
							is always present.This adapter has a VendorId of 0x1414 and a DeviceID of 0x8c.
							Wolf.Engine will skip it. Instead you can create w_graphics_device in wrap mode
						*/
						if (_adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
						{
							continue;
						}

						//Check to see if the adapter supports Direct3D 12, if not then skip it
						if (FAILED(D3D12CreateDevice(_adapter.Get(), _feature_level, _uuidof(ID3D12Device), nullptr)))
						{
							break;
						}

						auto _gDevice = std::make_shared<w_graphics_device>();
						_gDevice->device_name = std::string(_device_name.begin(), _device_name.end());
						_gDevice->dx_is_wrap_device = false;
						_gDevice->dx_feature_level = _feature_level;

						_hr = D3D12CreateDevice(
							_adapter.Get(),
							_feature_level,
							IID_PPV_ARGS(&_gDevice->dx_device));
						if (_hr != S_OK)
						{
							logger.write(_msg);
							release();
							V(_hr, L"creating harware device with feature level: " + std::to_wstring(_feature_level) +
								L".D3D_FEATURE_LEVEL_9_1 = 0x9100 ... D3D_FEATURE_LEVEL_12_1 = 0xc100", this->_name, 3, true, true);
						}

						//add harware device to graphics devices list
						pGraphicsDevices.push_back(_gDevice);

						//create necessry presentation windows for each graphics device
						auto _win = this->_windows_info.find(static_cast<int>(i));
						if (_win != this->_windows_info.end())
						{
							for (size_t j = 0; j < _win->second.size(); ++j)
							{
								auto _window = _win->second[j];
								
								w_output_presentation_window _out_window;
								_out_window.width = _window.width;
								_out_window.height = _window.height;
								_out_window.aspectRatio = (float)_window.width / (float)_window.height;
								_out_window.v_sync = _window.v_sync_enable;
								_out_window.index = static_cast<int>(j);
								_out_window.hwnd = _window.hwnd;
								_out_window.hInstance = _window.hInstance;

								_gDevice->output_presentation_windows.push_back(_out_window);

								_create_swap_chain(_gDevice, j);
								_create_depth_buffer(_gDevice, j);
								_create_synchronization(_gDevice, j);
								_create_command_queue(_gDevice, j);
							}
						}
					}
				}

#elif defined(__VULKAN__)

				auto _vk_major = VK_VERSION_MAJOR(VK_API_VERSION_1_0);
				auto _vk_minor = VK_VERSION_MINOR(VK_API_VERSION_1_0);
				auto _vk_patch = VK_VERSION_PATCH(VK_HEADER_VERSION);

				std::string _msg;
				_msg += "++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n";
				_msg += "\t\t\t\t\tVulkan API version: " +
					std::to_string(_vk_major) + "." +
					std::to_string(_vk_minor) + "." +
					std::to_string(_vk_patch) + "\r\n";

				//first find number of Vulkan instances
				uint32_t _instance_layer_count;

				// Use second parameter as NULL to return the layer count
				vkEnumerateInstanceLayerProperties(&_instance_layer_count, nullptr);

				VkLayerProperties* _layer_property = nullptr;
				vkEnumerateInstanceLayerProperties(&_instance_layer_count, _layer_property);

				uint32_t _extension_count = 0;

				// get available extensions count
				auto _hr = vkEnumerateInstanceExtensionProperties(nullptr,
					&_extension_count,
					nullptr);
				if (_hr)
				{
					//write the buffer to output before exiting
					logger.write(_msg);
					release();
				}

				V(_hr, L"vkEnumerateInstanceExtensionProperties", this->_name, 3, true, true);

				auto _extensions_available = new VkExtensionProperties[_extension_count];
				vkEnumerateInstanceExtensionProperties(nullptr, &_extension_count, _extensions_available);

				_msg += "\t\t\t\t\tVulkan extension(s) available:";
				for (size_t i = 0; i < _extension_count; ++i)
				{
					_msg += "\r\n\t\t\t\t\t\t" + std::string(_extensions_available[i].extensionName);
				}

				//Vulkan instance object

				VkApplicationInfo _app_info = {};
				_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
				_app_info.pApplicationName = "Wolf.Engine";
				_app_info.pEngineName = "Wolf.Engine";
				_app_info.apiVersion = VK_API_VERSION_1_0;


				std::vector<const char*> _enabled_extensions = { VK_KHR_SURFACE_EXTENSION_NAME };

				// Enable surface extensions depending on OS
#if defined(_WIN32)
				_enabled_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(__ANDROID__)
				_enabled_extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(_DIRECT2DISPLAY)
				_enabled_extensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#elif defined(__linux__)
				_enabled_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(__APPLE__)
				_enabled_extensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#endif
				VkInstanceCreateInfo _instance_create_info = {};
				_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
				_instance_create_info.pNext = nullptr;
				_instance_create_info.pApplicationInfo = &_app_info;
				if (_enabled_extensions.size() > 0)
				{
					//            if (this->enable_validation)
					//            {
					//                _enabled_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
					//            }
					_instance_create_info.enabledExtensionCount = (uint32_t)_enabled_extensions.size();
					_instance_create_info.ppEnabledExtensionNames = _enabled_extensions.data();
				}
				//	if (this->enable_validation)
				//	{
				//		_instance_create_info.enabledLayerCount = vkDebug::validationLayerCount;
				//		_instance_create_info.ppEnabledLayerNames = vkDebug::validationLayerNames;
				//	}

				//create Vulkan instance
				_hr = vkCreateInstance(&_instance_create_info, nullptr, &w_graphics_device::vk_instance);
				V(_hr, L"creating vulkan instance", this->_name, 3, true, true);

				//Enumerate physical devices
				uint32_t _gpu_count = 0;

				//Get number of GPU count
				vkEnumeratePhysicalDevices(w_graphics_device::vk_instance, &_gpu_count, nullptr);

				//Get GPU list names
				if (_gpu_count == 0)
				{
					//write the buffer to output before exiting
					logger.write(_msg);
					release();
					V(S_FALSE, L"GPU count. No GPU found.", this->_name, 3, true, true);
				}

				_msg += "\r\n\t\t\t\t\tGPU(s) founded:\r\n";

				auto _gpus = new VkPhysicalDevice[_gpu_count];
				vkEnumeratePhysicalDevices(w_graphics_device::vk_instance, &_gpu_count, _gpus);

				for (size_t i = 0; i < _gpu_count; ++i)
				{
					VkPhysicalDeviceProperties _device_properties;
					vkGetPhysicalDeviceProperties(_gpus[i], &_device_properties);

					auto _device_name = std::string(_device_properties.deviceName);
					_msg += "\t\t\t\t\t\tDevice Name: " + _device_name + "\r\n";
					_msg += "\t\t\t\t\t\tDevice ID: " + std::to_string(_device_properties.deviceID) + "\r\n";
					_msg += "\t\t\t\t\t\tDevice Vendor: " + std::to_string(_device_properties.vendorID) + "\r\n";
					_msg += "\t\t\t\t\t\tAPI Version: " + std::to_string(_device_properties.apiVersion >> 22) + "." +
						std::to_string((_device_properties.apiVersion >> 12) & 0x3ff) + "." +
						std::to_string(_device_properties.apiVersion & 0xfff) + "\r\n";
					_msg += "\t\t\t\t\t\tDriver Version: " + std::to_string(_device_properties.driverVersion) + "\r\n";
					_msg += "\t\t\t\t\t\tDevice Type: ";

					switch (_device_properties.deviceType)
					{
					case VK_PHYSICAL_DEVICE_TYPE_OTHER:
						_msg += "VK_PHYSICAL_DEVICE_TYPE_OTHER";
						break;
					case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
						_msg += "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
						break;
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
						_msg += "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
						break;
					case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
						_msg += "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
						break;
					case VK_PHYSICAL_DEVICE_TYPE_CPU:
						_msg += "VK_PHYSICAL_DEVICE_TYPE_CPU";
						break;
					case VK_PHYSICAL_DEVICE_TYPE_RANGE_SIZE:
						_msg += "VK_PHYSICAL_DEVICE_TYPE_RANGE_SIZE";
						break;
					case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
						_msg += "VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM";
						break;
					}

					_msg += "\r\n\t\t\t\t\t\tPhysical Device Limits:";
					_msg += "\r\n\t\t\t\t\t\t\tbufferImageGranularity: " + std::to_string(_device_properties.limits.bufferImageGranularity);
					_msg += "\r\n\t\t\t\t\t\t\tdiscreteQueuePriorities: " + std::to_string(_device_properties.limits.discreteQueuePriorities);
					_msg += "\r\n\t\t\t\t\t\t\tframebufferColorSampleCounts: " + std::to_string(_device_properties.limits.framebufferColorSampleCounts);
					_msg += "\r\n\t\t\t\t\t\t\tframebufferDepthSampleCounts: " + std::to_string(_device_properties.limits.framebufferDepthSampleCounts);
					_msg += "\r\n\t\t\t\t\t\t\tframebufferNoAttachmentsSampleCounts: " + std::to_string(_device_properties.limits.framebufferNoAttachmentsSampleCounts);
					_msg += "\r\n\t\t\t\t\t\t\tframebufferStencilSampleCounts: " + std::to_string(_device_properties.limits.framebufferStencilSampleCounts);
					_msg += "\r\n\t\t\t\t\t\t\tlineWidthGranularity: " + std::to_string(_device_properties.limits.lineWidthGranularity);
					_msg += "\r\n\t\t\t\t\t\t\tmaxBoundDescriptorSets: " + std::to_string(_device_properties.limits.maxBoundDescriptorSets);
					_msg += "\r\n\t\t\t\t\t\t\tmaxClipDistances: " + std::to_string(_device_properties.limits.maxClipDistances);
					_msg += "\r\n\t\t\t\t\t\t\tmaxColorAttachments: " + std::to_string(_device_properties.limits.maxColorAttachments);
					_msg += "\r\n\t\t\t\t\t\t\tmaxCombinedClipAndCullDistances: " + std::to_string(_device_properties.limits.maxCombinedClipAndCullDistances);
					_msg += "\r\n\t\t\t\t\t\t\tmaxComputeSharedMemorySize: " + std::to_string(_device_properties.limits.maxComputeSharedMemorySize);
					_msg += "\r\n\t\t\t\t\t\t\tmaxComputeWorkGroupInvocations: " + std::to_string(_device_properties.limits.maxComputeWorkGroupInvocations);
					_msg += "\r\n\t\t\t\t\t\t\tmaxCullDistances: " + std::to_string(_device_properties.limits.maxCullDistances);
					_msg += "\r\n\t\t\t\t\t\t\tmaxDescriptorSetInputAttachments: " + std::to_string(_device_properties.limits.maxDescriptorSetInputAttachments);
					_msg += "\r\n\t\t\t\t\t\t\tmaxDescriptorSetSampledImages: " + std::to_string(_device_properties.limits.maxDescriptorSetSampledImages);
					_msg += "\r\n\t\t\t\t\t\t\tmaxDescriptorSetSamplers: " + std::to_string(_device_properties.limits.maxDescriptorSetSamplers);
					_msg += "\r\n\t\t\t\t\t\t\tmaxDescriptorSetStorageBuffers: " + std::to_string(_device_properties.limits.maxDescriptorSetStorageBuffers);
					_msg += "\r\n\t\t\t\t\t\t\tmaxDescriptorSetStorageBuffersDynamic: " + std::to_string(_device_properties.limits.maxDescriptorSetStorageBuffersDynamic);
					_msg += "\r\n\t\t\t\t\t\t\tmaxDrawIndexedIndexValue: " + std::to_string(_device_properties.limits.maxDrawIndexedIndexValue);
					_msg += "\r\n\t\t\t\t\t\t\tmaxDrawIndirectCount: " + std::to_string(_device_properties.limits.maxDrawIndirectCount);
					_msg += "\r\n\t\t\t\t\t\t\tmaxFragmentCombinedOutputResources: " + std::to_string(_device_properties.limits.maxFragmentCombinedOutputResources);
					_msg += "\r\n\t\t\t\t\t\t\tmaxFragmentDualSrcAttachments: " + std::to_string(_device_properties.limits.maxFragmentDualSrcAttachments);
					_msg += "\r\n\t\t\t\t\t\t\tmaxFragmentInputComponents: " + std::to_string(_device_properties.limits.maxFragmentInputComponents);
					_msg += "\r\n\t\t\t\t\t\t\tmaxFragmentOutputAttachments: " + std::to_string(_device_properties.limits.maxFragmentOutputAttachments);
					_msg += "\r\n\t\t\t\t\t\t\tmaxFramebufferWidth: " + std::to_string(_device_properties.limits.maxFramebufferWidth);
					_msg += "\r\n\t\t\t\t\t\t\tmaxFramebufferHeight: " + std::to_string(_device_properties.limits.maxFramebufferHeight);
					_msg += "\r\n\t\t\t\t\t\t\tmaxFramebufferLayers: " + std::to_string(_device_properties.limits.maxFramebufferLayers);
					_msg += "\r\n\t\t\t\t\t\t\tmaxGeometryInputComponents: " + std::to_string(_device_properties.limits.maxGeometryInputComponents);
					_msg += "\r\n\t\t\t\t\t\t\tmaxGeometryOutputComponents: " + std::to_string(_device_properties.limits.maxGeometryOutputComponents);
					_msg += "\r\n\t\t\t\t\t\t\tmaxGeometryOutputVertices: " + std::to_string(_device_properties.limits.maxGeometryOutputVertices);
					_msg += "\r\n\t\t\t\t\t\t\tmaxGeometryShaderInvocations: " + std::to_string(_device_properties.limits.maxGeometryShaderInvocations);
					_msg += "\r\n\t\t\t\t\t\t\tmaxGeometryTotalOutputComponents: " + std::to_string(_device_properties.limits.maxGeometryTotalOutputComponents);
					_msg += "\r\n\t\t\t\t\t\t\tmaxImageArrayLayers: " + std::to_string(_device_properties.limits.maxImageArrayLayers);
					_msg += "\r\n\t\t\t\t\t\t\tmaxImageDimension1D: " + std::to_string(_device_properties.limits.maxImageDimension1D);
					_msg += "\r\n\t\t\t\t\t\t\tmaxImageDimension2D: " + std::to_string(_device_properties.limits.maxImageDimension2D);
					_msg += "\r\n\t\t\t\t\t\t\tmaxImageDimension3D: " + std::to_string(_device_properties.limits.maxImageDimension3D);
					_msg += "\r\n\t\t\t\t\t\t\tmaxImageDimensionCube: " + std::to_string(_device_properties.limits.maxImageDimensionCube);
					_msg += "\r\n\t\t\t\t\t\t\tmaxInterpolationOffset: " + std::to_string(_device_properties.limits.maxInterpolationOffset);
					_msg += "\r\n\t\t\t\t\t\t\tmaxMemoryAllocationCount: " + std::to_string(_device_properties.limits.maxMemoryAllocationCount);
					_msg += "\r\n\t\t\t\t\t\t\tmaxPerStageDescriptorInputAttachments: " + std::to_string(_device_properties.limits.maxPerStageDescriptorInputAttachments);
					_msg += "\r\n\t\t\t\t\t\t\tmaxPerStageDescriptorSampledImages: " + std::to_string(_device_properties.limits.maxPerStageDescriptorSampledImages);
					_msg += "\r\n\t\t\t\t\t\t\tmaxPerStageDescriptorSamplers: " + std::to_string(_device_properties.limits.maxPerStageDescriptorSamplers);
					_msg += "\r\n\t\t\t\t\t\t\tmaxPerStageDescriptorStorageBuffers: " + std::to_string(_device_properties.limits.maxPerStageDescriptorStorageBuffers);
					_msg += "\r\n\t\t\t\t\t\t\tmaxPerStageDescriptorStorageImages: " + std::to_string(_device_properties.limits.maxPerStageDescriptorStorageImages);
					_msg += "\r\n\t\t\t\t\t\t\tmaxPerStageDescriptorUniformBuffers: " + std::to_string(_device_properties.limits.maxPerStageDescriptorUniformBuffers);
					_msg += "\r\n\t\t\t\t\t\t\tmaxPerStageResources: " + std::to_string(_device_properties.limits.maxPerStageResources);
					_msg += "\r\n\t\t\t\t\t\t\tmaxPushConstantsSize: " + std::to_string(_device_properties.limits.maxPushConstantsSize);
					_msg += "\r\n\t\t\t\t\t\t\tmaxSampleMaskWords: " + std::to_string(_device_properties.limits.maxSampleMaskWords);
					_msg += "\r\n\t\t\t\t\t\t\tmaxSamplerAllocationCount: " + std::to_string(_device_properties.limits.maxSamplerAllocationCount);
					_msg += "\r\n\t\t\t\t\t\t\tmaxSamplerAnisotropy: " + std::to_string(_device_properties.limits.maxSamplerAnisotropy);
					_msg += "\r\n\t\t\t\t\t\t\tmaxSamplerLodBias: " + std::to_string(_device_properties.limits.maxSamplerLodBias);
					_msg += "\r\n\t\t\t\t\t\t\tmaxStorageBufferRange: " + std::to_string(_device_properties.limits.maxStorageBufferRange);
					_msg += "\r\n\t\t\t\t\t\t\tmaxTessellationControlPerPatchOutputComponents: " + std::to_string(_device_properties.limits.maxTessellationControlPerPatchOutputComponents);
					_msg += "\r\n\t\t\t\t\t\t\tmaxTessellationControlPerVertexInputComponents: " + std::to_string(_device_properties.limits.maxTessellationControlPerVertexInputComponents);
					_msg += "\r\n\t\t\t\t\t\t\tmaxTessellationControlPerVertexOutputComponents: " + std::to_string(_device_properties.limits.maxTessellationControlPerVertexOutputComponents);
					_msg += "\r\n\t\t\t\t\t\t\tmaxTessellationControlTotalOutputComponents: " + std::to_string(_device_properties.limits.maxTessellationControlTotalOutputComponents);
					_msg += "\r\n\t\t\t\t\t\t\tmaxTessellationEvaluationInputComponents: " + std::to_string(_device_properties.limits.maxTessellationEvaluationInputComponents);
					_msg += "\r\n\t\t\t\t\t\t\tmaxTessellationEvaluationOutputComponents: " + std::to_string(_device_properties.limits.maxTessellationEvaluationOutputComponents);
					_msg += "\r\n\t\t\t\t\t\t\tmaxTessellationGenerationLevel: " + std::to_string(_device_properties.limits.maxTessellationGenerationLevel);
					_msg += "\r\n\t\t\t\t\t\t\tmaxTessellationPatchSize: " + std::to_string(_device_properties.limits.maxTessellationPatchSize);
					_msg += "\r\n\t\t\t\t\t\t\tmaxTexelBufferElements: " + std::to_string(_device_properties.limits.maxTexelBufferElements);
					_msg += "\r\n\t\t\t\t\t\t\tmaxTexelGatherOffset: " + std::to_string(_device_properties.limits.maxTexelGatherOffset);
					_msg += "\r\n\t\t\t\t\t\t\tmaxTexelOffset: " + std::to_string(_device_properties.limits.maxTexelOffset);
					_msg += "\r\n\t\t\t\t\t\t\tmaxUniformBufferRange: " + std::to_string(_device_properties.limits.maxUniformBufferRange);
					_msg += "\r\n\t\t\t\t\t\t\tmaxVertexInputAttributeOffset: " + std::to_string(_device_properties.limits.maxVertexInputAttributeOffset);
					_msg += "\r\n\t\t\t\t\t\t\tmaxVertexInputAttributes: " + std::to_string(_device_properties.limits.maxVertexInputAttributes);
					_msg += "\r\n\t\t\t\t\t\t\tmaxVertexInputBindingStride: " + std::to_string(_device_properties.limits.maxVertexInputBindingStride);
					_msg += "\r\n\t\t\t\t\t\t\tmaxVertexInputBindings: " + std::to_string(_device_properties.limits.maxVertexInputBindings);
					_msg += "\r\n\t\t\t\t\t\t\tmaxVertexOutputComponents: " + std::to_string(_device_properties.limits.maxVertexOutputComponents);
					_msg += "\r\n\t\t\t\t\t\t\tmaxViewports: " + std::to_string(_device_properties.limits.maxViewports);
					_msg += "\r\n\t\t\t\t\t\t\tminInterpolationOffset: " + std::to_string(_device_properties.limits.minInterpolationOffset);
					_msg += "\r\n\t\t\t\t\t\t\tminMemoryMapAlignment: " + std::to_string(_device_properties.limits.minMemoryMapAlignment);
					_msg += "\r\n\t\t\t\t\t\t\tminStorageBufferOffsetAlignment: " + std::to_string(_device_properties.limits.minStorageBufferOffsetAlignment);
					_msg += "\r\n\t\t\t\t\t\t\tminTexelBufferOffsetAlignment: " + std::to_string(_device_properties.limits.minTexelBufferOffsetAlignment);
					_msg += "\r\n\t\t\t\t\t\t\tminTexelGatherOffset: " + std::to_string(_device_properties.limits.minTexelGatherOffset);
					_msg += "\r\n\t\t\t\t\t\t\tminTexelOffset: " + std::to_string(_device_properties.limits.minTexelOffset);
					_msg += "\r\n\t\t\t\t\t\t\tminUniformBufferOffsetAlignment: " + std::to_string(_device_properties.limits.minUniformBufferOffsetAlignment);
					_msg += "\r\n\t\t\t\t\t\t\tmipmapPrecisionBits: " + std::to_string(_device_properties.limits.mipmapPrecisionBits);
					_msg += "\r\n\t\t\t\t\t\t\tnonCoherentAtomSize: " + std::to_string(_device_properties.limits.nonCoherentAtomSize);
					_msg += "\r\n\t\t\t\t\t\t\toptimalBufferCopyOffsetAlignment: " + std::to_string(_device_properties.limits.optimalBufferCopyOffsetAlignment);
					_msg += "\r\n\t\t\t\t\t\t\toptimalBufferCopyRowPitchAlignment: " + std::to_string(_device_properties.limits.optimalBufferCopyRowPitchAlignment);
					_msg += "\r\n\t\t\t\t\t\t\tpointSizeGranularity: " + std::to_string(_device_properties.limits.pointSizeGranularity);
					_msg += "\r\n\t\t\t\t\t\t\tsampledImageColorSampleCounts: " + std::to_string(_device_properties.limits.sampledImageColorSampleCounts);
					_msg += "\r\n\t\t\t\t\t\t\tsampledImageDepthSampleCounts: " + std::to_string(_device_properties.limits.sampledImageDepthSampleCounts);
					_msg += "\r\n\t\t\t\t\t\t\tsampledImageIntegerSampleCounts: " + std::to_string(_device_properties.limits.sampledImageIntegerSampleCounts);
					_msg += "\r\n\t\t\t\t\t\t\tsampledImageStencilSampleCounts: " + std::to_string(_device_properties.limits.sampledImageStencilSampleCounts);
					_msg += "\r\n\t\t\t\t\t\t\tsparseAddressSpaceSize: " + std::to_string(_device_properties.limits.sparseAddressSpaceSize);
					_msg += "\r\n\t\t\t\t\t\t\tstandardSampleLocations: " + std::to_string(_device_properties.limits.standardSampleLocations);
					_msg += "\r\n\t\t\t\t\t\t\tstorageImageSampleCounts: " + std::to_string(_device_properties.limits.storageImageSampleCounts);
					_msg += "\r\n\t\t\t\t\t\t\tstrictLines: " + std::to_string(_device_properties.limits.strictLines);
					_msg += "\r\n\t\t\t\t\t\t\tsubPixelInterpolationOffsetBits: " + std::to_string(_device_properties.limits.subPixelInterpolationOffsetBits);
					_msg += "\r\n\t\t\t\t\t\t\tsubPixelPrecisionBits: " + std::to_string(_device_properties.limits.subPixelPrecisionBits);
					_msg += "\r\n\t\t\t\t\t\t\tsubTexelPrecisionBits: " + std::to_string(_device_properties.limits.subTexelPrecisionBits);
					_msg += "\r\n\t\t\t\t\t\t\ttimestampComputeAndGraphics: " + std::to_string(_device_properties.limits.timestampComputeAndGraphics);
					_msg += "\r\n\t\t\t\t\t\t\ttimestampPeriod: " + std::to_string(_device_properties.limits.timestampPeriod);
					_msg += "\r\n\t\t\t\t\t\t\tviewportSubPixelBits: " + std::to_string(_device_properties.limits.viewportSubPixelBits);

					//create a shared graphics device for this GPU
					auto _gDevice = std::make_shared<w_graphics_device>();
					_gDevice->device_name = _device_name;
					_gDevice->vk_physical_device = _gpus[i];

					//get memory properties from the physical device or GPU
					vkGetPhysicalDeviceMemoryProperties(_gpus[i], &_gDevice->vk_physical_device_memory_properties);

					for (size_t j = 0; j < _gDevice->vk_physical_device_memory_properties.memoryHeapCount; ++j)
					{
						_msg += "\r\n\t\t\t\t\t\t\tmemoryHeapSize: " + std::to_string(_gDevice->vk_physical_device_memory_properties.memoryHeaps[j].size);
						switch (_gDevice->vk_physical_device_memory_properties.memoryHeaps[j].flags)
						{
						case VK_MEMORY_HEAP_DEVICE_LOCAL_BIT:
							_msg += "\r\n\t\t\t\t\t\t\tmemoryHeapFlag: VK_MEMORY_HEAP_DEVICE_LOCAL_BIT";
							break;
						case VK_MEMORY_HEAP_FLAG_BITS_MAX_ENUM:
							_msg += "\r\n\t\t\t\t\t\t\tmemoryHeapFlag: VK_MEMORY_HEAP_FLAG_BITS_MAX_ENUM";
							break;
						}
					}

					for (size_t j = 0; j < _gDevice->vk_physical_device_memory_properties.memoryTypeCount; ++j)
					{
						_msg += "\r\n\t\t\t\t\t\t\tmemoryTypeHeapIndex: " +
							std::to_string(_gDevice->vk_physical_device_memory_properties.memoryTypes[j].heapIndex);
						switch (_gDevice->vk_physical_device_memory_properties.memoryTypes[j].propertyFlags)
						{
						case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
							_msg += "\r\n\t\t\t\t\t\t\t\tmemoryTypeFlag: VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT";
							break;
						case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT:
							_msg += "\r\n\t\t\t\t\t\t\t\tmemoryTypeFlag: VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT";
							break;
						case VK_MEMORY_PROPERTY_HOST_COHERENT_BIT:
							_msg += "\r\n\t\t\t\t\t\t\t\tmemoryTypeFlag: VK_MEMORY_PROPERTY_HOST_COHERENT_BIT";
							break;
						case VK_MEMORY_PROPERTY_HOST_CACHED_BIT:
							_msg += "\r\n\t\t\t\t\t\t\t\tmemoryTypeFlag: VK_MEMORY_PROPERTY_HOST_CACHED_BIT";
							break;
						case VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT:
							_msg += "\r\n\t\t\t\t\t\t\t\tmemoryTypeFlag: VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT";
							break;
						case VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM:
							_msg += "\r\n\t\t\t\t\t\t\t\tmemoryTypeFlag: VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM";
							break;
						}
					}

					//write buffer to output
					logger.write(_msg);

					//get queue family from default GPU
					uint32_t _queue_family_property_count = 0;
					vkGetPhysicalDeviceQueueFamilyProperties(_gpus[i],
						&_queue_family_property_count,
						nullptr);
					V(_queue_family_property_count == 0 ? S_FALSE : S_OK,
						L"could not find queue family for default GPU", this->_name, 3, true, true);

					_gDevice->vk_queue_family_properties.resize(_queue_family_property_count);
					_gDevice->vk_queue_family_supports_present.resize(_queue_family_property_count);

					vkGetPhysicalDeviceQueueFamilyProperties(_gpus[i],
						&_queue_family_property_count,
						_gDevice->vk_queue_family_properties.data());

					_msg.clear();
					bool _queue_graphics_bit_found = false;
					for (size_t j = 0; j < _queue_family_property_count; ++j)
					{
						_msg += "\r\n\t\t\t\t\t\t_queue_family_properties: " + std::to_string(j);
						if (_gDevice->vk_queue_family_properties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
						{
							_queue_graphics_bit_found = true;
							_gDevice->vk_queue_family_selected_index = j;
							_msg += "\r\n\t\t\t\t\t\t\tVK_QUEUE_GRAPHICS_BIT supported";
						}
						if (_gDevice->vk_queue_family_properties[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
						{
							_msg += "\r\n\t\t\t\t\t\t\tVK_QUEUE_COMPUTE_BIT supported";
						}
						if (_gDevice->vk_queue_family_properties[j].queueFlags & VK_QUEUE_TRANSFER_BIT)
						{
							_msg += "\r\n\t\t\t\t\t\t\tVK_QUEUE_TRANSFER_BIT supported";
						}
						if (_gDevice->vk_queue_family_properties[j].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
						{
							_msg += "\r\n\t\t\t\t\t\t\tVK_QUEUE_SPARSE_BINDING_BIT supported";
						}
					}

					logger.write(_msg);
					V(!_queue_graphics_bit_found || _queue_family_property_count == 0 ? S_FALSE : S_OK,
						L"could not find queue family with VK_QUEUE_GRAPHICS_BIT for default GPU", this->_name, 3, true, true);

					//create queue info
					float _queue_priorities[1] = { 0.0 };
					VkDeviceQueueCreateInfo _queue_info = {};
					_queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					_queue_info.pNext = nullptr;
					_queue_info.queueCount = 1;
					_queue_info.pQueuePriorities = _queue_priorities;

					//create device info
					VkDeviceCreateInfo _device_info = {};
					_device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
					_device_info.pNext = nullptr;
					_device_info.queueCreateInfoCount = 1;
					_device_info.pQueueCreateInfos = &_queue_info;
					_device_info.enabledExtensionCount = 0;
					_device_info.ppEnabledExtensionNames = nullptr;
					_device_info.enabledLayerCount = 0;
					_device_info.ppEnabledLayerNames = nullptr;
					_device_info.pEnabledFeatures = nullptr;

					//create device
					_hr = vkCreateDevice(_gpus[i], &_device_info, nullptr, &_gDevice->vk_device);
					V(_hr, L"could not create Vulkan device", this->_name, 3, true, true);

					auto _win = this->_windows_info.find(static_cast<int>(i));
					if (_win != this->_windows_info.end())
					{
						for (size_t j = 0; j < _win->second.size(); ++j)
						{
							auto _window = _win->second[j];

							w_output_presentation_window _out_window;
							_out_window.width = _window.width;
							_out_window.height = _window.height;
							_out_window.aspectRatio = (float)_window.width / (float)_window.height;
							_out_window.v_sync = _window.v_sync_enable;
							_out_window.index = static_cast<int>(j);
#ifdef __WIN32
							_out_window.HWND = _window.hwnd;
							_out_window.hInstance = _window.hInstance;
#elif defined(__linux)
							_out_window.xcb_connection = _window.xcb_connection;
							_out_window.xcb_window = _window.xcb_window;
#elif defined(__APPLE__)
							_out_window.window = _window.window;
#endif

#if defined(VK_USE_PLATFORM_WIN32_KHR)
							VkWin32SurfaceCreateInfoKHR surface_create_info =
							{
								VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,    // VkStructureType                  sType
								nullptr,                                            // const void                      *pNext
								0,                                                  // VkWin32SurfaceCreateFlagsKHR     flags
								_out_window.Instance,                               // HINSTANCE                        hinstance
								_out_window.Handle                                  // HWND                             hwnd
							};

							_hr = vkCreateWin32SurfaceKHR(Vulkan.Instance, &surface_create_info, nullptr, &Vulkan.PresentationSurface);
							V(_hr, "create win32 surface for Vulkan", this->_name, 3, true, true);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
							VkXcbSurfaceCreateInfoKHR _surface_create_info =
							{
								VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,      // VkStructureType                  sType
								nullptr,                                            // const void                      *pNext
								0,                                                  // VkXcbSurfaceCreateFlagsKHR       flags
								_out_window.xcb_connection,                         // xcb_connection_t*                connection
								(*_out_window.xcb_window)                           // xcb_window_t                     window
							};
							_hr = vkCreateXcbSurfaceKHR(w_graphics_device::vk_instance,
								&_surface_create_info,
								nullptr,
								&_out_window.vk_presentation_surface);
							V(_hr, L"create xcb surface for Vulkan", this->_name, 3, true, true);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
							VkXlibSurfaceCreateInfoKHR surface_create_info =
							{
								VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,     // VkStructureType                sType
								nullptr,                                            // const void                    *pNext
								0,                                                  // VkXlibSurfaceCreateFlagsKHR    flags
								_window.DisplayPtr,                                 // Display                       *dpy
								_window.Handle                                      // Window                         window
							};
							_hr = vkCreateXlibSurfaceKHR(Vulkan.Instance,
								&surface_create_info, nullptr, &Vulkan.PresentationSurface);
							V(_hr, "create xlib surface for Vulkan", _super::name, 3, true, true);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
							VkMacOSSurfaceCreateInfoMVK _surface_create_info = {};
							_surface_create_info.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
							_surface_create_info.pNext = NULL;
							_surface_create_info.flags = 0;
							_surface_create_info.pView = _window.window;
							_hr = vkCreateMacOSSurfaceMVK(w_graphics_device::vk_instance,
								&_surface_create_info,
								NULL,
								&_out_window.vk_presentation_surface);
							V(_hr, L"create macOS surface for Vulkan", this->_name, 3, true, true);

#endif

							_gDevice->output_presentation_windows.push_back(_out_window);

							_create_swap_chain(_gDevice, j);
							_create_depth_buffer(_gDevice, j);
							_create_synchronization(_gDevice, j);
							_create_command_queue(_gDevice, j);
						}
					}

					//get graphics and presentation queues (which may be the same)
					vkGetDeviceQueue(_gDevice->vk_device,
						static_cast<uint32_t>(_gDevice->vk_queue_family_selected_index),
						0,
						&_gDevice->vk_graphics_queue);

					vkGetDeviceQueue(_gDevice->vk_device,
						static_cast<uint32_t>(_gDevice->vk_queue_family_selected_support_present_index),
						0,
						&_gDevice->vk_present_queue);

					pGraphicsDevices.push_back(_gDevice);
				}
#endif //__DX12__
			}
            
            //Release all resources
            ULONG release()
            {
                if (this->_is_released)  return 0;
                
                //release all windows info
                this->_is_released = true;
                this->_windows_info.clear();
                this->_name = "";

                return 1;
            }
            
#pragma region Getters
            
            w_color get_clear_color() const
            {
                return this->_clear_color;
            }
            
            bool get_is_released() const
            {
                return this->_is_released;
            }
            
#pragma endregion
            
#pragma region Setters
            
            void set_output_windows_info(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
            {
                this->_windows_info = pOutputWindowsInfo;
            }
            
            void set_clear_color(_In_ const w_color pColor)
            {
                this->_clear_color = pColor;
            }
#pragma endregion
            
        private:
    
            void _create_swap_chain(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                                    _In_ size_t pOutputPresentationWindowIndex)
            {
                auto _device_name =  wolf::system::convert::string_to_wstring(pGDevice->device_name);
                auto _output_presentation_window = &(pGDevice->output_presentation_windows.at(pOutputPresentationWindowIndex));

#ifdef __DX12__

#elif defined(__VULKAN__)
                auto _vk_presentation_surface = _output_presentation_window->vk_presentation_surface;
                
                pGDevice->vk_queue_family_selected_support_present_index = SIZE_MAX;
                for (size_t j = 0; j < pGDevice->vk_queue_family_properties.size(); ++j)
                {
                    //check if this device support presentation
                    auto _hr = vkGetPhysicalDeviceSurfaceSupportKHR(pGDevice->vk_physical_device,
                                                                     static_cast<uint32_t>(j),
                                                                    _vk_presentation_surface,
                                                                    &pGDevice->vk_queue_family_supports_present[j]);
                    V(_hr, L"could not get physical device surface support for graphics device: " + _device_name +
                      L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                      this->_name, 2);
                    
                    if (pGDevice->vk_queue_family_selected_support_present_index == SIZE_MAX && 
                        pGDevice->vk_queue_family_selected_index != SIZE_MAX &&
                        pGDevice->vk_queue_family_supports_present[j])
                    {
                        pGDevice->vk_queue_family_selected_support_present_index = j;
                    }
                }
                
                V(pGDevice->vk_queue_family_selected_support_present_index == SIZE_MAX ? S_FALSE : S_OK,
                  L"could not find queue family which supports presentation for graphics device: " + _device_name +
                  L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  this->_name, 2);
                
                //get the list of VkFormats that are supported:
                uint32_t _vk_format_count;
                auto _hr = vkGetPhysicalDeviceSurfaceFormatsKHR(pGDevice->vk_physical_device,
                                                                _vk_presentation_surface,
                                                                &_vk_format_count,
                                                                NULL);
                V(_hr, L"could not get number of physical device surface formats for graphics device: " +
                   wolf::system::convert::string_to_wstring(pGDevice->device_name) +
                   L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                   this->_name, 3, true, true);
                
                _output_presentation_window->vk_surface_formats.resize(_vk_format_count);
                
                _hr = vkGetPhysicalDeviceSurfaceFormatsKHR(pGDevice->vk_physical_device,
                                                           _vk_presentation_surface,
                                                           &_vk_format_count,
                                                           _output_presentation_window->vk_surface_formats.data());
                V(_hr, L"could not get physical device surface formats for graphics device: " + _device_name +
                   L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                   this->_name, 3, true, true);
                
                /*
                    If the format list includes just one entry of VK_FORMAT_UNDEFINED,
                    the surface has no preferred format.  Otherwise, at least one
                    supported format will be returned.
                */
                if (_vk_format_count == 1 && _output_presentation_window->vk_surface_formats[0].format == VK_FORMAT_UNDEFINED)
                {
                    _output_presentation_window->vk_swap_chain_selected_format = VK_FORMAT_B8G8R8A8_UNORM;
                }
                else
                {
                    _output_presentation_window->vk_swap_chain_selected_format = _output_presentation_window->vk_surface_formats[0].format;
                }
                
                VkSurfaceCapabilitiesKHR _surface_capabilities;
                _hr = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pGDevice->vk_physical_device,
                                                                _vk_presentation_surface,
                                                                &_surface_capabilities);
                V(_hr, L"create vulkan surface capabilities for graphics device: " + _device_name +
                   L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                   this->_name, 3, true, true);
                
                //get the count of present modes
                uint32_t _present_mode_count;
                _hr = vkGetPhysicalDeviceSurfacePresentModesKHR(pGDevice->vk_physical_device,
                                                                _vk_presentation_surface,
                                                                &_present_mode_count, nullptr);
                V(_hr, L"getting vulkan present mode(s) count for graphics device: " + _device_name +
                  L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  this->_name, 3, true, true);
                
                //get present modes
                std::vector<VkPresentModeKHR> _present_modes(_present_mode_count);
                _hr = vkGetPhysicalDeviceSurfacePresentModesKHR(pGDevice->vk_physical_device,
                                                                _vk_presentation_surface,
                                                                &_present_mode_count,
                                                                _present_modes.data());
                V(_hr, L"getting vulkan present mode(s) for graphics device: " + _device_name +
                  L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  this->_name, 3, true, true);
                
                // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
                VkExtent2D _swap_chain_extent;
                if (_surface_capabilities.currentExtent.width == 0xFFFFFFFF)
                {
                    // If the surface size is undefined, the size is set to the size of the images requested.
                    _swap_chain_extent.width =  _output_presentation_window->width;
                    _swap_chain_extent.height = _output_presentation_window->height;
                    
                    if (_swap_chain_extent.width < _surface_capabilities.minImageExtent.width)
                    {
                        _swap_chain_extent.width = _surface_capabilities.minImageExtent.width;
                    }
                    else if (_swap_chain_extent.width > _surface_capabilities.maxImageExtent.width)
                    {
                        _swap_chain_extent.width = _surface_capabilities.maxImageExtent.width;
                    }
                    
                    if (_swap_chain_extent.height < _surface_capabilities.minImageExtent.height)
                    {
                        _swap_chain_extent.height = _surface_capabilities.minImageExtent.height;
                    }
                    else if (_swap_chain_extent.height > _surface_capabilities.maxImageExtent.height)
                    {
                        _swap_chain_extent.height = _surface_capabilities.maxImageExtent.height;
                    }
                }
                else
                {
                    // If the surface size is defined, the swap chain size must match
                    _swap_chain_extent = _surface_capabilities.currentExtent;
                }
                
                //the FIFO present mode is guaranteed by the spec to be supported
                VkPresentModeKHR _swap_chain_present_mode =  _output_presentation_window->v_sync ?
                VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
                
                /*
                    Determine the number of VkImage's to use in the swap chain.
                    We need to acquire only 1 presentable image at at time.
                    Asking for minImageCount images ensures that we can acquire
                    1 presentable image as long as we present it before attempting
                    to acquire another.
                */
                uint32_t _desired_number_of_swapchain_images = _surface_capabilities.minImageCount;
                VkSurfaceTransformFlagBitsKHR _pre_transform;
                if (_surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
                {
                    _pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
                }
                else
                {
                    _pre_transform = _surface_capabilities.currentTransform;
                }
                
                VkSwapchainCreateInfoKHR _swap_chain_create_info = {};
                _swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                _swap_chain_create_info.pNext = nullptr;
                _swap_chain_create_info.surface =  _vk_presentation_surface;
                _swap_chain_create_info.minImageCount = _desired_number_of_swapchain_images;
                _swap_chain_create_info.imageFormat = _output_presentation_window->vk_swap_chain_selected_format;
                _swap_chain_create_info.imageExtent.width = _swap_chain_extent.width;
                _swap_chain_create_info.imageExtent.height = _swap_chain_extent.height;
                _swap_chain_create_info.preTransform = _pre_transform;
                _swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                _swap_chain_create_info.imageArrayLayers = 1;
                _swap_chain_create_info.presentMode = _swap_chain_present_mode;
                _swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;
                _swap_chain_create_info.clipped = true;
                _swap_chain_create_info.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
                _swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                _swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                _swap_chain_create_info.queueFamilyIndexCount = 0;
                _swap_chain_create_info.pQueueFamilyIndices = nullptr;
                
                uint32_t _queue_family_indices[2] =
                {
                    (uint32_t)pGDevice->vk_queue_family_selected_index,
                    (uint32_t)pGDevice->vk_queue_family_selected_support_present_index,
                };
                if (_queue_family_indices[0] != _queue_family_indices[1])
                {
                    /*
                        If the graphics and present queues are from different queue families,
                        we either have to explicitly transfer ownership of images between
                        the queues, or we have to create the swap chain with imageSharingMode
                        as VK_SHARING_MODE_CONCURRENT
                     */
                    _swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                    _swap_chain_create_info.queueFamilyIndexCount = 2;
                    _swap_chain_create_info.pQueueFamilyIndices = _queue_family_indices;
                }
                
                //create swap chain
                _hr = vkCreateSwapchainKHR(pGDevice->vk_device,
                                           &_swap_chain_create_info,
                                           nullptr,
                                           &_output_presentation_window->vk_swap_chain);
                V(_hr, L"creating swap chain for vulkan for graphics device: " + _device_name +
                  L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  this->_name, 3, true, true);
                
                //get the count of swap chain 's images
                uint32_t _swap_chain_image_count = UINT32_MAX;
                _hr = vkGetSwapchainImagesKHR(pGDevice->vk_device,
                                              _output_presentation_window->vk_swap_chain,
                                              &_swap_chain_image_count,
                                              nullptr);
                V(_hr, L"getting total available image counts of swap chain for graphics device: " +
                  _device_name + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  this->_name, 3, true, true);
                
                std::vector<VkImage> _swap_chain_images(_swap_chain_image_count);
                _hr = vkGetSwapchainImagesKHR(pGDevice->vk_device,
                                              _output_presentation_window->vk_swap_chain,
                                              &_swap_chain_image_count,
                                              _swap_chain_images.data());
                V(_hr, L"getting total available images of swap chain for graphics device: " +
                  _device_name + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  this->_name, 3, true, true);
                
                for (size_t j = 0; j < _swap_chain_images.size(); ++j)
                {
                    VkImageViewCreateInfo _color_image_view = {};
                    
                    _color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    _color_image_view.pNext = nullptr;
                    _color_image_view.flags = 0;
                    _color_image_view.image = _swap_chain_images[j];
                    _color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    _color_image_view.format = _output_presentation_window->vk_swap_chain_selected_format;
                    _color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
                    _color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
                    _color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
                    _color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
                    _color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    _color_image_view.subresourceRange.baseMipLevel = 0;
                    _color_image_view.subresourceRange.levelCount = 1;
                    _color_image_view.subresourceRange.baseArrayLayer = 0;
                    _color_image_view.subresourceRange.layerCount = 1;
                    
                    vk_image_view _image_view;
                    _image_view.image = _swap_chain_images[j];
                    
                    _hr = vkCreateImageView(pGDevice->vk_device,
                                            &_color_image_view, 
                                            nullptr,
                                            &_image_view.view);
                    V(_hr, L"creating image view total available images of swap chain for graphics device: " +
                      _device_name + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                      this->_name, 3, true, true);
                    
                    _output_presentation_window->vk_swap_chain_image_views.push_back(_image_view);
                }
#endif //__DX12__
            }
            
            void _create_depth_buffer(_In_ const std::shared_ptr<w_graphics_device>& pGDevice, _In_ size_t pOutputPresentationWindowIndex)
            {
                auto _device_name =  wolf::system::convert::string_to_wstring(pGDevice->device_name);
                auto _output_presentation_window = &(pGDevice->output_presentation_windows.at(pOutputPresentationWindowIndex));

#ifdef __DX12__ 

#elif defined(__VULKAN__)

                VkImageCreateInfo _depth_buffer_image_info = {};
                const VkFormat _depth_format = VK_FORMAT_D16_UNORM;
                
                VkFormatProperties _properties;
                vkGetPhysicalDeviceFormatProperties(pGDevice->vk_physical_device, _depth_format, &_properties);
                
                if (_properties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) 
                {
                    _depth_buffer_image_info.tiling = VK_IMAGE_TILING_LINEAR;
                } 
                else if (_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) 
                {
                    _depth_buffer_image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
                } 
                else 
                {
                    V(S_FALSE, L"VK_FORMAT_D16_UNORM Unsupported for depth buffer for graphics device: " +
                      _device_name + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                      this->_name, 3, true, true);
                }
                
                auto _window = pGDevice->output_presentation_windows.at(pOutputPresentationWindowIndex);
                
                _depth_buffer_image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                _depth_buffer_image_info.pNext = nullptr;
                _depth_buffer_image_info.imageType = VK_IMAGE_TYPE_2D;
                _depth_buffer_image_info.format = _depth_format;
                _depth_buffer_image_info.extent.width = _window.width;
                _depth_buffer_image_info.extent.height = _window.height;
                _depth_buffer_image_info.extent.depth = 1;
                _depth_buffer_image_info.mipLevels = 1;
                _depth_buffer_image_info.arrayLayers = 1;
                _depth_buffer_image_info.samples = NUM_SAMPLES;
                _depth_buffer_image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                _depth_buffer_image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                _depth_buffer_image_info.queueFamilyIndexCount = 0;
                _depth_buffer_image_info.pQueueFamilyIndices = nullptr;
                _depth_buffer_image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                _depth_buffer_image_info.flags = 0;
                
                VkMemoryAllocateInfo _mem_alloc = {};
                _mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                _mem_alloc.pNext = nullptr;
                _mem_alloc.allocationSize = 0;
                _mem_alloc.memoryTypeIndex = 0;
                
                _output_presentation_window->vk_depth_buffer_format = _depth_format;
                
                //Create image
                auto _hr = vkCreateImage(pGDevice->vk_device,
                                         &_depth_buffer_image_info, 
                                         nullptr, 
                                         &_output_presentation_window->vk_depth_buffer_image_view.image);
                V(_hr, L"creating depth buffer for graphics device: " + 
                  _device_name + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  this->_name, 3, true, true);
                
                VkMemoryRequirements _mem_reqs;
                vkGetImageMemoryRequirements(pGDevice->vk_device,
                                             _output_presentation_window->vk_depth_buffer_image_view.image, 
                                             &_mem_reqs);
                
                _mem_alloc.allocationSize = _mem_reqs.size;
                
                //use the memory properties to determine the type of memory required
                _hr = w_graphics_device_manager::memory_type_from_properties(pGDevice->vk_physical_device_memory_properties,
                                                   _mem_reqs.memoryTypeBits, 
                                                   0, // No Requirements
                                                   &_mem_alloc.memoryTypeIndex);
                V(_hr, L"determining the type of memory required from memory properties for graphics device: " +
                  _device_name + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  this->_name, 3, true, true);
                
                //allocate memory
                _hr = vkAllocateMemory(pGDevice->vk_device,
                                       &_mem_alloc, 
                                       nullptr, 
                                       &_output_presentation_window->vk_depth_buffer_memory);
                V(_hr, L"allocating memory for depth buffer image for graphics device: " + 
                  _device_name + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  this->_name, 3, true, true);
                
                //bind memory
                _hr = vkBindImageMemory(pGDevice->vk_device,
                                        _output_presentation_window->vk_depth_buffer_image_view.image, 
                                        _output_presentation_window->vk_depth_buffer_memory, 
                                        0);
                V(_hr, L"binding to memory for depth buffer image for graphics device: " +
                  _device_name + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  this->_name, 3, true, true);
                
                //create depth buffer image view
                VkImageViewCreateInfo _depth_buffer_view_info = {};
                _depth_buffer_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                _depth_buffer_view_info.pNext = nullptr;
                _depth_buffer_view_info.image = _output_presentation_window->vk_depth_buffer_image_view.image;
                _depth_buffer_view_info.format = _depth_format;
                _depth_buffer_view_info.components.r = VK_COMPONENT_SWIZZLE_R;
                _depth_buffer_view_info.components.g = VK_COMPONENT_SWIZZLE_G;
                _depth_buffer_view_info.components.b = VK_COMPONENT_SWIZZLE_B;
                _depth_buffer_view_info.components.a = VK_COMPONENT_SWIZZLE_A;
                _depth_buffer_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                _depth_buffer_view_info.subresourceRange.baseMipLevel = 0;
                _depth_buffer_view_info.subresourceRange.levelCount = 1;
                _depth_buffer_view_info.subresourceRange.baseArrayLayer = 0;
                _depth_buffer_view_info.subresourceRange.layerCount = 1;
                _depth_buffer_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
                _depth_buffer_view_info.flags = 0;
                
                _hr = vkCreateImageView(pGDevice->vk_device,
                                        &_depth_buffer_view_info, 
                                        nullptr, 
                                        &_output_presentation_window->vk_depth_buffer_image_view.view);
                V(_hr, L"creating image view for depth buffer image for graphics device: " + 
                  _device_name + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  this->_name, 3, true, true);
#endif
            }
            
            void _create_synchronization(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                                    _In_ size_t pOutputPresentationWindowIndex)
            {
#ifdef __DX12__ 

#elif defined(__VULKAN__)
                auto _device_name = wolf::system::convert::string_to_wstring(pGDevice->device_name);
                auto _output_presentation_window = &(pGDevice->output_presentation_windows.at(pOutputPresentationWindowIndex));
                
                VkSemaphoreCreateInfo _create_info = {};
                _create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                
                auto _hr = vkCreateSemaphore(pGDevice->vk_device,
                                             &_create_info,
                                             nullptr,
                                             &_output_presentation_window->vk_image_is_available_semaphore);
                V(_hr, L"creating image_is_available semaphore for graphics device: " + _device_name +
                  L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  _name, 3, true, true);
                
                _hr = vkCreateSemaphore(pGDevice->vk_device,
                                        &_create_info,
                                        nullptr,
                                        &_output_presentation_window->vk_rendering_done_semaphore);
                V(_hr, L"creating rendering_is_done semaphore for graphics device: " + _device_name +
                  L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  _name, 3, true, true);
#endif
            }
            
            void _create_command_queue(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                                       _In_ size_t pOutputPresentationWindowIndex)
            {
#ifdef __DX12__ 
				//// Describe and create the command queue.
				//D3D12_COMMAND_QUEUE_DESC queueDesc = {};
				//queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				//queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

				//ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

				//ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

#elif defined(__VULKAN__)
                auto _device_name = wolf::system::convert::string_to_wstring(pGDevice->device_name);
                auto _output_presentation_window = &(pGDevice->output_presentation_windows.at(pOutputPresentationWindowIndex));
                //auto _vk_presentation_surface = _output_presentation_window->vk_presentation_surface;
                
                //create a command pool to allocate our command buffer from
                VkCommandPoolCreateInfo _command_pool_info = {};
                _command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                _command_pool_info.pNext = nullptr;
                _command_pool_info.queueFamilyIndex =  static_cast<uint32_t>(pGDevice->vk_queue_family_selected_index);
                _command_pool_info.flags = 0;
                
                auto _hr = vkCreateCommandPool(pGDevice->vk_device,
                                               &_command_pool_info,
                                               nullptr,
                                               &_output_presentation_window->vk_command_allocator_pool);
                V(_hr, L"creating vulkan command pool for graphics device: " + _device_name +
                  L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  this->_name, 3, true, true);
                
                //resize command buffers based on swap chains
                auto _command_buffer_counts = static_cast<uint32_t>(_output_presentation_window->vk_swap_chain_image_views.size());
                
                //create the command buffer from the command pool
                VkCommandBufferAllocateInfo _command_buffer_info = {};
                _command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                _command_buffer_info.pNext = nullptr;
                _command_buffer_info.commandPool = _output_presentation_window->vk_command_allocator_pool;
                _command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                _command_buffer_info.commandBufferCount = _command_buffer_counts;
                
                _output_presentation_window->vk_command_queues.resize(_command_buffer_counts);
                
                _hr = vkAllocateCommandBuffers(pGDevice->vk_device,
                                               &_command_buffer_info,
                                               _output_presentation_window->vk_command_queues.data());
                V(_hr, L"creating vulkan command buffers for swap chain of graphics device: " + _device_name +
                  L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                  this->_name, 3, true, true);
                
                //prepare data for recording command buffers
                VkCommandBufferBeginInfo _command_buffer_begin_info = {};
                _command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                _command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
                
                VkImageSubresourceRange _sub_resource_range = {};
                _sub_resource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                _sub_resource_range.baseMipLevel = 0;
                _sub_resource_range.levelCount = 1;
                _sub_resource_range.baseArrayLayer = 0;
                _sub_resource_range.layerCount = 1;
                
                VkClearColorValue _vk_clear_color =
                {
                    static_cast<float>(this->_clear_color.r / 255.0f),
                    static_cast<float>(this->_clear_color.g / 255.0f),
                    static_cast<float>(this->_clear_color.b / 255.0f),
                    static_cast<float>(this->_clear_color.a / 255.0f)
                };
                
                //record the command buffer for every swap chain image
                for (uint32_t i = 0; i < _output_presentation_window->vk_swap_chain_image_views.size(); ++i)
                {
                    // Change layout of image to be optimal for clearing
                    // Note: previous layout doesn't matter, which will likely cause contents to be discarded
                    VkImageMemoryBarrier _present_to_clear_barrier = {};
                    _present_to_clear_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    _present_to_clear_barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                    _present_to_clear_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    _present_to_clear_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                    _present_to_clear_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    _present_to_clear_barrier.srcQueueFamilyIndex =  static_cast<uint32_t>(pGDevice->vk_queue_family_selected_support_present_index);
                    _present_to_clear_barrier.dstQueueFamilyIndex =  static_cast<uint32_t>(pGDevice->vk_queue_family_selected_support_present_index);
                    _present_to_clear_barrier.image = _output_presentation_window->vk_swap_chain_image_views[i].image;
                    _present_to_clear_barrier.subresourceRange = _sub_resource_range;
                    
                    //change layout of image to be optimal for presenting
                    VkImageMemoryBarrier _clear_to_present_barrier = {};
                    _clear_to_present_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    _clear_to_present_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    _clear_to_present_barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                    _clear_to_present_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    _clear_to_present_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                    _clear_to_present_barrier.srcQueueFamilyIndex =  static_cast<uint32_t>(pGDevice->vk_queue_family_selected_support_present_index);
                    _clear_to_present_barrier.dstQueueFamilyIndex =  static_cast<uint32_t>(pGDevice->vk_queue_family_selected_support_present_index);
                    _clear_to_present_barrier.image = _output_presentation_window->vk_swap_chain_image_views[i].image;
                    _clear_to_present_barrier.subresourceRange = _sub_resource_range;
                    
                    //record command buffer
                    _hr = vkBeginCommandBuffer(_output_presentation_window->vk_command_queues[i],
                                               &_command_buffer_begin_info);
                    
                    V(_hr, L"beginning command buffer of graphics device: " + _device_name +
                      L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                      this->_name, 3, true, true);
                    
                    vkCmdPipelineBarrier(_output_presentation_window->vk_command_queues[i],
                                         VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                         0,
                                         0,
                                         nullptr,
                                         0,
                                         nullptr,
                                         1,
                                         &_present_to_clear_barrier);
                    
                    vkCmdClearColorImage(_output_presentation_window->vk_command_queues[i],
                                         _output_presentation_window->vk_swap_chain_image_views[i].image, 
                                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                                         &_vk_clear_color, 
                                         1, 
                                         &_sub_resource_range);
                    
                    vkCmdPipelineBarrier(_output_presentation_window->vk_command_queues[i],
                                         VK_PIPELINE_STAGE_TRANSFER_BIT, 
                                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
                                         0, 
                                         0, 
                                         nullptr, 
                                         0, 
                                         nullptr, 
                                         1, 
                                         &_clear_to_present_barrier);
                    
                    _hr = vkEndCommandBuffer(_output_presentation_window->vk_command_queues[i]);
                    V(_hr, L"ending command buffer of graphics device: " + _device_name +
                      L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                      this->_name, 3, true, true);
                }
#endif
            }
                        
            bool                                                _is_released;
			w_graphics_device_manager_configs					_config;
            std::map<int, std::vector<w_window_info>>           _windows_info;
            w_color                                             _clear_color;
            std::string                                         _name;
        };
    }
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

w_graphics_device_manager::w_graphics_device_manager(_In_ w_graphics_device_manager_configs pConfig) : _pimp(new w_graphics_device_manager_pimp(pConfig))
{
	_super::set_class_name(typeid(this).name());
        
#ifdef __WIN32
        auto _hr = CoInitialize(NULL);
	V(_hr, L"CoInitialize", _super::name, 3, true, false);
#endif
}

w_graphics_device_manager::~w_graphics_device_manager()
{
	release();
}

void w_graphics_device_manager::initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
    //store information of windows for the first time
    std::call_once(sOnceFlag,[this, pOutputWindowsInfo]()
    {
         this->_pimp->set_output_windows_info(pOutputWindowsInfo);
    });
    
    this->_pimp->enumerate_devices(this->graphics_devices);
    
    //If there is no associated graphics device
    if (this->graphics_devices.size() == 0)
    {
        V(S_FALSE, L"getting graphics device. No device available", _super::name, 3, true, false);
    }
}

void w_graphics_device_manager::on_device_lost()
{
//	logger.write(L"Device is going to reset");
//	
//	initialize();
//	initialize_output_windows(this->_windows_info);
//	
//	logger.write(L"Device reseted successfully");
}

void w_graphics_device_manager::on_window_resized(UINT pIndex)
{
//	if (pIndex >= this->graphics_devices.size()) return;
//
//	auto _gDevice = this->graphics_devices.at(pIndex);
//	auto _size = _gDevice->output_windows.size();
//	for (size_t j = 0; j < _size; ++j)
//	{
//		auto _window = _gDevice->output_windows.at(j);
//		_create_swapChain_for_window(_gDevice, _window);
//	}
}

void w_graphics_device_manager::begin_render()
{
    for (size_t i = 0; i < this->graphics_devices.size(); ++i)
    {
        auto _gDevice  = this->graphics_devices[i];
        for (size_t j = 0 ; j < _gDevice->output_presentation_windows.size(); ++j)
        {
            auto _present_window = &(_gDevice->output_presentation_windows[j]);
            
#ifdef __DX12__

#elif defined(__VULKAN__)
            auto _hr = vkAcquireNextImageKHR(_gDevice->vk_device,
                                            _present_window->vk_swap_chain,
                                             UINT64_MAX,
                                             _present_window->vk_image_is_available_semaphore,
                                             VK_NULL_HANDLE,
                                             &_present_window->vk_swap_chain_image_index);
            
            if (_hr != VK_SUCCESS && _hr != VK_SUBOPTIMAL_KHR)
            {
                V(S_FALSE, L"acquiring image of graphics device's swap chain : " + 
                    std::to_wstring(i) + L" and presentation window: " + std::to_wstring(j), 
                    _super::name, 3, true, true);
            }

            //wait for image to be available and draw
            VkSubmitInfo _submit_info = {};
            _submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            _submit_info.waitSemaphoreCount = 1;
            _submit_info.pWaitSemaphores = &_present_window->vk_image_is_available_semaphore;

            _submit_info.signalSemaphoreCount = 1;
            _submit_info.pSignalSemaphores = &_present_window->vk_rendering_done_semaphore;

            //this is the stage where the queue should wait on the semaphore (it doesn't have to wait with drawing, for example)
            VkPipelineStageFlags _wait_dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            _submit_info.pWaitDstStageMask = &_wait_dst_stage_mask;

            _submit_info.commandBufferCount = 1;
            _submit_info.pCommandBuffers = &_present_window->vk_command_queues[_present_window->vk_swap_chain_image_index];
            
            //submit queue
            _hr = vkQueueSubmit(
                    _gDevice->vk_present_queue, 
                    1, 
                    &_submit_info, 
                    VK_NULL_HANDLE);
            V(_hr, L"presenting queue graphics device: " + 
                    std::to_wstring(i) + L" and presentation window: " + std::to_wstring(j), 
                    _super::name, 3, true, true);
#endif
        }
    }
}

void w_graphics_device_manager::end_render()
{
    for (size_t i = 0; i < this->graphics_devices.size(); ++i)
    {
        auto _gDevice  = this->graphics_devices[i];
        for (size_t j = 0 ; j < _gDevice->output_presentation_windows.size(); ++j)
        {
            auto _present_window = &(_gDevice->output_presentation_windows[j]);
 
#ifdef __DX12__

#elif defined(__VULKAN__)
            VkPresentInfoKHR _present_info = {};
            
            _present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            _present_info.waitSemaphoreCount = 1;
            _present_info.pWaitSemaphores = &_present_window->vk_rendering_done_semaphore;
  
            _present_info.swapchainCount = 1;
            _present_info.pSwapchains = &_present_window->vk_swap_chain;
            _present_info.pImageIndices = &_present_window->vk_swap_chain_image_index;

            auto _hr = vkQueuePresentKHR(_gDevice->vk_present_queue,
                                         &_present_info);
            V(_hr, L"presenting queue of graphics device: " + 
              wolf::system::convert::string_to_wstring(_gDevice->device_name) +
              L" and presentation window: " + std::to_wstring(j),
              _super::name, 3, true, true);
#endif
        }
    }
}

ULONG w_graphics_device_manager::release()
{
    //release the private implementation
    if(this->_pimp && !this->_pimp->release())
    {
        SAFE_RELEASE(this->_pimp);
    }
    
    //release all graphics devices
    while (this->graphics_devices.size() > 0)
	{
		auto _gDevice = this->graphics_devices.front();

		SHARED_RELEASE(_gDevice);

		this->graphics_devices.pop_back();
	}
    
#ifdef __DX12__

	//release 
	COMPTR_RELEASE(w_graphics_device::dx_dxgi_factory);

#elif defined(__VULKAN__)
    //release vulkan instance
    if (w_graphics_device::vk_instance)
    {
        vkDestroyInstance(w_graphics_device::vk_instance, nullptr);
        w_graphics_device::vk_instance = nullptr;
    }
#endif

	return _super::release();
}

#ifdef __VULKAN__
VkResult w_graphics_device_manager::memory_type_from_properties(
        VkPhysicalDeviceMemoryProperties pMemoryProperties,
        uint32_t pTypeBits,
        VkFlags pRequirementsMask,
        uint32_t* pTypeIndex)
{
    // Search mem types to find first index with those properties
    for (size_t i = 0; i <  pMemoryProperties.memoryTypeCount; ++i) 
    {
        if ((pTypeBits & 1) == 1) 
        {
            // Type is available, does it match user properties?
            if ((pMemoryProperties.memoryTypes[i].propertyFlags & pRequirementsMask) == pRequirementsMask) 
            {
                *pTypeIndex =  static_cast<uint32_t>(i);
                return VkResult::VK_SUCCESS;
            }
        }
        pTypeBits >>= 1;
    }
    // No memory types matched, return failure
    return VkResult::VK_INCOMPLETE;
}
#endif
            
#pragma endregion

#pragma region Getters

//Get the main graphics device, this is first and the primary device.
std::shared_ptr<w_graphics_device> w_graphics_device_manager::get_graphics_device() const
{
    return this->graphics_devices.size() > 0 ? this->graphics_devices.at(0) : nullptr;
}

//Returns number of available graphics devices
const ULONG w_graphics_device_manager::get_number_of_graphics_devices() const
{
    return static_cast<ULONG>(this->graphics_devices.size());
}

#ifdef	__DX11_X__
concurrency::accelerator_view w_graphics_device_manager::get_camp_accelerator_view(const CPP_AMP_DEVICE_TYPE pCppAMPDeviceType, ID3D11Device1* pDevice)
{
	if (pCppAMPDeviceType == CPP_AMP_DEVICE_TYPE::CPU)
	{
		//Create C++ Amp Accelerator from Windows CPU
		auto _acc = concurrency::accelerator(concurrency::accelerator::cpu_accelerator);
		return _acc.default_view;
	}
	else if (pCppAMPDeviceType == CPP_AMP_DEVICE_TYPE::GPU_REF)
	{
		//Create C++ Amp Accelerator from GPU reference
		return concurrency::accelerator(concurrency::accelerator::direct3d_ref).default_view;
	}
	else if (pCppAMPDeviceType == CPP_AMP_DEVICE_TYPE::GPU_WARP)
	{
		//Create C++ Amp Accelerator from Windows Advanced Rasterization Platform(WARP)
		return concurrency::accelerator(concurrency::accelerator::direct3d_warp).default_view;
	}
	else if (pCppAMPDeviceType == CPP_AMP_DEVICE_TYPE::GPU)
	{
		//logger.write(L"C++ AMP created with following device : " + _cAmpAcc->accelerator.description);

		//Create C++ Amp Accelerator from first DirectX device
		auto _acc_view = concurrency::direct3d::create_accelerator_view(reinterpret_cast<IUnknown*>(pDevice));
		return _acc_view;
	}
	else
	{
		//Default
		return concurrency::accelerator(concurrency::accelerator::default_accelerator).default_view;
	}
}
#endif

//const DirectX::XMFLOAT2 w_graphics_device_manager::get_dpi() const
//{
//	auto _gDevice = get_graphics_device();
//	DirectX::XMFLOAT2 dpi;
//	if (_gDevice && _gDevice->factory_2D)
//	{
//		_gDevice->factory_2D->GetDesktopDpi(&dpi.x, &dpi.y);
//	}
//
//	return dpi;
//}

//const DirectX::XMFLOAT2 w_graphics_device_manager::get_pixels_to_inches(float pX, float pY) const
//{
//	DirectX::XMFLOAT2 _inches = DirectX::XMFLOAT2(0, 0);
//	auto _dpi = get_dpi();
//	if (_dpi.x != 0 && _dpi.y != 0)
//	{
//		_inches.x = pX / _dpi.x; _inches.y = pY / _dpi.y;
//		return _inches;
//	}
//	return _inches;
//}

#pragma endregion

#pragma region w_graphics_device

#ifdef	__DX11_X__
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
	V(_hr, L"creating blend state", "w_graphics_device", 3, true, true);
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
	V(_hr, L"creating depth state", "w_graphics_device", 3, true, true);
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
	V(_hr, L"creating rasterizer state", "w_graphics_device", 3, true, true);
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
	V(_hr, L"creating sampler state", "w_graphics_device", 3, true, true);
}

void w_graphics_device::enable_alpha_blend(std::initializer_list<float> pBlendFactor, UINT pSampleMask)
{
	auto _size = pBlendFactor.size();
	assert(_size != 0, "pBlendFactor must have 4 float number");
	std::vector<float> _blend_factor(pBlendFactor.begin(), pBlendFactor.end());

	this->context->OMSetBlendState(this->_alpha_blend.Get(), _blend_factor.data(), pSampleMask);
}

//#endif
#pragma endregion

#endif
