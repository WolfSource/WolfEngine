/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_graphics_device_manager.h
	Description		 : The manager for graphics devices.
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_GRAPHICS_DEVICE_MANAGER_H__
#define __W_GRAPHICS_DEVICE_MANAGER_H__

#include "w_graphics_headers.h"
#include "w_render_export.h"
#include <w_object.h>
#include <w_window.h>
#include <w_color.h>
#include <w_signal.h>
#include <w_point.h>
#include <map>
#include <mutex>
#include <vector>
#include <array>
#include "w_graphics/w_queue.h"
#include "w_graphics/w_semaphore.h"
#include "w_graphics/w_fences.h"

namespace wolf
{
	namespace graphics
	{
        //forward declaration
        struct w_graphics_device_manager_configs;
        struct w_viewport;
        struct w_viewport_scissor;
        struct w_buffer;
        class  w_pipeline;
        
        struct w_image_view
        {
#ifdef __VULKAN__
            VkImage                             image = 0;
            VkImageView                         view = 0;
#endif
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
#elif defined(__ANDROID) || defined(__APPLE__)
				this->window = nullptr;
#elif defined(__linux)
                this->xcb_connection = nullptr;
                this->xcb_window = nullptr;
#endif 
                                
				return 1;
			}

			bool											is_full_screen = false;
			float											aspect_ratio = 0;

#if defined(__WIN32) || defined(__linux) || defined(__APPLE__) || defined(__ANDROID)
			uint32_t										width = 0;
			uint32_t										height = 0;

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
                      
			bool									        v_sync = true;
            bool                                            cpu_access_to_swapchain_buffer = false;

#ifdef __DX12__		
			DXGI_FORMAT								        dx_swap_chain_selected_format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			ComPtr<IDXGISwapChain3>					        dx_swap_chain;

			std::vector<ID3D12Resource*>			        dx_swap_chain_image_views;
			uint32_t								        dx_swap_chain_image_index = 0;

			ComPtr<ID3D12DescriptorHeap>			        dx_render_target_view_heap;
			UINT									        dx_render_target_descriptor_size = 0;

			ComPtr<ID3D12CommandAllocator>			        dx_command_allocator_pool;
			ComPtr<ID3D12CommandQueue>				        dx_command_queue;
			ComPtr<ID3D12GraphicsCommandList>		        dx_command_list;
			ComPtr<ID3D12PipelineState>				        dx_pipeline_state;

			//Synchronization objects.
			HANDLE									        dx_fence_event = 0;
			ComPtr<ID3D12Fence>						        dx_fence;
			UINT64									        dx_fence_value = 0;

#elif defined(__VULKAN__)
            VkSurfaceKHR                                    vk_presentation_surface = 0;
            VkSurfaceFormatKHR                              vk_swap_chain_selected_format;
			VkSwapchainKHR                                  vk_swap_chain = 0;
            std::vector<w_image_view>				        vk_swap_chain_image_views;
            uint32_t								        vk_swap_chain_image_index = 0;
            
            std::vector<VkSurfaceFormatKHR>			        vk_surface_formats;
                        
            VkFormat								        vk_depth_buffer_format = VkFormat::VK_FORMAT_UNDEFINED;
            w_image_view							        vk_depth_buffer_image_view;
            VkDeviceMemory							        vk_depth_buffer_memory = 0;
            
			//Synchronization objects
            w_semaphore								        vk_swap_chain_image_is_available_semaphore;
            w_semaphore								        vk_rendering_done_semaphore;

            //Required objects for sharing swap chain's buffer with CPU
            struct shared_objs_between_cpu_gpu
            {
                VkImage                                     destination_image = 0;
                VkDeviceMemory                              destination_image_memory = 0;
                VkCommandBuffer                             copy_command_buffer = 0;
                VkFence                                     copy_fence = 0;
                bool                                        command_buffer_began = false;
            };shared_objs_between_cpu_gpu*                  objs_between_cpu_gpu = nullptr;
            bool                                            bliting_supported_by_swap_chain = true;
#endif

        private:
            bool									        _is_released = false;
        };
        
		struct w_device_info
		{
		public:
			w_device_info()
			{
			}

			const uint32_t              get_device_id() const { return this->device_properties->deviceID; }
			const std::string           get_device_name() const { return this->device_properties->deviceName; }
			const uint32_t				get_device_vendor_id() const { return this->device_properties->vendorID; }

#ifdef __VULKAN__
			VkPhysicalDeviceProperties* device_properties = nullptr;
			VkPhysicalDeviceFeatures*   device_features = nullptr;
			std::vector<const char*>    device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
			
#endif

			void release()
			{
				this->device_features = nullptr;
				SAFE_DELETE(this->device_properties);
				device_extensions.clear();
			}
		};
		
		//contains graphics device which performs primitive-based rendering
		class w_graphics_device
		{
			friend class w_graphics_device_manager;
		public:
            w_graphics_device();

            //print info
            W_EXP const std::string print_info();

            //release all resources
            W_EXP ULONG release();

            w_output_presentation_window               output_presentation_windows;
            
            //draw primitive(s) and instances using vertex & index buffer
            W_EXP void draw(_In_ VkCommandBuffer pCommandBuffer,
                            _In_ uint32_t        pVertexCount,
                            _In_ uint32_t        pInstanceCount,
                            _In_ uint32_t        pFirstVertex,
                            _In_ uint32_t        pFirstInstance);
            
            //submit command buffer
            W_EXP HRESULT submit(_In_ const std::vector<VkCommandBuffer>&   pCommandBuffers,
                                 _In_ const w_queue&                        pQueue,
                                 _In_ const VkPipelineStageFlags*           pWaitDstStageMask,
                                 _In_ std::vector<VkSemaphore>              pWaitForSemaphores,
                                 _In_ std::vector<VkSemaphore>              pSignalForSemaphores,
                                 _In_ w_fences*                             pFence);
            
            /*
                capture image buffer's data and save to D-RAM and make it accessable by CPU, 
				for capturing last presented swap chain image's buffer, use "w_graphics_device::capture_presented_swap_chain_buffer" function
                The source buffer must be created with  VK_IMAGE_USAGE_TRANSFER_SRC_BIT flag
                @param pSourceImage, inputed source image
				@param pSourceImageLayout, inputed source image layout
				@param pWidth, width of inputed source image
				@param pHeight, height of inputed source image
                @param pOnPixelsDataCaptured, raised when pixels just mapped to RAM and become accessable by CPU. (inputs are: const w_point_t Width_Height, const uint8_t* Pixels and outsput is void) 
                @return S_OK means function did succesfully and S_FALSE means function failed
            */
			W_EXP HRESULT capture(
				_In_ VkImage pSourceImage,
				_In_ VkFormat pSourceImageFormat,
				_In_ VkImageLayout pSourceImageLayout,
				_In_ const uint32_t& pWidth,
				_In_ const uint32_t& pHeight,
				_In_ system::w_signal<void(const w_point_t, uint8_t*)>& pOnPixelsDataCaptured);

			/*
				capture last presented swap chain image buffer's data and save to the D-RAM and make it accessable by CPU,
				make sure set true to w_window_info::cpu_access_swap_chain_buffer flag before creating graphics device
				@return S_OK means function did succesfully and S_FALSE means function failed
			*/
            W_EXP HRESULT capture_presented_swap_chain_buffer(_In_ wolf::system::w_signal<void(const w_point_t, uint8_t*)>& pOnPixelsDataCaptured);

			w_device_info*												device_info = nullptr;

#ifdef __DX12__

			static ComPtr<IDXGIFactory4>							dx_dxgi_factory;
			
			ComPtr<IDXGIOutput>										dx_dxgi_outputs;

			bool													dx_device_removed;
			bool													dx_is_wrap_device;
			D3D_FEATURE_LEVEL										dx_feature_level;
			ComPtr<IDXGIAdapter1>									dx_adaptor;
			ComPtr<ID3D12Device>									dx_device;

#elif defined(__VULKAN__)
            static VkInstance                                               vk_instance;
                        
            VkPhysicalDevice                                                vk_physical_device;
            VkPhysicalDeviceFeatures                                        vk_physical_device_features;
            VkPhysicalDeviceMemoryProperties                                vk_physical_device_memory_properties;
            
            std::vector<VkQueueFamilyProperties>                            vk_queue_family_properties;
            std::vector<VkBool32>                                           vk_queue_family_supports_present;
			
            w_queue                                                         vk_graphics_queue;
            w_queue                                                         vk_present_queue;
            w_queue                                                         vk_compute_queue;
            w_queue                                                         vk_transfer_queue;
            w_queue                                                         vk_sparse_queue;

            VkDevice                                                        vk_device;

            VkCommandPool                                                   vk_command_allocator_pool;
                        
            //static pipeline defaults
			struct defaults
            {
                W_EXP static std::vector<VkSubpassDependency>               vk_default_subpass_dependencies;
                W_EXP static VkPipelineLayoutCreateInfo                     vk_default_pipeline_layout_create_info;
                W_EXP static VkPipelineVertexInputStateCreateInfo           vk_default_pipeline_vertex_input_state_create_info;
                W_EXP static VkPipelineInputAssemblyStateCreateInfo         vk_default_pipeline_input_assembly_state_create_info;
                W_EXP static VkPipelineRasterizationStateCreateInfo         vk_default_pipeline_rasterization_state_create_info;
                W_EXP static VkPipelineMultisampleStateCreateInfo           vk_default_pipeline_multisample_state_create_info;
            };

            struct w_blend_states
            {
                W_EXP static VkPipelineColorBlendAttachmentState            blend_none;
                W_EXP static VkPipelineColorBlendAttachmentState            premulitplied_alpha;
            };

#endif //__DX12__ __VULKAN__
                                    
		private:
            //prevent copying
            w_graphics_device(w_graphics_device const&);
            w_graphics_device& operator= (w_graphics_device const&);
            
            bool                                                            _is_released;
            std::string                                                     _name;
		};

        //forward declaration
        class w_graphics_device_manager_pimp;
		//handles the configuration and management of the graphics device.
		class w_graphics_device_manager : public system::w_object
		{
		public:               
			W_EXP w_graphics_device_manager();                                        
            W_EXP virtual ~w_graphics_device_manager();
                        
			//Initialize graphics devices
			W_EXP virtual void initialize(_In_ std::map<int, w_window_info> pOutputWindowsInfo) = 0;

			//Called when corresponding window resized
			W_EXP virtual void on_window_resized(_In_ const uint32_t& pGraphicsDeviceIndex);
			//Called when any graphics device lost
			W_EXP virtual void on_device_lost();
			//Called when the APP suspends. It provides a hint to the driver that the APP is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
			W_EXP virtual void on_suspend();
            //Prepare frame on all graphics devices
            W_EXP virtual HRESULT prepare();
			//Present on all graphics devices
			W_EXP virtual HRESULT present();
			//Release all resources
			W_EXP ULONG release() override;


			//convert DPIs to pixels
            W_EXP static const float convert_dips_to_pixels(_In_ float pDIPS, _In_ float pDPI);

            system::w_signal<void(w_device_info**)> on_device_info_fetched;

#pragma region Getters
			//Get the main graphics device, this is first and the primary device.
            W_EXP std::shared_ptr<w_graphics_device> get_graphics_device(_In_ const size_t& pGraphicsDeviceIndex) const;
			//Returns number of available graphics devices
            W_EXP const ULONG get_number_of_graphics_devices() const;

//#ifdef __DX11__
//            //Get DPI
//            W_EXP const DirectX::XMFLOAT2 get_dpi() const;
//            //Get pixels to inches
//            W_EXP const DirectX::XMFLOAT2 get_pixels_to_inches(_In_ float pX, _In_ float pY) const;
//#endif

#pragma endregion

#pragma region Setters
			W_EXP void set_graphics_device_manager_configs(_In_ const w_graphics_device_manager_configs& pConfig);
#pragma endregion


#ifdef __VULKAN__
            W_EXP static VkResult memory_type_from_properties(VkPhysicalDeviceMemoryProperties pMemoryProperties,
                                                  uint32_t pTypeBits,
                                                  VkFlags pRequirementsFlags,
                                                  uint32_t* pTypeIndex);
            W_EXP static VkFormat find_supported_format(
                _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const std::vector<VkFormat>& pFormatCandidates,
                _In_ VkImageTiling pImageTiling,
                _In_ VkFormatFeatureFlags pFormatFeatureFlags);
            
            W_EXP static void set_src_dst_masks_of_image_barrier(_Inout_ VkImageMemoryBarrier& pImageMemoryBarrier);
#endif
                        
		protected:
			std::vector<std::shared_ptr<w_graphics_device>>		graphics_devices;

		private:
            //prevent copying
            w_graphics_device_manager(w_graphics_device_manager const&);
            w_graphics_device_manager& operator= (w_graphics_device_manager const&);
            
            void _load_shared_resources();

			void _wait_for_previous_frame(_In_ const std::shared_ptr<w_graphics_device>& pGDevice);
            
            typedef  system::w_object                           _super;
            w_graphics_device_manager_pimp*                     _pimp;
		};
        
#pragma region structures
        
        //the default config for creating graphics devices, you can edit the config before calling w_graphics_device_manager::initialize
        struct w_graphics_device_manager_configs
        {
#ifdef __DX12__
            bool							use_wrap_mode = false;
            D3D_FEATURE_LEVEL				wrap_mode_feature_level = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1;
            std::vector<D3D_FEATURE_LEVEL>	hardware_feature_levels = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_12_1 };
#endif
            bool debug_gpu = false;
			//used for compute mode
			bool off_screen_mode = false;
        };

        struct w_viewport : 
#ifdef __VULKAN__
        public VkViewport
#elif defined(__DX12__)
        public RECT   
#endif
        {
            w_viewport()
            {
#ifdef __VULKAN__
                this->x = 0.0f;
                this->y = 0.0f;
                this->width = 800.0f;
                this->height = 600.0f;
                this->minDepth = 0.0f;
                this->maxDepth = 1.0f;
#elif defined(__DX12__)
                this->left = 0.0f;
                this->top = 0.0f;
                this->right = 800.0f;
                this->bottom = 600.0f;
#endif

            }
        };
        
#ifdef __VULKAN__
        struct w_viewport_scissor :
        public VkRect2D
        {
            w_viewport_scissor()
            {
                this->offset.x = 0;
                this->offset.y = 0;
                this->extent.width = 800;
                this->extent.height = 600;
            }
            

#ifdef __PYTHON__
            w_point py_get_offset() const
            {
                auto _point = w_point();
                _point.x = this->offset.x;
                _point.y = this->offset.y;
                return _point;
            }
            void py_set_offset(_In_ const int32_t& pX, _In_ const int32_t& pY)
            {
                this->offset.x = pX;
                this->offset.y = pY;
            }

            w_point_t py_get_extent() const
            {
                auto _point = w_point_t();
                _point.x = this->extent.width;
                _point.y = this->extent.height;
                return _point;
            }
            void py_set_extent(_In_ const uint32_t& pWidth, _In_ const uint32_t& pHeight)
            {
                this->extent.width = pWidth;
                this->extent.height = pHeight;
            }
#endif
        };
#endif
        
#pragma endregion
        
	}
}

#include "python_exporter/py_graphics_device_manager.h"

#endif
