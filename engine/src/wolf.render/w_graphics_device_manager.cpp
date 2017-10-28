#include "w_render_pch.h"
#include "w_graphics_device_manager.h"
#include <w_logger.h>
#include <w_convert.h>
#include "w_graphics/w_command_buffers.h"
#include "w_graphics/w_texture.h"
#include "w_graphics/w_shader.h"
#include <signal.h>

#ifdef __DX12__
#include <w_directX_helper.h>
#elif defined(__APPLE__)
#include <math.h>
#endif

using namespace std;
using namespace wolf::system;
using namespace wolf::graphics;

#define NUM_SAMPLES     VK_SAMPLE_COUNT_1_BIT

#ifdef __DX12__
ComPtr<IDXGIFactory4>	w_graphics_device::dx_dxgi_factory = nullptr;
#elif defined(__VULKAN__)
VkInstance w_graphics_device::vk_instance = NULL;

VkAttachmentDescription	w_graphics_device::w_render_pass_attachments::color_attachment_description =
{
    0,								        // Additional properties of attachment.Currently, only an aliasing flag is available, which informs the driver that the attachment shares the same physical memory with another attachment.
    VkFormat::VK_FORMAT_B8G8R8A8_UNORM,     // Format of an image used for the attachment.
    VK_SAMPLE_COUNT_1_BIT,                  // Number of samples of the image; The value greater than 1 means multisampling.
    VK_ATTACHMENT_LOAD_OP_CLEAR,            // Specifies what to do with the image�s contents at the beginning of a render pass, whether we want them to be cleared, preserved, or we don�t care about them (as we will overwrite them all). Here we want to clear the image to the specified value. This parameter also refers to depth part of depth/stencil images.
    VK_ATTACHMENT_STORE_OP_STORE,           // Informs the driver what to do with the image�s contents after the render pass (after a subpass in which the image was used for the last time). Here we want the contents of the image to be preserved after the render pass as we intend to display them on screen. This parameter also refers to the depth part of depth/stencil images.
    VK_ATTACHMENT_LOAD_OP_DONT_CARE,        // The same as loadOp but for the stencil part of depth/stencil images; for color attachments it is ignored.
    VK_ATTACHMENT_STORE_OP_DONT_CARE,       // The same as storeOp but for the stencil part of depth/stencil images; for color attachments this parameter is ignored.
    VK_IMAGE_LAYOUT_UNDEFINED,              // The layout the given attachment will have when the render pass starts (what the layout image is provided with by the application).
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR         // The layout the driver will automatically transition the given image into at the end of a render pass.
};

VkAttachmentReference w_graphics_device::w_render_pass_attachments::color_attachment_reference =
{
    0,                                                                  // Attachment
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL                            // The layout of attachment
};

VkAttachmentDescription	w_graphics_device::w_render_pass_attachments::depth_attachment_description =
{
	0,								        // Additional properties of attachment.Currently, only an aliasing flag is available, which informs the driver that the attachment shares the same physical memory with another attachment.
	VK_FORMAT_D32_SFLOAT_S8_UINT,           // Format of an image used for the attachment.
	VK_SAMPLE_COUNT_1_BIT,                  // Number of samples of the image; The value greater than 1 means multisampling.
	VK_ATTACHMENT_LOAD_OP_CLEAR,            // Specifies what to do with the images contents at the beginning of a render pass, whether we want them to be cleared, preserved, or we don�t care about them (as we will overwrite them all). Here we want to clear the image to the specified value. This parameter also refers to depth part of depth/stencil images.
    VK_ATTACHMENT_STORE_OP_STORE,           // Informs the driver what to do with the image�s contents after the render pass (after a subpass in which the image was used for the last time). Here we want the contents of the image to be preserved after the render pass as we intend to display them on screen. This parameter also refers to the depth part of depth/stencil images.
    VK_ATTACHMENT_LOAD_OP_DONT_CARE,        // The same as loadOp but for the stencil part of depth/stencil images; for color attachments it is ignored.
    VK_ATTACHMENT_STORE_OP_DONT_CARE,       // The same as storeOp but for the stencil part of depth/stencil images; for color attachments this parameter is ignored.
    VK_IMAGE_LAYOUT_UNDEFINED,              // The layout the given attachment will have when the render pass starts (what the layout image is provided with by the application).
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL         // The layout the driver will automatically transition the given image into at the end of a render pass.
};

VkAttachmentReference w_graphics_device::w_render_pass_attachments::depth_attachment_reference =
{
    1,                                                                  // Attachment
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL                    // The layout of attachment
};

std::vector<VkSubpassDependency> w_graphics_device::defaults::vk_default_subpass_dependencies =
{
    {
        VK_SUBPASS_EXTERNAL,                            // uint32_t                       srcSubpass
        0,                                              // uint32_t                       dstSubpass
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,           // VkPipelineStageFlags           srcStageMask
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags           dstStageMask
        VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags                  srcAccessMask
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags                  dstAccessMask
        VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags              dependencyFlags
    },
    {
        0,                                              // uint32_t                       srcSubpass
        VK_SUBPASS_EXTERNAL,                            // uint32_t                       dstSubpass
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags           srcStageMask
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,           // VkPipelineStageFlags           dstStageMask
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags                  srcAccessMask
        VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags                  dstAccessMask
        VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags              dependencyFlags
    }
};

VkPipelineLayoutCreateInfo w_graphics_device::defaults::vk_default_pipeline_layout_create_info
{
    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,                      // Type
    nullptr,                                                            // Next
    0,                                                                  // Flags
    0,                                                                  // SetLayoutCount
    nullptr,                                                            // SetLayouts
    0,                                                                  // PushConstantRangeCount
    nullptr                                                             // PushConstantRanges
};

VkPipelineVertexInputStateCreateInfo w_graphics_device::defaults::vk_default_pipeline_vertex_input_state_create_info =
{
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,      // Type
        nullptr,                                                        // Next
        0,                                                              // Flags
        0,                                                              // Count of vertex binding descriptions
        nullptr,                                                        // Vertex binding descriptions
        0,                                                              // Count of vertex attribute descriptions
        nullptr                                                         // Vertex attribute descriptions
};
     
VkPipelineInputAssemblyStateCreateInfo w_graphics_device::defaults::vk_default_pipeline_input_assembly_state_create_info = 
{
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,    // Type
        nullptr,                                                        // Next
        0,                                                              // Flags
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                            // Topology
        VK_FALSE                                                        // Enable restart primitive
};

VkPipelineRasterizationStateCreateInfo w_graphics_device::defaults::vk_default_pipeline_rasterization_state_create_info = 
{
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,     // Type
        nullptr,                                                        // Next
        0,                                                              // flags
        VK_FALSE,                                                       // depthClampEnable
        VK_FALSE,                                                       // rasterizerDiscardEnable
        VK_POLYGON_MODE_FILL,                                           // polygonMode
		VK_CULL_MODE_BACK_BIT,                                          // cullMode
		VK_FRONT_FACE_COUNTER_CLOCKWISE,                                // frontFace
        VK_FALSE,                                                       // depthBiasEnable
        0.0f,                                                           // depthBiasConstantFactor
        0.0f,                                                           // depthBiasClamp
        0.0f,                                                           // depthBiasSlopeFactor
        1.0f                                                            // lineWidth
};

VkPipelineMultisampleStateCreateInfo w_graphics_device::defaults::vk_default_pipeline_multisample_state_create_info = 
{
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,         // Type
      nullptr,                                                          // Next
      0,                                                                // flags
      VK_SAMPLE_COUNT_1_BIT,                                            // rasterizationSamples
      VK_FALSE,                                                         // sampleShadingEnable
      1.0f,                                                             // minSampleShading
      nullptr,                                                          // pSampleMask
      VK_FALSE,                                                         // alphaToCoverageEnable
      VK_FALSE                                                          // alphaToOneEnable
};

VkPipelineColorBlendAttachmentState w_graphics_device::w_blend_states::blend_none = 
{
      VK_FALSE,                                                         // blendEnable
      VK_BLEND_FACTOR_ONE,                                              // srcColorBlendFactor
      VK_BLEND_FACTOR_ZERO,                                             // dstColorBlendFactor
      VK_BLEND_OP_ADD,                                                  // colorBlendOp
      VK_BLEND_FACTOR_ONE,                                              // srcAlphaBlendFactor
      VK_BLEND_FACTOR_ZERO,                                             // DstAlphaBlendFactor
      VK_BLEND_OP_ADD,                                                  // AlphaBlendOp
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |             // ColorWriteMask
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
};

VkPipelineColorBlendAttachmentState w_graphics_device::w_blend_states::premulitplied_alpha =
{
    VK_TRUE,                                                            // blendEnable
    VK_BLEND_FACTOR_ONE,                                                // srcColorBlendFactor
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,                                // dstColorBlendFactor
    VK_BLEND_OP_ADD,                                                    // colorBlendOp
    VK_BLEND_FACTOR_ONE,                                                // srcAlphaBlendFactor
    VK_BLEND_FACTOR_ZERO,                                               // DstAlphaBlendFactor
    VK_BLEND_OP_ADD,                                                    // AlphaBlendOp
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |               // ColorWriteMask
    VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
};

#endif

w_graphics_device::w_graphics_device():
_is_released(false), _name("w_graphics_device")

#ifdef __VULKAN__
,
vk_device(0),
vk_command_allocator_pool(0)
#endif
{
}

const std::string w_graphics_device::print_info()
{
    return std::string(
                       "graphics device: " + this->device_name +
                       " ID:" + std::to_string(this->device_id) +
                       " VendorID:" + std::to_string(this->device_vendor_id));
}

void w_graphics_device::draw(_In_ VkCommandBuffer pCommandBuffer,
                             _In_ uint32_t        pVertexCount,
                             _In_ uint32_t        pInstanceCount,
                             _In_ uint32_t        pFirstVertex,
                             _In_ uint32_t        pFirstInstance)
{
#ifdef __VULKAN__
    vkCmdDraw( pCommandBuffer, pVertexCount, pInstanceCount, pFirstVertex, pFirstInstance );
#elif defined(__DX12__)
    
#endif
}

HRESULT w_graphics_device::submit(_In_ const std::vector<VkCommandBuffer>&  pCommandBuffers,
                                  _In_ const w_queue&                       pQueue,
                                  _In_ const VkPipelineStageFlags*          pWaitDstStageMask,
                                  _In_ std::vector<VkSemaphore>             pWaitForSemaphores,
                                  _In_ std::vector<VkSemaphore>             pSignalForSemaphores,
                                  _In_ w_fences&                            pFence)
{
    const std::string _trace_info = "w_graphics_device::submit";
    
    VkSubmitInfo _submit_info = {};
    _submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    _submit_info.commandBufferCount = static_cast<uint32_t>(pCommandBuffers.size());
    _submit_info.pCommandBuffers = pCommandBuffers.data();
    _submit_info.pWaitDstStageMask = pWaitDstStageMask;
    _submit_info.waitSemaphoreCount = static_cast<uint32_t>(pWaitForSemaphores.size());
    _submit_info.pWaitSemaphores = pWaitForSemaphores.data();
    _submit_info.signalSemaphoreCount = static_cast<uint32_t>(pSignalForSemaphores.size());;
    _submit_info.pSignalSemaphores = pSignalForSemaphores.data();
    
    // Submit to queue
    if (vkQueueSubmit(pQueue.queue, 1, &_submit_info, *(pFence.get())))
    {
        V(S_FALSE,
          "submiting queue for graphics device" + this->device_name + " ID:" + std::to_string(this->device_id),
          _trace_info,
          3,
          false,
          true);
        return S_FALSE;
    }
    
    return S_OK;
}

w_output_presentation_window w_graphics_device::main_window()
{
    if (this->output_presentation_windows.size() > 0)
    {
        return this->output_presentation_windows.at(0);
    }
    
    //return null window;qwwa
	return w_output_presentation_window();
}

ULONG w_graphics_device::release()
{
	//release all resources
	if (this->_is_released) return 0;

	this->_is_released = true;

#ifdef __DX12__

	//wait for previous frame

	this->device_name.clear();
    this->device_id = 0;
    
	//release all output presentation windows
	for (size_t i = 0; i < this->output_presentation_windows.size(); ++i)
	{
		auto _output_window = &(this->output_presentation_windows.at(i));

		if (_output_window->dx_swap_chain)
		{
			_output_window->dx_swap_chain->SetFullscreenState(false, NULL);
		}

		COMPTR_RELEASE(_output_window->dx_command_allocator_pool);
		COMPTR_RELEASE(_output_window->dx_command_queue);
		COMPTR_RELEASE(_output_window->dx_command_list);
		COMPTR_RELEASE(_output_window->dx_fence);
		COMPTR_RELEASE(_output_window->dx_pipeline_state);

		for (size_t i = 0; i < _output_window->dx_swap_chain_image_views.size(); ++i)
		{
			COM_RELEASE(_output_window->dx_swap_chain_image_views[i]);
		}
		COMPTR_RELEASE(_output_window->dx_render_target_view_heap);
		COMPTR_RELEASE(_output_window->dx_swap_chain);

#ifdef __WIN32
		_output_window->hwnd = NULL;
#endif

		COMPTR_RELEASE(_output_window->dx_fence);
		CloseHandle(_output_window->dx_fence_event);
	}

	COMPTR_RELEASE(this->dx_device);
	COMPTR_RELEASE(this->dx_dxgi_outputs);
	COMPTR_RELEASE(this->dx_adaptor);

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
        _output_window->vk_presentation_surface = 0;

		//release semaphores
        _output_window->vk_rendering_done_semaphore.release();
        _output_window->vk_swap_chain_image_is_available_semaphore.release();
        
		//release all image view of swap chains
		for (size_t i = 0; i < _output_window->vk_swap_chain_image_views.size(); ++i)
		{
			//release both image and view,
			vkDestroyImageView(this->vk_device,
				_output_window->vk_swap_chain_image_views[i].view,
				nullptr);
            _output_window->vk_swap_chain_image_views[i].view = 0;
            
            //No need to destroy image
//            vkDestroyImage(this->vk_device,
//                               _output_window->vk_swap_chain_image_views[i].image,
//                               nullptr);
            _output_window->vk_swap_chain_image_views[i].image = 0;
		}
		_output_window->vk_swap_chain_image_views.clear();

		//release swap chain
		vkDestroySwapchainKHR(this->vk_device,
			_output_window->vk_swap_chain,
			nullptr);
        _output_window->vk_swap_chain = 0;

		this->output_presentation_windows.at(i).release();
	}

	this->output_presentation_windows.clear();

	this->vk_queue_family_properties.clear();
	this->vk_queue_family_supports_present.clear();

	this->vk_graphics_queue.release();
    this->vk_present_queue.release();
    this->vk_compute_queue.release();
    this->vk_transfer_queue.release();

    //destroy command pool
    vkDestroyCommandPool(this->vk_device,
                         this->vk_command_allocator_pool,
                         nullptr);
    this->vk_command_allocator_pool = 0;
    
	//release vulkan resources
	vkDestroyDevice(this->vk_device, nullptr);
    
#endif

	return 1;
}

#pragma region w_graphics_device_manager

#ifdef __GNUC__
#pragma GCC visibility push(hidden)
#endif

#pragma region w_graphics_device_manager private implementation

#ifdef __VULKAN__
//callbacks for vulkan debugging
static PFN_vkCreateDebugReportCallbackEXT sCreateDebugReportCallback = 0;
static PFN_vkDestroyDebugReportCallbackEXT sDestroyDebugReportCallback = 0;
static PFN_vkDebugReportMessageEXT sDebugBreakCallback = 0;

static VkDebugReportCallbackEXT MsgCallback;
static VkBool32 DebugMessageCallback(
    VkDebugReportFlagsEXT pFlags,
    VkDebugReportObjectTypeEXT pObjType,
    uint64_t pSrcObject,
    size_t pLocation,
    int32_t pMsgCode,
    const char* pLayerPrefix,
    const char* pMsg,
    void* pUserData)
{
    // Error that may result in undefined behaviour
    if (pFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        logger.error("[Vulkan Debugger] Flag: ERROR - Message code: " + std::to_string(pMsgCode) + " - Message: " + std::string(pMsg));
    }
    // Warnings may hint at unexpected / non-spec API usage
    if (pFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        logger.warning("[Vulkan Debugger] Flag: WARNING - Message code: " + std::to_string(pMsgCode) + " - Message: " + std::string(pMsg));
    }
    // May indicate sub-optimal usage of the API
    if (pFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        logger.write("[Vulkan Debugger] Flag: PERFORMANCE - Message code: " + std::to_string(pMsgCode) + " - Message: " + std::string(pMsg));
    }
    // Informal messages that may become handy during debugging
    if (pFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        logger.user("[Vulkan Debugger] Flag: INFO - Message code: " + std::to_string(pMsgCode) + " - Message: " + std::string(pMsg));
    }
    // Diagnostic info from the Vulkan loader and layers
    // Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
    if (pFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        logger.write("[Vulkan Debugger] Flag: DEBUG - Message code: " + std::to_string(pMsgCode) + " - Message: " + std::string(pMsg));
    }

    // The return value of this callback controls wether the Vulkan call that caused
    // the validation message will be aborted or not
    // We return VK_FALSE as we DON'T want Vulkan calls that cause a validation message 
    // (and return a VkResult) to abort
    // If you instead want to have calls abort, pass in VK_TRUE and the function will 
    // return VK_ERROR_VALIDATION_FAILED_EXT 
    return VK_FALSE;
}
#endif

namespace wolf
{
    namespace graphics
    {
		class w_graphics_device_manager_pimp
		{
		public:
			w_graphics_device_manager_pimp() :
				_name("w_graphics_device_manager_pimp")
			{
			}

			void enumerate_devices(_Inout_  std::vector<std::shared_ptr<w_graphics_device>>& pGraphicsDevices, 
                _In_ wolf::system::w_signal<void(w_graphics_device_manager::w_device_features_extensions&)>* pOnDeviceFeaturesFetched)
			{
#ifdef __DX12__

				HRESULT _hr = S_FALSE;

				std::wstring _msg;
				_msg += L"++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n\t\t\t\t\tDirectX API version: 12";

				UINT _dxgi_factory_flags = 0;
#ifdef _DEBUG
                if (this->_config.debug_gpu)
                {
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
                }
#endif

                _hr = CreateDXGIFactory2(_dxgi_factory_flags, IID_PPV_ARGS(&w_graphics_device::dx_dxgi_factory));
                if (FAILED(_hr))
                {
                    logger.error(L"error on getting dxgi factory");
                    release();
                    std::exit(EXIT_FAILURE);
                }

                const D3D_FEATURE_LEVEL _feature_levels[] =
                {
                    D3D_FEATURE_LEVEL_12_1,
                    D3D_FEATURE_LEVEL_12_0,
                    D3D_FEATURE_LEVEL_11_1,
                    D3D_FEATURE_LEVEL_11_0,
                    D3D_FEATURE_LEVEL_10_1,
                    D3D_FEATURE_LEVEL_10_0,
                    D3D_FEATURE_LEVEL_9_3,
                    D3D_FEATURE_LEVEL_9_2,
                    D3D_FEATURE_LEVEL_9_1
                };

                const size_t _features_len = ARRAYSIZE(_feature_levels);
                size_t _feature_level_start_index = 0;

                //create wrap mode device or hardware device?
                if (this->_config.use_wrap_mode)
                {
                    if (this->_config.wrap_mode_feature_level < D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0)
                    {
                        this->_config.wrap_mode_feature_level = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
                        logger.warning("Minimum feature level must be at least D3D_FEATURE_LEVEL_11_0 for a D3D12 device. Wrap mode feature level set to D3D_FEATURE_LEVEL_11_0.");
                    }
                    for (_feature_level_start_index = 0; _feature_level_start_index < _features_len; ++_feature_level_start_index)
                    {
                        if (_feature_levels[_feature_level_start_index] == this->_config.wrap_mode_feature_level)
                        {
                            break;
                        }
                    }

                    auto _gDevice = std::make_shared<w_graphics_device>();
                    _gDevice->dx_device_removed = false;
                    _gDevice->dx_is_wrap_device = true;

                    ComPtr<IDXGIAdapter> _warp_adapter;
                    _hr = w_graphics_device::dx_dxgi_factory->EnumWarpAdapter(IID_PPV_ARGS(&_warp_adapter));
                    if (FAILED(_hr))
                    {
                        logger.error(L"error on getting wrap adaptor");
                        release();
                        std::exit(EXIT_FAILURE);
                    }

                    bool _device_created = false;
                    for (size_t i = _feature_level_start_index; i < _features_len; i++)
                    {
                        _hr = D3D12CreateDevice(
                            _warp_adapter.Get(),
                            _feature_levels[i],
                            IID_PPV_ARGS(&_gDevice->dx_device));
                        if (FAILED(_hr))
                        {
                            _msg += L"creating wrap device with feature level: " + DirectX::GetFeatureLevelStrW(this->_config.wrap_mode_feature_level);
                            logger.error(_msg);
                        }
                        else
                        {
                            this->_config.wrap_mode_feature_level = _feature_levels[i];
                            _device_created = true;
                            break;
                        }
                    }

                    if (!_device_created)
                    {
                        logger.error(L"no device was created");
                        release();
                        std::exit(EXIT_FAILURE);
                    }

                    _gDevice->device_name = "DirectX Wrap mode with " + DirectX::GetFeatureLevelStr(this->_config.wrap_mode_feature_level);
                    _gDevice->device_id = 0;
                    _gDevice->device_vendor_id = 0;
                    _msg += L"\r\n\t\t\t\t\tDirectX Wrap mode with " + DirectX::GetFeatureLevelStrW(this->_config.wrap_mode_feature_level);


                    //add the wrap device graphics devices list
                    pGraphicsDevices.push_back(_gDevice);

                    //create first presentation windows for wrap graphics device
                    auto _win = this->_windows_info.find(0);
                    if (_win != this->_windows_info.end())
                    {
                        for (size_t j = 0; j < _win->second.size(); ++j)
                        {
                            auto _window = _win->second[j];

                            w_output_presentation_window _out_window;
                            _out_window.index = static_cast<int>(j);
                            _out_window.dx_swap_chain_selected_format = (DXGI_FORMAT)_window.swap_chain_format;

#if defined(__WIN32) || ( defined(__linux) && !defined(__ANDROID) ) || defined(__APPLE__)
                            _out_window.width = _window.width;
                            _out_window.height = _window.height;
                            _out_window.aspect_ratio = (float)_window.width / (float)_window.height;
                            _out_window.v_sync = _window.v_sync_enable;
                            _out_window.is_full_screen = _window.is_full_screen;
#ifdef __WIN32
                            _out_window.hwnd = _window.hwnd;
                            _out_window.hInstance = _window.hInstance;
#endif

#elif defined(__UWP)
                            _out_window.window = _window.window;
                            _out_window.window_size = _window.window_size;
                            _out_window.window_current_orientation = _window.window_current_orientation;
                            _out_window.window_native_orientation = _window.window_native_orientation;
                            _out_window.window_dpi = _window.window_dpi;
                            _out_window.support_high_resolutions = _window.support_high_resolutions;
                            _out_window.aspect_ratio = _window.window_size.Width / _window.window_size.Height;
#endif

                            _gDevice->output_presentation_windows.push_back(_out_window);

                            create_or_resize_swap_chain(_gDevice, j);
                            _create_depth_stencil_buffer(_gDevice, j);
                            _create_fences(_gDevice, j);

                        }
                    }
                }
                else
                {
                    //looking for hardware GPUs 
                    _msg += L"\r\n\t\t\t\t\tGPU(s) founded:\r\n";

                    //Iterate for all adaptors
                    ComPtr<IDXGIAdapter1> _adapter;
                    for (int i = 0; w_graphics_device::dx_dxgi_factory->EnumAdapters1(i, &_adapter) != DXGI_ERROR_NOT_FOUND; ++i)
                    {
                        //if the feature level not specified in configs, the default feature level is D3D_FEATURE_LEVEL_11_0
                        auto _selected_feature_level = i >= this->_config.hardware_feature_levels.size() ? D3D_FEATURE_LEVEL_11_0 : this->_config.hardware_feature_levels[i];
                        if (_selected_feature_level < D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0)
                        {
                            _selected_feature_level = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
                            logger.warning("Minimum feature level must be at least D3D_FEATURE_LEVEL_11_0 for a D3D12 device. Hardware feature level for graphics device set to D3D_FEATURE_LEVEL_11_0.");
                        }
                        for (_feature_level_start_index = 0; _feature_level_start_index < _features_len; ++_feature_level_start_index)
                        {
                            if (_feature_levels[_feature_level_start_index] == _selected_feature_level)
                            {
                                break;
                            }
                        }

                        DXGI_ADAPTER_DESC1 _adapter_desc = {};
                        auto _hr = _adapter->GetDesc1(&_adapter_desc);
                        if (_hr != S_OK)
                        {
                            logger.write(_msg);
                            _msg.clear();
                            _msg = L"Could not get adaptor.";
                            logger.error(_msg);
                            release();
                            std::exit(EXIT_FAILURE);
                        }

                        auto _device_name = std::wstring(_adapter_desc.Description);
                        auto _device_id = _adapter_desc.DeviceId;
                        auto _device_vendor_id = _adapter_desc.VendorId;

                        _msg += L"\t\t\t\t\t\tDevice Name: " + _device_name + L"\r\n";
                        _msg += L"\t\t\t\t\t\tDevice ID: " + std::to_wstring(_device_id) + L"\r\n";
                        _msg += L"\t\t\t\t\t\tDevice Vendor: " + std::to_wstring(_device_vendor_id) + L"\r\n";
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

                        bool _device_created = false;
                        for (size_t _index = _feature_level_start_index; _index < _features_len; ++_index)
                        {
                            //Check to see if the adapter supports Direct3D 12, if not then skip it
                            if (FAILED(D3D12CreateDevice(_adapter.Get(), _feature_levels[_index], _uuidof(ID3D12Device), nullptr)))
                            {
                                _msg += L"\t\t\t\t\t\tFeature level: " + DirectX::GetFeatureLevelStrW(_feature_levels[_index]) +
                                    L" not supported for " + _device_name + L"\r\n";
                            }
                            else
                            {
                                _selected_feature_level = _feature_levels[_index];
                                _device_created = true;
                                break;
                            }
                        }

                        if (!_device_created)
                        {
                            continue;
                        }

                        _msg += L"\t\t\t\t\t\tFeature level: " + DirectX::GetFeatureLevelStrW(_selected_feature_level) + L" supported\r\n";

                        auto _gDevice = std::make_shared<w_graphics_device>();
                        _gDevice->device_name = std::string(_device_name.begin(), _device_name.end());
                        _gDevice->dx_device_removed = false;
                        _gDevice->dx_is_wrap_device = false;
                        _gDevice->dx_feature_level = _selected_feature_level;

                        _hr = D3D12CreateDevice(
                            _adapter.Get(),
                            _selected_feature_level,
                            IID_PPV_ARGS(&_gDevice->dx_device));
                        if (_hr != S_OK)
                        {
                            logger.write(_msg);
                            _msg.clear();
                            _msg = L"creating hadrware device with feature level: " + DirectX::GetFeatureLevelStrW((_selected_feature_level));
                            logger.error(_msg);
                            release();
                            std::exit(EXIT_FAILURE);
                        }

                        //write to output
                        logger.write(_msg);
                        _msg.clear();


#ifdef __WIN32
                        //get the monitors numerator and denominator
                        _hr = _adapter->EnumOutputs(0, &_gDevice->dx_dxgi_outputs);
                        V(_hr, "enumurate output monitors", this->_name, 2);

#endif
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
                                _out_window.index = static_cast<int>(j);
                                _out_window.dx_swap_chain_selected_format = (DXGI_FORMAT)_window.swap_chain_format;

#if defined(__WIN32) || ( defined(__linux) && !defined(__ANDROID) ) || defined(__APPLE__)
                                _out_window.width = _window.width;
                                _out_window.height = _window.height;
                                _out_window.aspect_ratio = (float)_window.width / (float)_window.height;
                                _out_window.v_sync = _window.v_sync_enable;
                                _out_window.is_full_screen = _window.is_full_screen;
#ifdef __WIN32
                                _out_window.hwnd = _window.hwnd;
                                _out_window.hInstance = _window.hInstance;
#endif

#elif defined(__UWP)
                                _out_window.window = _window.window;
                                _out_window.window_size = _window.window_size;
                                _out_window.window_current_orientation = _window.window_current_orientation;
                                _out_window.window_native_orientation = _window.window_native_orientation;
                                _out_window.window_dpi = _window.window_dpi;
                                _out_window.support_high_resolutions = _window.support_high_resolutions;
                                _out_window.aspect_ratio = _window.window_size.Width / _window.window_size.Height;
#endif

                                _gDevice->output_presentation_windows.push_back(_out_window);

                                create_or_resize_swap_chain(_gDevice, j);
                                _create_depth_stencil_buffer(_gDevice, j);
                                _create_fences(_gDevice, j);

                            }
                        }
                    }
                }

                if (_msg.length())
                {
                    //write to output
                    logger.write(_msg);
                    _msg.clear();
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

                //get available extensions count
                auto _hr =

                    vkEnumerateInstanceExtensionProperties(nullptr,
                        &_extension_count,
                        nullptr);
                if (_hr)
                {
                    //write the buffer to output before exiting
                    logger.write(_msg);
                    _msg.clear();
                    logger.error("error on enumerating instance extension properties.");
                    release();
                    std::exit(EXIT_FAILURE);
                }

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

                std::vector<const char*> _vk_instance_enabled_extensions =
                {
                    VK_KHR_SURFACE_EXTENSION_NAME,
                };

                // Enable surface extensions depending on OS
#if defined(__ANDROID)
                _vk_instance_enabled_extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(__WIN32)
                _vk_instance_enabled_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(_DIRECT2DISPLAY)
                _vk_instance_enabled_extensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#elif defined(__linux)
                _vk_instance_enabled_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(__iOS__)
                _vk_instance_enabled_extensions.push_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
#elif defined(__APPLE__)
                _vk_instance_enabled_extensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#endif

                if (this->_config.debug_gpu)
                {
                    _vk_instance_enabled_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
                }

                VkInstanceCreateInfo _instance_create_info = {};
                _instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                _instance_create_info.pNext = nullptr;
                _instance_create_info.pApplicationInfo = &_app_info;
                if (_vk_instance_enabled_extensions.size() > 0)
                {
                    _instance_create_info.enabledExtensionCount = (uint32_t)_vk_instance_enabled_extensions.size();
                    _instance_create_info.ppEnabledExtensionNames = _vk_instance_enabled_extensions.data();
                }
                if (this->_config.debug_gpu)
                {
                    const char* _validation_layer_names[] =
                    {
                        "VK_LAYER_LUNARG_core_validation",
#ifdef __ANDROID
                        "VK_LAYER_GOOGLE_threading",
                        "VK_LAYER_LUNARG_parameter_validation",
                        "VK_LAYER_LUNARG_device_limits,"
                        "VK_LAYER_LUNARG_object_tracker",
                        "VK_LAYER_LUNARG_image",
                        "VK_LAYER_LUNARG_core_validation",
                        "VK_LAYER_LUNARG_swapchain",
                        "VK_LAYER_GOOGLE_unique_objects"
#endif
                    };

                    _instance_create_info.enabledLayerCount = (uint32_t)W_ARRAY_SIZE(_validation_layer_names);
                    _instance_create_info.ppEnabledLayerNames = _validation_layer_names;
                }

                //create Vulkan instance
                _hr = vkCreateInstance(&_instance_create_info, nullptr, &w_graphics_device::vk_instance);
                _vk_instance_enabled_extensions.clear();
                if (_hr)
                {
                    logger.write(_msg);
                    _msg.clear();
                    logger.error("error on creating Vulkan instance.");
                    release();
                    std::exit(EXIT_FAILURE);
                }


                if (this->_config.debug_gpu)
                {
                    // Set what type of debug message should be report
                    VkDebugReportFlagsEXT _debug_report_flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;

                    //vks::debug::setupDebugging(instance, debugReportFlags, VK_NULL_HANDLE);
                    sCreateDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(w_graphics_device::vk_instance, "vkCreateDebugReportCallbackEXT"));
                    sDestroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(w_graphics_device::vk_instance, "vkDestroyDebugReportCallbackEXT"));
                    sDebugBreakCallback = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(w_graphics_device::vk_instance, "vkDebugReportMessageEXT"));

                    VkDebugReportCallbackCreateInfoEXT _debug_create_info = {};
                    _debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
                    _debug_create_info.pfnCallback = (PFN_vkDebugReportCallbackEXT)DebugMessageCallback;
                    _debug_create_info.flags = _debug_report_flags;

                    auto _hr = sCreateDebugReportCallback(
                        w_graphics_device::vk_instance,
                        &_debug_create_info,
                        nullptr,
                        &MsgCallback);
                    if (_hr != VK_SUCCESS)
                    {
                        logger.error("Could not create debug callback for vulkan");
                    }
                }


				//Enumerate physical devices
				uint32_t _gpu_count = 0;

				//Get number of GPU count
				vkEnumeratePhysicalDevices(w_graphics_device::vk_instance, &_gpu_count, nullptr);

				//Get GPU list names
				if (_gpu_count == 0)
				{
					//write the buffer to output before exiting
					logger.write(_msg);
					_msg.clear();
					logger.error("GPU count. No GPU found.");
					release();
					std::exit(EXIT_FAILURE);
				}

				_msg += "\r\n\t\t\t\t\tGPU(s) founded:\r\n";

				auto _gpus = new VkPhysicalDevice[_gpu_count];

				vkEnumeratePhysicalDevices(w_graphics_device::vk_instance, &_gpu_count, _gpus);

				for (size_t i = 0; i < _gpu_count; ++i)
				{
					VkPhysicalDeviceProperties _device_properties;

					vkGetPhysicalDeviceProperties(_gpus[i], &_device_properties);
					auto _device_name = std::string(_device_properties.deviceName);
					auto _device_id = _device_properties.deviceID;
					auto _device_vendor_id = _device_properties.vendorID;

					_msg += "\t\t\t\t\t\tDevice Name: " + _device_name + "\r\n";
					_msg += "\t\t\t\t\t\tDevice ID: " + std::to_string(_device_id) + "\r\n";
					_msg += "\t\t\t\t\t\tDevice Vendor: " + std::to_string(_device_vendor_id) + "\r\n";
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
					_gDevice->device_id = _device_id;
					_gDevice->device_vendor_id = _device_vendor_id;
					_gDevice->vk_physical_device = _gpus[i];

                    //get device features
                    vkGetPhysicalDeviceFeatures(_gpus[i], &_gDevice->vk_physical_device_features);

                    //call event callback for changing device extensions and features from user
                    w_graphics_device_manager::w_device_features_extensions _features_extensions(_device_id, _device_name.c_str());
                    _features_extensions.device_features = &_gDevice->vk_physical_device_features;

                    if (pOnDeviceFeaturesFetched)
                    {
                        (*pOnDeviceFeaturesFetched)(_features_extensions);
                    }

                    //swap device extensions with user defined extentions
                    _gDevice->vk_device_extensions.swap(_features_extensions.device_extensions);

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
					_msg.clear();

					//get queue family from default GPU
					uint32_t _queue_family_property_count = 0;

					vkGetPhysicalDeviceQueueFamilyProperties(_gpus[i],
						&_queue_family_property_count,
						nullptr);

					if (_queue_family_property_count == 0)
					{
						logger.write(_msg);
						_msg.clear();
						logger.error("could not find queue family for default GPU.");
						release();
						std::exit(EXIT_FAILURE);
					}

					_gDevice->vk_queue_family_properties.resize(_queue_family_property_count);
					_gDevice->vk_queue_family_supports_present.resize(_queue_family_property_count);

					vkGetPhysicalDeviceQueueFamilyProperties(_gpus[i],
						&_queue_family_property_count,
						_gDevice->vk_queue_family_properties.data());


					bool _queue_graphics_bit_found = false;
                    for (size_t j = 0; j < _queue_family_property_count; ++j)
                    {
                        _msg += "\r\n\t\t\t\t\t\t_queue_family_properties: " + std::to_string(j);
                        if (_gDevice->vk_queue_family_properties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                        {
                            _queue_graphics_bit_found = true;
                            _gDevice->vk_graphics_queue.index = static_cast<uint32_t>(j);
                            _msg += "\r\n\t\t\t\t\t\t\tVK_QUEUE_GRAPHICS_BIT supported.";
                            break;
                        }
                    }

                    for (size_t j = 0; j < _queue_family_property_count; ++j)
                    {
                        _msg += "\r\n\t\t\t\t\t\t_queue_family_properties: " + std::to_string(j);
                        if (_gDevice->vk_queue_family_properties[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
                        {
                            _queue_graphics_bit_found = true;
                            _gDevice->vk_compute_queue.index = static_cast<uint32_t>(j);
                            _msg += "\r\n\t\t\t\t\t\t\tVK_QUEUE_COMPUTE_BIT supported.";
                            break;
                        }
                    }


                    for (size_t j = 0; j < _queue_family_property_count; ++j)
                    {
                        _msg += "\r\n\t\t\t\t\t\t_queue_family_properties: " + std::to_string(j);
                        if (_gDevice->vk_queue_family_properties[j].queueFlags & VK_QUEUE_TRANSFER_BIT)
                        {
                            _queue_graphics_bit_found = true;
                            _gDevice->vk_transfer_queue.index = static_cast<uint32_t>(j);
                            _msg += "\r\n\t\t\t\t\t\t\tVK_QUEUE_TRANSFER_BIT supported.";
                            break;
                        }
                    }


                    for (size_t j = 0; j < _queue_family_property_count; ++j)
                    {
                        _msg += "\r\n\t\t\t\t\t\t_queue_family_properties: " + std::to_string(j);
                        if (_gDevice->vk_queue_family_properties[j].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
                        {
                            _queue_graphics_bit_found = true;
                            _gDevice->vk_sparse_queue.index = static_cast<uint32_t>(j);
                            _msg += "\r\n\t\t\t\t\t\t\tVK_QUEUE_SPARSE_BINDING_BIT supported.";
                            break;
                        }
                    }

					logger.write(_msg);
					_msg.clear();
					if (!_queue_graphics_bit_found || _queue_family_property_count == 0)
					{
						//write the buffer to output before exiting
						logger.write(_msg);
						_msg.clear();
						logger.error("could not find queue family with VK_QUEUE_GRAPHICS_BIT for default GPU.");
						release();
						std::exit(EXIT_FAILURE);
					}

					//create queue info
					float _queue_priorities[1] = { 1.0f };
					VkDeviceQueueCreateInfo _queue_info = {};
					_queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					_queue_info.pNext = nullptr;
					_queue_info.flags = 0;
					_queue_info.queueCount = 1;
					_queue_info.queueFamilyIndex = 0;
					_queue_info.pQueuePriorities = _queue_priorities;

					//create device info
					VkDeviceCreateInfo _device_info = {};
					_device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
					_device_info.pNext = nullptr;
					_device_info.queueCreateInfoCount = 1;
					_device_info.pQueueCreateInfos = &_queue_info;
					_device_info.enabledLayerCount = 0;
					_device_info.ppEnabledLayerNames = nullptr;
					_device_info.pEnabledFeatures = &_gDevice->vk_physical_device_features;
					if (_gDevice->vk_device_extensions.size())
					{
						_device_info.enabledExtensionCount = static_cast<uint32_t>(_gDevice->vk_device_extensions.size());
						_device_info.ppEnabledExtensionNames = _gDevice->vk_device_extensions.data();
					}

					//create device
					_hr = vkCreateDevice(_gpus[i], &_device_info, nullptr, &_gDevice->vk_device);
					if (_hr)
					{
						logger.write(_msg);
						_msg.clear();
						logger.error("error on creating Vulkan device.");
						release();
						std::exit(EXIT_FAILURE);
					}
                    
                    //create command pool
                    //create a command pool to allocate our command buffer from
                    VkCommandPoolCreateInfo _command_pool_info = {};
                    _command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                    _command_pool_info.pNext = nullptr;
                    _command_pool_info.queueFamilyIndex = _gDevice->vk_graphics_queue.index;
                    _command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
                    
                    _hr = vkCreateCommandPool(_gDevice->vk_device,
                                                   &_command_pool_info,
                                                   nullptr,
                                                   &_gDevice->vk_command_allocator_pool);
                    if (_hr)
                    {
                        logger.error("error on creating vulkan command pool for graphics device: " +
                                     _device_name + " ID:" + std::to_string(_device_id));
                        release();
                        std::exit(EXIT_FAILURE);
                    }

					auto _win = this->_windows_info.find(static_cast<int>(i));
					if (_win != this->_windows_info.end())
					{
						for (size_t j = 0; j < _win->second.size(); ++j)
						{
							auto _window = _win->second[j];

							w_output_presentation_window _out_window;
							_out_window.width = _window.width;
							_out_window.height = _window.height;
							_out_window.aspect_ratio = (float)_window.width / (float)_window.height;
							_out_window.index = static_cast<int>(j);
							_out_window.vk_swap_chain_selected_format.format = (VkFormat)_window.swap_chain_format;
							_out_window.vk_swap_chain_selected_format.colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

#if defined(__WIN32) || defined(__linux) || defined(__APPLE__) || defined(__ANDROID)

#if defined(__WIN32) || defined(__linux) 
							_out_window.v_sync = _window.v_sync_enable;
#endif

#ifdef __WIN32
							_out_window.hwnd = _window.hwnd;
							_out_window.hInstance = _window.hInstance;
#elif defined(__ANDROID)
							_out_window.window = _window.window;
#elif defined(__linux)
							_out_window.xcb_connection = _window.xcb_connection;
							_out_window.xcb_window = _window.xcb_window;
#elif defined(__APPLE__)
							_out_window.window = _window.window;
#endif

#endif

#if defined(VK_USE_PLATFORM_WIN32_KHR)
							VkWin32SurfaceCreateInfoKHR surface_create_info =
							{
								VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,    // Type
								nullptr,                                            // Next
								0,                                                  // Flags
								_out_window.hInstance,                              // Hinstance
								_out_window.hwnd                                    // Hwnd
							};

							_hr = vkCreateWin32SurfaceKHR(w_graphics_device::vk_instance, 
														  &surface_create_info, 
														  nullptr, 
								                          &_out_window.vk_presentation_surface);
							if (_hr)
							{
								logger.write(_msg);
								_msg.clear();
								logger.error("error on creating win32 surface for Vulkan.");
								release();
								std::exit(EXIT_FAILURE);
							}

#elif defined(VK_USE_PLATFORM_XCB_KHR)
							VkXcbSurfaceCreateInfoKHR _surface_create_info =
							{
								VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,      // Type
								nullptr,                                            // Next
								0,                                                  // Flags
								_out_window.xcb_connection,                         // Connection
								(*_out_window.xcb_window)                           // Window
							};
							_hr = vkCreateXcbSurfaceKHR(w_graphics_device::vk_instance,
								&_surface_create_info,
								nullptr,
								&_out_window.vk_presentation_surface);
							if (_hr)
							{
								logger.write(_msg);
								_msg.clear();
								logger.error("error on creating xcb surface for Vulkan.");
								release();
								std::exit(EXIT_FAILURE);
							}
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
							if (_hr)
							{
								logger.write(_msg);
								_msg.clear();
								logger.error("error on creating xlib surface for Vulkan.");
								release();
								std::exit(EXIT_FAILURE);
							}
#elif defined(__ANDROID)
							VkAndroidSurfaceCreateInfoKHR _android_surface_create_info = {};
							_android_surface_create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
							_android_surface_create_info.window = _out_window.window;
							_android_surface_create_info.flags = 0;
							_android_surface_create_info.pNext = nullptr;

							_hr = vkCreateAndroidSurfaceKHR(w_graphics_device::vk_instance,
								&_android_surface_create_info,
								nullptr,
								&_out_window.vk_presentation_surface);
							if (_hr)
							{
								logger.write(_msg);
								_msg.clear();
								logger.error("error on creating android surface for Vulkan.");
								release();
								std::exit(EXIT_FAILURE);
							}
#elif defined(VK_USE_PLATFORM_IOS_MVK)
                            VkIOSSurfaceCreateInfoMVK _surface_create_info = {};
                            _surface_create_info.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
                            _surface_create_info.pNext = NULL;
                            _surface_create_info.flags = 0;
                            _surface_create_info.pView = _window.window;
                            
                            _hr = vkCreateIOSSurfaceMVK(w_graphics_device::vk_instance,
                                                          &_surface_create_info,
                                                          NULL,
                                                          &_out_window.vk_presentation_surface);
                            if (_hr)
                            {
                                logger.write(_msg);
                                _msg.clear();
                                logger.error("error on creating iOS surface for Vulkan.");
                                release();
                                std::exit(EXIT_FAILURE);
                            }
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
							if (_hr)
							{
								logger.write(_msg);
								_msg.clear();
								logger.error("error on creating macOS surface for Vulkan.");
								release();
								std::exit(EXIT_FAILURE);
							}
#endif

							_gDevice->output_presentation_windows.push_back(_out_window);

							create_or_resize_swap_chain(_gDevice, j);
							_create_depth_stencil_buffer(_gDevice, j);
                            _create_fences(_gDevice, j);
							//_record_command_buffers(_gDevice, j);
						}
					}

					//get queues (graphics and present may be the same)
                    
                    //graphics
                    if (_gDevice->vk_graphics_queue.index != UINT32_MAX)
                    {
                        vkGetDeviceQueue(_gDevice->vk_device,
                            _gDevice->vk_graphics_queue.index,
                            0,
                            &_gDevice->vk_graphics_queue.queue);
                    }

                    //present
                    if (_gDevice->vk_compute_queue.index != UINT32_MAX)
                    {
                        vkGetDeviceQueue(_gDevice->vk_device,
                            _gDevice->vk_present_queue.index,
                            0,
                            &_gDevice->vk_present_queue.queue);
                    }

                    //compute
                    if (_gDevice->vk_compute_queue.index != UINT32_MAX)
                    {
                        vkGetDeviceQueue(_gDevice->vk_device,
                            _gDevice->vk_compute_queue.index,
                            0,
                            &_gDevice->vk_compute_queue.queue);
                    }

                    //transfer
                    if (_gDevice->vk_transfer_queue.index != UINT32_MAX)
                    {
                        vkGetDeviceQueue(_gDevice->vk_device,
                            _gDevice->vk_transfer_queue.index,
                            0,
                            &_gDevice->vk_transfer_queue.queue);
                    }

                    //sparse
                    if (_gDevice->vk_sparse_queue.index != UINT32_MAX)
                    {
                        vkGetDeviceQueue(_gDevice->vk_device,
                            _gDevice->vk_sparse_queue.index,
                            0,
                            &_gDevice->vk_sparse_queue.queue);
                    }

					pGraphicsDevices.push_back(_gDevice);
				}
#endif //__DX12__, __VULKAN__
			}

			void create_or_resize_swap_chain(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ size_t pOutputPresentationWindowIndex)
			{
#ifdef __DX12__
				auto _device_name = wolf::system::convert::string_to_wstring(pGDevice->device_name);
				auto _device_id = pGDevice->device_id;
				auto _output_presentation_window = &(pGDevice->output_presentation_windows.at(pOutputPresentationWindowIndex));

				const size_t _desired_number_of_swapchain_images = 2;
                
                //release dx_swap_chain_image_views
				for (size_t i = 0; i < _output_presentation_window->dx_swap_chain_image_views.size(); i++)
				{
					COM_RELEASE(_output_presentation_window->dx_swap_chain_image_views[i]);
				}

				_output_presentation_window->dx_swap_chain_image_views.resize(_desired_number_of_swapchain_images);				
				_output_presentation_window->force_to_clear_color_times = _desired_number_of_swapchain_images;


				HRESULT _hr = S_FALSE;
#ifdef __WIN32

				if (_output_presentation_window->dx_swap_chain != nullptr)
				{
					//If the swap chain already exists, resize it.
					_hr = _output_presentation_window->dx_swap_chain->ResizeBuffers(_desired_number_of_swapchain_images,
						_output_presentation_window->width,
						_output_presentation_window->height,
						_output_presentation_window->dx_swap_chain_selected_format,
						0);
					if (_hr == DXGI_ERROR_DEVICE_REMOVED || _hr == DXGI_ERROR_DEVICE_RESET)
					{
						logger.error(L"Error on resizing swap chain, because of DXGI_ERROR_DEVICE_REMOVED or DXGI_ERROR_DEVICE_RESET for graphics device: "
							+ _device_name + L" ID:" + std::to_wstring(_device_id));
						// If the device was removed for any reason, a new device and swap chain will need to be created.
						pGDevice->dx_device_removed = true;
						return;
					}
					else
					{
						logger.error(L"Error on resizing swap chain, unknown error for graphics device: "
							+ _device_name + L" ID:" + std::to_wstring(_device_id));
						release();
						std::exit(EXIT_FAILURE);
					}
				}
                else
                {
#pragma region NO NEED ANYMORE
                    //float _numerator = 0;
                    //float _denominator = 1;

                    ////if this window does not need v-sync, then it is important to get the refresh rate of displays
                    //if (!_output_presentation_window->v_sync)
                    //{
                    //	//we need to get the numerator and denominator of display monitors
                    //	UINT _num_modes = 0;
                    //	_hr = pGDevice->dx_dxgi_outputs->GetDisplayModeList(_output_presentation_window->dx_swap_chain_selected_format,
                    //		DXGI_ENUM_MODES_INTERLACED,
                    //		&_num_modes,
                    //		NULL);
                    //	if (SUCCEEDED(_hr))
                    //	{
                    //		std::vector<DXGI_MODE_DESC> _display_modes(_num_modes);
                    //		_hr = pGDevice->dx_dxgi_outputs->GetDisplayModeList(_output_presentation_window->dx_swap_chain_selected_format,
                    //			DXGI_ENUM_MODES_INTERLACED,
                    //			&_num_modes,
                    //			_display_modes.data());
                    //		if (SUCCEEDED(_hr))
                    //		{
                    //			for (size_t i = 0; i < _num_modes; ++i)
                    //			{
                    //				if (_output_presentation_window->height == _display_modes[i].Height &&
                    //					_output_presentation_window->width == _display_modes[i].Width)
                    //				{
                    //					auto _refresh_rate = _display_modes[i].RefreshRate;
                    //					_numerator = _refresh_rate.Numerator;
                    //					_denominator = _refresh_rate.Denominator;
                    //				}
                    //			}
                    //		}
                    //		else
                    //		{
                    //			logger.warning(L"Could not get display modes list for graphics device: " + _device_name +
                    //				L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex));
                    //		}
                    //		_display_modes.clear();
                    //	}
                    //	else
                    //	{
                    //		logger.warning(L"Could not get number of display modes list for graphics device: " + _device_name +
                    //			L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex));
                    //	}

                    //}
#pragma endregion

                    // Disable full screen with ALT+Enter
                    _hr = w_graphics_device::dx_dxgi_factory->MakeWindowAssociation(_output_presentation_window->hwnd, DXGI_MWA_NO_ALT_ENTER);
                    V(_hr, L"disabling ALT+Enter for presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                        this->_name, 2);

                    // Describe and create the swap chain.
                    DXGI_SWAP_CHAIN_DESC1 _swap_chain_desc = {};
                    _swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
                    _swap_chain_desc.BufferCount = _desired_number_of_swapchain_images;
                    _swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                    _swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                    _swap_chain_desc.SampleDesc.Count = 1;//No hardware multisampling
                    _swap_chain_desc.SampleDesc.Quality = 0;
                    _swap_chain_desc.Format = _output_presentation_window->dx_swap_chain_selected_format;
                    _swap_chain_desc.Width = _output_presentation_window->width;
                    _swap_chain_desc.Height = _output_presentation_window->height;
                    _swap_chain_desc.Scaling = DXGI_SCALING::DXGI_SCALING_NONE;
                    _swap_chain_desc.Stereo = FALSE;
                    _swap_chain_desc.Flags = 0;

                    {
                        ComPtr<IDXGISwapChain1> _swap_chain = nullptr;

                        _hr = w_graphics_device::dx_dxgi_factory->CreateSwapChainForHwnd(
                            _output_presentation_window->dx_command_queue.Get(),//Swap chain needs the queue so that it can force a flush on it.
                            _output_presentation_window->hwnd,
                            &_swap_chain_desc,
                            nullptr,
                            nullptr,
                            &_swap_chain);

                        V(_hr, L"create swap chain from hwnd for graphics device: " + _device_name + L" ID:" + std::to_wstring(_device_id) +
                            L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
                            this->_name, 2);

                        _hr = _swap_chain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&_output_presentation_window->dx_swap_chain);
                        if (FAILED(_hr))
                        {
                            logger.error(L"error on getting swap chain 3 from swap chain 1 for graphics device: " + L" ID:" + std::to_wstring(_device_id) +
                                _device_name + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex));
                            release();
                            std::exit(EXIT_FAILURE);
                        }
                    }
                }

#elif defined(__UWP)

				auto _display_rotation = _compute_display_rotation(_output_presentation_window->window_native_orientation,
					_output_presentation_window->window_current_orientation);

				float _back_buffer_width = 0;
				float _back_buffer_height = 0;

				//_update_render_target_size
				{
					auto _effective_dpi = _output_presentation_window->window_dpi;

					/*
						To improve battery life on high resolution devices, render to a smaller render target
						and allow the GPU to scale the output when it is presented.

						The default thresholds that define a "high resolution" display. If the thresholds
						are exceeded and SupportHighResolutions is false, the dimensions will be scaled
						by 50%.
					*/
					static const float _DpiThreshold = 192.0f;		// 200% of standard desktop display.
					static const float _WidthThreshold = 1920.0f;	// 1080p width.
					static const float _HeightThreshold = 1080.0f;	// 1080p height.
					if (!_output_presentation_window->support_high_resolutions && _output_presentation_window->window_dpi > _DpiThreshold)
					{
						float _width = w_graphics_device_manager::convert_dips_to_pixels(
							_output_presentation_window->window_size.Width,
							_output_presentation_window->window_dpi);

						float _height = w_graphics_device_manager::convert_dips_to_pixels(_output_presentation_window->window_size.Height,
							_output_presentation_window->window_dpi);

						/*
							When the device is in portrait orientation, height > width. Compare the
							larger dimension against the width threshold and the smaller dimension
							against the height threshold.
						*/
						if (max(_width, _height) > _WidthThreshold && min(_width, _height) > _HeightThreshold)
						{
							// To scale the app we change the effective DPI. Logical size does not change.
							_effective_dpi /= 2.0f;
						}
					}

					// Calculate the necessary render target size in pixels.
					_back_buffer_width = w_graphics_device_manager::convert_dips_to_pixels(_output_presentation_window->window_size.Width,
						_effective_dpi);
					_back_buffer_height = w_graphics_device_manager::convert_dips_to_pixels(_output_presentation_window->window_size.Height,
						_effective_dpi);

					// Prevent zero size DirectX content from being created.
					_back_buffer_width = max(_back_buffer_width, 1.0f);
					_back_buffer_height = max(_back_buffer_height, 1.0f);
				}

				bool _swap_dimensions = _display_rotation == DXGI_MODE_ROTATION_ROTATE90 || _display_rotation == DXGI_MODE_ROTATION_ROTATE270;
				if (_swap_dimensions)
				{
					//need to swap size
					_back_buffer_width = lround(_back_buffer_height);
					_back_buffer_height = lround(_back_buffer_width);
				}
				else
				{
					_back_buffer_width = lround(_back_buffer_width);
					_back_buffer_height = lround(_back_buffer_height);
				}

				if (_output_presentation_window->dx_swap_chain != nullptr)
				{
					//If the swap chain already exists, resize it.
					_hr = _output_presentation_window->dx_swap_chain->ResizeBuffers(_desired_number_of_swapchain_images,
						static_cast<UINT>(_back_buffer_width),
						static_cast<UINT>(_back_buffer_height),
						_output_presentation_window->dx_swap_chain_selected_format,
						0);

					if (_hr == DXGI_ERROR_DEVICE_REMOVED || _hr == DXGI_ERROR_DEVICE_RESET)
					{
						logger.error(L"Error on resizing swap chain, because of DXGI_ERROR_DEVICE_REMOVED or DXGI_ERROR_DEVICE_RESET for graphics device: "
							+ _device_name L" ID:" + std::to_wstring(_device_id));
						// If the device was removed for any reason, a new device and swap chain will need to be created.
						pGDevice->dx_device_removed = true;
						return;
					}
					else if(_hr != S_OK)
					{
						logger.error(L"Error on resizing swap chain, unknown error for graphics device: "
							+ _device_name L" ID:" + std::to_wstring(_device_id));
						release();
						std::exit(EXIT_FAILURE);
					}
				}
				else
				{
					DXGI_SCALING _scaling = _output_presentation_window->support_high_resolutions ? DXGI_SCALING_NONE : DXGI_SCALING_STRETCH;

					// Describe and create the swap chain.
					DXGI_SWAP_CHAIN_DESC1 _swap_chain_desc = {};
					_swap_chain_desc.BufferCount = _desired_number_of_swapchain_images;
					_swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
					_swap_chain_desc.Stereo = false;
					_swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
					_swap_chain_desc.SampleDesc.Count = 1;//No hardware multisampling
					_swap_chain_desc.SampleDesc.Quality = 0;
					_swap_chain_desc.Width = static_cast<UINT>(_back_buffer_width);
					_swap_chain_desc.Height = static_cast<UINT>(_back_buffer_height);
					_swap_chain_desc.Format = _output_presentation_window->dx_swap_chain_selected_format;
					_swap_chain_desc.Scaling = _scaling;
					_swap_chain_desc.Flags = 0;
					_swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

					ComPtr<IDXGISwapChain1> _swap_chain;
					_hr = pGDevice->dx_dxgi_factory->CreateSwapChainForCoreWindow(
#ifdef __DX12__
						_output_presentation_window->dx_command_queue.Get(),//Swap chain needs the queue so that it can force a flush on it.
#else
						pGDevice->dx_device.Get(),
#endif
						_output_presentation_window->window,
						&_swap_chain_desc,
						nullptr,
						&_swap_chain);
					V(_hr, L"create swap chain from core window for graphics device: " + _device_name + L" ID:" + std::to_wstring(_device_id) +
						L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
						this->_name, 2);

					//getting swap chain 3 from swap chain 1
					_hr = _swap_chain.As(&_output_presentation_window->dx_swap_chain);
					if (FAILED(_hr))
					{
						logger.error(L"error on getting swap chain 3 from swap chain 1 for graphics device: " + L" ID:" + std::to_wstring(_device_id) +
							_device_name + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex));
						release();
						std::exit(EXIT_FAILURE);
					}

					_hr = _output_presentation_window->dx_swap_chain->SetRotation(_display_rotation);
					if (FAILED(_hr))
					{
						logger.error(L"setting rotation of swap chain for graphics device: " + L" ID:" + std::to_wstring(_device_id) +
							_device_name + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex));
						release();
						std::exit(EXIT_FAILURE);
					}

					switch (_display_rotation)
					{
					case DXGI_MODE_ROTATION_IDENTITY:
						// 0-degree Z-rotation
						_output_presentation_window->orientation_transform_3D = DirectX::XMFLOAT4X4(
							1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f
						);
						break;

					case DXGI_MODE_ROTATION_ROTATE90:
						// 90-degree Z-rotation
						_output_presentation_window->orientation_transform_3D = DirectX::XMFLOAT4X4(
							0.0f, 1.0f, 0.0f, 0.0f,
							-1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f
						);
						break;

					case DXGI_MODE_ROTATION_ROTATE180:
						// 180-degree Z-rotation
						_output_presentation_window->orientation_transform_3D = DirectX::XMFLOAT4X4(
							-1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, -1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f
						);
						break;

					case DXGI_MODE_ROTATION_ROTATE270:
						// 270-degree Z-rotation
						_output_presentation_window->orientation_transform_3D = DirectX::XMFLOAT4X4(
							0.0f, -1.0f, 0.0f, 0.0f,
							1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f
						);
						break;

					case DXGI_MODE_ROTATION_UNSPECIFIED:
						logger.warning(L"DXGI_MODE_ROTATION_UNSPECIFIED for graphics device: " + L" ID:" + std::to_wstring(_device_id) +
							_device_name + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex));
					}

				}
#endif

				// Describe and create a render target view (RTV) descriptor heap.
				D3D12_DESCRIPTOR_HEAP_DESC _render_target_view_heap_desc = {};
				_render_target_view_heap_desc.NumDescriptors = _desired_number_of_swapchain_images;
				_render_target_view_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				_render_target_view_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

				_hr = pGDevice->dx_device->CreateDescriptorHeap(&_render_target_view_heap_desc, IID_PPV_ARGS(&_output_presentation_window->dx_render_target_view_heap));
				V(_hr, L"creating render target heap descriptorfor graphics device: " + _device_name + L" ID:" + std::to_wstring(_device_id) +
					L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
					this->_name, 2);

				_output_presentation_window->dx_render_target_descriptor_size = pGDevice->dx_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

				// Create frame resources.
				DirectX::CD3DX12_CPU_DESCRIPTOR_HANDLE _render_target_descriptor_handle(_output_presentation_window->dx_render_target_view_heap->GetCPUDescriptorHandleForHeapStart());

				// Create a render target view for each image of swap chain.
				_output_presentation_window->dx_swap_chain_image_views.resize(_desired_number_of_swapchain_images);
				for (UINT i = 0; i < _desired_number_of_swapchain_images; ++i)
				{
					_hr = _output_presentation_window->dx_swap_chain->GetBuffer(i, IID_PPV_ARGS(&_output_presentation_window->dx_swap_chain_image_views[i]));
					V(_hr, L"creating render target image view for swap chain of graphics device: " + _device_name + L" ID:" + std::to_wstring(_device_id) +
						L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
						this->_name, 2);

					pGDevice->dx_device->CreateRenderTargetView(_output_presentation_window->dx_swap_chain_image_views[i], nullptr, _render_target_descriptor_handle);

					_render_target_descriptor_handle.Offset(1, _output_presentation_window->dx_render_target_descriptor_size);
				}

				//get the swap chain frame index
				_output_presentation_window->dx_swap_chain_image_index = _output_presentation_window->dx_swap_chain->GetCurrentBackBufferIndex();

#elif defined(__VULKAN__)
				auto _device_name = pGDevice->device_name;
				auto _device_id = pGDevice->device_id;
				auto _output_presentation_window = &(pGDevice->output_presentation_windows.at(pOutputPresentationWindowIndex));

				auto _vk_presentation_surface = _output_presentation_window->vk_presentation_surface;

				for (size_t j = 0; j < pGDevice->vk_queue_family_properties.size(); ++j)
				{
					//check if this device support presentation
					auto _hr = vkGetPhysicalDeviceSurfaceSupportKHR(pGDevice->vk_physical_device,
						static_cast<uint32_t>(j),
						_vk_presentation_surface,
						&pGDevice->vk_queue_family_supports_present[j]);

					V(_hr, L"could not get physical device surface support for graphics device: " +
						std::wstring(_device_name.begin(), _device_name.end()) + L" ID:" + std::to_wstring(_device_id) +
						L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
						this->_name, 2);

					if (pGDevice->vk_present_queue.index == UINT32_MAX &&
						pGDevice->vk_graphics_queue.index != UINT32_MAX &&
						pGDevice->vk_queue_family_supports_present[j])
					{
						pGDevice->vk_present_queue.index = static_cast<uint32_t>(j);
					}
				}

				V(pGDevice->vk_present_queue.index == UINT32_MAX ? S_FALSE : S_OK,
					L"could not find queue family which supports presentation for graphics device: " +
					std::wstring(_device_name.begin(), _device_name.end()) + L" ID:" + std::to_wstring(_device_id) +
					L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex),
					this->_name, 2);

				//get the list of VkFormats that are supported:
				uint32_t _vk_format_count;
				auto _hr = vkGetPhysicalDeviceSurfaceFormatsKHR(pGDevice->vk_physical_device,
					_vk_presentation_surface,
					&_vk_format_count,
					NULL);
				if (_hr)
				{
					logger.error("could not get number of physical device surface formats for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

				_output_presentation_window->vk_surface_formats.resize(_vk_format_count);

				_hr = vkGetPhysicalDeviceSurfaceFormatsKHR(pGDevice->vk_physical_device,
					_vk_presentation_surface,
					&_vk_format_count,
					_output_presentation_window->vk_surface_formats.data());
				if (_hr)
				{
					logger.error("could not get physical device surface formats for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

				/*
					If the format list includes just one entry of VK_FORMAT_UNDEFINED,
					the surface has no preferred format.  Otherwise, at least one
					supported format will be returned.
				*/
				if (_vk_format_count == 1 && _output_presentation_window->vk_surface_formats[0].format == VkFormat::VK_FORMAT_UNDEFINED)
				{
#ifdef __ANDROID
					_output_presentation_window->vk_swap_chain_selected_format.format = VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
#else
					_output_presentation_window->vk_swap_chain_selected_format.format = VK_FORMAT_B8G8R8A8_UNORM;
#endif // __ANDROID

					_output_presentation_window->vk_swap_chain_selected_format.colorSpace = VkColorSpaceKHR::VK_COLORSPACE_SRGB_NONLINEAR_KHR;
				}
				else
				{
					bool _find_format = false;
					for (auto _iter : _output_presentation_window->vk_surface_formats)
					{
						if (_iter.format == _output_presentation_window->vk_swap_chain_selected_format.format)
						{
							_find_format = true;
							break;
						}
					}
					//use the default one
					if (!_find_format)
					{
						_output_presentation_window->vk_swap_chain_selected_format = _output_presentation_window->vk_surface_formats[0];
					}
				}

				VkSurfaceCapabilitiesKHR _surface_capabilities;
				_hr = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pGDevice->vk_physical_device,
					_vk_presentation_surface,
					&_surface_capabilities);
				if (_hr)
				{
					logger.error("error on create vulkan surface capabilities for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) +
						" and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

				//get the count of present modes
				uint32_t _present_mode_count;
				_hr = vkGetPhysicalDeviceSurfacePresentModesKHR(pGDevice->vk_physical_device,
					_vk_presentation_surface,
					&_present_mode_count, nullptr);
				if (_hr)
				{
					logger.error("error on getting vulkan present mode(s) count for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

				//get present modes
				std::vector<VkPresentModeKHR> _present_modes(_present_mode_count);
				_hr = vkGetPhysicalDeviceSurfacePresentModesKHR(pGDevice->vk_physical_device,
					_vk_presentation_surface,
					&_present_mode_count,
					_present_modes.data());
				if (_hr)
				{
					logger.error("error on getting vulkan present mode(s) for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

				//width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
				VkExtent2D _swap_chain_extent;
				if (_surface_capabilities.currentExtent.width == 0xFFFFFFFF)
				{
					// If the surface size is undefined, the size is set to the size of the images requested.
					_swap_chain_extent.width = _output_presentation_window->width;
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
				VkPresentModeKHR _swap_chain_present_mode = _output_presentation_window->v_sync ?
					VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;

				/*
					Determine the number of VkImage's to use in the swap chain.
					We need to acquire only 1 presentable image at at time.
					Asking for minImageCount images ensures that we can acquire
					1 presentable image as long as we present it before attempting
					to acquire another.
				*/
                uint32_t _desired_number_of_swapchain_images = _surface_capabilities.minImageCount + 1;
                if ((_surface_capabilities.maxImageCount > 0) &&
                    (_desired_number_of_swapchain_images > _surface_capabilities.maxImageCount))
                {
                    _desired_number_of_swapchain_images = _surface_capabilities.maxImageCount;
                }

                if (_desired_number_of_swapchain_images < 2)
                {
                    logger.warning("The images count of surface capabilities and swap chain is less than two.");
                }
                
				VkSurfaceTransformFlagBitsKHR _pre_transform;
				if (_surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
				{
					_pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
				}
				else
				{
					_pre_transform = _surface_capabilities.currentTransform;
				}

				uint32_t _queue_family = 0;
				VkSwapchainCreateInfoKHR _swap_chain_create_info = {};
				_swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				_swap_chain_create_info.pNext = nullptr;
				_swap_chain_create_info.surface = _vk_presentation_surface;
				_swap_chain_create_info.minImageCount = _desired_number_of_swapchain_images;
				_swap_chain_create_info.imageFormat = _output_presentation_window->vk_swap_chain_selected_format.format;
				_swap_chain_create_info.imageColorSpace = _output_presentation_window->vk_swap_chain_selected_format.colorSpace;
				_swap_chain_create_info.imageExtent = _surface_capabilities.currentExtent;
				_swap_chain_create_info.preTransform = _pre_transform;
				_swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
				_swap_chain_create_info.imageArrayLayers = 1;
				_swap_chain_create_info.presentMode = _swap_chain_present_mode;
				_swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;
				_swap_chain_create_info.clipped = VK_TRUE;
				_swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
				_swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				_swap_chain_create_info.queueFamilyIndexCount = 1;
				_swap_chain_create_info.pQueueFamilyIndices = &_queue_family;

				UINT _queue_family_indices[2] =
				{
					pGDevice->vk_graphics_queue.index,
					pGDevice->vk_present_queue.index,
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
				if (_hr || !_output_presentation_window->vk_swap_chain)
				{
					logger.error("error on creating swap chain for vulkan for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

                //get the count of swap chain 's images
				uint32_t _swap_chain_image_count = UINT32_MAX;
				_hr = vkGetSwapchainImagesKHR(pGDevice->vk_device,
					_output_presentation_window->vk_swap_chain,
					&_swap_chain_image_count,
					nullptr);
				if (_hr || _swap_chain_image_count == UINT32_MAX)
				{
					logger.error("error on getting total available image counts of swap chain for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

				std::vector<VkImage> _swap_chain_images(_swap_chain_image_count);
				_hr = vkGetSwapchainImagesKHR(pGDevice->vk_device,
					_output_presentation_window->vk_swap_chain,
					&_swap_chain_image_count,
					_swap_chain_images.data());
				if (_hr)
				{
					logger.error("error on getting total available images of swap chain for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

				for (size_t j = 0; j < _swap_chain_images.size(); ++j)
				{
					VkImageViewCreateInfo _color_image_view = {};

					_color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					_color_image_view.pNext = nullptr;
					_color_image_view.flags = 0;
					_color_image_view.image = _swap_chain_images[j];
					_color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
					_color_image_view.format = _output_presentation_window->vk_swap_chain_selected_format.format;
					_color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
					_color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
					_color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
					_color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
					_color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					_color_image_view.subresourceRange.baseMipLevel = 0;
					_color_image_view.subresourceRange.levelCount = 1;
					_color_image_view.subresourceRange.baseArrayLayer = 0;
					_color_image_view.subresourceRange.layerCount = 1;

					w_image_view _image_view;
					_image_view.image = _swap_chain_images[j];

					_hr = vkCreateImageView(pGDevice->vk_device,
						&_color_image_view,
						nullptr,
						&_image_view.view);
					if (_hr)
					{
						logger.error("error on creating image view total available images of swap chain for graphics device: " +
							_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
						release();
						std::exit(EXIT_FAILURE);
					}

					_output_presentation_window->vk_swap_chain_image_views.push_back(_image_view);
				}
#endif //__DX12__ __VULKAN__
			}

			//Release all resources
			ULONG release()
			{
				//release all windows info
				this->_windows_info.clear();
				this->_name = "";
                if (this->_config.debug_gpu)
                {
#ifdef __VULKAN__
                    sDestroyDebugReportCallback(w_graphics_device::vk_instance, MsgCallback, nullptr);
#endif
                }

				return 1;
			}

#pragma region Getters

			std::map<int, std::vector<w_window_info>> get_output_windows_info() const
			{
				return this->_windows_info;
			}

#pragma endregion

#pragma region Setters

			void set_graphics_device_manager_configs(_In_ const w_graphics_device_manager_configs& pConfig)
			{
				this->_config = pConfig;
			}

			void set_output_windows_info(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
			{
				this->_windows_info = pOutputWindowsInfo;
			}

#pragma endregion

		private:
            
			void _create_depth_stencil_buffer(_In_ const std::shared_ptr<w_graphics_device>& pGDevice, _In_ size_t pOutputPresentationWindowIndex)
			{
                auto _device_id = pGDevice->device_id;
                auto _window = &(pGDevice->output_presentation_windows.at(pOutputPresentationWindowIndex));

#ifdef __DX12__ 
                
#elif defined(__VULKAN__)
                auto _device_name = pGDevice->device_name;

				VkImageCreateInfo _depth_stencil_image_create_info = {};
				const VkFormat _depth_format = w_graphics_device_manager::find_supported_format(
                    pGDevice,
                    { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
                if (_depth_format == VkFormat::VK_FORMAT_UNDEFINED)
                {
                    logger.error("Depth format not supported for graphics device: " +
                        _device_name + " ID:" + std::to_string(_device_id) + 
                        " and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
                    release();
                    std::exit(EXIT_FAILURE);
                }

                _window->vk_depth_buffer_format = _depth_format;

				//define depth stencil image description
				_depth_stencil_image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				_depth_stencil_image_create_info.pNext = nullptr;
				_depth_stencil_image_create_info.imageType = VK_IMAGE_TYPE_2D;
				_depth_stencil_image_create_info.format = _depth_format;
				_depth_stencil_image_create_info.extent.width = _window->width;
				_depth_stencil_image_create_info.extent.height = _window->height;
				_depth_stencil_image_create_info.extent.depth = 1;
				_depth_stencil_image_create_info.mipLevels = 1;
				_depth_stencil_image_create_info.arrayLayers = 1;
				_depth_stencil_image_create_info.samples = NUM_SAMPLES;
				_depth_stencil_image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				_depth_stencil_image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
				_depth_stencil_image_create_info.queueFamilyIndexCount = 0;
				_depth_stencil_image_create_info.pQueueFamilyIndices = nullptr;
				_depth_stencil_image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				_depth_stencil_image_create_info.flags = 0;

				VkMemoryAllocateInfo _mem_alloc = {};
				_mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				_mem_alloc.pNext = nullptr;
				_mem_alloc.allocationSize = 0;
				_mem_alloc.memoryTypeIndex = 0;

				//Create image of depth stencil
				auto _hr = vkCreateImage(pGDevice->vk_device,
					&_depth_stencil_image_create_info,
					nullptr,
					&_window->vk_depth_buffer_image_view.image);
				if (_hr)
				{
					logger.error("error on creating depth buffer for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

				VkMemoryRequirements _mem_reqs;
				vkGetImageMemoryRequirements(pGDevice->vk_device,
					_window->vk_depth_buffer_image_view.image,
					&_mem_reqs);

				_mem_alloc.allocationSize = _mem_reqs.size;

				//use the memory properties to determine the type of memory required
				_hr = w_graphics_device_manager::memory_type_from_properties(pGDevice->vk_physical_device_memory_properties,
					_mem_reqs.memoryTypeBits,
					0, // No Requirements
					&_mem_alloc.memoryTypeIndex);
				if (_hr)
				{
					logger.error("error on determining the type of memory required from memory properties for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + 
                        std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

				//allocate memory
				_hr = vkAllocateMemory(pGDevice->vk_device,
					&_mem_alloc,
					nullptr,
					&_window->vk_depth_buffer_memory);
				if (_hr)
				{
					logger.error("error on allocating memory for depth buffer image for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " +
                        std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

				//bind memory
				_hr = vkBindImageMemory(pGDevice->vk_device,
					_window->vk_depth_buffer_image_view.image,
					_window->vk_depth_buffer_memory,
					0);
				if (_hr)
				{
					logger.error("error on binding to memory for depth buffer image for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

				//create depth stencil buffer image view
				VkImageViewCreateInfo _depth_stencil_view_info = {};
				_depth_stencil_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				_depth_stencil_view_info.pNext = nullptr;
				_depth_stencil_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
				_depth_stencil_view_info.image = _window->vk_depth_buffer_image_view.image;
				_depth_stencil_view_info.format = _depth_format;
				_depth_stencil_view_info.components.r = VK_COMPONENT_SWIZZLE_R;
				_depth_stencil_view_info.components.g = VK_COMPONENT_SWIZZLE_G;
				_depth_stencil_view_info.components.b = VK_COMPONENT_SWIZZLE_B;
				_depth_stencil_view_info.components.a = VK_COMPONENT_SWIZZLE_A;
				_depth_stencil_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				_depth_stencil_view_info.subresourceRange.baseMipLevel = 0;
				_depth_stencil_view_info.subresourceRange.levelCount = 1;
				_depth_stencil_view_info.subresourceRange.baseArrayLayer = 0;
				_depth_stencil_view_info.subresourceRange.layerCount = 1;
				_depth_stencil_view_info.flags = 0;

				_hr = vkCreateImageView(pGDevice->vk_device,
					&_depth_stencil_view_info,
					nullptr,
					&_window->vk_depth_buffer_image_view.view);
				if (_hr)
				{
					logger.error("error on creating image view for depth buffer image for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + 
                        std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}
#endif
			}
            
			void _create_fences(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ size_t pOutputPresentationWindowIndex)
			{
#ifdef __DX12__ 
				auto _device_name = wolf::system::convert::string_to_wstring(pGDevice->device_name);
				auto _device_id = pGDevice->device_id;

				auto _output_presentation_window = &(pGDevice->output_presentation_windows.at(pOutputPresentationWindowIndex));

				auto _hr = pGDevice->dx_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_output_presentation_window->dx_fence));
				if (FAILED(_hr))
				{
					logger.error(L"error on creating directx fence for graphics device: " +
						_device_name + L" ID:" + std::to_wstring(_device_id) + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

				_output_presentation_window->dx_fence_event = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
				if (_output_presentation_window->dx_fence_event == NULL)
				{
					logger.error(L"error on creating directx event handle for graphics device: " +
						_device_name + L" ID:" + std::to_wstring(_device_id) + L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}

				_output_presentation_window->dx_fence_value = 1;

#elif defined(__VULKAN__)
				auto _device_name = pGDevice->device_name;
				auto _device_id = pGDevice->device_id;
				auto _output_presentation_window = &(pGDevice->output_presentation_windows.at(pOutputPresentationWindowIndex));

                //create semaphores fro this graphics device
                if (_output_presentation_window->vk_swap_chain_image_is_available_semaphore.initialize(pGDevice) == S_FALSE)
                {
                    logger.error("error on creating image_is_available semaphore for graphics device: " +
                                 _device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
                    release();
                    std::exit(EXIT_FAILURE);
                }
                if (_output_presentation_window->vk_rendering_done_semaphore.initialize(pGDevice) == S_FALSE)
                {
					logger.error("error on creating rendering_is_done semaphore for graphics device: " +
						_device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputPresentationWindowIndex));
					release();
					std::exit(EXIT_FAILURE);
				}
#endif
			}
            
//			HRESULT _record_command_buffers(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
//                                         _In_ const size_t pOutputWindowIndex)
//			{
//                
//                auto _output_window = &(pGDevice->output_presentation_windows.at(pOutputWindowIndex));
//                const char* _command_buffers_name = "clear_color_screen";
//                
//#ifdef __DX12__ 
//				auto _device_name = wolf::system::convert::string_to_wstring(pGDevice->device_name);
//				auto _device_id = pGDevice->device_id;
//
//				auto _output_presentation_window = &(pGDevice->output_presentation_windows.at(pOutputWindowIndex));
//
//				//create command allocator pool
//				auto _hr = pGDevice->dx_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
//					IID_PPV_ARGS(&_output_presentation_window->dx_command_allocator_pool));
//				if (FAILED(_hr))
//				{
//					logger.error(L"creating directx command allocator pook for graphics device: " + _device_name + L" ID:" + std::to_wstring(_device_id) + 
//						L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex));
//					release();
//					std::exit(EXIT_FAILURE);
//				}
//
//				//Describe and create the command queue.
//				D3D12_COMMAND_QUEUE_DESC _command_queue_desc = {};
//				_command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
//				_command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
//				_command_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY::D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
//				_command_queue_desc.NodeMask = 0;
//
//				_hr = pGDevice->dx_device->CreateCommandQueue(&_command_queue_desc, IID_PPV_ARGS(&_output_presentation_window->dx_command_queue));
//				if (FAILED(_hr))
//				{
//					logger.error(L"creating directx command queue for graphics device : " + _device_name + L" ID:" + std::to_wstring(_device_id) +
//						L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex));
//					release();
//					std::exit(EXIT_FAILURE);
//				}
//
//				//create basic command list
//				_hr = pGDevice->dx_device->CreateCommandList(0,
//					D3D12_COMMAND_LIST_TYPE_DIRECT,
//					_output_presentation_window->dx_command_allocator_pool.Get(),
//					nullptr,
//					IID_PPV_ARGS(&_output_presentation_window->dx_command_list));
//				if (FAILED(_hr))
//				{
//					logger.error(L"creating command list for graphics device : " + _device_name + L" ID:" + std::to_wstring(_device_id) +
//						L" and presentation window: " + std::to_wstring(pOutputPresentationWindowIndex));
//					release();
//					std::exit(EXIT_FAILURE);
//				}
//
//				//close command list for now
//				_output_presentation_window->dx_command_list->Close();
//
//#elif defined(__VULKAN__) 
//
//				auto _device_name = pGDevice->device_name;
//				auto _device_id = pGDevice->device_id;
//                
//                auto _clear_screen_command_buffer = new w_command_buffers();
//                _clear_screen_command_buffer->load(pGDevice, _output_window->vk_swap_chain_image_views.size());
//                auto _hr = pGDevice->store_to_global_command_buffers(_command_buffers_name,
//                                                                     _clear_screen_command_buffer,
//                                                                     pOutputWindowIndex);
//                if (_hr)
//                {
//                    //we already output error in create_command_buffers
//                    release();
//                    std::exit(EXIT_FAILURE);
//                }
//                
//				VkImageSubresourceRange _sub_resource_range = {};
//				_sub_resource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//				_sub_resource_range.baseMipLevel = 0;
//				_sub_resource_range.levelCount = 1;
//				_sub_resource_range.baseArrayLayer = 0;
//				_sub_resource_range.layerCount = 1;
//
//				VkClearColorValue _vk_clear_color =
//				{
//					static_cast<float>(_output_window->clear_color.r / 255.0f),
//					static_cast<float>(_output_window->clear_color.g / 255.0f),
//					static_cast<float>(_output_window->clear_color.b / 255.0f),
//					static_cast<float>(_output_window->clear_color.a / 255.0f)
//				};
//
//				//record clear screen command buffer for every swap chain image
//				for (uint32_t i = 0; i < _output_window->command_buffers.at(_command_buffers_name)->get_commands_size(); ++i)
//				{
//					// Change layout of image to be optimal for clearing
//					// Note: previous layout doesn't matter, which will likely cause contents to be discarded
//					VkImageMemoryBarrier _present_to_clear_barrier = {};
//					_present_to_clear_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//					_present_to_clear_barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
//					_present_to_clear_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//					_present_to_clear_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//					_present_to_clear_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//					_present_to_clear_barrier.srcQueueFamilyIndex = pGDevice->vk_present_queue.index;
//					_present_to_clear_barrier.dstQueueFamilyIndex = pGDevice->vk_graphics_queue.index;
//					_present_to_clear_barrier.image = _output_window->vk_swap_chain_image_views[i].image;
//					_present_to_clear_barrier.subresourceRange = _sub_resource_range;
//
//					//change layout of image to be optimal for presenting
//					VkImageMemoryBarrier _clear_to_present_barrier = {};
//					_clear_to_present_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//					_clear_to_present_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//					_clear_to_present_barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
//					_clear_to_present_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//					_clear_to_present_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//					_clear_to_present_barrier.srcQueueFamilyIndex = pGDevice->vk_graphics_queue.index;
//					_clear_to_present_barrier.dstQueueFamilyIndex = pGDevice->vk_present_queue.index;
//					_clear_to_present_barrier.image = _output_window->vk_swap_chain_image_views[i].image;
//					_clear_to_present_barrier.subresourceRange = _sub_resource_range;
//                    
//                    
//					//record command buffer
//                    auto _cmd = _clear_screen_command_buffer->get_command_at(i);
//                    
//                    auto _hr = _clear_screen_command_buffer->begin(i);
//					if (_hr)
//					{
//						logger.error("error on beginning command buffer of graphics device: " + _device_name +
//							" ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputWindowIndex));
//						release();
//						std::exit(EXIT_FAILURE);
//					}
//
//					vkCmdPipelineBarrier(_cmd,
//						VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
//						0,
//						0,
//						nullptr,
//						0,
//						nullptr,
//						1,
//						&_present_to_clear_barrier);
//
//					vkCmdClearColorImage(_cmd,
//						_output_window->vk_swap_chain_image_views[i].image,
//						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//						&_vk_clear_color,
//						1,
//						&_sub_resource_range);
//
//					vkCmdPipelineBarrier(_cmd,
//						VK_PIPELINE_STAGE_TRANSFER_BIT,
//						VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
//						0,
//						0,
//						nullptr,
//						0,
//						nullptr,
//						1,
//						&_clear_to_present_barrier);
//
//                    _hr = _clear_screen_command_buffer->end(i);
//                    if (_hr)
//                    {
//                        logger.error("error on ending command buffer of graphics device: " +
//                                     _device_name + " ID:" + std::to_string(_device_id) + " and presentation window: " + std::to_string(pOutputWindowIndex));
//                        release();
//                        std::exit(EXIT_FAILURE);
//                    }
//				}
//                
//#endif
//                return S_OK;
//			}
            
#ifdef __UWP
			// This method determines the rotation between the display device's native Orientation and the current display orientation.
			static DXGI_MODE_ROTATION _compute_display_rotation(_In_ Windows::Graphics::Display::DisplayOrientations pNativeRotation,
				_In_ Windows::Graphics::Display::DisplayOrientations pCurrentRotation)
			{
				auto _rotation = DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_UNSPECIFIED;

				using namespace Windows::Graphics::Display;

				// pNativeOrientation can only be Landscape or Portrait even though, the DisplayOrientations enum has other values.
				switch (pNativeRotation)
				{
				case DisplayOrientations::Landscape:
					switch (pCurrentRotation)
					{
					case DisplayOrientations::Landscape:
						_rotation = DXGI_MODE_ROTATION_IDENTITY;
						break;

					case DisplayOrientations::Portrait:
						_rotation = DXGI_MODE_ROTATION_ROTATE270;
						break;

					case DisplayOrientations::LandscapeFlipped:
						_rotation = DXGI_MODE_ROTATION_ROTATE180;
						break;

					case DisplayOrientations::PortraitFlipped:
						_rotation = DXGI_MODE_ROTATION_ROTATE90;
						break;
					}
					break;

				case DisplayOrientations::Portrait:
					switch (pCurrentRotation)
					{
					case DisplayOrientations::Landscape:
						_rotation = DXGI_MODE_ROTATION_ROTATE90;
						break;

					case DisplayOrientations::Portrait:
						_rotation = DXGI_MODE_ROTATION_IDENTITY;
						break;

					case DisplayOrientations::LandscapeFlipped:
						_rotation = DXGI_MODE_ROTATION_ROTATE270;
						break;

					case DisplayOrientations::PortraitFlipped:
						_rotation = DXGI_MODE_ROTATION_ROTATE180;
						break;
					}
					break;
				}
				return _rotation;
			}
#endif
			w_graphics_device_manager_configs					_config;
            std::map<int, std::vector<w_window_info>>           _windows_info;
            std::string                                         _name;;
        };
    }
}

#pragma endregion

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

w_graphics_device_manager::w_graphics_device_manager() :
	_pimp(new w_graphics_device_manager_pimp())
{
	_super::set_class_name("w_graphics_device_manager_pimp");// typeid(this).name());
   
#ifdef __WIN32
        auto _hr = CoInitialize(NULL);
	V(_hr, L"CoInitialize", _super::name, 3, true, false);
#endif

#ifdef __ANDROID
	InitVulkan();
#endif
}

w_graphics_device_manager::~w_graphics_device_manager()
{
	release();
}

void w_graphics_device_manager::initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
    //store information of windows for the first time
    this->_pimp->set_output_windows_info(pOutputWindowsInfo);
    this->_pimp->enumerate_devices(this->graphics_devices, &this->on_device_features_fetched);
    
    //If there is no associated graphics device
    if (this->graphics_devices.size() == 0)
    {
		logger.error("No graphics device created");
		release();
		std::exit(EXIT_FAILURE);
    }

    _load_shared_resources();
}

void w_graphics_device_manager::_load_shared_resources()
{
    auto _gDevice = this->graphics_devices.at(0);

    std::wstring _path = content_path + L"../logo.jpg";
    w_texture* _default = nullptr;
    if (w_texture::load_to_shared_textures(_gDevice, _path, &_default) == S_OK)
    {
        w_texture::default_texture = _default;
    }
    _default = nullptr;
}

void w_graphics_device_manager::on_device_lost()
{
	logger.write(L"Graphics device is going to reset");
	
	auto _windows_info = this->_pimp->get_output_windows_info();
	release();
	this->_pimp = new w_graphics_device_manager_pimp();
	initialize(_windows_info);
	
	logger.write(L"Graphics device reseted successfully");
}

void w_graphics_device_manager::on_suspend()
{
#ifdef __DX12__
	for (size_t i = 0; i < this->graphics_devices.size(); ++i)
	{
		auto _gDevice = this->graphics_devices[i];
		if (_gDevice->dx_device_removed)
		{
			on_device_lost();
		}

		ComPtr<IDXGIDevice3> _dxgi_device;
		auto _hr = _gDevice->dx_device.As(&_dxgi_device);
		if (FAILED(_hr))
		{
			V(S_FALSE, "getting dxgi device from d3d11device for graphics device: " + _gDevice->device_name + " ID:" + std::to_string(_gDevice->device_id) + , this->name, 2);
		}
		else
		{
			_dxgi_device->Trim();
		}
	}
#endif
}

void w_graphics_device_manager::on_window_resized(_In_ UINT pIndex)
{
	if (pIndex >= this->graphics_devices.size()) return;

	auto _gDevice = this->graphics_devices.at(pIndex);
	auto _size = _gDevice->output_presentation_windows.size();
	for (size_t i = 0; i < _size; ++i)
	{
		auto _window = _gDevice->output_presentation_windows.at(i);
		_wait_for_previous_frame(_gDevice, i);
		this->_pimp->create_or_resize_swap_chain(_gDevice, i);
	}
}

void w_graphics_device_manager::_wait_for_previous_frame(_In_ const std::shared_ptr<w_graphics_device>& pGDevice, _In_ size_t pOutputPresentationWindowIndex)
{
#ifdef __DX12__
    auto _output_presentation_window = &(pGDevice->output_presentation_windows.at(pOutputPresentationWindowIndex));

	const UINT64 _fence_value = _output_presentation_window->dx_fence_value;
	auto _hr = _output_presentation_window->dx_command_queue->Signal(_output_presentation_window->dx_fence.Get(), _fence_value);
	if (FAILED(_hr))
	{
		logger.error("error on signaling fence");
		return;
	}
	_output_presentation_window->dx_fence_value++;

	// Wait until the previous frame is finished.
	if (_output_presentation_window->dx_fence->GetCompletedValue() < _fence_value)
	{
		_hr = _output_presentation_window->dx_fence->SetEventOnCompletion(_fence_value, _output_presentation_window->dx_fence_event);
		if (FAILED(_hr))
		{
			logger.error("error on setting event on completion");
			return;
		}
		WaitForSingleObject(_output_presentation_window->dx_fence_event, INFINITE);
	}

	_output_presentation_window->dx_swap_chain_image_index = _output_presentation_window->dx_swap_chain->GetCurrentBackBufferIndex();

#endif
}

HRESULT w_graphics_device_manager::prepare()
{
    for (size_t i = 0; i < this->graphics_devices.size(); ++i)
    {
        auto _gDevice = this->graphics_devices[i];

        if (_gDevice->_is_released) continue;

#ifdef __DX12__
        if (_gDevice->dx_device_removed)
        {
            on_device_lost();
            continue;
        }
#endif

        for (size_t j = 0; j < _gDevice->output_presentation_windows.size(); ++j)
        {
            auto _output_window = &(_gDevice->output_presentation_windows[j]);           

#ifdef __DX12__

            _wait_for_previous_frame(_gDevice, j);

#elif defined(__VULKAN__)
            auto _semaphore = _output_window->vk_swap_chain_image_is_available_semaphore.get();
            auto _hr = vkAcquireNextImageKHR(_gDevice->vk_device,
                _output_window->vk_swap_chain,
                UINT64_MAX,
                *_semaphore,
                VK_NULL_HANDLE,
                &_output_window->vk_swap_chain_image_index);

            if (_hr != VK_SUCCESS && _hr != VK_SUBOPTIMAL_KHR)
            {
                logger.error("error acquiring image of graphics device's swap chain : " +
                    std::to_string(i) + " and presentation window: " + std::to_string(j));
                release();
                std::exit(EXIT_FAILURE);
            }
#endif
        }
    }

    return S_OK;
}

HRESULT w_graphics_device_manager::submit()
{
	for (size_t i = 0; i < this->graphics_devices.size(); ++i)
	{
		auto _gDevice = this->graphics_devices[i];

        if (_gDevice->_is_released) continue;
        
#ifdef __DX12__
		if (_gDevice->dx_device_removed)
		{
			on_device_lost();
			continue;
		}
#endif

		for (size_t j = 0; j < _gDevice->output_presentation_windows.size(); ++j)
		{
			auto _output_window = &(_gDevice->output_presentation_windows[j]);

#ifdef __DX12__

			_wait_for_previous_frame(_gDevice, j);

			/*
				Command list allocators can only be reset when the associated
				command lists have finished execution on the GPU; apps should use
				fences to determine GPU execution progress.
			*/
			auto _hr = _output_window->dx_command_allocator_pool->Reset();
			if (FAILED(_hr)) return S_FALSE;

			/*
				However, when ExecuteCommandList() is called on a particular command
				list, that command list can then be reset at any time and must be before
				re-recording.
			*/
			_hr = _output_window->dx_command_list->Reset(_output_window->dx_command_allocator_pool.Get(), _output_window->dx_pipeline_state.Get());
			if (FAILED(_hr)) return S_FALSE;

			D3D12_RESOURCE_BARRIER	_barrier = {};
			_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE;
			_barrier.Transition.pResource = _output_window->dx_swap_chain_image_views[_output_window->dx_swap_chain_image_index];
			_barrier.Transition.StateBefore = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT;
			_barrier.Transition.StateAfter = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
			_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

            _output_window->dx_command_list->ResourceBarrier(1, &_barrier);

			//get the swap chan target image view (render target view) for current frame buffer
			auto _render_target_view_handle = _output_window->dx_render_target_view_heap->GetCPUDescriptorHandleForHeapStart();
			auto _render_target_view_desc_size = _gDevice->dx_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			if (_output_window->dx_swap_chain_image_index == 1)
			{
				_render_target_view_handle.ptr += _render_target_view_desc_size;
			}

			//no need to clear render target views
			if (_output_window->force_to_clear_color_times > 0)
			{
				//clear the first swap chain image view
                _output_window->force_to_clear_color_times--;

				//set the back buffer render target
                _output_window->dx_command_list->OMSetRenderTargets(1, &_render_target_view_handle, FALSE, NULL);

				float _clear_color[4] =
				{
					static_cast<float>(_output_window->clear_color.r / 255.0f),
					static_cast<float>(_output_window->clear_color.g / 255.0f),
					static_cast<float>(_output_window->clear_color.b / 255.0f),
					static_cast<float>(_output_window->clear_color.a / 255.0f)
				};
                _output_window->dx_command_list->ClearRenderTargetView(_render_target_view_handle, _clear_color, 0, NULL);
			}

			//change the state of back buffer to transition into the presentation
			_barrier.Transition.StateBefore = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
			_barrier.Transition.StateAfter = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT;

			//store barrier to command list
            _output_window->dx_command_list->ResourceBarrier(1, &_barrier);

			//close command list
			_hr = _output_window->dx_command_list->Close();
			if (FAILED(_hr)) return S_FALSE;

#elif defined(__VULKAN__)

			//wait for image to be available and draw
			//VkSubmitInfo _submit_info = {};
			//_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			//_submit_info.waitSemaphoreCount = 1;
			//_submit_info.pWaitSemaphores = &_output_window->vk_image_is_available_semaphore;

			//_submit_info.signalSemaphoreCount = 1;
			//_submit_info.pSignalSemaphores = &_output_window->vk_rendering_done_semaphore;

			////this is the stage where the queue should wait on the semaphore (it doesn't have to wait with drawing)
			//VkPipelineStageFlags _wait_dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			//_submit_info.pWaitDstStageMask = &_wait_dst_stage_mask;
   //         
   //         //sort and get all avaiable commands buffers pointers
   //         std::vector<w_command_buffers*> _sorted_command_buffers;
   //         for (auto _iter =  _output_window->command_buffers.begin();
   //              _iter !=  _output_window->command_buffers.end(); ++_iter)
   //         {
   //             _sorted_command_buffers.push_back(_iter->second);
   //         }
   //         std::sort(_sorted_command_buffers.begin(), _sorted_command_buffers.end(),
   //                   [](_In_ w_command_buffers* a, _In_ w_command_buffers* b)
   //                   {
   //                       return a && b ? a->get_order() < b->get_order() : true;
   //                   });

   //         std::vector<VkCommandBuffer> _command_buffers;
   //         for (auto _iter : _sorted_command_buffers)
   //         {
   //             if(_iter && _iter->get_enable())
   //             {
   //                 auto _cmds = _iter->get_commands();
   //                 _command_buffers.push_back(_cmds[_output_window->vk_swap_chain_image_index]);
   //             }
   //         }
   //         _sorted_command_buffers.clear();
   //         
   //         auto _size = _command_buffers.size();
   //         if (_size)
   //         {
   //             //submit them
   //             _submit_info.commandBufferCount = static_cast<uint32_t>(_size);
   //             _submit_info.pCommandBuffers = _command_buffers.data();
   //         
   //             _command_buffers.clear();
   //         }
   //         
			////submit queue
			//auto _hr = vkQueueSubmit(
			//	_gDevice->vk_present_queue.queue,
			//	1,
			//	&_submit_info,
			//	VK_NULL_HANDLE);
			//if (_hr)
			//{
			//	logger.error("error on submitting queue of graphics device: " +
			//		_gDevice->device_name + " and presentation window: " + std::to_string(j));
			//	release();
			//	std::exit(EXIT_FAILURE);
			//}
#endif
		}
	}

	return S_OK;
}

HRESULT w_graphics_device_manager::present()
{
    for (size_t i = 0; i < this->graphics_devices.size(); ++i)
    {
        auto _gDevice = this->graphics_devices[i];
        
#ifdef __DX12__
        if (_gDevice->dx_device_removed)
        {
            on_device_lost();
            continue;
        }
#endif
        for (size_t j = 0; j < _gDevice->output_presentation_windows.size(); ++j)
        {
            auto _present_window = &(_gDevice->output_presentation_windows[j]);

#ifdef __DX12__
            
            //submit command list for executing
            ID3D12CommandList* _command_lists[1] = { _present_window->dx_command_list.Get() };
            _present_window->dx_command_queue->ExecuteCommandLists(1, _command_lists);
            
#ifdef __WIN32
            auto _hr = _present_window->dx_swap_chain->Present(_present_window->v_sync ? 1 : 0, 0);
#elif defined(__UWP)
            auto _hr = _present_window->dx_swap_chain->Present(1, 0);
#endif
            
            /*
             If the device was removed either by a disconnection or a driver upgrade, we
             must recreate all device resources.
             */
            if (_hr == DXGI_ERROR_DEVICE_REMOVED || _hr == DXGI_ERROR_DEVICE_RESET)
            {
                logger.error("Error on presenting swap chain, because of DXGI_ERROR_DEVICE_REMOVED or DXGI_ERROR_DEVICE_RESET for graphics device: "
                             + _gDevice->device_name);
                // If the device was removed for any reason, a new device and swap chain will need to be created.
                _gDevice->dx_device_removed = true;
                return S_FALSE;
            }
            
#elif defined(__VULKAN__)
            VkPresentInfoKHR _present_info = {};
            
            _present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            _present_info.waitSemaphoreCount = 1;
            _present_info.pWaitSemaphores = _present_window->vk_rendering_done_semaphore.get();
            _present_info.swapchainCount = 1;
            _present_info.pSwapchains = &_present_window->vk_swap_chain;
            _present_info.pImageIndices = &_present_window->vk_swap_chain_image_index;
            
            auto _hr = vkQueuePresentKHR(_gDevice->vk_present_queue.queue,
                                         &_present_info);
            if (_hr)
            {
                if (_hr == VK_ERROR_OUT_OF_DATE_KHR || _hr == VK_SUBOPTIMAL_KHR)
                {
                    on_window_resized(static_cast<UINT>(j));
                    return S_FALSE;
                }
                
                logger.error("error on presenting queue of graphics device: " +
                             _gDevice->device_name + " ID:" + std::to_string(_gDevice->device_id) +
                             " and presentation window: " + std::to_string(j));
                release();
                std::exit(EXIT_FAILURE);
            }

            _hr = vkQueueWaitIdle(_gDevice->vk_present_queue.queue);
            if (_hr)
            {
                logger.error("error on wait idle queue of graphics device: " +
                    _gDevice->device_name + " and presentation window: " + std::to_string(j));
                release();
                std::exit(EXIT_FAILURE);
            }
#endif
        }
    }
    
    return S_OK;
}

ULONG w_graphics_device_manager::release()
{
    //release all shared resources
    w_texture::release_shared_textures();
    w_shader::release_shared_shaders();

    //release the private implementation
    SAFE_RELEASE(this->_pimp);
    
    //release all graphics devices
    while (this->graphics_devices.size() > 0)
    {
        auto _gDevice = this->graphics_devices.front();
        
        //wait for all presentation windows to perform their tasks
        for (size_t j = 0; j < _gDevice->output_presentation_windows.size(); ++j)
        {
            _wait_for_previous_frame(_gDevice, j);
            //Present to avoid leak all references held by previous render
            present();
        }
        
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
    
#ifdef __ANDROID
    FreeVulkan();
#endif
    
    return _super::release();
}

#ifdef __VULKAN__
VkResult w_graphics_device_manager::memory_type_from_properties(
        VkPhysicalDeviceMemoryProperties pMemoryProperties,
        uint32_t pTypeBits,
        VkFlags pRequirementsFlags,
        uint32_t* pTypeIndex)
{
    // Search mem types to find first index with those properties
    for (size_t i = 0; i <  pMemoryProperties.memoryTypeCount; ++i) 
    {
        if ((pTypeBits & 1) == 1) 
        {
            // Type is available, does it match user properties?
            if ((pMemoryProperties.memoryTypes[i].propertyFlags & pRequirementsFlags) == pRequirementsFlags)
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

VkFormat w_graphics_device_manager::find_supported_format(
    _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ const std::vector<VkFormat>& pFormatCandidates,
    _In_ VkImageTiling pImageTiling,
    _In_ VkFormatFeatureFlags pFormatFeatureFlags)
{
    VkFormat _selected_format = VkFormat::VK_FORMAT_UNDEFINED;
    for (auto& _format : pFormatCandidates)
    {

        VkFormatProperties _properties;
        vkGetPhysicalDeviceFormatProperties(pGDevice->vk_physical_device, _format, &_properties);
        if (pImageTiling == VK_IMAGE_TILING_LINEAR &&
            (_properties.linearTilingFeatures & pFormatFeatureFlags) == pFormatFeatureFlags)
        {
            _selected_format = _format;
            break;
        }
        else if (pImageTiling == VK_IMAGE_TILING_OPTIMAL &&
            (_properties.optimalTilingFeatures & pFormatFeatureFlags) == pFormatFeatureFlags)
        {
            _selected_format = _format;
            break;
        }
    }
    return _selected_format;
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

w_color w_graphics_device_manager::get_output_window_clear_color(_In_ size_t pGraphicsDeviceIndex,
	_In_ size_t pOutputPresentationWindowIndex) const
{
	auto _output_presentation_window = &(this->graphics_devices[pGraphicsDeviceIndex]->output_presentation_windows[pOutputPresentationWindowIndex]);
	return _output_presentation_window->clear_color;
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

const float w_graphics_device_manager::convert_dips_to_pixels(_In_ float pDIPS, _In_ float pDPI)
{
	static const float dipsPerInch = 96.0f;
	return floorf(pDIPS * pDPI / dipsPerInch + 0.5f); // Round to nearest integer.
}

//const DirectX::XMFLOAT2 w_graphics_device_manager::get_dpi() const
//{
//	auto _gDevice = get_graphics_device();
//	DirectX::XMFLOAT2 dpi;
//	if (_gDevice && _gDevice->dx_dxgi_factory)
//	{
//		_gDevice->factory_2D->GetDesktopDpi(&dpi.x, &dpi.y);
//	}
//
//	return dpi;
//}
//
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

#pragma region Setters

void w_graphics_device_manager::set_graphics_device_manager_configs(_In_ const w_graphics_device_manager_configs& pConfig)
{
	if (this->_pimp)
	{
		this->_pimp->set_graphics_device_manager_configs(pConfig);
	}
}

void w_graphics_device_manager::set_output_window_clear_color(_In_ size_t pGraphicsDeviceIndex,
	_In_ size_t pOutputPresentationWindowIndex, _In_ w_color pClearColor)
{
	auto _output_presentation_window = &(this->graphics_devices[pGraphicsDeviceIndex]->output_presentation_windows[pOutputPresentationWindowIndex]);
	_output_presentation_window->clear_color = pClearColor;

	//set how many images need to be refresh
#ifdef __DX12__
	_output_presentation_window->force_to_clear_color_times = _output_presentation_window->dx_swap_chain_image_views.size();
#elif defined(__VULKAN__)
	_output_presentation_window->force_to_clear_color_times = static_cast<int>(_output_presentation_window->vk_swap_chain_image_views.size());
#endif
}

#pragma endregion

#pragma region w_graphics_device

#ifdef	__DX12__
//void w_graphics_device::create_blend_state(
//	BOOL pBlendEnable, 
//	D3D11_BLEND pSrcBlend, D3D11_BLEND pDestBlend, 
//	D3D11_BLEND pSrcBlendAlpha, D3D11_BLEND pDestBlendAlpha, 
//	D3D11_BLEND_OP pBlendOp, D3D11_BLEND_OP pBlendOpAlpha,
//	UINT8 pRenderTargetWriteMask,
//	_Out_ ID3D11BlendState1** pBlendstate)
//{
//	D3D11_BLEND_DESC1 _blend_desc;
//	std::memset(&_blend_desc, 0, sizeof(_blend_desc));
//
//	_blend_desc.RenderTarget[0].BlendEnable = pBlendEnable;
//	_blend_desc.RenderTarget[0].SrcBlend = pSrcBlend;
//	_blend_desc.RenderTarget[0].DestBlend = pDestBlend;
//	_blend_desc.RenderTarget[0].BlendOp = pBlendOp;
//	_blend_desc.RenderTarget[0].SrcBlendAlpha = pSrcBlendAlpha;
//	_blend_desc.RenderTarget[0].DestBlendAlpha = pDestBlendAlpha;
//	_blend_desc.RenderTarget[0].BlendOpAlpha = pBlendOpAlpha;
//	_blend_desc.RenderTarget[0].RenderTargetWriteMask = pRenderTargetWriteMask;
//
//	auto _hr = this->device->CreateBlendState1(&_blend_desc, pBlendstate);
//	V(_hr, L"creating blend state", "w_graphics_device", 3, true, true);
//}
//
//void w_graphics_device::create_depth_stencil_state(bool pEnable, bool pWriteEnable, _Out_ ID3D11DepthStencilState** pDepthStencilState)
//{
//	D3D11_DEPTH_STENCIL_DESC _depth_desc;
//	std::memset(&_depth_desc, 0, sizeof(_depth_desc));
//
//	_depth_desc.DepthEnable = pEnable;
//	_depth_desc.DepthWriteMask = pWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
//	_depth_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
//	_depth_desc.StencilEnable = false;
//	_depth_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
//	_depth_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
//	_depth_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//	_depth_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	_depth_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	_depth_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
//	_depth_desc.BackFace = _depth_desc.FrontFace;
//
//	auto _hr = device->CreateDepthStencilState(&_depth_desc, pDepthStencilState);
//	V(_hr, L"creating depth state", "w_graphics_device", 3, true, true);
//}
//
//void w_graphics_device::create_rasterizer_state(D3D11_CULL_MODE pCullMode, D3D11_FILL_MODE pFillMode, _Out_ ID3D11RasterizerState** pRasterizerState)
//{
//	D3D11_RASTERIZER_DESC _rasterizer_desc;
//	std::memset(&_rasterizer_desc, 0, sizeof(_rasterizer_desc));
//
//	_rasterizer_desc.CullMode = pCullMode;
//	_rasterizer_desc.FillMode = pFillMode;
//	_rasterizer_desc.DepthClipEnable = true;
//	_rasterizer_desc.MultisampleEnable = true;
//	auto _hr = this->device->CreateRasterizerState(&_rasterizer_desc, pRasterizerState);
//	V(_hr, L"creating rasterizer state", "w_graphics_device", 3, true, true);
//}
//
//void w_graphics_device::create_sampler_state(D3D11_FILTER pFilter, D3D11_TEXTURE_ADDRESS_MODE pAddressMode, _Out_ ID3D11SamplerState** pSamplerState)
//{
//	D3D11_SAMPLER_DESC _sampler_desc;
//	std::memset(&_sampler_desc, 0, sizeof(_sampler_desc));
//
//	_sampler_desc.Filter = pFilter;
//	_sampler_desc.AddressU = pAddressMode;
//	_sampler_desc.AddressV = pAddressMode;
//	_sampler_desc.AddressW = pAddressMode;
//	_sampler_desc.MaxAnisotropy = (this->device->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1) ? 16 : 2;
//	_sampler_desc.MaxLOD = FLT_MAX;
//	_sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//
//	auto _hr = this->device->CreateSamplerState(&_sampler_desc, pSamplerState);
//	V(_hr, L"creating sampler state", "w_graphics_device", 3, true, true);
//}
//
//void w_graphics_device::enable_alpha_blend(std::initializer_list<float> pBlendFactor, UINT pSampleMask)
//{
//	auto _size = pBlendFactor.size();
//	assert(_size != 0, "pBlendFactor must have 4 float number");
//	std::vector<float> _blend_factor(pBlendFactor.begin(), pBlendFactor.end());
//
//	this->context->OMSetBlendState(this->_alpha_blend.Get(), _blend_factor.data(), pSampleMask);
//}

#pragma endregion

#endif
