/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_graphics_device_manager.h
	Description		 : The manager for graphics devices. Wolf.Engine supports two render APIs, the first one is DirectX 12 
						which supports both Windows 10 and Universal Windows P(UWP) and the second one is Vulkan which supports
						Windows, Linux, Android and OSX/IOS(with MoltenVK)  
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_GRAPHICS_DEVICE_MANAGER_H__
#define __W_GRAPHICS_DEVICE_MANAGER_H__

#ifdef __DX12__

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>

using Microsoft::WRL::ComPtr;

#elif defined (__VULKAN__) 
	#ifdef __WIN32
		#ifndef VK_USE_PLATFORM_WIN32_KHR
			#define VK_USE_PLATFORM_WIN32_KHR
		#endif
	#elif defined(__linux) && !defined(__ANDROID)
		#ifndef VK_USE_PLATFORM_XCB_KHR
			#define VK_USE_PLATFORM_XCB_KHR
		#endif
	#endif

	#ifdef __APPLE__
		#ifndef VK_USE_PLATFORM_MACOS_MVK
		#define VK_USE_PLATFORM_MACOS_MVK
		#endif

		#include <vulkan/vulkan.h>
		#include <MoltenVK/vk_mvk_moltenvk.h>
		#include <unistd.h>
	#elif defined(__ANDROID)
		#include "android/vulkan_wrapper.h"
	#else
		#include <vulkan/vulkan.hpp>
	#endif
#endif

#include "w_render_export.h"
#include <w_std.h>
#include <w_object.h>
#include <w_window.h>
#include <w_color.h>
#include <map>
#include <vector>

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
	namespace graphics
	{
		//the default config for creating graphics devices, you can edit the config before calling w_graphics_device_manager::initialize
		struct w_graphics_device_manager_configs
		{
#ifdef __DX12__
			bool							use_wrap_mode = false;
			D3D_FEATURE_LEVEL				wrap_mode_feature_level = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
			std::vector<D3D_FEATURE_LEVEL>	hardware_feature_levels = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1 };
#endif
		};
                
#ifdef __VULKAN__
        struct vk_image_view
        {
            VkImage                             image = NULL;
            VkImageView                         view = NULL;
        };
#endif
                
		//Output window which handles all 3d resources for output renderer
		struct w_output_presentation_window
		{
		public:
			//Release resources
			ULONG release()
			{
				if (this->_is_released) return 0;
				this->_is_released = true;
                                
#ifdef __WIN32
				this->hwnd = NULL;
				this->hInstance = NULL;
#elif defined(__ANDROID)
				this->window = nullptr;
#elif defined(__linux)
                this->xcb_connection = nullptr;
                this->xcb_window = nullptr;
#endif 
                                
				return 1;
			}

			UINT											index = 0;
			bool											is_full_screen = false;
			float											aspect_ratio = 0;

#if defined(__WIN32) || defined(__linux) || defined(__APPLE__) || defined(__ANDROID)
			UINT											width = 0;
			UINT											height = 0;

#ifdef  __WIN32
			DWORD											pdwCookie;
            HWND											hwnd = NULL;
			HINSTANCE										hInstance = NULL;
#elif defined(__ANDROID)
			ANativeWindow*									window = nullptr;
#elif defined(__linux)
            xcb_connection_t*								xcb_connection = nullptr;
            xcb_window_t*									xcb_window = nullptr;
#elif defined(__APPLE__)
            void*											window = nullptr;
#endif

#elif defined(__UWP)
			Windows::Graphics::Display::DisplayOrientations	window_native_orientation;
			Windows::Graphics::Display::DisplayOrientations	window_current_orientation;
			float											window_dpi;
			bool											support_high_resolutions = true;
			IUnknown*										window;
			Windows::Foundation::Rect						window_size;
			DirectX::XMFLOAT4X4								orientation_transform_3D;
#endif
                      
			bool									v_sync = true;
			w_color                                 clear_color = w_color::from_hex(w_color::CORNFLOWER_BLUE);
			int										force_to_clear_color_times;

#ifdef __DX12__
			
			DXGI_FORMAT								dx_swap_chain_selected_format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			ComPtr<IDXGISwapChain3>					dx_swap_chain;
			std::vector<ID3D12Resource*>			dx_swap_chain_image_views;
			uint32_t								dx_swap_chain_image_index = 0;

			ComPtr<ID3D12DescriptorHeap>			dx_render_target_view_heap;
			UINT									dx_render_target_descriptor_size = 0;

			ComPtr<ID3D12CommandAllocator>			dx_command_allocator_pool;
			ComPtr<ID3D12CommandQueue>				dx_command_queue;
			ComPtr<ID3D12GraphicsCommandList>		dx_command_list;
			ComPtr<ID3D12PipelineState>				dx_pipeline_state;

			//Synchronization objects.
			HANDLE									dx_fence_event = NULL;
			ComPtr<ID3D12Fence>						dx_fence;
			UINT64									dx_fence_value = 0;
#elif defined(__VULKAN__)
            VkSurfaceKHR							vk_presentation_surface = NULL;
                        
			VkSurfaceFormatKHR						vk_swap_chain_selected_format;
			
            VkSwapchainKHR							vk_swap_chain = NULL;
            std::vector<vk_image_view>				vk_swap_chain_image_views;
            uint32_t								vk_swap_chain_image_index = 0;
                        
            VkCommandPool							vk_command_allocator_pool = NULL;
            std::vector<VkCommandBuffer>			vk_command_queues;
            std::vector<VkSurfaceFormatKHR>			vk_surface_formats;
                        
            VkFormat								vk_depth_buffer_format = VkFormat::VK_FORMAT_UNDEFINED;
            vk_image_view							vk_depth_buffer_image_view;
            VkDeviceMemory							vk_depth_buffer_memory = NULL;
            
			//Synchronization objects
            VkSemaphore								vk_image_is_available_semaphore = NULL;
            VkSemaphore								vk_rendering_done_semaphore = NULL;
#endif

        private:
            bool									_is_released = false;
		};
        
		//contains graphics device which performs primitive-based rendering
		class w_graphics_device
		{
			friend class w_graphics_device_manager;
		public:
            w_graphics_device();

			//get the first and the primary window which was created with this device
            w_output_presentation_window main_window();

			//release all resources
            ULONG release();

            std::vector<w_output_presentation_window>               output_presentation_windows;
            
#ifdef __DX12__

			static ComPtr<IDXGIFactory4>							dx_dxgi_factory;
			
			ComPtr<IDXGIOutput>										dx_dxgi_outputs;

			bool													dx_device_removed;
			bool													dx_is_wrap_device;
			D3D_FEATURE_LEVEL										dx_feature_level;
			ComPtr<IDXGIAdapter1>									dx_adaptor;
			ComPtr<ID3D12Device>									dx_device;

#elif defined(__VULKAN__)
            static VkInstance                                       vk_instance;
                        
            VkPhysicalDevice                                        vk_physical_device;
            VkPhysicalDeviceMemoryProperties                        vk_physical_device_memory_properties;
                        
            std::vector<VkQueueFamilyProperties>                    vk_queue_family_properties;
            size_t                                                  vk_queue_family_selected_index;
            std::vector<VkBool32>                                   vk_queue_family_supports_present;
            size_t                                                  vk_queue_family_selected_support_present_index;
                        
            VkQueue                                                 vk_graphics_queue;
            VkQueue                                                 vk_present_queue;
                                
            VkDevice                                                vk_device;
#endif //__DX12__
            
            std::string                                             device_name;
            
		private:
            //prevent copying
            w_graphics_device(w_graphics_device const&);
            w_graphics_device& operator= (w_graphics_device const&);
            
            bool                                                    _is_released;
		};

        //forward decalaration
        class w_graphics_device_manager_pimp;
		//handles the configuration and management of the graphics device.
		class w_graphics_device_manager : public system::w_object
		{
		public:               
			W_EXP w_graphics_device_manager();                                        
            W_EXP virtual ~w_graphics_device_manager();
                        
			//Initialize graphics devices
			W_EXP virtual void initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo) = 0;
			//Called when corresponding window resized
			W_EXP virtual void on_window_resized(_In_ UINT pIndex);
			//Called when any graphics device lost
			W_EXP virtual void on_device_lost();
			//Begin render on all graphics devices
			W_EXP virtual HRESULT begin_render();
			//End render on all graphics devices
			W_EXP virtual void end_render();
			//Release all resources
			W_EXP ULONG release() override;

			//convert dpis to pixels
			static const float convert_dips_to_pixels(_In_ float pDIPS, _In_ float pDPI);

#pragma region Getters
			//Get the main graphics device, this is first and the primary device.
            std::shared_ptr<w_graphics_device> get_graphics_device() const;
			//Returns number of available graphics devices
            const ULONG get_number_of_graphics_devices() const;
			w_color get_output_window_clear_color(_In_ size_t pGraphicsDeviceIndex,
				_In_ size_t pOutputPresentationWindowIndex) const;
#pragma endregion

#pragma region Setters
			W_EXP void set_graphics_device_manager_configs(_In_ const w_graphics_device_manager_configs& pConfig);
			W_EXP void set_output_window_clear_color(_In_ size_t pGraphicsDeviceIndex,
				_In_ size_t pOutputPresentationWindowIndex, _In_ w_color pClearColor);
#pragma endregion


//			//Get deafult window HWND
//			const HWND get_window_HWND() const									{ return this->_windows_info.size() == 0 || this->_windows_info.at(0).size() == 0 ? NULL : this->_windows_info.at(0).at(0).hwnd; }
//			//Get deafult window HINSTANCE
//			const HINSTANCE get_window_HINSTANCE() const						{ return this->_windows_info.size() == 0 || this->_windows_info.at(0).size() == 0 ? NULL : this->_windows_info.at(0).at(0).hInstance; }
//			//Get deafult window width
//			const UINT get_window_width() const									{ return this->_windows_info.size() == 0 || this->_windows_info.at(0).size() == 0 ? 0 : this->_windows_info.at(0).at(0).width; }
//			//Get deafult window height
//			const UINT get_window_height() const								{ return this->_windows_info.size() == 0 || this->_windows_info.at(0).size() == 0 ? 0 : this->_windows_info.at(0).at(0).height; }
//			//Get DPI
//			const DirectX::XMFLOAT2 get_dpi() const;
//			//Get pixels to inches
//			const DirectX::XMFLOAT2 get_pixels_to_inches(_In_ float pX, _In_ float pY) const;
//
//#pragma endregion

#ifdef __VULKAN__
            static VkResult memory_type_from_properties(VkPhysicalDeviceMemoryProperties pMemoryProperties,
                                                  uint32_t pTypeBits,
                                                  VkFlags pRequirementsMask,
                                                  uint32_t* pTypeIndex);
#endif
                        
		protected:
			std::vector<std::shared_ptr<w_graphics_device>>		graphics_devices;

		private:
            //prevent copying
            w_graphics_device_manager(w_graphics_device_manager const&);
            w_graphics_device_manager& operator= (w_graphics_device_manager const&);
            
			void wait_for_previous_frame(_In_ const std::shared_ptr<w_graphics_device>& pGDevice, _In_ size_t pOutputPresentationWindowIndex);

            typedef  system::w_object                           _super;
            w_graphics_device_manager_pimp*                     _pimp;
		};
	}
}

//#endif

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif
