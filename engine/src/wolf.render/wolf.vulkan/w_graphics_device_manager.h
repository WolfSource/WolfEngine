/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_graphics_device_manager.h
	Description		 : The manager for graphics devices
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_GRAPHICS_DEVICE_MANAGER_H__
#define __W_GRAPHICS_DEVICE_MANAGER_H__

#ifdef __WIN32
    #ifndef VK_USE_PLATFORM_WIN32_KHR
        #define VK_USE_PLATFORM_WIN32_KHR
    #endif
#elif defined(__linux)
    #ifndef VK_USE_PLATFORM_XCB_KHR
        #define VK_USE_PLATFORM_XCB_KHR
    #endif
#endif

#ifdef __APPLE__

#ifndef VK_USE_PLATFORM_MACOS_MVK
#define VK_USE_PLATFORM_MACOS_MVK
#endif

#include <vulkan/vulkan.h>
#include <MoltenVK/vk_mvk_macos_surface.h>
#include <unistd.h>
#else
#include <vulkan/vulkan.hpp>
#endif

#include "w_vulkan_export.h"
#include <w_std.h>
#include <w_object.h>
#include <w_window.h>
#include <map>
#include <vector>

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
	namespace graphics
	{
        struct vk_image_view
        {
            VkImage                             image = nullptr;
            VkImageView                         view = nullptr;;
        };
                
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
                                
#elif defined(__linux)
                this->xcb_connection = nullptr;
                this->xcb_window = nullptr;
#endif 
                                
				return 1;
			}

			UINT                                index = 0;
			
			UINT                                width = 0;
			UINT                                height = 0;
			float                               aspectRatio = 0;
                        
#ifdef  __WIN32  
			DWORD                               pdwCookie;
            HWND                                hwnd = NULL;
			HINSTANCE                           hInstance = NULL;
#elif defined(__linux)
            xcb_connection_t*                   xcb_connection = nullptr;
            xcb_window_t*                       xcb_window = nullptr;
#elif defined(__APPLE__)
            void*                               window = nullptr;
#endif
                        
            VkSurfaceKHR                        vk_presentation_surface = nullptr;
                        
            bool                                v_sync = true;
                        
            VkFormat                            vk_swap_chain_selected_format = VkFormat::VK_FORMAT_UNDEFINED;
            VkSwapchainKHR                      vk_swap_chain = nullptr;
            std::vector<vk_image_view>          vk_swap_chain_image_views;
            uint32_t                            vk_swap_chain_image_index = 0;
                        
            VkCommandPool                       vk_command_pool = nullptr;
            std::vector<VkCommandBuffer>        vk_command_buffers;
            std::vector<VkSurfaceFormatKHR>     vk_surface_formats;
                        
            VkFormat                            vk_depth_buffer_format = VkFormat::VK_FORMAT_UNDEFINED;
            vk_image_view                       vk_depth_buffer_image_view;
            VkDeviceMemory                      vk_depth_buffer_memory = nullptr;
                        
            VkSemaphore                         vk_image_is_available_semaphore = nullptr;
            VkSemaphore                         vk_rendering_done_semaphore = nullptr;
            
        private:
            bool                                _is_released = false;
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
			WVK_EXP w_graphics_device_manager(bool pUse_Wrap_Mode = false);
			WVK_EXP virtual ~w_graphics_device_manager();

			//Initialize graphics devices
			WVK_EXP virtual void initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo) = 0;
			//Called when corresponding window resized
			WVK_EXP virtual void on_window_resized(_In_ UINT pIndex);
			//Called when any graphics device lost
			WVK_EXP virtual void on_device_lost();
			//Begin render on all graphics devices
			WVK_EXP virtual void begin_render();
			//End render on all graphics devices
			WVK_EXP virtual void end_render();
			//Release all resources
			WVK_EXP ULONG release() override;

			//Get the main graphics device, this is first and the primary device.
            std::shared_ptr<w_graphics_device> get_graphics_device() const;
			//Returns number of available graphics devices
            const ULONG get_number_of_graphics_devices() const;
            
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

		protected:
			std::vector<std::shared_ptr<w_graphics_device>>		graphics_devices;

		private:
            //prevent copying
            w_graphics_device_manager(w_graphics_device_manager const&);
            w_graphics_device_manager& operator= (w_graphics_device_manager const&);
            
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
