#include "w_render_pch.h"
#include "w_graphics_device_manager.h"
#include <w_logger.h>
#include <w_convert.h>
#include "vulkan/w_command_buffers.h"
#include "vulkan/w_texture.h"
#include "vulkan/w_shader.h"
#include <signal.h>

static std::once_flag _graphics_device_static_constructor;

#ifdef __DX12__
#include <w_directX_helper.h>
#elif defined(__APPLE__)
#include <math.h>
#endif

using namespace std;
using namespace wolf::system;
using namespace wolf::render::vulkan;


#define NUM_SAMPLES     VK_SAMPLE_COUNT_1_BIT

#ifdef __DX12__
ComPtr<IDXGIFactory4>	w_graphics_device::dx_dxgi_factory = nullptr;
#elif defined(__VULKAN__)
VkInstance w_graphics_device::vk_instance = NULL;

std::vector<VkSubpassDependency> w_graphics_device::defaults_states::vk_default_subpass_dependencies =
{
    {
        VK_SUBPASS_EXTERNAL,																// uint32_t                       srcSubpass
        0,																					// uint32_t                       dstSubpass
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,												// VkPipelineStageFlags           srcStageMask
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,										// VkPipelineStageFlags           dstStageMask
        VK_ACCESS_MEMORY_READ_BIT,															// VkAccessFlags                  srcAccessMask
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,        // VkAccessFlags                  dstAccessMask
        VK_DEPENDENCY_BY_REGION_BIT															// VkDependencyFlags              dependencyFlags
    },
    {
        0,																					// uint32_t                       srcSubpass
        VK_SUBPASS_EXTERNAL,																// uint32_t                       dstSubpass
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,										// VkPipelineStageFlags           srcStageMask
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,												// VkPipelineStageFlags           dstStageMask
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,			// VkAccessFlags                  srcAccessMask
        VK_ACCESS_MEMORY_READ_BIT,															// VkAccessFlags                  dstAccessMask
        VK_DEPENDENCY_BY_REGION_BIT															// VkDependencyFlags              dependencyFlags
    }
};

w_pipeline_layout_create_info w_graphics_device::defaults_states::pipelines::layout_create_info;
w_pipeline_vertex_input_state_create_info w_graphics_device::defaults_states::pipelines::vertex_input_create_info;
w_pipeline_input_assembly_state_create_info w_graphics_device::defaults_states::pipelines::input_assembly_create_info;
w_pipeline_rasterization_state_create_info w_graphics_device::defaults_states::pipelines::rasterization_create_info;
w_pipeline_multisample_state_create_info w_graphics_device::defaults_states::pipelines::multisample_create_info;

w_pipeline_color_blend_attachment_state w_graphics_device::defaults_states::blend_states::none;
w_pipeline_color_blend_attachment_state w_graphics_device::defaults_states::blend_states::premulitplied_alpha;

#endif

w_graphics_device::w_graphics_device() :
	_is_released(false), _name("w_graphics_device")

#ifdef __VULKAN__
	,
	vk_device(0),
	vk_command_allocator_pool(0)
#endif
{
	std::call_once(_graphics_device_static_constructor, []()
	{
		//static constructor blend states
		defaults_states::blend_states::none.blendEnable = VK_FALSE;
		defaults_states::blend_states::none.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		defaults_states::blend_states::none.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		defaults_states::blend_states::none.colorBlendOp = VK_BLEND_OP_ADD;
		defaults_states::blend_states::none.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		defaults_states::blend_states::none.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		defaults_states::blend_states::none.alphaBlendOp = VK_BLEND_OP_ADD;
		defaults_states::blend_states::none.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		defaults_states::blend_states::premulitplied_alpha.blendEnable = VK_TRUE;
		defaults_states::blend_states::premulitplied_alpha.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		defaults_states::blend_states::premulitplied_alpha.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		defaults_states::blend_states::premulitplied_alpha.colorBlendOp = VK_BLEND_OP_ADD;
		defaults_states::blend_states::premulitplied_alpha.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		defaults_states::blend_states::premulitplied_alpha.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		defaults_states::blend_states::premulitplied_alpha.alphaBlendOp = VK_BLEND_OP_ADD;
		defaults_states::blend_states::premulitplied_alpha.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		defaults_states::pipelines::layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		defaults_states::pipelines::layout_create_info.pNext = nullptr;
		defaults_states::pipelines::layout_create_info.flags = 0;
		defaults_states::pipelines::layout_create_info.setLayoutCount = 0;
		defaults_states::pipelines::layout_create_info.pSetLayouts = nullptr;
		defaults_states::pipelines::layout_create_info.pushConstantRangeCount  = 0;
		defaults_states::pipelines::layout_create_info.pPushConstantRanges = nullptr;

		defaults_states::pipelines::vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		defaults_states::pipelines::vertex_input_create_info.pNext = nullptr;
		defaults_states::pipelines::vertex_input_create_info.flags = 0;
		defaults_states::pipelines::vertex_input_create_info.vertexBindingDescriptionCount = 0;
		defaults_states::pipelines::vertex_input_create_info.pVertexBindingDescriptions = nullptr;
		defaults_states::pipelines::vertex_input_create_info.vertexAttributeDescriptionCount = 0;
		defaults_states::pipelines::vertex_input_create_info.pVertexAttributeDescriptions = nullptr;
		
		defaults_states::pipelines::input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		defaults_states::pipelines::input_assembly_create_info.pNext = nullptr;
		defaults_states::pipelines::input_assembly_create_info.flags = 0;
		defaults_states::pipelines::input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		defaults_states::pipelines::input_assembly_create_info.primitiveRestartEnable = VK_FALSE;
		
		defaults_states::pipelines::rasterization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		defaults_states::pipelines::rasterization_create_info.pNext = nullptr;
		defaults_states::pipelines::rasterization_create_info.flags = 0;
		defaults_states::pipelines::rasterization_create_info.depthClampEnable = VK_FALSE;
		defaults_states::pipelines::rasterization_create_info.rasterizerDiscardEnable = VK_FALSE;
		defaults_states::pipelines::rasterization_create_info.polygonMode = VK_POLYGON_MODE_FILL;
		defaults_states::pipelines::rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
		defaults_states::pipelines::rasterization_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		defaults_states::pipelines::rasterization_create_info.depthBiasEnable = VK_FALSE;
		defaults_states::pipelines::rasterization_create_info.depthBiasConstantFactor = 0.0f;
		defaults_states::pipelines::rasterization_create_info.depthBiasClamp = 0.0f;
		defaults_states::pipelines::rasterization_create_info.depthBiasSlopeFactor = 0.0f;
		defaults_states::pipelines::rasterization_create_info.lineWidth = 1.0f;

		defaults_states::pipelines::multisample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		defaults_states::pipelines::multisample_create_info.pNext = nullptr;
		defaults_states::pipelines::multisample_create_info.flags = 0;
		defaults_states::pipelines::multisample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		defaults_states::pipelines::multisample_create_info.sampleShadingEnable = VK_FALSE;
		defaults_states::pipelines::multisample_create_info.minSampleShading = 1.0f;
		defaults_states::pipelines::multisample_create_info.pSampleMask = nullptr;
		defaults_states::pipelines::multisample_create_info.alphaToCoverageEnable = VK_FALSE;
		defaults_states::pipelines::multisample_create_info.alphaToOneEnable = VK_FALSE;
	});
}

const std::string w_graphics_device::get_info()
{
	std::string _device_name = "";
	uint32_t _device_id = 0, _device_vendor_id = 0;

	if (this->device_info)
	{
		_device_name = this->device_info->get_device_name();
		_device_id = this->device_info->get_device_id();
		_device_vendor_id = this->device_info->get_device_vendor_id();
	}
	return std::string(
		"graphics device: " + _device_name +
		" ID:" + std::to_string(_device_id) +
		" VendorID:" + std::to_string(_device_vendor_id));
}

const size_t w_graphics_device::get_number_of_swap_chains()
{
	return this->output_presentation_window.swap_chain_image_views.size();
}

W_RESULT w_graphics_device::draw(
	_In_ const w_command_buffer&	pCommandBuffer,
	_In_ const uint32_t&			pVertexCount,
	_In_ const uint32_t&			pInstanceCount,
	_In_ const uint32_t&			pFirstVertex,
	_In_ const uint32_t&			pFirstInstance)
{
	if (!pCommandBuffer.handle) return W_FAILED;
#ifdef __VULKAN__
	vkCmdDraw(pCommandBuffer.handle, pVertexCount, pInstanceCount, pFirstVertex, pFirstInstance);
#elif defined(__DX12__)

#endif
	return W_PASSED;
}

W_RESULT w_graphics_device::submit(
	_In_ const std::vector<const w_command_buffer*>&	pCommandBuffers,
	_In_ const w_queue&									pQueue,
	_In_ const std::vector<w_pipeline_stage_flag_bits>&	pWaitDstStageMask,
	_In_ std::vector<w_semaphore>						pWaitForSemaphores,
	_In_ std::vector<w_semaphore>						pSignalForSemaphores,
	_In_ w_fences*										pFence,
	_In_ const bool&									pWaitIdleForDone)
{
	const std::string _trace_info = "w_graphics_device::submit";

	auto _size = static_cast<uint32_t>(pCommandBuffers.size());
	if (!_size) return W_FAILED;
	
	W_RESULT _hr = W_PASSED;
	std::vector<VkCommandBuffer> _cmds(_size);
	for (size_t i = 0; i < _size; ++i)
	{
		if (pCommandBuffers[i])
		{
			_cmds[i] = pCommandBuffers[i]->handle;
		}
	}

	VkSubmitInfo _submit_info = {};
	_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	_submit_info.commandBufferCount = _size;
	_submit_info.pCommandBuffers = _cmds.data();

	_cmds.clear();
	
	_size = static_cast<uint32_t>(pWaitDstStageMask.size());
	_submit_info.pWaitDstStageMask = _size ? (VkPipelineStageFlags*)(&pWaitDstStageMask[0]) : nullptr;
	
	_size = static_cast<uint32_t>(pWaitForSemaphores.size());
	std::vector<VkSemaphore> _wait_semaphors;
	if (_size)
	{
		_wait_semaphors.resize(_size);
		for (size_t i = 0; i < _size; ++i)
		{
			_wait_semaphors[i] = *pWaitForSemaphores[i].get();
		}
		_submit_info.waitSemaphoreCount = _size;
		_submit_info.pWaitSemaphores = _wait_semaphors.data();
	}

	_size = static_cast<uint32_t>(pSignalForSemaphores.size());
	std::vector<VkSemaphore> _signal_semaphors;
	if (_size)
	{
		_signal_semaphors.resize(_size);
		for (size_t i = 0; i < _size; ++i)
		{
			_signal_semaphors[i] = *pSignalForSemaphores[i].get();
		}
		_submit_info.signalSemaphoreCount = _size;
		_submit_info.pSignalSemaphores = _signal_semaphors.data();
	}

	VkFence _fence = pFence ? *(pFence->get()) : 0;

	// Submit to queue
	if (vkQueueSubmit(pQueue.queue, 1, &_submit_info, _fence))
	{
		_hr = W_FAILED;
		V(_hr,
			w_log_type::W_ERROR,
			"submiting queue for graphics device: {}. trace info: {}", 
			this->get_info(),
			_trace_info);
		goto submit_clear;
	}

	if (pWaitIdleForDone)
	{
		if (vkQueueWaitIdle(pQueue.queue))
		{
			_hr = W_FAILED;
			V(_hr,
				w_log_type::W_ERROR,
				"waiting idle queue for graphics device: {} . trace info: {}",
				this->get_info(),
				_trace_info);
				goto submit_clear;
		}
	}

submit_clear:
	_cmds.clear();
	_wait_semaphors.clear();
	_signal_semaphors.clear();

	return _hr;
}

W_RESULT w_graphics_device::capture(
	_In_ VkImage pSourceImage, 
	_In_ VkFormat pSourceImageFormat, 
	_In_ VkImageLayout pSourceImageLayout,
	_In_ const uint32_t& pWidth, 
	_In_ const uint32_t& pHeight, 
	_In_ wolf::system::w_signal<void(const w_point_t, uint8_t*)>& pOnPixelsDataCaptured)
{
    W_RESULT _return_result = W_PASSED;

    const std::string _trace_info = "w_graphics_device::capture";
    
    VkImage _dst_image = 0;
    VkDeviceMemory _dst_image_memory = 0;
    VkCommandBuffer _copy_cmd = 0;
    VkFence _copy_fence = 0;
    bool _command_buffer_began = false;
	bool _bliting_supported = true;

	//check whether blitting is supported or not
	VkFormatProperties _format_properties;
	// Check for whether blitting is supported from optimal image
	vkGetPhysicalDeviceFormatProperties(
		this->vk_physical_device,
		pSourceImageFormat,
		&_format_properties);
	if (!(_format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
	{
		logger.warning("Blitting feature not supported from optimal tiled image for graphics device: {} and following format: {}",
			get_info(), pSourceImageFormat);
		_bliting_supported = false;
	}
	if (_bliting_supported)
	{
		// Check for whether blitting is supported for linear image
		if (!(_format_properties.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT))
		{
			logger.warning("Blitting feature not supported from linear tiled image for graphics device: {} and following format: {}",
				get_info(), pSourceImageFormat);
			_bliting_supported = false;
		}
	}

    //create destination image in order to get data of swap chain's image and later allow access from cpu
    VkImageCreateInfo _image_create_info = {};
    _image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    _image_create_info.imageType = VK_IMAGE_TYPE_2D;
    _image_create_info.format = pSourceImageFormat;
    _image_create_info.extent.width = pWidth;
    _image_create_info.extent.height = pHeight;
    _image_create_info.extent.depth = 1;
    _image_create_info.arrayLayers = 1;
    _image_create_info.mipLevels = 1;
    _image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    _image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    _image_create_info.tiling = VK_IMAGE_TILING_LINEAR;
    _image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    //create the destination image
    auto _hr = vkCreateImage(
        this->vk_device,
        &_image_create_info,
        nullptr,
        &_dst_image);
	if (_hr)
	{
		_return_result = W_FAILED;
		V(_return_result,
			w_log_type::W_ERROR,
			"creating destination image for graphics device: {}. trace info: {}",
			this->get_info(),
			_trace_info);
		goto clean_up;
	}


    {
        //get memory requirements
        VkMemoryRequirements _mem_requirements;
        vkGetImageMemoryRequirements(this->vk_device, _dst_image, &_mem_requirements);

        uint32_t _mem_type_index;
        w_graphics_device_manager::memory_type_from_properties(
            this->vk_physical_device_memory_properties,
            _mem_requirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &_mem_type_index);

        //allocate memory info
        VkMemoryAllocateInfo _mem_alloc_info = {};
        _mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        _mem_alloc_info.allocationSize = _mem_requirements.size;
        _mem_alloc_info.memoryTypeIndex = _mem_type_index;

        //allocate memory in host
        _hr = vkAllocateMemory(this->vk_device, &_mem_alloc_info, nullptr, &_dst_image_memory);
        if (_hr)
        {
            _return_result = W_FAILED;
            V(_return_result,
				w_log_type::W_ERROR,
                "creating destination image for graphics device: {}. trace info: {}", 
				this->get_info(),
                _trace_info);
            goto clean_up;
        }
    }

	//TODO: this must do once over instnace
    //bind to image memory
    _hr = vkBindImageMemory(this->vk_device, _dst_image, _dst_image_memory, 0);
    if (_hr)
    {
        _return_result = W_FAILED;
        V(_return_result,
			w_log_type::W_ERROR,
            "binding to destination image for graphics graphics device: {}. trace info: {}",
			this->get_info(),
            _trace_info);
        goto clean_up;
    }

    {
        const auto _sub_res_range = VkImageSubresourceRange
        {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            1,
            0,
            1
        };
        
		// create command buffer on primary
		VkCommandBufferAllocateInfo _copy_cmd_buf_allocate_info = {};
		_copy_cmd_buf_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		_copy_cmd_buf_allocate_info.commandPool = this->vk_command_allocator_pool;
		_copy_cmd_buf_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		_copy_cmd_buf_allocate_info.commandBufferCount = 1;
		_hr = vkAllocateCommandBuffers(this->vk_device, &_copy_cmd_buf_allocate_info, &_copy_cmd);
		if (_hr)
		{
			_return_result = W_FAILED;
			V(_return_result,
				w_log_type::W_ERROR,
				"allocating buffer for copy command buffer for graphics device: {}. trace info: {}",
				this->get_info(),
				_trace_info);
			goto clean_up;
		}

		//begin command buffer
		VkCommandBufferBeginInfo _copy_cmd_buff_info = {};
		_copy_cmd_buff_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		_hr = vkBeginCommandBuffer(_copy_cmd, &_copy_cmd_buff_info);
		_command_buffer_began = true;
		if (_hr)
		{
			_return_result = W_FAILED;
			V(_return_result,
				w_log_type::W_ERROR,
				"beginning copy command buffer for graphics device: {} . trace info: {}",
				this->get_info(),
				_trace_info);
			goto clean_up;
		}

        //transition from memory to transfer write for destination image
        VkImageMemoryBarrier _imb_undefined_to_dst_transfer= {};
        _imb_undefined_to_dst_transfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        _imb_undefined_to_dst_transfer.srcAccessMask = 0;
        _imb_undefined_to_dst_transfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        _imb_undefined_to_dst_transfer.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        _imb_undefined_to_dst_transfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        _imb_undefined_to_dst_transfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        _imb_undefined_to_dst_transfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        _imb_undefined_to_dst_transfer.image = _dst_image;
        _imb_undefined_to_dst_transfer.subresourceRange = _sub_res_range;

        vkCmdPipelineBarrier(
            _copy_cmd,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &_imb_undefined_to_dst_transfer);

        //transition from memory read to transfer read for source image
        VkImageMemoryBarrier _imb_memory_to_transfer_read = {};
        _imb_memory_to_transfer_read.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        _imb_memory_to_transfer_read.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        _imb_memory_to_transfer_read.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        _imb_memory_to_transfer_read.oldLayout = pSourceImageLayout;
        _imb_memory_to_transfer_read.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        _imb_memory_to_transfer_read.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        _imb_memory_to_transfer_read.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        _imb_memory_to_transfer_read.image = pSourceImage;
        _imb_memory_to_transfer_read.subresourceRange = _sub_res_range;
        
        vkCmdPipelineBarrier(
            _copy_cmd,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &_imb_memory_to_transfer_read);

    }

    if (_bliting_supported)
    {
        // apply bliting
        VkOffset3D _src_offset =
        {
            (int32_t)pWidth,	//x
            (int32_t)pHeight,	//y
            1					//z
        };

        VkImageBlit _image_blit_region = {};
        _image_blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        _image_blit_region.srcSubresource.layerCount = 1;
        _image_blit_region.srcOffsets[1] = _src_offset;
        _image_blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        _image_blit_region.dstSubresource.layerCount = 1;
        _image_blit_region.dstOffsets[1] = _src_offset;

        // apply blit command
        vkCmdBlitImage(
            _copy_cmd,
			pSourceImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            _dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &_image_blit_region,
            VK_FILTER_NEAREST);
    }
    else
    {
        // apply image copy
        VkImageCopy _image_copy_region = {};
        _image_copy_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        _image_copy_region.srcSubresource.layerCount = 1;
        _image_copy_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        _image_copy_region.dstSubresource.layerCount = 1;
        _image_copy_region.extent.width = pWidth;
        _image_copy_region.extent.height = pHeight;
        _image_copy_region.extent.depth = 1;

        //Apply copy region command
        vkCmdCopyImage(
            _copy_cmd,
			pSourceImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            _dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &_image_copy_region);
    }


	{
        const auto _sub_res_range = VkImageSubresourceRange
        {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            1,
            0,
            1
        };
        
		//transition from transfer write to memory read for destination image
		VkImageMemoryBarrier _imb_transfer_write_to_memory_read = {};
		_imb_transfer_write_to_memory_read.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		_imb_transfer_write_to_memory_read.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		_imb_transfer_write_to_memory_read.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		_imb_transfer_write_to_memory_read.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		_imb_transfer_write_to_memory_read.newLayout = VK_IMAGE_LAYOUT_GENERAL;
		_imb_transfer_write_to_memory_read.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		_imb_transfer_write_to_memory_read.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		_imb_transfer_write_to_memory_read.image = _dst_image;
		_imb_transfer_write_to_memory_read.subresourceRange = _sub_res_range;

		vkCmdPipelineBarrier(
			_copy_cmd,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&_imb_transfer_write_to_memory_read);

		//transition from trasfer read to memory read for source image
		VkImageMemoryBarrier _imb_transfer_read_to_memory_read = {};
		_imb_transfer_read_to_memory_read.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		_imb_transfer_read_to_memory_read.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		_imb_transfer_read_to_memory_read.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		_imb_transfer_read_to_memory_read.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		_imb_transfer_read_to_memory_read.newLayout = pSourceImageLayout;
		_imb_transfer_read_to_memory_read.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		_imb_transfer_read_to_memory_read.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		_imb_transfer_read_to_memory_read.image = pSourceImage;
		_imb_transfer_read_to_memory_read.subresourceRange = _sub_res_range;

		vkCmdPipelineBarrier(
			_copy_cmd,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&_imb_transfer_read_to_memory_read);

		_hr = vkEndCommandBuffer(_copy_cmd);
		if (_hr)
		{
			_return_result = W_FAILED;
			V(_return_result,
				w_log_type::W_ERROR,
				"ending copy command buffer for graphics device: {}. trace info: {}",
				this->get_info(),
				_trace_info);
			goto clean_up;
		}
		_command_buffer_began = false;

		// Create fence to ensure that the command buffer has finished executing
		VkFenceCreateInfo _fence_info = {};
		_fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		_fence_info.flags = 0;

		_hr = vkCreateFence(this->vk_device, &_fence_info, nullptr, &_copy_fence);
		if (_hr)
		{
			_return_result = W_FAILED;
			V(_return_result,
				w_log_type::W_ERROR,
				"creating fence of copy command buffer for graphics device: {} . trace info: {}",
				this->get_info(),
				_trace_info);
			goto clean_up;
		}

		//execute command buffer
		VkSubmitInfo _submit_info = {};
		_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		_submit_info.pCommandBuffers = &_copy_cmd;
		_submit_info.commandBufferCount = 1;
		
		//Submit to the queue
		_hr = vkQueueSubmit(this->vk_graphics_queue.queue, 1, &_submit_info, _copy_fence);
		if (_hr)
		{
			_return_result = W_FAILED;
			V(_return_result,
				"submiting copy command buffer to queue for graphics device: {}. trace info: {}",
				this->get_info(),
				_trace_info);
			goto clean_up;
		}
		// Wait for the fence to signal that command buffer has finished executing
		_hr = vkWaitForFences(this->vk_device, 1, &_copy_fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);
		if (_hr)
		{
			_return_result = W_FAILED;
			V(_return_result,
				"submiting copy command buffer to queue for graphics device: {}. trace info: {}",
				this->get_info(),
				_trace_info);
			goto clean_up;
		}

		VkImageSubresource _sub_resource = {};
		_sub_resource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		VkSubresourceLayout _sub_resource_layout;
		vkGetImageSubresourceLayout(this->vk_device, _dst_image, &_sub_resource, &_sub_resource_layout);

		//Map the memory
		uint8_t* _data;
		w_point_t _size;
		_size.x = pWidth;
		_size.y = pHeight;

		vkMapMemory(this->vk_device, _dst_image_memory, 0, VK_WHOLE_SIZE, 0, (void**)&_data);
		_data += _sub_resource_layout.offset;
		pOnPixelsDataCaptured.emit(_size, _data);
		vkUnmapMemory(this->vk_device, _dst_image_memory);
		
		_data = nullptr;
	}

clean_up:
    if (_dst_image_memory)
    {
        vkFreeMemory(this->vk_device, _dst_image_memory, nullptr);
    }
    if (_dst_image)
    {
        vkDestroyImage(
            this->vk_device,
            _dst_image,
            nullptr);
    }
    if (_copy_cmd)
    {
        if (_command_buffer_began)
        {
            _hr = vkEndCommandBuffer(_copy_cmd);
			if (_hr)
			{
				V(W_FAILED,
					"ending copy command buffer for graphics device: {}. trace info: {}",
					this->get_info(),
					_trace_info);
			}
        }
        vkFreeCommandBuffers(
            this->vk_device,
            this->vk_command_allocator_pool,
            1,
            &_copy_cmd
        );
    }
    //release fence
    if (_copy_fence)
    {
        vkDestroyFence(this->vk_device, _copy_fence, nullptr);
    }


    return _return_result;
}

W_RESULT w_graphics_device::capture_presented_swap_chain_buffer(
	_In_ wolf::system::w_signal<void(const w_point_t, uint8_t*)>& pOnPixelsDataCaptured)
{
	W_RESULT _return_result = W_PASSED;
	const std::string _trace_info = "w_graphics_device::capture_presented_swap_chain_buffer";

	auto _objs_ptr = this->output_presentation_window.objs_between_cpu_gpu;
	if (!_objs_ptr) return W_FAILED;

	//source image is swap chain last presented buffer
	auto _src_image = this->output_presentation_window.swap_chain_image_views[this->output_presentation_window.swap_chain_image_index].image;

	const auto _sub_res_range = VkImageSubresourceRange
	{
		VK_IMAGE_ASPECT_COLOR_BIT,
		0,
		1,
		0,
		1
	};

	//begin command buffer
	VkCommandBufferBeginInfo _copy_cmd_buff_info = {};
	_copy_cmd_buff_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	auto _hr = vkBeginCommandBuffer(_objs_ptr->copy_command_buffer, &_copy_cmd_buff_info);
	_objs_ptr->command_buffer_began = true;

	if (_hr)
	{
		V(_return_result,
			w_log_type::W_ERROR,
			"beginning copy command buffer for graphics device {}. trace info: {}",
			this->get_info(),
			_trace_info);
		goto clean_up;
	}

	{
		//transition from memory to transfer write for destination image
		VkImageMemoryBarrier _imb_undefined_to_dst_transfer = {};
		_imb_undefined_to_dst_transfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		_imb_undefined_to_dst_transfer.srcAccessMask = 0;
		_imb_undefined_to_dst_transfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		_imb_undefined_to_dst_transfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		_imb_undefined_to_dst_transfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		_imb_undefined_to_dst_transfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		_imb_undefined_to_dst_transfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		_imb_undefined_to_dst_transfer.image = _objs_ptr->destination_image;
		_imb_undefined_to_dst_transfer.subresourceRange = _sub_res_range;

		vkCmdPipelineBarrier(
			_objs_ptr->copy_command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&_imb_undefined_to_dst_transfer);

		//transition from memory read to transfer read for source image
		VkImageMemoryBarrier _imb_memory_to_transfer_read = {};
		_imb_memory_to_transfer_read.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		_imb_memory_to_transfer_read.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		_imb_memory_to_transfer_read.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		_imb_memory_to_transfer_read.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		_imb_memory_to_transfer_read.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		_imb_memory_to_transfer_read.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		_imb_memory_to_transfer_read.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		_imb_memory_to_transfer_read.image = _src_image;
		_imb_memory_to_transfer_read.subresourceRange = _sub_res_range;

		vkCmdPipelineBarrier(
			_objs_ptr->copy_command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&_imb_memory_to_transfer_read);

		if (this->output_presentation_window.bliting_supported_by_swap_chain)
		{
			// apply bliting
			VkOffset3D _src_offset =
			{
				(int32_t)this->output_presentation_window.width, //x
				(int32_t)this->output_presentation_window.height,//y
				1												 //z
			};

			VkImageBlit _image_blit_region = {};
			_image_blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			_image_blit_region.srcSubresource.layerCount = 1;
			_image_blit_region.srcOffsets[1] = _src_offset;
			_image_blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			_image_blit_region.dstSubresource.layerCount = 1;
			_image_blit_region.dstOffsets[1] = _src_offset;

			// apply blit command
			vkCmdBlitImage(
				_objs_ptr->copy_command_buffer,
				_src_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				_objs_ptr->destination_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&_image_blit_region,
				VK_FILTER_NEAREST);
		}
		else
		{
			// apply image copy
			VkImageCopy _image_copy_region = {};
			_image_copy_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			_image_copy_region.srcSubresource.layerCount = 1;
			_image_copy_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			_image_copy_region.dstSubresource.layerCount = 1;
			_image_copy_region.extent.width = this->output_presentation_window.width;
			_image_copy_region.extent.height = this->output_presentation_window.height;
			_image_copy_region.extent.depth = 1;

			//Apply copy region command
			vkCmdCopyImage(
				_objs_ptr->copy_command_buffer,
				_src_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				_objs_ptr->destination_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&_image_copy_region);
		}

		{
			//transition from transfer write to memory read for destination image
			VkImageMemoryBarrier _imb_transfer_write_to_memory_read = {};
			_imb_transfer_write_to_memory_read.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			_imb_transfer_write_to_memory_read.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			_imb_transfer_write_to_memory_read.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			_imb_transfer_write_to_memory_read.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			_imb_transfer_write_to_memory_read.newLayout = VK_IMAGE_LAYOUT_GENERAL;
			_imb_transfer_write_to_memory_read.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			_imb_transfer_write_to_memory_read.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			_imb_transfer_write_to_memory_read.image = _objs_ptr->destination_image;
			_imb_transfer_write_to_memory_read.subresourceRange = _sub_res_range;

			vkCmdPipelineBarrier(
				_objs_ptr->copy_command_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&_imb_transfer_write_to_memory_read);

			//transition from trasfer read to memory read for source image
			VkImageMemoryBarrier _imb_transfer_read_to_memory_read = {};
			_imb_transfer_read_to_memory_read.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			_imb_transfer_read_to_memory_read.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			_imb_transfer_read_to_memory_read.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			_imb_transfer_read_to_memory_read.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			_imb_transfer_read_to_memory_read.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			_imb_transfer_read_to_memory_read.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			_imb_transfer_read_to_memory_read.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			_imb_transfer_read_to_memory_read.image = _src_image;
			_imb_transfer_read_to_memory_read.subresourceRange = _sub_res_range;

			vkCmdPipelineBarrier(
				_objs_ptr->copy_command_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&_imb_transfer_read_to_memory_read);

			_hr = vkEndCommandBuffer(_objs_ptr->copy_command_buffer);
			if (_hr)
			{
				_return_result = W_FAILED;
				V(_return_result,
					"ending copy command buffer for graphics device: {}. trace info: {}",
					this->get_info(),
					_trace_info);
				goto clean_up;
			}
			_objs_ptr->command_buffer_began = false;

			// Create fence to ensure that the command buffer has finished executing
			VkFenceCreateInfo _fence_info = {};
			_fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			_fence_info.flags = 0;

			_hr = vkCreateFence(this->vk_device, &_fence_info, nullptr, &_objs_ptr->copy_fence);
			if (_hr)
			{
				_return_result = W_FAILED;
				V(_return_result,
					"creating fence of copy command buffer for graphics device: {}. trace info: {}",
					this->get_info(),
					_trace_info);
				goto clean_up;
			}

			//execute command buffer
			VkSubmitInfo _submit_info = {};
			_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			_submit_info.pCommandBuffers = &_objs_ptr->copy_command_buffer;
			_submit_info.commandBufferCount = 1;

			//Submit to the queue
			_hr = vkQueueSubmit(this->vk_graphics_queue.queue, 1, &_submit_info, _objs_ptr->copy_fence);
			if (_hr)
			{
				_return_result = W_FAILED;
				V(_return_result,
					"submiting copy command buffer to queue for graphics device: {}. trace info: {}",
					this->get_info(),
					_trace_info);
				goto clean_up;
			}
			// Wait for the fence to signal that command buffer has finished executing
			_hr = vkWaitForFences(this->vk_device, 1, &_objs_ptr->copy_fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);
			if (_hr)
			{
				_return_result = W_FAILED;
				V(_return_result,
					"submiting copy command buffer to queue for graphics device: {}. trace info: {}",
					this->get_info(),
					_trace_info);
				goto clean_up;
			}

			VkImageSubresource _sub_resource = {};
			_sub_resource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			VkSubresourceLayout _sub_resource_layout;
			vkGetImageSubresourceLayout(this->vk_device, _objs_ptr->destination_image, &_sub_resource, &_sub_resource_layout);

			//Map the memory
			uint8_t* _data;
			w_point_t _size;
			_size.x = this->output_presentation_window.width;
			_size.y = this->output_presentation_window.height;

			vkMapMemory(this->vk_device, _objs_ptr->destination_image_memory, 0, VK_WHOLE_SIZE, 0, (void**)&_data);
			_data += _sub_resource_layout.offset;
			pOnPixelsDataCaptured.emit(_size, _data);
			vkUnmapMemory(this->vk_device, _objs_ptr->destination_image_memory);

			_data = nullptr;
		}
	}

clean_up:
	if (_objs_ptr->command_buffer_began)
	{
		_hr = vkEndCommandBuffer(_objs_ptr->copy_command_buffer);
		if (_hr)
		{
			_return_result = W_FAILED;
			V(_return_result,
				"ending copy command buffer for graphics device: {}. trace info: {}",
				this->get_info(),
				_trace_info);
		}
		_objs_ptr->command_buffer_began = false;
	}
	
	return _return_result;
}

void w_graphics_device::_clean_swap_chain()
{
	//release shared objects between cpu and gpu
#pragma region Release Shared Objs CPU-GPU Mapping
	if (this->output_presentation_window.objs_between_cpu_gpu)
	{
		auto _shared_obj = this->output_presentation_window.objs_between_cpu_gpu;
		//release objects destination image memory
		if (_shared_obj->destination_image_memory)
		{
			vkFreeMemory(this->vk_device, _shared_obj->destination_image_memory, nullptr);
		}
		//release destination image
		if (_shared_obj->destination_image)
		{
			vkDestroyImage(
				this->vk_device,
				_shared_obj->destination_image,
				nullptr);
		}
		//release command buffer
		if (_shared_obj->copy_command_buffer)
		{
			if (_shared_obj->command_buffer_began)
			{
				auto _hr = vkEndCommandBuffer(_shared_obj->copy_command_buffer);
				if (_hr)
				{
					V(W_FAILED,
						"ending remained copy command buffer between CPU-GPU for graphics device: {}. trace info: {}",
						this->get_info(),
						"w_graphics_device::release",
						3,
						false);
				}
				_shared_obj->command_buffer_began = false;
			}
			vkFreeCommandBuffers(
				this->vk_device,
				this->vk_command_allocator_pool,
				1,
				&_shared_obj->copy_command_buffer
			);
		}
		//release fence
		if (_shared_obj->copy_fence)
		{
			vkDestroyFence(this->vk_device, _shared_obj->copy_fence, nullptr);
		}
	}
#pragma endregion

	//release all image view of swap chains
	for (size_t i = 0; i < this->output_presentation_window.swap_chain_image_views.size(); ++i)
	{
		//release both color image and view,
		vkDestroyImageView(
			this->vk_device,
			this->output_presentation_window.swap_chain_image_views[i].view,
			nullptr);
		this->output_presentation_window.swap_chain_image_views[i].view = 0;
		this->output_presentation_window.swap_chain_image_views[i].image = 0;
	}
	this->output_presentation_window.swap_chain_image_views.clear();

	//release depth image and view,
	vkDestroyImageView(
		this->vk_device,
		this->output_presentation_window.depth_buffer_image_view.view,
		nullptr);
	this->output_presentation_window.depth_buffer_image_view.view = 0;
	this->output_presentation_window.depth_buffer_image_view.image = 0;

	//release swap chain
	vkDestroySwapchainKHR(this->vk_device,
		this->output_presentation_window.vk_swap_chain,
		nullptr);
	this->output_presentation_window.vk_swap_chain = 0;
}

ULONG w_graphics_device::release()
{
    //release all resources
    if (this->_is_released) return 1;

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

	//release memory
	this->memory_allocator.release();

	_clean_swap_chain();

	this->output_presentation_window.release();

	//release the surface
	vkDestroySurfaceKHR(w_graphics_device::vk_instance,
		this->output_presentation_window.vk_presentation_surface,
		nullptr);
	this->output_presentation_window.vk_presentation_surface = 0;

    this->vk_queue_family_properties.clear();
    this->vk_queue_family_supports_present.clear();

    this->vk_graphics_queue.release();
    this->vk_present_queue.release();
    this->vk_compute_queue.release();
    this->vk_transfer_queue.release();
	this->vk_sparse_queue.release();

    //destroy command pool
    vkDestroyCommandPool(this->vk_device,
        this->vk_command_allocator_pool,
        nullptr);
    this->vk_command_allocator_pool = 0;

    SAFE_RELEASE(this->device_info);

    //release vulkan resources
    vkDestroyDevice(this->vk_device, nullptr);
	
#endif

    return 1;
}

#pragma region w_graphics_device_manager

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
    using namespace wolf;

    // Error that may result in undefined behaviour
    if (pFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
		logger.error("[Vulkan Debugger] Flag: ERROR - Message code: {} - Message: {}", pMsgCode, pMsg);
    }
    // Warnings may hint at unexpected / non-spec API usage
    if (pFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        logger.warning("[Vulkan Debugger] Flag: WARNING - Message code: {} - Message: {}", pMsgCode, pMsg);
    }
    // May indicate sub-optimal usage of the API
    if (pFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        logger.write("[Vulkan Debugger] Flag: PERFORMANCE - Message code: {} - Message: {}", pMsgCode, pMsg);
    }
    // Informal messages that may become handy during debugging
    if (pFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
		logger.write("[Vulkan Debugger] Flag: INFO - Message code: {} - Message: {}", pMsgCode, pMsg);
    }
    // Diagnostic info from the Vulkan loader and layers
    // Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
    if (pFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
		logger.write("[Vulkan Debugger] Flag: DEBUG - Message code: {} - Message: {}", pMsgCode, pMsg);
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
    namespace render
    {
		namespace vulkan
		{
		class w_graphics_device_manager_pimp
		{
		public:
			w_graphics_device_manager_pimp() :
				_name("w_graphics_device_manager_pimp")
			{
			}

			void enumerate_devices(_Inout_  std::vector<std::shared_ptr<w_graphics_device>>& pGraphicsDevices,
				_In_ system::w_signal<void(w_device_info**)>* pOnDeviceFeaturesFetched)
			{
#ifdef __DX12__

				W_RESULT _hr = W_FAILED;

				std::wstring _msg;
				_msg += L"++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n\t\t\t\t\tDirectX API version: 12";

				UINT _dxgi_factory_flags = 0;
                
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
							_out_window.v_sync = _window.v_sync;
							_out_window.force_to_disable_v_sync = _window.force_to_disable_v_sync;
							_out_window.is_full_screen = _window.is_full_screen;
#ifdef __WIN32
							_out_window.hwnd = _window.hwnd;
							_out_window.hInstance = _window.hInstance == NULL ? (HINSTANCE)GetModuleHandle(NULL) : _window.hInstance;
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
						if (_hr != W_PASSED)
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
						if (_hr != W_PASSED)
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
								_out_window.v_sync = _window.v_sync;
								_out_window.force_to_disable_v_sync = _window.force_to_disable_v_sync;
								_out_window.is_full_screen = _window.is_full_screen;
#ifdef __WIN32
								_out_window.hwnd = _window.hwnd;
								_out_window.hInstance = _out_window.hInstance = _window.hInstance == NULL ? (HINSTANCE)GetModuleHandle(NULL) : _window.hInstance;
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
				auto _vk_major = VK_VERSION_MAJOR(VK_API_VERSION_1_1);
				auto _vk_minor = VK_VERSION_MINOR(VK_API_VERSION_1_1);
				auto _vk_patch = VK_VERSION_PATCH(VK_HEADER_VERSION);

				std::ostringstream _msg;
				_msg << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n";
				_msg << "\t\t\t\t\tVulkan API version: " << _vk_major << "." << _vk_minor << "." << _vk_patch << "\r\n";

				//first find number of Vulkan instances
				uint32_t _instance_layer_count;

				// Use second parameter as NULL to return the layer count
				vkEnumerateInstanceLayerProperties(&_instance_layer_count, nullptr);
                
				VkLayerProperties* _layer_property = nullptr;
				vkEnumerateInstanceLayerProperties(&_instance_layer_count, _layer_property);

				uint32_t _extension_count = 0;

				//get available extensions count
				auto _hr = vkEnumerateInstanceExtensionProperties(nullptr,
						&_extension_count,
						nullptr);
				if (_hr)
				{
					//write the buffer to output before exiting
					logger.write(_msg.str().c_str());
					_msg.str("");
					_msg.clear();
					
					logger.error("error on enumerating instance extension properties.");

					release();
					std::exit(EXIT_FAILURE);
				}

				auto _extensions_available = new VkExtensionProperties[_extension_count];
				vkEnumerateInstanceExtensionProperties(nullptr, &_extension_count, _extensions_available);

				_msg << "\t\t\t\t\tVulkan extension(s) available:";
				for (size_t i = 0; i < _extension_count; ++i)
				{
					_msg << "\r\n\t\t\t\t\t\t" << _extensions_available[i].extensionName;
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

				if (!this->_config.off_screen_mode)
				{
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
				}

#if !defined(__APPLE__) && !defined(__iOS__)
                if (this->_config.debug_gpu)
                {
                    _vk_instance_enabled_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
                }
#endif

                VkInstanceCreateInfo _instance_create_info = {};
                _instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                _instance_create_info.pNext = nullptr;
                _instance_create_info.pApplicationInfo = &_app_info;
                if (_vk_instance_enabled_extensions.size() > 0)
                {
                    _instance_create_info.enabledExtensionCount = (uint32_t)_vk_instance_enabled_extensions.size();
                    _instance_create_info.ppEnabledExtensionNames = _vk_instance_enabled_extensions.data();
                }
                
#if !defined(__APPLE__) && !defined(__iOS__)
                if (this->_config.debug_gpu)
                {
                    const char* _validation_layer_names[] =
                    {
                        "VK_LAYER_LUNARG_standard_validation",
#ifdef __ANDROID
                        "VK_LAYER_GOOGLE_threading",
                        "VK_LAYER_LUNARG_parameter_validation",
                        "VK_LAYER_LUNARG_object_tracker,"
                        "VK_LAYER_LUNARG_core_validation",
                        "VK_LAYER_LUNARG_swapchain",
                        "VK_LAYER_GOOGLE_unique_objects"
#endif
                    };

                    _instance_create_info.enabledLayerCount = (uint32_t)W_ARRAY_SIZE(_validation_layer_names);
                    _instance_create_info.ppEnabledLayerNames = _validation_layer_names;
                }
#endif
                //create Vulkan instance
                _hr = vkCreateInstance(&_instance_create_info, nullptr, &w_graphics_device::vk_instance);
                _vk_instance_enabled_extensions.clear();
				if (_hr)
				{
					logger.write(_msg.str().c_str());
					_msg.str("");
					_msg.clear();
					logger.error("error on creating Vulkan instance.");
					release();
					std::exit(EXIT_FAILURE);
				}

#if !defined(__APPLE__) && !defined(__iOS__)
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
#endif

				//Enumerate physical devices
				uint32_t _gpu_count = 0;

				//Get number of GPU count
				vkEnumeratePhysicalDevices(w_graphics_device::vk_instance, &_gpu_count, nullptr);

				//Get GPU list names
				if (_gpu_count == 0)
				{
					//write the buffer to output before exiting
					logger.write(_msg.str().c_str());
					_msg.str("");
					_msg.clear();
					logger.error("GPU count. No GPU found.");
					release();
					std::exit(EXIT_FAILURE);
				}

				_msg << "\r\n\t\t\t\t\tGPU(s) founded:\r\n";

				auto _gpus = new VkPhysicalDevice[_gpu_count];

				vkEnumeratePhysicalDevices(w_graphics_device::vk_instance, &_gpu_count, _gpus);

				for (size_t i = 0; i < _gpu_count; ++i)
				{
					auto _device_properties = new (std::nothrow) VkPhysicalDeviceProperties();
					if (!_device_properties)
					{
						_msg << "Error on allocating memory for device properties";
						logger.write(_msg.str().c_str());
						_msg.str("");
						_msg.clear();
						release();
						std::exit(EXIT_FAILURE);
					}
					vkGetPhysicalDeviceProperties(_gpus[i], _device_properties);
					
					_msg << "\t\t\t\t\t\tDevice Name: " << _device_properties->deviceName << "\r\n" << 
						"\t\t\t\t\t\tDevice ID: " << _device_properties->deviceID << "\r\n" << 
						"\t\t\t\t\t\tDevice Vendor: " << _device_properties->vendorID << "\r\n" << 
					    "\t\t\t\t\t\tAPI Version: " << (_device_properties->apiVersion >> 22) << "." << 
						((_device_properties->apiVersion >> 12) & 0x3ff) << "." << (_device_properties->apiVersion & 0xfff) << "\r\n" <<
						"\t\t\t\t\t\tDriver Version: " << (_device_properties->driverVersion) << "\r\n\t\t\t\t\t\tDevice Type: ";

					switch (_device_properties->deviceType)
					{
					case VK_PHYSICAL_DEVICE_TYPE_OTHER:
						_msg << "VK_PHYSICAL_DEVICE_TYPE_OTHER";
						break;
					case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
						_msg << "VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU";
						break;
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
						_msg << "VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU";
						break;
					case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
						_msg << "VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU";
						break;
					case VK_PHYSICAL_DEVICE_TYPE_CPU:
						_msg << "VK_PHYSICAL_DEVICE_TYPE_CPU";
						break;
					case VK_PHYSICAL_DEVICE_TYPE_RANGE_SIZE:
						_msg << "VK_PHYSICAL_DEVICE_TYPE_RANGE_SIZE";
						break;
					case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
						_msg << "VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM";
						break;
					}

					_msg << "\r\n\t\t\t\t\t\tPhysical Device Limits:" << 
					"\r\n\t\t\t\t\t\t\tbufferImageGranularity: " << _device_properties->limits.bufferImageGranularity << 
					"\r\n\t\t\t\t\t\t\tdiscreteQueuePriorities: " << _device_properties->limits.discreteQueuePriorities << 
					"\r\n\t\t\t\t\t\t\tframebufferColorSampleCounts: " << _device_properties->limits.framebufferColorSampleCounts <<
					"\r\n\t\t\t\t\t\t\tframebufferDepthSampleCounts: " << _device_properties->limits.framebufferDepthSampleCounts << 
					"\r\n\t\t\t\t\t\t\tframebufferNoAttachmentsSampleCounts: " << _device_properties->limits.framebufferNoAttachmentsSampleCounts << 
					"\r\n\t\t\t\t\t\t\tframebufferStencilSampleCounts: " << _device_properties->limits.framebufferStencilSampleCounts << 
					"\r\n\t\t\t\t\t\t\tlineWidthGranularity: " << _device_properties->limits.lineWidthGranularity << 
					"\r\n\t\t\t\t\t\t\tmaxBoundDescriptorSets: " << _device_properties->limits.maxBoundDescriptorSets << 
					"\r\n\t\t\t\t\t\t\tmaxClipDistances: " << _device_properties->limits.maxClipDistances <<
					"\r\n\t\t\t\t\t\t\tmaxColorAttachments: " << _device_properties->limits.maxColorAttachments << 
					"\r\n\t\t\t\t\t\t\tmaxCombinedClipAndCullDistances: " << _device_properties->limits.maxCombinedClipAndCullDistances << 
					"\r\n\t\t\t\t\t\t\tmaxComputeSharedMemorySize: " << _device_properties->limits.maxComputeSharedMemorySize << 
					"\r\n\t\t\t\t\t\t\tmaxComputeWorkGroupInvocations: " << _device_properties->limits.maxComputeWorkGroupInvocations <<
					"\r\n\t\t\t\t\t\t\tmaxCullDistances: " << _device_properties->limits.maxCullDistances <<
					"\r\n\t\t\t\t\t\t\tmaxDescriptorSetInputAttachments: " << _device_properties->limits.maxDescriptorSetInputAttachments <<
					"\r\n\t\t\t\t\t\t\tmaxDescriptorSetSampledImages: " << _device_properties->limits.maxDescriptorSetSampledImages <<
					"\r\n\t\t\t\t\t\t\tmaxDescriptorSetSamplers: " << _device_properties->limits.maxDescriptorSetSamplers <<
					"\r\n\t\t\t\t\t\t\tmaxDescriptorSetStorageBuffers: " << _device_properties->limits.maxDescriptorSetStorageBuffers <<
					"\r\n\t\t\t\t\t\t\tmaxDescriptorSetStorageBuffersDynamic: " << _device_properties->limits.maxDescriptorSetStorageBuffersDynamic <<
					"\r\n\t\t\t\t\t\t\tmaxDrawIndexedIndexValue: " << _device_properties->limits.maxDrawIndexedIndexValue <<
					"\r\n\t\t\t\t\t\t\tmaxDrawIndirectCount: " << _device_properties->limits.maxDrawIndirectCount <<
					"\r\n\t\t\t\t\t\t\tmaxFragmentCombinedOutputResources: " << _device_properties->limits.maxFragmentCombinedOutputResources <<
					"\r\n\t\t\t\t\t\t\tmaxFragmentDualSrcAttachments: " << _device_properties->limits.maxFragmentDualSrcAttachments <<
					"\r\n\t\t\t\t\t\t\tmaxFragmentInputComponents: " << _device_properties->limits.maxFragmentInputComponents <<
					"\r\n\t\t\t\t\t\t\tmaxFragmentOutputAttachments: " << _device_properties->limits.maxFragmentOutputAttachments <<
					"\r\n\t\t\t\t\t\t\tmaxFramebufferWidth: " << _device_properties->limits.maxFramebufferWidth <<
					"\r\n\t\t\t\t\t\t\tmaxFramebufferHeight: " << _device_properties->limits.maxFramebufferHeight <<
					"\r\n\t\t\t\t\t\t\tmaxFramebufferLayers: " << _device_properties->limits.maxFramebufferLayers <<
					"\r\n\t\t\t\t\t\t\tmaxGeometryInputComponents: " << _device_properties->limits.maxGeometryInputComponents <<
					"\r\n\t\t\t\t\t\t\tmaxGeometryOutputComponents: " << _device_properties->limits.maxGeometryOutputComponents <<
					"\r\n\t\t\t\t\t\t\tmaxGeometryOutputVertices: " << _device_properties->limits.maxGeometryOutputVertices <<
					"\r\n\t\t\t\t\t\t\tmaxGeometryShaderInvocations: " << _device_properties->limits.maxGeometryShaderInvocations <<
					"\r\n\t\t\t\t\t\t\tmaxGeometryTotalOutputComponents: " << _device_properties->limits.maxGeometryTotalOutputComponents <<
					"\r\n\t\t\t\t\t\t\tmaxImageArrayLayers: " << _device_properties->limits.maxImageArrayLayers <<
					"\r\n\t\t\t\t\t\t\tmaxImageDimension1D: " << _device_properties->limits.maxImageDimension1D <<
					"\r\n\t\t\t\t\t\t\tmaxImageDimension2D: " << _device_properties->limits.maxImageDimension2D <<
					"\r\n\t\t\t\t\t\t\tmaxImageDimension3D: " << _device_properties->limits.maxImageDimension3D <<
					"\r\n\t\t\t\t\t\t\tmaxImageDimensionCube: " << _device_properties->limits.maxImageDimensionCube <<
					"\r\n\t\t\t\t\t\t\tmaxInterpolationOffset: " << _device_properties->limits.maxInterpolationOffset <<
					"\r\n\t\t\t\t\t\t\tmaxMemoryAllocationCount: " << _device_properties->limits.maxMemoryAllocationCount <<
					"\r\n\t\t\t\t\t\t\tmaxPerStageDescriptorInputAttachments: " << _device_properties->limits.maxPerStageDescriptorInputAttachments <<
					"\r\n\t\t\t\t\t\t\tmaxPerStageDescriptorSampledImages: " << _device_properties->limits.maxPerStageDescriptorSampledImages <<
					"\r\n\t\t\t\t\t\t\tmaxPerStageDescriptorSamplers: " << _device_properties->limits.maxPerStageDescriptorSamplers <<
					"\r\n\t\t\t\t\t\t\tmaxPerStageDescriptorStorageBuffers: " << _device_properties->limits.maxPerStageDescriptorStorageBuffers <<
					"\r\n\t\t\t\t\t\t\tmaxPerStageDescriptorStorageImages: " << _device_properties->limits.maxPerStageDescriptorStorageImages <<
					"\r\n\t\t\t\t\t\t\tmaxPerStageDescriptorUniformBuffers: " << _device_properties->limits.maxPerStageDescriptorUniformBuffers <<
					"\r\n\t\t\t\t\t\t\tmaxPerStageResources: " << _device_properties->limits.maxPerStageResources <<
					"\r\n\t\t\t\t\t\t\tmaxPushConstantsSize: " << _device_properties->limits.maxPushConstantsSize <<
					"\r\n\t\t\t\t\t\t\tmaxSampleMaskWords: " << _device_properties->limits.maxSampleMaskWords <<
					"\r\n\t\t\t\t\t\t\tmaxSamplerAllocationCount: " << _device_properties->limits.maxSamplerAllocationCount <<
					"\r\n\t\t\t\t\t\t\tmaxSamplerAnisotropy: " << _device_properties->limits.maxSamplerAnisotropy <<
					"\r\n\t\t\t\t\t\t\tmaxSamplerLodBias: " << _device_properties->limits.maxSamplerLodBias <<
					"\r\n\t\t\t\t\t\t\tmaxStorageBufferRange: " << _device_properties->limits.maxStorageBufferRange <<
					"\r\n\t\t\t\t\t\t\tmaxTessellationControlPerPatchOutputComponents: " << _device_properties->limits.maxTessellationControlPerPatchOutputComponents <<
					"\r\n\t\t\t\t\t\t\tmaxTessellationControlPerVertexInputComponents: " << _device_properties->limits.maxTessellationControlPerVertexInputComponents <<
					"\r\n\t\t\t\t\t\t\tmaxTessellationControlPerVertexOutputComponents: " << _device_properties->limits.maxTessellationControlPerVertexOutputComponents <<
					"\r\n\t\t\t\t\t\t\tmaxTessellationControlTotalOutputComponents: " << _device_properties->limits.maxTessellationControlTotalOutputComponents <<
					"\r\n\t\t\t\t\t\t\tmaxTessellationEvaluationInputComponents: " << _device_properties->limits.maxTessellationEvaluationInputComponents <<
					"\r\n\t\t\t\t\t\t\tmaxTessellationEvaluationOutputComponents: " << _device_properties->limits.maxTessellationEvaluationOutputComponents <<
					"\r\n\t\t\t\t\t\t\tmaxTessellationGenerationLevel: " << _device_properties->limits.maxTessellationGenerationLevel <<
					"\r\n\t\t\t\t\t\t\tmaxTessellationPatchSize: " << _device_properties->limits.maxTessellationPatchSize <<
					"\r\n\t\t\t\t\t\t\tmaxTexelBufferElements: " << _device_properties->limits.maxTexelBufferElements <<
					"\r\n\t\t\t\t\t\t\tmaxTexelGatherOffset: " << _device_properties->limits.maxTexelGatherOffset <<
					"\r\n\t\t\t\t\t\t\tmaxTexelOffset: " << _device_properties->limits.maxTexelOffset <<
					"\r\n\t\t\t\t\t\t\tmaxUniformBufferRange: " << _device_properties->limits.maxUniformBufferRange <<
					"\r\n\t\t\t\t\t\t\tmaxVertexInputAttributeOffset: " << _device_properties->limits.maxVertexInputAttributeOffset <<
					"\r\n\t\t\t\t\t\t\tmaxVertexInputAttributes: " << _device_properties->limits.maxVertexInputAttributes <<
					"\r\n\t\t\t\t\t\t\tmaxVertexInputBindingStride: " << _device_properties->limits.maxVertexInputBindingStride <<
					"\r\n\t\t\t\t\t\t\tmaxVertexInputBindings: " << _device_properties->limits.maxVertexInputBindings <<
					"\r\n\t\t\t\t\t\t\tmaxVertexOutputComponents: " << _device_properties->limits.maxVertexOutputComponents <<
					"\r\n\t\t\t\t\t\t\tmaxViewports: " << _device_properties->limits.maxViewports <<
					"\r\n\t\t\t\t\t\t\tminInterpolationOffset: " << _device_properties->limits.minInterpolationOffset <<
					"\r\n\t\t\t\t\t\t\tminMemoryMapAlignment: " << _device_properties->limits.minMemoryMapAlignment <<
					"\r\n\t\t\t\t\t\t\tminStorageBufferOffsetAlignment: " << _device_properties->limits.minStorageBufferOffsetAlignment <<
					"\r\n\t\t\t\t\t\t\tminTexelBufferOffsetAlignment: " << _device_properties->limits.minTexelBufferOffsetAlignment <<
					"\r\n\t\t\t\t\t\t\tminTexelGatherOffset: " << _device_properties->limits.minTexelGatherOffset <<
					"\r\n\t\t\t\t\t\t\tminTexelOffset: " << _device_properties->limits.minTexelOffset <<
					"\r\n\t\t\t\t\t\t\tminUniformBufferOffsetAlignment: " << _device_properties->limits.minUniformBufferOffsetAlignment <<
					"\r\n\t\t\t\t\t\t\tmipmapPrecisionBits: " << _device_properties->limits.mipmapPrecisionBits <<
					"\r\n\t\t\t\t\t\t\tnonCoherentAtomSize: " << _device_properties->limits.nonCoherentAtomSize <<
					"\r\n\t\t\t\t\t\t\toptimalBufferCopyOffsetAlignment: " << _device_properties->limits.optimalBufferCopyOffsetAlignment <<
					"\r\n\t\t\t\t\t\t\toptimalBufferCopyRowPitchAlignment: " << _device_properties->limits.optimalBufferCopyRowPitchAlignment <<
					"\r\n\t\t\t\t\t\t\tpointSizeGranularity: " << _device_properties->limits.pointSizeGranularity <<
					"\r\n\t\t\t\t\t\t\tsampledImageColorSampleCounts: " << _device_properties->limits.sampledImageColorSampleCounts <<
					"\r\n\t\t\t\t\t\t\tsampledImageDepthSampleCounts: " << _device_properties->limits.sampledImageDepthSampleCounts <<
					"\r\n\t\t\t\t\t\t\tsampledImageIntegerSampleCounts: " << _device_properties->limits.sampledImageIntegerSampleCounts <<
					"\r\n\t\t\t\t\t\t\tsampledImageStencilSampleCounts: " << _device_properties->limits.sampledImageStencilSampleCounts <<
					"\r\n\t\t\t\t\t\t\tsparseAddressSpaceSize: " << _device_properties->limits.sparseAddressSpaceSize <<
					"\r\n\t\t\t\t\t\t\tstandardSampleLocations: " << _device_properties->limits.standardSampleLocations <<
					"\r\n\t\t\t\t\t\t\tstorageImageSampleCounts: " << _device_properties->limits.storageImageSampleCounts <<
					"\r\n\t\t\t\t\t\t\tstrictLines: " << _device_properties->limits.strictLines <<
					"\r\n\t\t\t\t\t\t\tsubPixelInterpolationOffsetBits: " << _device_properties->limits.subPixelInterpolationOffsetBits <<
					"\r\n\t\t\t\t\t\t\tsubPixelPrecisionBits: " << _device_properties->limits.subPixelPrecisionBits <<
					"\r\n\t\t\t\t\t\t\tsubTexelPrecisionBits: " << _device_properties->limits.subTexelPrecisionBits <<
					"\r\n\t\t\t\t\t\t\ttimestampComputeAndGraphics: " << _device_properties->limits.timestampComputeAndGraphics <<
					"\r\n\t\t\t\t\t\t\ttimestampPeriod: " << _device_properties->limits.timestampPeriod <<
					"\r\n\t\t\t\t\t\t\tviewportSubPixelBits: " << _device_properties->limits.viewportSubPixelBits;

					//create a shared graphics device for this GPU
					auto _gDevice = std::make_shared<w_graphics_device>();
					_gDevice->vk_physical_device = _gpus[i];

					auto _device_info = new (std::nothrow) w_device_info();
					if (!_device_info)
					{
						logger.write(_msg.str().c_str());
						_msg.str("");
						_msg.clear();
						logger.error("could not find allocate memory for device_info.");
						release();
						std::exit(EXIT_FAILURE);
					}
					_device_info->device_properties = _device_properties;
					_device_info->device_features = &_gDevice->vk_physical_device_features;

                    //get device features
                    vkGetPhysicalDeviceFeatures(_gpus[i], &_gDevice->vk_physical_device_features);

                    //call event callback for changing device extensions and features from user
                    if (pOnDeviceFeaturesFetched)
                    {
                        (*pOnDeviceFeaturesFetched)(&_device_info);
                    }
					
                    //swap device extensions with user defined extentions
                    _gDevice->device_info = _device_info;

					//get memory properties from the physical device or GPU
					vkGetPhysicalDeviceMemoryProperties(_gpus[i], &_gDevice->vk_physical_device_memory_properties);

					for (size_t j = 0; j < _gDevice->vk_physical_device_memory_properties.memoryHeapCount; ++j)
					{
						_msg << "\r\n\t\t\t\t\t\t\tmemoryHeapSize: "  << _gDevice->vk_physical_device_memory_properties.memoryHeaps[j].size;
						switch (_gDevice->vk_physical_device_memory_properties.memoryHeaps[j].flags)
						{
						case VK_MEMORY_HEAP_DEVICE_LOCAL_BIT:
							_msg << "\r\n\t\t\t\t\t\t\tmemoryHeapFlag: VK_MEMORY_HEAP_DEVICE_LOCAL_BIT";
							break;
						case VK_MEMORY_HEAP_FLAG_BITS_MAX_ENUM:
							_msg << "\r\n\t\t\t\t\t\t\tmemoryHeapFlag: VK_MEMORY_HEAP_FLAG_BITS_MAX_ENUM";
							break;
						}
					}

					for (size_t j = 0; j < _gDevice->vk_physical_device_memory_properties.memoryTypeCount; ++j)
					{
						_msg << "\r\n\t\t\t\t\t\t\tmemoryTypeHeapIndex: " <<
							_gDevice->vk_physical_device_memory_properties.memoryTypes[j].heapIndex;
						switch (_gDevice->vk_physical_device_memory_properties.memoryTypes[j].propertyFlags)
						{
						case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
							_msg << "\r\n\t\t\t\t\t\t\t\tmemoryTypeFlag: VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT";
							break;
						case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT:
							_msg << "\r\n\t\t\t\t\t\t\t\tmemoryTypeFlag: VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT";
							break;
						case VK_MEMORY_PROPERTY_HOST_COHERENT_BIT:
							_msg << "\r\n\t\t\t\t\t\t\t\tmemoryTypeFlag: VK_MEMORY_PROPERTY_HOST_COHERENT_BIT";
							break;
						case VK_MEMORY_PROPERTY_HOST_CACHED_BIT:
							_msg << "\r\n\t\t\t\t\t\t\t\tmemoryTypeFlag: VK_MEMORY_PROPERTY_HOST_CACHED_BIT";
							break;
						case VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT:
							_msg << "\r\n\t\t\t\t\t\t\t\tmemoryTypeFlag: VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT";
							break;
						case VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM:
							_msg << "\r\n\t\t\t\t\t\t\t\tmemoryTypeFlag: VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM";
							break;
						}
					}

					//write buffer to output
					logger.write(_msg.str().c_str());
					_msg.str("");
					_msg.clear();

					//get queue family from default GPU
					uint32_t _queue_family_property_count = 0;

					vkGetPhysicalDeviceQueueFamilyProperties(_gpus[i],
						&_queue_family_property_count,
						nullptr);

					if (_queue_family_property_count == 0)
					{
						logger.write(_msg.str().c_str());
						_msg.str("");
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
						_msg << "\r\n\t\t\t\t\t\t_queue_family_properties: " << j;
						if (_gDevice->vk_queue_family_properties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
						{
							_queue_graphics_bit_found = true;
							_gDevice->vk_graphics_queue.index = static_cast<uint32_t>(j);
							_msg << "\r\n\t\t\t\t\t\t\tVK_QUEUE_GRAPHICS_BIT supported.";
							break;
						}
					}

                    for (size_t j = 0; j < _queue_family_property_count; ++j)
                    {
                        _msg << "\r\n\t\t\t\t\t\t_queue_family_properties: "  << j;
                        if (_gDevice->vk_queue_family_properties[j].queueFlags & VK_QUEUE_COMPUTE_BIT)
                        {
                            _queue_graphics_bit_found = true;
                            _gDevice->vk_compute_queue.index = static_cast<uint32_t>(j);
                            _msg << "\r\n\t\t\t\t\t\t\tVK_QUEUE_COMPUTE_BIT supported.";
                            break;
                        }
                    }


                    for (size_t j = 0; j < _queue_family_property_count; ++j)
                    {
                        _msg << "\r\n\t\t\t\t\t\t_queue_family_properties: " << j;
                        if (_gDevice->vk_queue_family_properties[j].queueFlags & VK_QUEUE_TRANSFER_BIT)
                        {
                            _queue_graphics_bit_found = true;
                            _gDevice->vk_transfer_queue.index = static_cast<uint32_t>(j);
                            _msg << "\r\n\t\t\t\t\t\t\tVK_QUEUE_TRANSFER_BIT supported.";
                            break;
                        }
                    }


                    for (size_t j = 0; j < _queue_family_property_count; ++j)
                    {
                        _msg << "\r\n\t\t\t\t\t\t_queue_family_properties: " + std::to_string(j);
                        if (_gDevice->vk_queue_family_properties[j].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
                        {
                            _queue_graphics_bit_found = true;
                            _gDevice->vk_sparse_queue.index = static_cast<uint32_t>(j);
                            _msg << "\r\n\t\t\t\t\t\t\tVK_QUEUE_SPARSE_BINDING_BIT supported.";
                            break;
                        }
                    }

					logger.write(_msg.str().c_str());
					_msg.str("");
					_msg.clear();
					if (!_queue_graphics_bit_found || _queue_family_property_count == 0)
					{
						//write the buffer to output before exiting
						logger.write(_msg.str().c_str());
						_msg.str("");
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
					VkDeviceCreateInfo _create_device_info = {};
					_create_device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
					_create_device_info.pNext = nullptr;
					_create_device_info.queueCreateInfoCount = 1;
					_create_device_info.pQueueCreateInfos = &_queue_info;
					_create_device_info.enabledLayerCount = 0;
					_create_device_info.ppEnabledLayerNames = nullptr;
					_create_device_info.pEnabledFeatures = &_gDevice->vk_physical_device_features;
					if (_gDevice->device_info->device_extensions.size())
					{
						_create_device_info.enabledExtensionCount = static_cast<uint32_t>(_gDevice->device_info->device_extensions.size());
						_create_device_info.ppEnabledExtensionNames = _gDevice->device_info->device_extensions.data();
					}

					//create device
					_hr = vkCreateDevice(_gpus[i], &_create_device_info, nullptr, &_gDevice->vk_device);
					if (_hr)
					{
						logger.write(_msg.str().c_str());
						_msg.str("");
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
						logger.error("error on creating vulkan command pool for graphics device:{} with device ID: {}",
							_device_properties->deviceName, _device_properties->deviceID);
						release();
						std::exit(EXIT_FAILURE);
					}


					if (!this->_config.off_screen_mode)
					{
						auto _win = this->_windows_info.find(static_cast<int>(i));
                        if (_win != this->_windows_info.end())
                        {
                            auto _window = _win->second;

                            w_output_presentation_window _out_window;
                            _out_window.width = _window.width;
                            _out_window.height = _window.height;
                            _out_window.aspect_ratio = (float)_window.width / (float)_window.height;
                            _out_window.vk_swap_chain_selected_format.format = (VkFormat)_window.swap_chain_format;
                            _out_window.vk_swap_chain_selected_format.colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
                            _out_window.cpu_access_to_swapchain_buffer = _window.cpu_access_swap_chain_buffer;
							_out_window.double_buffering = _window.double_buffering;
							_out_window.v_sync = _window.v_sync;

#if defined(__WIN32) || defined(__linux) || defined(__APPLE__) || defined(__ANDROID)
							
#ifdef __WIN32
                            _out_window.hwnd = _window.hwnd;
                            _out_window.hInstance = _window.hInstance == NULL ? (HINSTANCE)GetModuleHandle(NULL) : _window.hInstance;
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
								logger.write(_msg.str().c_str());
								_msg.str("");
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
                                logger.write(_msg.str().c_str());
								_msg.str("");
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
                                logger.write(_msg.str().c_str());
								_msg.str("");
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
                                logger.write(_msg.str().c_str());
								_msg.str("");
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
								logger.write(_msg.str().c_str());
								_msg.str("");
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
								logger.write(_msg.str().c_str());
								_msg.str("");
								_msg.clear();
                                logger.error("error on creating macOS surface for Vulkan.");
                                release();
                                std::exit(EXIT_FAILURE);
                            }
#endif

                            _gDevice->output_presentation_window = _out_window;

							create_swap_chain(_gDevice);
                            _create_fences(_gDevice);
                        }
					}
					else
					{
						w_output_presentation_window _out_window;
						_out_window.width = 0;
						_out_window.height = 0;
						_out_window.aspect_ratio = 0.0f;
						_gDevice->output_presentation_window = _out_window;
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
						if (!this->_config.off_screen_mode)
						{
							vkGetDeviceQueue(_gDevice->vk_device,
								_gDevice->vk_present_queue.index,
								0,
								&_gDevice->vk_present_queue.queue);
						}
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

					//initialize memory allocator
					if (_gDevice->memory_allocator.initialize(_gDevice) == W_FAILED)
					{
						logger.error("error on initializing graphics device memory allocator.");
						release();
						std::exit(EXIT_FAILURE);
					}

					pGraphicsDevices.push_back(_gDevice);

					//each window for each gpu
					if (this->_windows_info.size() == pGraphicsDevices.size())
					{
						break;
					}

				}
#endif //__DX12__, __VULKAN__
			}

			void create_swap_chain(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
			{
				const std::string _trace_info = this->_name + "::create_swap_chain";

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


				W_RESULT _hr = W_FAILED;
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
					else if (_hr != W_PASSED)
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
				auto _device_name = pGDevice->device_info->get_device_name();
				//auto _device_id = pGDevice->device_info->get_device_id();
				auto _output_presentation_window = &(pGDevice->output_presentation_window);

				if (!_output_presentation_window) return;
				auto _vk_presentation_surface = _output_presentation_window->vk_presentation_surface;
				if (!_vk_presentation_surface) return;


				for (size_t j = 0; j < pGDevice->vk_queue_family_properties.size(); ++j)
				{
					//check if this device support presentation
					auto _hr = vkGetPhysicalDeviceSurfaceSupportKHR(pGDevice->vk_physical_device,
						static_cast<uint32_t>(j),
						_vk_presentation_surface,
						&pGDevice->vk_queue_family_supports_present[j]);

					V(_hr == 0 ? W_PASSED : W_FAILED,
						w_log_type::W_WARNING,
						"could not get physical device surface support for graphics device: {}. trace info: {}",
						pGDevice->get_info(),
						_trace_info);

					if (pGDevice->vk_present_queue.index == UINT32_MAX &&
						pGDevice->vk_graphics_queue.index != UINT32_MAX &&
						pGDevice->vk_queue_family_supports_present[j])
					{
						pGDevice->vk_present_queue.index = static_cast<uint32_t>(j);
					}
				}

				V(pGDevice->vk_present_queue.index == UINT32_MAX ? W_FAILED : W_PASSED,
					w_log_type::W_WARNING,
					"could not find queue family which supports presentation for graphics device: {}. trace info: {}",
					pGDevice->get_info(),
					_trace_info);

				//get the list of VkFormats that are supported:
				uint32_t _vk_format_count;
				auto _hr = vkGetPhysicalDeviceSurfaceFormatsKHR(pGDevice->vk_physical_device,
					_vk_presentation_surface,
					&_vk_format_count,
					NULL);
				if (_hr)
				{
					logger.error("could not get number of physical device surface formats for graphics device: {}",
						pGDevice->get_info());
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
					logger.error("could not get physical device surface formats for graphics device: {}",
						pGDevice->get_info());
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
					_output_presentation_window->vk_swap_chain_selected_format.format = VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
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
						logger.error("preferred swap chain format \'{}\' not found for graphics device: {}", 
							_output_presentation_window->vk_swap_chain_selected_format.format,  
							pGDevice->get_info());

						_output_presentation_window->vk_swap_chain_selected_format = _output_presentation_window->vk_surface_formats[0];
					}
				}

				VkSurfaceCapabilitiesKHR _surface_capabilities;
				_hr = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pGDevice->vk_physical_device,
					_vk_presentation_surface,
					&_surface_capabilities);
				if (_hr == -3)
				{
					logger.error("error on create vulkan surface capabilities for graphics device: {}",
						pGDevice->get_info());

					//manually create _surface_capabilities
					_surface_capabilities.currentExtent.width = _output_presentation_window->width;
					_surface_capabilities.currentExtent.height = _output_presentation_window->height;
					_surface_capabilities.currentTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
					_surface_capabilities.maxImageArrayLayers = 1;
					_surface_capabilities.maxImageCount = 16;
					_surface_capabilities.maxImageExtent.width = _output_presentation_window->width;
					_surface_capabilities.maxImageExtent.height = _output_presentation_window->height;
					_surface_capabilities.minImageCount = 1;
					_surface_capabilities.minImageExtent.width = 1;
					_surface_capabilities.minImageExtent.height = 1;
					_surface_capabilities.supportedCompositeAlpha = 1;
					_surface_capabilities.supportedTransforms = 1;
					_surface_capabilities.supportedUsageFlags = 159;
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

				auto _desired_number_of_swapchain_images = _surface_capabilities.minImageCount;
				/*
					Determine the number of VkImage's to use in the swap chain.
					We need to acquire only 1 presentable image at at time.
					Asking for minImageCount images ensures that we can acquire
					1 presentable image as long as we present it before attempting
					to acquire another.
				*/
				if ((_surface_capabilities.maxImageCount > 0) &&
					(_desired_number_of_swapchain_images > _surface_capabilities.maxImageCount))
				{
					_desired_number_of_swapchain_images = _surface_capabilities.maxImageCount;
				}

				if (_desired_number_of_swapchain_images == 0)
				{
					logger.error("The images count of surface capabilities and swap chain is zero, for graphics device: {}",
						pGDevice->get_info());
					release();
					std::exit(EXIT_FAILURE);
				}
				if (_desired_number_of_swapchain_images < 2)
				{
					if (_output_presentation_window->double_buffering)
					{
						logger.warning("Double buffering for swap chain forced by user, for graphics device: {}",
							pGDevice->get_info());
						_desired_number_of_swapchain_images = 2;
					}
				}

				logger.write(
					"Desired number of swapchain image(s) is {} for graphics device: {}",
					_desired_number_of_swapchain_images,
					pGDevice->get_info());

				//Find a supported composite alpha format
				VkCompositeAlphaFlagBitsKHR _composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
				std::vector<VkCompositeAlphaFlagBitsKHR> _desired_composite_alphas = 
				{
					VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
					VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
					VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
					VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
				};
				for (auto& _flag : _desired_composite_alphas) 
				{
					if (_surface_capabilities.supportedCompositeAlpha & _flag)
					{
						_composite_alpha = _flag;
						break;
					};
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

				auto _image_extend = _surface_capabilities.currentExtent;
				if (_image_extend.width != _output_presentation_window->width)
				{
					_output_presentation_window->width = _image_extend.width;
				}
				if (_image_extend.height != _output_presentation_window->height)
				{
					_output_presentation_window->height = _image_extend.height;
				}

				//get the count of present modes
				uint32_t _present_mode_count;
				_hr = vkGetPhysicalDeviceSurfacePresentModesKHR(pGDevice->vk_physical_device,
					_vk_presentation_surface,
					&_present_mode_count, nullptr);
				if (_hr)
				{
					logger.error("error on getting vulkan present mode(s) count for graphics device: {}",
						pGDevice->get_info());
					release();
					std::exit(EXIT_FAILURE);
				}

				//get present modes
				std::vector<VkPresentModeKHR> _avaiable_present_modes(_present_mode_count);
				_hr = vkGetPhysicalDeviceSurfacePresentModesKHR(pGDevice->vk_physical_device,
					_vk_presentation_surface,
					&_present_mode_count,
					_avaiable_present_modes.data());
				if (_hr)
				{
					logger.error("error on getting vulkan present mode(s) for graphics device: {}",
						pGDevice->get_info());
					release();
					std::exit(EXIT_FAILURE);
				}

				if (_avaiable_present_modes.size() == 0)
				{
					logger.error("no avaiable present mode founded for graphics device: {}",
						pGDevice->get_info());
					release();
					std::exit(EXIT_FAILURE);
				}

				//select present mode
				/*
					VK_PRESENT_MODE_IMMEDIATE_KHR:		The presentation engine does not wait for a vertical blanking period to 
															update the current image, meaning this mode may result in visible tearing. 
															No internal queuing of presentation requests is needed, as the requests are 
															applied immediately.
					VK_PRESENT_MODE_MAILBOX_KHR:		The presentation engine waits for the next vertical blanking period to update 
															the current image. Tearing cannot be observed. An internal single-entry queue 
															is used to hold pending presentation requests. If the queue is full when a new 
															presentation request is received, the new request replaces the existing entry, 
															and any images associated with the prior entry become available for re-use by 
															the application. One request is removed from the queue and processed during each 
															vertical blanking period in which the queue is non-empty.
					VK_PRESENT_MODE_FIFO_KHR:			The presentation engine waits for the next vertical blanking period to update the current image. 
															Tearing cannot be observed. An internal queue is used to hold pending presentation requests. 
															New requests are appended to the end of the queue, and one request is removed from the beginning 
															of the queue and processed during each vertical blanking period in which the queue is non-empty. 
															This is the only value of presentMode that is required to be supported.
					VK_PRESENT_MODE_FIFO_RELAXED_KHR:	The presentation engine generally waits for the next vertical blanking period to update 
															the current image. If a vertical blanking period has already passed since the last update 
															of the current image then the presentation engine does not wait for another vertical blanking 
															period for the update, meaning this mode may result in visible tearing in this case. 
															This mode is useful for reducing visual stutter with an application that will mostly present 
															a new image before the next vertical blanking period, but may occasionally be late, and present 
															a new image just after the next vertical blanking period. An internal queue is used to hold pending 
															presentation requests. New requests are appended to the end of the queue, and one request is removed 
															from the beginning of the queue and processed during or after each vertical blanking period in which 
															the queue is non-empty.
				*/

				std::vector<VkPresentModeKHR> _desired_present_modes;
				if (_output_presentation_window->v_sync)
				{
					_desired_present_modes.push_back(VK_PRESENT_MODE_FIFO_KHR);
					_desired_present_modes.push_back(VK_PRESENT_MODE_FIFO_RELAXED_KHR);
					_desired_present_modes.push_back(VK_PRESENT_MODE_MAILBOX_KHR);
				}
				else
				{
					_desired_present_modes.push_back(VK_PRESENT_MODE_IMMEDIATE_KHR);
				}

				auto _present_mode = _select_present_mode(_desired_present_modes, _avaiable_present_modes);
				
				VkSwapchainCreateInfoKHR _swap_chain_create_info = {};
				_swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				_swap_chain_create_info.pNext = nullptr;
				_swap_chain_create_info.surface = _vk_presentation_surface;
				_swap_chain_create_info.minImageCount = _desired_number_of_swapchain_images;
				_swap_chain_create_info.imageFormat = _output_presentation_window->vk_swap_chain_selected_format.format;
				_swap_chain_create_info.imageColorSpace = _output_presentation_window->vk_swap_chain_selected_format.colorSpace;
				_swap_chain_create_info.imageExtent = _image_extend;
				_swap_chain_create_info.preTransform = _pre_transform;
				_swap_chain_create_info.compositeAlpha = _composite_alpha;
				_swap_chain_create_info.imageArrayLayers = 1;
				_swap_chain_create_info.presentMode = _present_mode;
				_swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;
				_swap_chain_create_info.clipped = VK_TRUE;//Discard rendering outside of the surface area
				_swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				_swap_chain_create_info.queueFamilyIndexCount = 0;
				_swap_chain_create_info.pQueueFamilyIndices = NULL;
				_swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
				if (_output_presentation_window->cpu_access_to_swapchain_buffer)
				{
					_swap_chain_create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
				}

				uint32_t _queue_family_indices[2] =
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
					logger.error("error on creating swap chain for vulkan for graphics device: {}", pGDevice->get_info());
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
					logger.error("error on getting total available image counts of swap chain for graphics device: {}",
						pGDevice->get_info());
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
					logger.error("error on getting total available images of swap chain for graphics device: {}",
						pGDevice->get_info());
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

                    auto _hr = vkCreateImageView(pGDevice->vk_device,
                        &_color_image_view,
                        nullptr,
                        &_image_view.view);
                    if (_hr)
                    {
                        logger.error("error on creating image view total available images of swap chain for graphics device: {}",
							pGDevice->get_info());
                        release();
                        std::exit(EXIT_FAILURE);
                    }

					_image_view.width = _output_presentation_window->width;
					_image_view.height = _output_presentation_window->height;
					_image_view.attachment_desc = w_attachment_buffer_desc::create_color_desc_buffer();
                    _output_presentation_window->swap_chain_image_views.push_back(_image_view);
                }

#pragma region Create Depth Buffer

				VkImageCreateInfo _depth_stencil_image_create_info = {};
				const VkFormat _depth_format = w_graphics_device_manager::find_supported_format(
					pGDevice,
					{ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
					VK_IMAGE_TILING_OPTIMAL,
					VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
				if (_depth_format == VkFormat::VK_FORMAT_UNDEFINED)
				{
					logger.error("Depth format not supported for graphics device: {}",
						pGDevice->get_info());
					release();
					std::exit(EXIT_FAILURE);
				}

				_output_presentation_window->depth_buffer_format = (w_format)_depth_format;

				//define depth stencil image description
				_depth_stencil_image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				_depth_stencil_image_create_info.pNext = nullptr;
				_depth_stencil_image_create_info.imageType = VK_IMAGE_TYPE_2D;
				_depth_stencil_image_create_info.format = _depth_format;
				_depth_stencil_image_create_info.extent.width = _output_presentation_window->width;
				_depth_stencil_image_create_info.extent.height = _output_presentation_window->height;
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
				_hr = vkCreateImage(pGDevice->vk_device,
					&_depth_stencil_image_create_info,
					nullptr,
					&_output_presentation_window->depth_buffer_image_view.image);
				if (_hr)
				{
					logger.error("error on creating depth buffer for graphics device: {}",
						pGDevice->get_info());
					release();
					std::exit(EXIT_FAILURE);
				}

				VkMemoryRequirements _mem_reqs;
				vkGetImageMemoryRequirements(pGDevice->vk_device,
					_output_presentation_window->depth_buffer_image_view.image,
					&_mem_reqs);

				_mem_alloc.allocationSize = _mem_reqs.size;

				//use the memory properties to determine the type of memory required
				_hr = w_graphics_device_manager::memory_type_from_properties(pGDevice->vk_physical_device_memory_properties,
					_mem_reqs.memoryTypeBits,
					0, // No Requirements
					&_mem_alloc.memoryTypeIndex);
				if (_hr)
				{
					logger.error("error on determining the type of memory required from memory properties for graphics device: {}",
						pGDevice->get_info());
					release();
					std::exit(EXIT_FAILURE);
				}

				//allocate memory
				_hr = vkAllocateMemory(pGDevice->vk_device,
					&_mem_alloc,
					nullptr,
					&_output_presentation_window->depth_buffer_memory);
				if (_hr)
				{
					logger.error("error on allocating memory for depth buffer image for graphics device: {}",
						pGDevice->get_info());
					release();
					std::exit(EXIT_FAILURE);
				}

				//bind memory
				_hr = vkBindImageMemory(pGDevice->vk_device,
					_output_presentation_window->depth_buffer_image_view.image,
					_output_presentation_window->depth_buffer_memory,
					0);
				if (_hr)
				{
					logger.error("error on binding to memory for depth buffer image for graphics device: {}",
						pGDevice->get_info());
					release();
					std::exit(EXIT_FAILURE);
				}

				//create depth stencil buffer image view
				VkImageViewCreateInfo _depth_stencil_view_info = {};
				_depth_stencil_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				_depth_stencil_view_info.pNext = nullptr;
				_depth_stencil_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
				_depth_stencil_view_info.image = _output_presentation_window->depth_buffer_image_view.image;
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
					&_output_presentation_window->depth_buffer_image_view.view);
				if (_hr)
				{
					logger.error("error on creating image view for depth buffer image for graphics device: {}",
						pGDevice->get_info());
					release();
					std::exit(EXIT_FAILURE);
				}

				_output_presentation_window->depth_buffer_image_view.width = _output_presentation_window->width;
				_output_presentation_window->depth_buffer_image_view.height = _output_presentation_window->height;
				_output_presentation_window->depth_buffer_image_view.attachment_desc = w_attachment_buffer_desc::create_depth_desc_buffer();

#pragma endregion

                //create required objects for accessing swap chain buffer by CPU
                if (_output_presentation_window->cpu_access_to_swapchain_buffer)
                {
                    _create_shared_objects_between_cpu_gpu(pGDevice);
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
#if defined(__VULKAN__) && !defined(__APPLE__) && !defined(__iOS__)
                    sDestroyDebugReportCallback(w_graphics_device::vk_instance, MsgCallback, nullptr);
#endif
                }

				return 1;
			}

#pragma region Getters

			std::map<int, w_present_info> get_output_windows_info() const
			{
				return this->_windows_info;
			}

			w_graphics_device_manager_configs get_graphics_device_manager_configs() const
			{
				return this->_config;
			}

#pragma endregion

#pragma region Setters

			void set_graphics_device_manager_configs(_In_ const w_graphics_device_manager_configs& pConfig)
			{
				this->_config = pConfig;
			}

			void set_output_windows_info(_In_ std::map<int, w_present_info> pOutputWindowsInfo)
			{
				this->_windows_info = pOutputWindowsInfo;
			}

#pragma endregion

		private:
                        
			void _create_fences(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
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
				auto _device_name = pGDevice->device_info->get_device_name();
				//auto _device_id = pGDevice->device_info->get_device_id();
				auto _output_presentation_window = &(pGDevice->output_presentation_window);

                //create semaphores fro this graphics device
                if (_output_presentation_window->swap_chain_image_is_available_semaphore.initialize(pGDevice) == W_FAILED)
                {
					logger.error("error on creating image_is_available semaphore for graphics device: {}",
						pGDevice->get_info());
                    release();
                    std::exit(EXIT_FAILURE);
                }
                if (_output_presentation_window->rendering_done_semaphore.initialize(pGDevice) == W_FAILED)
                {
					logger.error("error on creating rendering_is_done semaphore for graphics device: {}",
						pGDevice->get_info());
					release();
					std::exit(EXIT_FAILURE);
				}
#endif
			}
            
            void _create_shared_objects_between_cpu_gpu(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
            {
                auto _output_window = &(pGDevice->output_presentation_window);
                if (!_output_window) return;

                const std::string _trace_info = "w_graphics_device::_create_shared_objects_between_cpu_gpu";
                //auto _device_id = pGDevice->device_info->get_device_id();
                auto _device_name = pGDevice->device_info->get_device_name();

                auto _src_format = _output_window->vk_swap_chain_selected_format.format;
                _output_window->objs_between_cpu_gpu = new (std::nothrow) w_output_presentation_window::shared_objs_between_cpu_gpu();
                if (!_output_window->objs_between_cpu_gpu)
                {
                    logger.error("error on allocating memory for shared objects between CPU and GPU for graphics device: {}",
						pGDevice->get_info());
                    release();
                    std::exit(EXIT_FAILURE);
                }
				
				auto _objs_ptr = _output_window->objs_between_cpu_gpu;

                //check whether blitting is supported or not
                VkFormatProperties _format_properties;
                // Check for whether blitting is supported from optimal image
                vkGetPhysicalDeviceFormatProperties(
                    pGDevice->vk_physical_device,
                    _src_format,
                    &_format_properties);
                if (!(_format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
                {
					logger.warning("Blitting feature not supported from optimal tiled image for graphics device: {} and following format: {}",
						pGDevice->get_info(),
						_src_format);
                    _output_window->bliting_supported_by_swap_chain = false;
                }
                if (_output_window->bliting_supported_by_swap_chain)
                {
                    // Check for whether blitting is supported for linear image
                    if (!(_format_properties.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT))
                    {
						logger.warning("Blitting feature not supported from linear tiled image for graphics device: {} and following format: {}",
							pGDevice->get_info(),
							_src_format);
                        _output_window->bliting_supported_by_swap_chain = false;
                    }
                }

                //create destination image in order to get data of swap chain's image and later allow access from cpu
                VkImageCreateInfo _image_create_info = {};
                _image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                _image_create_info.imageType = VK_IMAGE_TYPE_2D;
                _image_create_info.format = _src_format;
                _image_create_info.extent.width = _output_window->width;
                _image_create_info.extent.height = _output_window->height;
                _image_create_info.extent.depth = 1;
                _image_create_info.arrayLayers = 1;
                _image_create_info.mipLevels = 1;
                _image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                _image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
                _image_create_info.tiling = VK_IMAGE_TILING_LINEAR;
                _image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

                //create the destination image
                auto _hr = vkCreateImage(
                    pGDevice->vk_device,
                    &_image_create_info,
                    nullptr,
                    &_objs_ptr->destination_image);
				if (_hr)
				{
					logger.error("error on creating destination image for graphics device: {}",
						pGDevice->get_info());
					release();
					std::exit(EXIT_FAILURE);
				}

                //get memory requirements
                VkMemoryRequirements _mem_requirements;
                vkGetImageMemoryRequirements(pGDevice->vk_device, _objs_ptr->destination_image, &_mem_requirements);

                uint32_t _mem_type_index;
                //check for host memory index
                w_graphics_device_manager::memory_type_from_properties(
                    pGDevice->vk_physical_device_memory_properties,
                    _mem_requirements.memoryTypeBits,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    &_mem_type_index);

                //allocate memory info
                VkMemoryAllocateInfo _mem_alloc_info = {};
                _mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                _mem_alloc_info.allocationSize = _mem_requirements.size;
                _mem_alloc_info.memoryTypeIndex = _mem_type_index;

                //allocate memory in host
                _hr = vkAllocateMemory(pGDevice->vk_device, &_mem_alloc_info, nullptr, &_objs_ptr->destination_image_memory);
                if (_hr)
                {
					logger.error("error on creating destination image for graphics device: {}",
						pGDevice->get_info());
                    release();
                    std::exit(EXIT_FAILURE);
                }

                // create command buffer on primary
                VkCommandBufferAllocateInfo _copy_cmd_buf_allocate_info = {};
                _copy_cmd_buf_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                _copy_cmd_buf_allocate_info.commandPool = pGDevice->vk_command_allocator_pool;
                _copy_cmd_buf_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                _copy_cmd_buf_allocate_info.commandBufferCount = 1;
                _hr = vkAllocateCommandBuffers(pGDevice->vk_device, &_copy_cmd_buf_allocate_info, &_objs_ptr->copy_command_buffer);
                if (_hr)
                {
                    logger.error("error on allocating buffer for copy command buffer for graphics device: {}",
						pGDevice->get_info());
                    release();
                    std::exit(EXIT_FAILURE);
                }

				//bind to image memory
				_hr = vkBindImageMemory(pGDevice->vk_device, _objs_ptr->destination_image, _objs_ptr->destination_image_memory, 0);
				if (_hr)
				{
					logger.error("binding to destination image  for graphics device: {}",
						pGDevice->get_info());
					release();
					std::exit(EXIT_FAILURE);
				}
            }

			static VkPresentModeKHR _select_present_mode(
				_In_ std::vector<VkPresentModeKHR> pDesiredPresentModes,
				_In_ std::vector<VkPresentModeKHR> pAvaiablePresentModes)
			{
				VkPresentModeKHR _selected = pAvaiablePresentModes.size() > 0 ? pAvaiablePresentModes[0] :
					VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
				bool _found = false;
				for (auto& _desired : pDesiredPresentModes)
				{
					for (auto& _avaiable : pAvaiablePresentModes)
					{
						if (_desired == _avaiable)
						{
							_found = true;
							break;
						}
					}

					if (_found) break;
				}

				return _selected;
			}

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
            std::map<int, w_present_info>                        _windows_info;
            std::string                                         _name;;
		};
    }
	}
}

#pragma endregion

w_graphics_device_manager::w_graphics_device_manager() : _pimp(new w_graphics_device_manager_pimp())
{
	_super::set_class_name("w_graphics_device_manager_pimp");// typeid(this).name());
   
#if defined(__WIN32) && !defined(__PYTHON__)
    auto _hr = CoInitialize(NULL);
	V(_hr == S_OK ? W_PASSED : W_FAILED, L"CoInitialize already been called", _super::name, 3, false);
#endif

#ifdef __ANDROID
	InitVulkan();
#endif
}

w_graphics_device_manager::~w_graphics_device_manager()
{
	release();
}

void w_graphics_device_manager::initialize(_In_ std::map<int, w_present_info> pOutputWindowsInfo)
{
    //store information of windows for the first time
    this->_pimp->set_output_windows_info(pOutputWindowsInfo);
    this->_pimp->enumerate_devices(this->graphics_devices, &this->on_device_info_fetched);
    
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

#pragma region pixels data
	std::map<int, uint8_t> _logo_data = { { 32,253 },{ 33,253 },{ 34,253 },{ 44,254 },{ 45,254 },{ 46,254 },{ 48,252 },{ 49,252 },{ 50,252 },{ 56,249 },{ 57,249 },{ 58,249 },{ 60,253 },{ 61,253 },{ 62,253 },{ 64,253 },{ 65,253 },{ 66,253 },{ 72,253 },{ 73,253 },{ 74,253 },{ 76,251 },{ 77,251 },{ 78,251 },{ 84,254 },{ 85,254 },{ 86,254 },{ 88,253 },{ 89,253 },{ 90,253 },{ 100,253 },{ 101,253 },{ 102,253 },{ 108,252 },{ 109,252 },{ 110,252 },{ 116,253 },{ 117,253 },{ 118,253 },{ 124,252 },{ 125,252 },{ 126,252 },{ 128,251 },{ 129,251 },{ 130,251 },{ 132,253 },{ 133,253 },{ 134,253 },{ 140,252 },{ 141,252 },{ 142,252 },{ 152,252 },{ 153,252 },{ 154,252 },{ 228,253 },{ 229,253 },{ 230,253 },{ 240,254 },{ 241,254 },{ 242,254 },{ 244,250 },{ 245,250 },{ 246,250 },{ 264,253 },{ 265,253 },{ 266,253 },{ 268,251 },{ 269,251 },{ 270,251 },{ 272,254 },{ 273,254 },{ 274,254 },{ 276,251 },{ 277,251 },{ 278,251 },{ 284,251 },{ 285,251 },{ 286,251 },{ 288,250 },{ 289,250 },{ 290,250 },{ 296,252 },{ 297,252 },{ 298,252 },{ 304,253 },{ 305,253 },{ 306,253 },{ 308,251 },{ 309,251 },{ 310,251 },{ 324,254 },{ 325,254 },{ 326,254 },{ 328,254 },{ 329,254 },{ 330,254 },{ 332,253 },{ 333,253 },{ 334,253 },{ 336,254 },{ 337,254 },{ 338,254 },{ 340,251 },{ 341,251 },{ 342,251 },{ 424,253 },{ 425,253 },{ 426,253 },{ 428,254 },{ 429,254 },{ 430,254 },{ 432,253 },{ 433,253 },{ 434,253 },{ 436,253 },{ 437,253 },{ 438,253 },{ 440,253 },{ 441,253 },{ 442,253 },{ 448,253 },{ 449,253 },{ 450,253 },{ 456,252 },{ 457,252 },{ 458,252 },{ 460,254 },{ 461,254 },{ 462,254 },{ 464,251 },{ 465,251 },{ 466,251 },{ 472,249 },{ 473,249 },{ 474,249 },{ 476,251 },{ 477,251 },{ 478,251 },{ 484,249 },{ 485,249 },{ 486,249 },{ 488,253 },{ 489,253 },{ 490,253 },{ 492,253 },{ 493,253 },{ 494,253 },{ 496,254 },{ 497,254 },{ 498,254 },{ 508,253 },{ 509,253 },{ 510,253 },{ 512,253 },{ 513,253 },{ 514,253 },{ 532,252 },{ 533,252 },{ 534,252 },{ 608,254 },{ 609,254 },{ 610,254 },{ 620,253 },{ 621,253 },{ 622,253 },{ 632,250 },{ 633,250 },{ 634,250 },{ 636,37 },{ 637,37 },{ 638,37 },{ 640,1 },{ 641,1 },{ 642,1 },{ 644,186 },{ 645,186 },{ 646,186 },{ 660,248 },{ 661,248 },{ 662,248 },{ 672,250 },{ 673,250 },{ 674,250 },{ 680,254 },{ 681,254 },{ 682,254 },{ 684,253 },{ 685,253 },{ 686,253 },{ 696,97 },{ 697,97 },{ 698,97 },{ 700,2 },{ 701,2 },{ 702,2 },{ 708,237 },{ 709,237 },{ 710,237 },{ 716,251 },{ 717,251 },{ 718,251 },{ 720,254 },{ 721,254 },{ 722,254 },{ 804,249 },{ 805,249 },{ 806,249 },{ 808,252 },{ 809,252 },{ 810,252 },{ 820,217 },{ 821,217 },{ 822,217 },{ 824,36 },{ 825,36 },{ 826,36 },{ 832,1 },{ 833,1 },{ 834,1 },{ 836,27 },{ 837,27 },{ 838,27 },{ 840,253 },{ 841,253 },{ 842,253 },{ 844,249 },{ 845,249 },{ 846,249 },{ 848,254 },{ 849,254 },{ 850,254 },{ 860,250 },{ 861,250 },{ 862,250 },{ 864,253 },{ 865,253 },{ 866,253 },{ 872,250 },{ 873,250 },{ 874,250 },{ 884,129 },{ 885,129 },{ 886,129 },{ 888,1 },{ 889,1 },{ 890,1 },{ 892,2 },{ 893,2 },{ 894,2 },{ 904,205 },{ 905,205 },{ 906,205 },{ 912,252 },{ 913,252 },{ 914,252 },{ 920,253 },{ 921,253 },{ 922,253 },{ 988,254 },{ 989,254 },{ 990,254 },{ 992,254 },{ 993,254 },{ 994,254 },{ 1004,250 },{ 1005,250 },{ 1006,250 },{ 1008,239 },{ 1009,239 },{ 1010,239 },{ 1012,14 },{ 1013,14 },{ 1014,14 },{ 1016,3 },{ 1017,3 },{ 1018,3 },{ 1020,72 },{ 1021,72 },{ 1022,72 },{ 1024,54 },{ 1025,54 },{ 1026,54 },{ 1032,126 },{ 1033,126 },{ 1034,126 },{ 1040,251 },{ 1041,251 },{ 1042,251 },{ 1048,253 },{ 1049,253 },{ 1050,253 },{ 1056,254 },{ 1057,254 },{ 1058,254 },{ 1060,254 },{ 1061,254 },{ 1062,254 },{ 1064,250 },{ 1065,250 },{ 1066,250 },{ 1068,254 },{ 1069,254 },{ 1070,254 },{ 1072,178 },{ 1073,178 },{ 1074,178 },{ 1084,7 },{ 1085,7 },{ 1086,7 },{ 1088,20 },{ 1089,20 },{ 1090,20 },{ 1096,2 },{ 1097,2 },{ 1098,2 },{ 1100,228 },{ 1101,228 },{ 1102,228 },{ 1108,253 },{ 1109,253 },{ 1110,253 },{ 1116,253 },{ 1117,253 },{ 1118,253 },{ 1120,254 },{ 1121,254 },{ 1122,254 },{ 1176,254 },{ 1177,254 },{ 1178,254 },{ 1180,254 },{ 1181,254 },{ 1182,254 },{ 1184,254 },{ 1185,254 },{ 1186,254 },{ 1188,245 },{ 1189,245 },{ 1190,245 },{ 1200,63 },{ 1201,63 },{ 1202,63 },{ 1204,6 },{ 1205,6 },{ 1206,6 },{ 1208,164 },{ 1209,164 },{ 1210,164 },{ 1212,254 },{ 1213,254 },{ 1214,254 },{ 1220,135 },{ 1221,135 },{ 1222,135 },{ 1224,4 },{ 1225,4 },{ 1226,4 },{ 1228,225 },{ 1229,225 },{ 1230,225 },{ 1236,254 },{ 1237,254 },{ 1238,254 },{ 1240,251 },{ 1241,251 },{ 1242,251 },{ 1244,253 },{ 1245,253 },{ 1246,253 },{ 1256,253 },{ 1257,253 },{ 1258,253 },{ 1264,119 },{ 1265,119 },{ 1266,119 },{ 1268,4 },{ 1269,4 },{ 1270,4 },{ 1272,87 },{ 1273,87 },{ 1274,87 },{ 1276,253 },{ 1277,253 },{ 1278,253 },{ 1284,161 },{ 1285,161 },{ 1286,161 },{ 1288,1 },{ 1289,1 },{ 1290,1 },{ 1292,1 },{ 1293,1 },{ 1294,1 },{ 1304,250 },{ 1305,250 },{ 1306,250 },{ 1312,254 },{ 1313,254 },{ 1314,254 },{ 1316,254 },{ 1317,254 },{ 1318,254 },{ 1368,254 },{ 1369,254 },{ 1370,254 },{ 1372,254 },{ 1373,254 },{ 1374,254 },{ 1376,254 },{ 1377,254 },{ 1378,254 },{ 1384,253 },{ 1385,253 },{ 1386,253 },{ 1388,170 },{ 1389,170 },{ 1390,170 },{ 1392,6 },{ 1393,6 },{ 1394,6 },{ 1396,219 },{ 1397,219 },{ 1398,219 },{ 1404,254 },{ 1405,254 },{ 1406,254 },{ 1408,254 },{ 1409,254 },{ 1410,254 },{ 1420,151 },{ 1421,151 },{ 1422,151 },{ 1428,253 },{ 1429,253 },{ 1430,253 },{ 1436,254 },{ 1437,254 },{ 1438,254 },{ 1440,254 },{ 1441,254 },{ 1442,254 },{ 1444,254 },{ 1445,254 },{ 1446,254 },{ 1452,250 },{ 1453,250 },{ 1454,250 },{ 1456,80 },{ 1457,80 },{ 1458,80 },{ 1464,254 },{ 1465,254 },{ 1466,254 },{ 1472,254 },{ 1473,254 },{ 1474,254 },{ 1476,254 },{ 1477,254 },{ 1478,254 },{ 1480,213 },{ 1481,213 },{ 1482,213 },{ 1484,17 },{ 1485,17 },{ 1486,17 },{ 1488,102 },{ 1489,102 },{ 1490,102 },{ 1492,254 },{ 1493,254 },{ 1494,254 },{ 1500,254 },{ 1501,254 },{ 1502,254 },{ 1504,254 },{ 1505,254 },{ 1506,254 },{ 1508,254 },{ 1509,254 },{ 1510,254 },{ 1556,254 },{ 1557,254 },{ 1558,254 },{ 1560,254 },{ 1561,254 },{ 1562,254 },{ 1564,253 },{ 1565,253 },{ 1566,253 },{ 1568,254 },{ 1569,254 },{ 1570,254 },{ 1572,252 },{ 1573,252 },{ 1574,252 },{ 1576,234 },{ 1577,234 },{ 1578,234 },{ 1584,252 },{ 1585,252 },{ 1586,252 },{ 1592,252 },{ 1593,252 },{ 1594,252 },{ 1596,250 },{ 1597,250 },{ 1598,250 },{ 1600,252 },{ 1601,252 },{ 1602,252 },{ 1608,1 },{ 1609,1 },{ 1610,1 },{ 1612,135 },{ 1613,135 },{ 1614,135 },{ 1620,251 },{ 1621,251 },{ 1622,251 },{ 1628,219 },{ 1629,219 },{ 1630,219 },{ 1632,251 },{ 1633,251 },{ 1634,251 },{ 1640,253 },{ 1641,253 },{ 1642,253 },{ 1644,249 },{ 1645,249 },{ 1646,249 },{ 1648,24 },{ 1649,24 },{ 1650,24 },{ 1660,252 },{ 1661,252 },{ 1662,252 },{ 1668,254 },{ 1669,254 },{ 1670,254 },{ 1672,254 },{ 1673,254 },{ 1674,254 },{ 1676,246 },{ 1677,246 },{ 1678,246 },{ 1684,247 },{ 1685,247 },{ 1686,247 },{ 1692,251 },{ 1693,251 },{ 1694,251 },{ 1696,254 },{ 1697,254 },{ 1698,254 },{ 1700,254 },{ 1701,254 },{ 1702,254 },{ 1704,254 },{ 1705,254 },{ 1706,254 },{ 1748,254 },{ 1749,254 },{ 1750,254 },{ 1752,254 },{ 1753,254 },{ 1754,254 },{ 1756,253 },{ 1757,253 },{ 1758,253 },{ 1760,252 },{ 1761,252 },{ 1762,252 },{ 1768,98 },{ 1769,98 },{ 1770,98 },{ 1772,166 },{ 1773,166 },{ 1774,166 },{ 1776,253 },{ 1777,253 },{ 1778,253 },{ 1780,251 },{ 1781,251 },{ 1782,251 },{ 1796,252 },{ 1797,252 },{ 1798,252 },{ 1800,111 },{ 1801,111 },{ 1802,111 },{ 1812,2 },{ 1813,2 },{ 1814,2 },{ 1820,3 },{ 1821,3 },{ 1822,3 },{ 1824,5 },{ 1825,5 },{ 1826,5 },{ 1832,6 },{ 1833,6 },{ 1834,6 },{ 1844,93 },{ 1845,93 },{ 1846,93 },{ 1848,254 },{ 1849,254 },{ 1850,254 },{ 1852,254 },{ 1853,254 },{ 1854,254 },{ 1856,249 },{ 1857,249 },{ 1858,249 },{ 1860,253 },{ 1861,253 },{ 1862,253 },{ 1864,254 },{ 1865,254 },{ 1866,254 },{ 1872,244 },{ 1873,244 },{ 1874,244 },{ 1876,199 },{ 1877,199 },{ 1878,199 },{ 1884,254 },{ 1885,254 },{ 1886,254 },{ 1888,254 },{ 1889,254 },{ 1890,254 },{ 1892,254 },{ 1893,254 },{ 1894,254 },{ 1896,254 },{ 1897,254 },{ 1898,254 },{ 1940,254 },{ 1941,254 },{ 1942,254 },{ 1944,254 },{ 1945,254 },{ 1946,254 },{ 1948,253 },{ 1949,253 },{ 1950,253 },{ 1952,254 },{ 1953,254 },{ 1954,254 },{ 1956,251 },{ 1957,251 },{ 1958,251 },{ 1960,157 },{ 1961,157 },{ 1962,157 },{ 1964,222 },{ 1965,222 },{ 1966,222 },{ 1980,247 },{ 1981,247 },{ 1982,247 },{ 1984,254 },{ 1985,254 },{ 1986,254 },{ 1992,184 },{ 1993,184 },{ 1994,184 },{ 1996,1 },{ 1997,1 },{ 1998,1 },{ 2000,3 },{ 2001,3 },{ 2002,3 },{ 2008,4 },{ 2009,4 },{ 2010,4 },{ 2020,1 },{ 2021,1 },{ 2022,1 },{ 2028,3 },{ 2029,3 },{ 2030,3 },{ 2036,176 },{ 2037,176 },{ 2038,176 },{ 2040,253 },{ 2041,253 },{ 2042,253 },{ 2044,254 },{ 2045,254 },{ 2046,254 },{ 2056,253 },{ 2057,253 },{ 2058,253 },{ 2060,253 },{ 2061,253 },{ 2062,253 },{ 2068,254 },{ 2069,254 },{ 2070,254 },{ 2072,251 },{ 2073,251 },{ 2074,251 },{ 2076,252 },{ 2077,252 },{ 2078,252 },{ 2080,254 },{ 2081,254 },{ 2082,254 },{ 2084,254 },{ 2085,254 },{ 2086,254 },{ 2132,254 },{ 2133,254 },{ 2134,254 },{ 2136,254 },{ 2137,254 },{ 2138,254 },{ 2140,253 },{ 2141,253 },{ 2142,253 },{ 2148,238 },{ 2149,238 },{ 2150,238 },{ 2152,213 },{ 2153,213 },{ 2154,213 },{ 2160,254 },{ 2161,254 },{ 2162,254 },{ 2164,252 },{ 2165,252 },{ 2166,252 },{ 2168,251 },{ 2169,251 },{ 2170,251 },{ 2180,254 },{ 2181,254 },{ 2182,254 },{ 2184,222 },{ 2185,222 },{ 2186,222 },{ 2200,1 },{ 2201,1 },{ 2202,1 },{ 2204,2 },{ 2205,2 },{ 2206,2 },{ 2208,4 },{ 2209,4 },{ 2210,4 },{ 2212,2 },{ 2213,2 },{ 2214,2 },{ 2224,2 },{ 2225,2 },{ 2226,2 },{ 2228,219 },{ 2229,219 },{ 2230,219 },{ 2236,253 },{ 2237,253 },{ 2238,253 },{ 2248,254 },{ 2249,254 },{ 2250,254 },{ 2256,254 },{ 2257,254 },{ 2258,254 },{ 2264,253 },{ 2265,253 },{ 2266,253 },{ 2272,254 },{ 2273,254 },{ 2274,254 },{ 2324,254 },{ 2325,254 },{ 2326,254 },{ 2328,254 },{ 2329,254 },{ 2330,254 },{ 2332,253 },{ 2333,253 },{ 2334,253 },{ 2336,251 },{ 2337,251 },{ 2338,251 },{ 2340,220 },{ 2341,220 },{ 2342,220 },{ 2344,171 },{ 2345,171 },{ 2346,171 },{ 2348,249 },{ 2349,249 },{ 2350,249 },{ 2360,242 },{ 2361,242 },{ 2362,242 },{ 2364,225 },{ 2365,225 },{ 2366,225 },{ 2368,253 },{ 2369,253 },{ 2370,253 },{ 2376,245 },{ 2377,245 },{ 2378,245 },{ 2380,53 },{ 2381,53 },{ 2382,53 },{ 2388,4 },{ 2389,4 },{ 2390,4 },{ 2392,6 },{ 2393,6 },{ 2394,6 },{ 2400,1 },{ 2401,1 },{ 2402,1 },{ 2408,7 },{ 2409,7 },{ 2410,7 },{ 2412,1 },{ 2413,1 },{ 2414,1 },{ 2416,8 },{ 2417,8 },{ 2418,8 },{ 2420,228 },{ 2421,228 },{ 2422,228 },{ 2424,254 },{ 2425,254 },{ 2426,254 },{ 2432,231 },{ 2433,231 },{ 2434,231 },{ 2436,239 },{ 2437,239 },{ 2438,239 },{ 2444,252 },{ 2445,252 },{ 2446,252 },{ 2448,253 },{ 2449,253 },{ 2450,253 },{ 2460,249 },{ 2461,249 },{ 2462,249 },{ 2516,254 },{ 2517,254 },{ 2518,254 },{ 2520,254 },{ 2521,254 },{ 2522,254 },{ 2524,253 },{ 2525,253 },{ 2526,253 },{ 2532,218 },{ 2533,218 },{ 2534,218 },{ 2536,172 },{ 2537,172 },{ 2538,172 },{ 2544,250 },{ 2545,250 },{ 2546,250 },{ 2548,253 },{ 2549,253 },{ 2550,253 },{ 2552,210 },{ 2553,210 },{ 2554,210 },{ 2556,253 },{ 2557,253 },{ 2558,253 },{ 2560,253 },{ 2561,253 },{ 2562,253 },{ 2568,115 },{ 2569,115 },{ 2570,115 },{ 2572,1 },{ 2573,1 },{ 2574,1 },{ 2576,4 },{ 2577,4 },{ 2578,4 },{ 2588,2 },{ 2589,2 },{ 2590,2 },{ 2592,1 },{ 2593,1 },{ 2594,1 },{ 2596,1 },{ 2597,1 },{ 2598,1 },{ 2608,2 },{ 2609,2 },{ 2610,2 },{ 2612,101 },{ 2613,101 },{ 2614,101 },{ 2616,254 },{ 2617,254 },{ 2618,254 },{ 2620,254 },{ 2621,254 },{ 2622,254 },{ 2624,254 },{ 2625,254 },{ 2626,254 },{ 2628,205 },{ 2629,205 },{ 2630,205 },{ 2636,253 },{ 2637,253 },{ 2638,253 },{ 2640,253 },{ 2641,253 },{ 2642,253 },{ 2644,254 },{ 2645,254 },{ 2646,254 },{ 2648,252 },{ 2649,252 },{ 2650,252 },{ 2708,254 },{ 2709,254 },{ 2710,254 },{ 2712,254 },{ 2713,254 },{ 2714,254 },{ 2716,253 },{ 2717,253 },{ 2718,253 },{ 2720,254 },{ 2721,254 },{ 2722,254 },{ 2724,248 },{ 2725,248 },{ 2726,248 },{ 2728,252 },{ 2729,252 },{ 2730,252 },{ 2732,254 },{ 2733,254 },{ 2734,254 },{ 2744,188 },{ 2745,188 },{ 2746,188 },{ 2748,239 },{ 2749,239 },{ 2750,239 },{ 2756,203 },{ 2757,203 },{ 2758,203 },{ 2776,5 },{ 2777,5 },{ 2778,5 },{ 2792,6 },{ 2793,6 },{ 2794,6 },{ 2804,2 },{ 2805,2 },{ 2806,2 },{ 2808,138 },{ 2809,138 },{ 2810,138 },{ 2812,252 },{ 2813,252 },{ 2814,252 },{ 2820,197 },{ 2821,197 },{ 2822,197 },{ 2828,253 },{ 2829,253 },{ 2830,253 },{ 2844,250 },{ 2845,250 },{ 2846,250 },{ 2900,254 },{ 2901,254 },{ 2902,254 },{ 2904,254 },{ 2905,254 },{ 2906,254 },{ 2908,253 },{ 2909,253 },{ 2910,253 },{ 2916,253 },{ 2917,253 },{ 2918,253 },{ 2920,252 },{ 2921,252 },{ 2922,252 },{ 2924,254 },{ 2925,254 },{ 2926,254 },{ 2932,251 },{ 2933,251 },{ 2934,251 },{ 2936,240 },{ 2937,240 },{ 2938,240 },{ 2940,5 },{ 2941,5 },{ 2942,5 },{ 2948,22 },{ 2949,22 },{ 2950,22 },{ 2952,4 },{ 2953,4 },{ 2954,4 },{ 2960,1 },{ 2961,1 },{ 2962,1 },{ 2992,3 },{ 2993,3 },{ 2994,3 },{ 3000,32 },{ 3001,32 },{ 3002,32 },{ 3008,1 },{ 3009,1 },{ 3010,1 },{ 3012,212 },{ 3013,212 },{ 3014,212 },{ 3020,253 },{ 3021,253 },{ 3022,253 },{ 3032,254 },{ 3033,254 },{ 3034,254 },{ 3096,254 },{ 3097,254 },{ 3098,254 },{ 3100,254 },{ 3101,254 },{ 3102,254 },{ 3104,253 },{ 3105,253 },{ 3106,253 },{ 3108,254 },{ 3109,254 },{ 3110,254 },{ 3112,253 },{ 3113,253 },{ 3114,253 },{ 3120,253 },{ 3121,253 },{ 3122,253 },{ 3124,254 },{ 3125,254 },{ 3126,254 },{ 3128,97 },{ 3129,97 },{ 3130,97 },{ 3132,2 },{ 3133,2 },{ 3134,2 },{ 3136,67 },{ 3137,67 },{ 3138,69 },{ 3140,2 },{ 3141,2 },{ 3142,4 },{ 3146,2 },{ 3148,3 },{ 3149,3 },{ 3150,5 },{ 3152,1 },{ 3153,1 },{ 3154,3 },{ 3156,3 },{ 3157,3 },{ 3158,5 },{ 3160,47 },{ 3161,47 },{ 3162,49 },{ 3164,72 },{ 3165,72 },{ 3166,74 },{ 3168,48 },{ 3169,48 },{ 3170,50 },{ 3172,78 },{ 3173,78 },{ 3174,80 },{ 3176,3 },{ 3177,3 },{ 3178,5 },{ 3182,2 },{ 3184,2 },{ 3185,2 },{ 3186,4 },{ 3188,1 },{ 3189,1 },{ 3190,3 },{ 3192,8 },{ 3193,8 },{ 3194,10 },{ 3196,93 },{ 3197,93 },{ 3198,95 },{ 3200,4 },{ 3201,4 },{ 3202,4 },{ 3204,129 },{ 3205,129 },{ 3206,129 },{ 3208,254 },{ 3209,254 },{ 3210,254 },{ 3212,253 },{ 3213,253 },{ 3214,253 },{ 3232,254 },{ 3233,254 },{ 3234,254 },{ 3288,254 },{ 3289,254 },{ 3290,254 },{ 3292,254 },{ 3293,254 },{ 3294,254 },{ 3300,252 },{ 3301,252 },{ 3302,252 },{ 3308,253 },{ 3309,253 },{ 3310,253 },{ 3312,252 },{ 3313,252 },{ 3314,252 },{ 3320,50 },{ 3321,50 },{ 3322,50 },{ 3324,2 },{ 3325,2 },{ 3326,2 },{ 3330,2 },{ 3332,3 },{ 3333,3 },{ 3334,5 },{ 3338,2 },{ 3342,2 },{ 3346,2 },{ 3348,74 },{ 3349,74 },{ 3350,76 },{ 3352,252 },{ 3353,252 },{ 3354,254 },{ 3356,247 },{ 3357,247 },{ 3358,249 },{ 3360,239 },{ 3361,239 },{ 3362,241 },{ 3364,250 },{ 3365,250 },{ 3366,252 },{ 3368,62 },{ 3369,62 },{ 3370,64 },{ 3372,2 },{ 3373,2 },{ 3374,4 },{ 3378,2 },{ 3382,2 },{ 3384,4 },{ 3385,4 },{ 3386,6 },{ 3390,2 },{ 3392,2 },{ 3393,2 },{ 3394,2 },{ 3396,37 },{ 3397,37 },{ 3398,37 },{ 3404,254 },{ 3405,254 },{ 3406,254 },{ 3408,252 },{ 3409,252 },{ 3410,252 },{ 3412,253 },{ 3413,253 },{ 3414,253 },{ 3424,254 },{ 3425,254 },{ 3426,254 },{ 3480,254 },{ 3481,254 },{ 3482,254 },{ 3484,254 },{ 3485,254 },{ 3486,254 },{ 3492,251 },{ 3493,251 },{ 3494,251 },{ 3500,251 },{ 3501,251 },{ 3502,251 },{ 3504,147 },{ 3505,147 },{ 3506,147 },{ 3508,120 },{ 3509,120 },{ 3510,120 },{ 3512,12 },{ 3513,12 },{ 3514,12 },{ 3520,4 },{ 3521,4 },{ 3522,6 },{ 3526,2 },{ 3528,1 },{ 3529,1 },{ 3530,3 },{ 3532,3 },{ 3533,3 },{ 3534,5 },{ 3536,1 },{ 3537,1 },{ 3538,1 },{ 3540,254 },{ 3541,254 },{ 3542,254 },{ 3560,250 },{ 3561,250 },{ 3562,250 },{ 3564,2 },{ 3565,2 },{ 3566,2 },{ 3570,2 },{ 3572,3 },{ 3573,3 },{ 3574,5 },{ 3578,2 },{ 3582,2 },{ 3588,33 },{ 3589,33 },{ 3590,33 },{ 3592,127 },{ 3593,127 },{ 3594,127 },{ 3596,165 },{ 3597,165 },{ 3598,165 },{ 3600,243 },{ 3601,243 },{ 3602,243 },{ 3608,252 },{ 3609,252 },{ 3610,252 },{ 3616,254 },{ 3617,254 },{ 3618,254 },{ 3620,254 },{ 3621,254 },{ 3622,254 },{ 3672,254 },{ 3673,254 },{ 3674,254 },{ 3676,254 },{ 3677,254 },{ 3678,254 },{ 3680,254 },{ 3681,254 },{ 3682,254 },{ 3688,218 },{ 3689,218 },{ 3690,218 },{ 3692,92 },{ 3693,92 },{ 3694,92 },{ 3696,2 },{ 3697,2 },{ 3698,2 },{ 3700,15 },{ 3701,15 },{ 3702,15 },{ 3704,2 },{ 3705,2 },{ 3706,2 },{ 3708,5 },{ 3709,5 },{ 3710,5 },{ 3712,4 },{ 3713,4 },{ 3714,6 },{ 3716,2 },{ 3717,2 },{ 3718,4 },{ 3722,2 },{ 3724,1 },{ 3725,1 },{ 3726,3 },{ 3736,254 },{ 3737,254 },{ 3738,254 },{ 3740,251 },{ 3741,251 },{ 3742,251 },{ 3744,254 },{ 3745,254 },{ 3746,254 },{ 3748,254 },{ 3749,254 },{ 3750,254 },{ 3756,2 },{ 3757,2 },{ 3758,2 },{ 3760,2 },{ 3761,2 },{ 3762,4 },{ 3764,2 },{ 3765,2 },{ 3766,4 },{ 3770,2 },{ 3772,2 },{ 3773,2 },{ 3774,4 },{ 3776,2 },{ 3777,2 },{ 3778,2 },{ 3780,6 },{ 3781,6 },{ 3782,6 },{ 3784,2 },{ 3785,2 },{ 3786,2 },{ 3788,66 },{ 3789,66 },{ 3790,66 },{ 3792,146 },{ 3793,146 },{ 3794,146 },{ 3796,236 },{ 3797,236 },{ 3798,236 },{ 3804,254 },{ 3805,254 },{ 3806,254 },{ 3808,254 },{ 3809,254 },{ 3810,254 },{ 3812,254 },{ 3813,254 },{ 3814,254 },{ 3864,254 },{ 3865,254 },{ 3866,254 },{ 3868,254 },{ 3869,254 },{ 3870,254 },{ 3872,251 },{ 3873,251 },{ 3874,251 },{ 3880,224 },{ 3881,224 },{ 3882,224 },{ 3884,106 },{ 3885,106 },{ 3886,106 },{ 3888,178 },{ 3889,178 },{ 3890,178 },{ 3892,43 },{ 3893,43 },{ 3894,43 },{ 3906,2 },{ 3908,1 },{ 3909,1 },{ 3910,3 },{ 3912,2 },{ 3913,2 },{ 3914,4 },{ 3916,1 },{ 3917,1 },{ 3918,3 },{ 3920,32 },{ 3921,32 },{ 3922,32 },{ 3928,254 },{ 3929,254 },{ 3930,254 },{ 3936,253 },{ 3937,253 },{ 3938,253 },{ 3944,251 },{ 3945,251 },{ 3946,251 },{ 3948,146 },{ 3949,146 },{ 3950,146 },{ 3954,2 },{ 3956,3 },{ 3957,3 },{ 3958,5 },{ 3960,3 },{ 3961,3 },{ 3962,5 },{ 3966,2 },{ 3968,1 },{ 3969,1 },{ 3970,1 },{ 3976,32 },{ 3977,32 },{ 3978,32 },{ 3980,169 },{ 3981,169 },{ 3982,169 },{ 3984,127 },{ 3985,127 },{ 3986,127 },{ 3988,227 },{ 3989,227 },{ 3990,227 },{ 3996,250 },{ 3997,250 },{ 3998,250 },{ 4000,254 },{ 4001,254 },{ 4002,254 },{ 4004,254 },{ 4005,254 },{ 4006,254 },{ 4056,254 },{ 4057,254 },{ 4058,254 },{ 4060,254 },{ 4061,254 },{ 4062,254 },{ 4068,248 },{ 4069,248 },{ 4070,248 },{ 4076,253 },{ 4077,253 },{ 4078,253 },{ 4080,121 },{ 4081,121 },{ 4082,121 },{ 4084,65 },{ 4085,65 },{ 4086,65 },{ 4088,2 },{ 4089,2 },{ 4090,2 },{ 4092,2 },{ 4093,2 },{ 4094,2 },{ 4098,2 },{ 4100,3 },{ 4101,3 },{ 4102,5 },{ 4106,2 },{ 4110,2 },{ 4112,159 },{ 4113,159 },{ 4114,159 },{ 4116,252 },{ 4117,252 },{ 4118,252 },{ 4120,249 },{ 4121,249 },{ 4122,249 },{ 4132,252 },{ 4133,252 },{ 4134,252 },{ 4136,253 },{ 4137,253 },{ 4138,253 },{ 4140,170 },{ 4141,170 },{ 4142,170 },{ 4146,2 },{ 4150,2 },{ 4152,2 },{ 4153,2 },{ 4154,4 },{ 4156,1 },{ 4157,1 },{ 4158,3 },{ 4168,65 },{ 4169,65 },{ 4170,65 },{ 4172,125 },{ 4173,125 },{ 4174,125 },{ 4176,215 },{ 4177,215 },{ 4178,215 },{ 4184,254 },{ 4185,254 },{ 4186,254 },{ 4192,254 },{ 4193,254 },{ 4194,254 },{ 4196,254 },{ 4197,254 },{ 4198,254 },{ 4200,254 },{ 4201,254 },{ 4202,254 },{ 4248,254 },{ 4249,254 },{ 4250,254 },{ 4252,254 },{ 4253,254 },{ 4254,254 },{ 4256,254 },{ 4257,254 },{ 4258,254 },{ 4260,254 },{ 4261,254 },{ 4262,254 },{ 4268,195 },{ 4269,195 },{ 4270,195 },{ 4272,21 },{ 4273,21 },{ 4274,21 },{ 4276,4 },{ 4277,4 },{ 4278,4 },{ 4280,4 },{ 4281,4 },{ 4282,4 },{ 4288,5 },{ 4289,5 },{ 4290,5 },{ 4300,1 },{ 4301,1 },{ 4302,1 },{ 4304,133 },{ 4305,133 },{ 4306,133 },{ 4308,253 },{ 4309,253 },{ 4310,253 },{ 4316,252 },{ 4317,252 },{ 4318,252 },{ 4320,252 },{ 4321,252 },{ 4322,252 },{ 4324,254 },{ 4325,254 },{ 4326,254 },{ 4328,253 },{ 4329,253 },{ 4330,253 },{ 4332,143 },{ 4333,143 },{ 4334,143 },{ 4340,1 },{ 4341,1 },{ 4342,1 },{ 4352,2 },{ 4353,2 },{ 4354,2 },{ 4356,1 },{ 4357,1 },{ 4358,1 },{ 4360,6 },{ 4361,6 },{ 4362,6 },{ 4364,23 },{ 4365,23 },{ 4366,23 },{ 4368,167 },{ 4369,167 },{ 4370,167 },{ 4372,253 },{ 4373,253 },{ 4374,253 },{ 4376,250 },{ 4377,250 },{ 4378,250 },{ 4380,252 },{ 4381,252 },{ 4382,252 },{ 4384,254 },{ 4385,254 },{ 4386,254 },{ 4388,254 },{ 4389,254 },{ 4390,254 },{ 4392,254 },{ 4393,254 },{ 4394,254 },{ 4440,254 },{ 4441,254 },{ 4442,254 },{ 4444,254 },{ 4445,254 },{ 4446,254 },{ 4452,254 },{ 4453,254 },{ 4454,254 },{ 4456,253 },{ 4457,253 },{ 4458,253 },{ 4464,67 },{ 4465,67 },{ 4466,67 },{ 4472,3 },{ 4473,3 },{ 4474,3 },{ 4484,1 },{ 4485,1 },{ 4486,1 },{ 4488,4 },{ 4489,4 },{ 4490,4 },{ 4496,177 },{ 4497,177 },{ 4498,177 },{ 4504,249 },{ 4505,249 },{ 4506,249 },{ 4516,253 },{ 4517,253 },{ 4518,253 },{ 4524,182 },{ 4525,182 },{ 4526,182 },{ 4528,1 },{ 4529,1 },{ 4530,1 },{ 4532,1 },{ 4533,1 },{ 4534,1 },{ 4536,2 },{ 4537,2 },{ 4538,2 },{ 4544,1 },{ 4545,1 },{ 4546,1 },{ 4556,42 },{ 4557,42 },{ 4558,42 },{ 4560,235 },{ 4561,235 },{ 4562,235 },{ 4568,253 },{ 4569,253 },{ 4570,253 },{ 4576,253 },{ 4577,253 },{ 4578,253 },{ 4580,254 },{ 4581,254 },{ 4582,254 },{ 4584,254 },{ 4585,254 },{ 4586,254 },{ 4644,254 },{ 4645,254 },{ 4646,254 },{ 4656,235 },{ 4657,235 },{ 4658,235 },{ 4660,28 },{ 4661,28 },{ 4662,28 },{ 4668,1 },{ 4669,1 },{ 4670,1 },{ 4672,2 },{ 4673,4 },{ 4674,3 },{ 4677,1 },{ 4680,189 },{ 4681,189 },{ 4682,189 },{ 4688,254 },{ 4689,254 },{ 4690,254 },{ 4696,251 },{ 4697,251 },{ 4698,251 },{ 4704,253 },{ 4705,253 },{ 4706,253 },{ 4712,253 },{ 4713,253 },{ 4714,253 },{ 4716,239 },{ 4717,239 },{ 4718,239 },{ 4724,172 },{ 4725,172 },{ 4726,172 },{ 4728,4 },{ 4729,6 },{ 4730,5 },{ 4733,1 },{ 4736,2 },{ 4737,2 },{ 4738,2 },{ 4744,40 },{ 4745,40 },{ 4746,40 },{ 4748,223 },{ 4749,223 },{ 4750,223 },{ 4752,250 },{ 4753,250 },{ 4754,250 },{ 4764,254 },{ 4765,254 },{ 4766,254 },{ 4844,253 },{ 4845,253 },{ 4846,253 },{ 4852,215 },{ 4853,215 },{ 4854,215 },{ 4856,86 },{ 4857,86 },{ 4858,86 },{ 4865,1 },{ 4869,1 },{ 4872,250 },{ 4873,250 },{ 4874,250 },{ 4880,251 },{ 4881,251 },{ 4882,251 },{ 4892,250 },{ 4893,250 },{ 4894,250 },{ 4896,253 },{ 4897,253 },{ 4898,253 },{ 4900,252 },{ 4901,252 },{ 4902,252 },{ 4904,254 },{ 4905,254 },{ 4906,254 },{ 4908,253 },{ 4909,253 },{ 4910,253 },{ 4912,253 },{ 4913,253 },{ 4914,253 },{ 4921,2 },{ 4922,1 },{ 4925,1 },{ 4932,89 },{ 4933,89 },{ 4934,89 },{ 4936,183 },{ 4937,183 },{ 4938,183 },{ 4948,252 },{ 4949,252 },{ 4950,252 },{ 4956,253 },{ 4957,253 },{ 4958,253 },{ 5024,253 },{ 5025,253 },{ 5026,253 },{ 5036,254 },{ 5037,254 },{ 5038,254 },{ 5040,254 },{ 5041,254 },{ 5042,254 },{ 5044,251 },{ 5045,251 },{ 5046,251 },{ 5048,234 },{ 5049,234 },{ 5050,234 },{ 5052,127 },{ 5053,127 },{ 5054,127 },{ 5058,2 },{ 5060,3 },{ 5061,4 },{ 5062,6 },{ 5064,253 },{ 5065,254 },{ 5068,252 },{ 5069,253 },{ 5076,253 },{ 5077,253 },{ 5078,253 },{ 5080,253 },{ 5081,253 },{ 5082,253 },{ 5084,252 },{ 5085,252 },{ 5086,252 },{ 5088,253 },{ 5089,253 },{ 5090,253 },{ 5092,252 },{ 5093,252 },{ 5094,252 },{ 5100,250 },{ 5101,250 },{ 5102,250 },{ 5104,253 },{ 5105,254 },{ 5108,254 },{ 5114,2 },{ 5116,4 },{ 5117,5 },{ 5118,7 },{ 5120,92 },{ 5121,92 },{ 5122,92 },{ 5124,253 },{ 5125,253 },{ 5126,253 },{ 5128,253 },{ 5129,253 },{ 5130,253 },{ 5132,251 },{ 5133,251 },{ 5134,251 },{ 5140,254 },{ 5141,254 },{ 5142,254 },{ 5224,249 },{ 5225,249 },{ 5226,249 },{ 5232,253 },{ 5233,253 },{ 5234,253 },{ 5244,125 },{ 5245,125 },{ 5246,125 },{ 5250,2 },{ 5253,1 },{ 5254,2 },{ 5256,90 },{ 5257,91 },{ 5258,93 },{ 5260,254 },{ 5264,250 },{ 5265,250 },{ 5266,250 },{ 5268,252 },{ 5269,252 },{ 5270,252 },{ 5280,254 },{ 5281,254 },{ 5282,254 },{ 5284,253 },{ 5285,253 },{ 5286,253 },{ 5288,251 },{ 5289,251 },{ 5290,251 },{ 5292,252 },{ 5293,252 },{ 5294,252 },{ 5296,254 },{ 5300,150 },{ 5301,151 },{ 5302,153 },{ 5305,4 },{ 5306,5 },{ 5310,2 },{ 5312,50 },{ 5313,50 },{ 5314,50 },{ 5320,250 },{ 5321,250 },{ 5322,250 },{ 5328,253 },{ 5329,253 },{ 5330,253 },{ 5336,253 },{ 5337,253 },{ 5338,253 },{ 5420,253 },{ 5421,253 },{ 5422,253 },{ 5424,247 },{ 5425,247 },{ 5426,247 },{ 5428,252 },{ 5429,252 },{ 5430,252 },{ 5442,2 },{ 5444,76 },{ 5445,80 },{ 5446,81 },{ 5448,9 },{ 5449,13 },{ 5450,14 },{ 5452,180 },{ 5453,181 },{ 5454,183 },{ 5456,251 },{ 5457,253 },{ 5458,252 },{ 5460,253 },{ 5462,254 },{ 5464,253 },{ 5465,253 },{ 5466,253 },{ 5468,254 },{ 5469,254 },{ 5470,254 },{ 5472,254 },{ 5473,254 },{ 5474,254 },{ 5476,254 },{ 5477,254 },{ 5478,254 },{ 5480,254 },{ 5484,254 },{ 5488,204 },{ 5489,205 },{ 5490,207 },{ 5492,1 },{ 5493,5 },{ 5494,6 },{ 5496,27 },{ 5497,31 },{ 5498,32 },{ 5502,2 },{ 5516,251 },{ 5517,251 },{ 5518,251 },{ 5524,253 },{ 5525,253 },{ 5526,253 },{ 5600,254 },{ 5601,254 },{ 5602,254 },{ 5604,252 },{ 5605,252 },{ 5606,252 },{ 5620,250 },{ 5621,250 },{ 5622,250 },{ 5629,1 },{ 5633,1 },{ 5634,2 },{ 5636,217 },{ 5637,221 },{ 5638,222 },{ 5640,201 },{ 5641,205 },{ 5642,206 },{ 5644,36 },{ 5645,40 },{ 5646,41 },{ 5648,108 },{ 5649,110 },{ 5650,109 },{ 5652,254 },{ 5656,251 },{ 5657,251 },{ 5658,251 },{ 5664,252 },{ 5665,252 },{ 5666,252 },{ 5668,254 },{ 5669,254 },{ 5670,254 },{ 5672,254 },{ 5676,157 },{ 5677,159 },{ 5678,158 },{ 5680,39 },{ 5681,43 },{ 5682,44 },{ 5684,179 },{ 5685,183 },{ 5686,184 },{ 5688,221 },{ 5689,225 },{ 5690,226 },{ 5692,29 },{ 5693,33 },{ 5694,34 },{ 5697,1 },{ 5700,245 },{ 5701,245 },{ 5702,245 },{ 5704,253 },{ 5705,253 },{ 5706,253 },{ 5708,253 },{ 5709,253 },{ 5710,253 },{ 5720,252 },{ 5721,252 },{ 5722,252 },{ 5804,252 },{ 5805,252 },{ 5806,252 },{ 5808,254 },{ 5809,254 },{ 5810,254 },{ 5816,249 },{ 5817,249 },{ 5818,249 },{ 5821,2 },{ 5822,1 },{ 5825,1 },{ 5826,2 },{ 5828,85 },{ 5829,91 },{ 5830,91 },{ 5832,227 },{ 5833,231 },{ 5834,232 },{ 5836,91 },{ 5837,95 },{ 5838,96 },{ 5841,1 },{ 5842,3 },{ 5846,2 },{ 5848,173 },{ 5849,174 },{ 5850,176 },{ 5852,253 },{ 5853,254 },{ 5856,254 },{ 5860,179 },{ 5861,180 },{ 5862,182 },{ 5865,1 },{ 5866,3 },{ 5868,2 },{ 5869,3 },{ 5870,5 },{ 5872,109 },{ 5873,113 },{ 5874,114 },{ 5876,236 },{ 5877,240 },{ 5878,241 },{ 5880,93 },{ 5881,99 },{ 5882,99 },{ 5885,1 },{ 5886,2 },{ 5889,1 },{ 5896,254 },{ 5897,254 },{ 5898,254 },{ 5900,252 },{ 5901,252 },{ 5902,252 },{ 5908,254 },{ 5909,254 },{ 5910,254 },{ 5992,252 },{ 5993,252 },{ 5994,252 },{ 6004,248 },{ 6005,248 },{ 6006,248 },{ 6012,139 },{ 6013,141 },{ 6014,140 },{ 6017,3 },{ 6018,4 },{ 6021,1 },{ 6022,2 },{ 6025,1 },{ 6026,2 },{ 6029,1 },{ 6030,3 },{ 6032,2 },{ 6033,3 },{ 6034,5 },{ 6036,18 },{ 6037,19 },{ 6038,21 },{ 6040,224 },{ 6041,225 },{ 6042,227 },{ 6044,254 },{ 6048,254 },{ 6052,254 },{ 6056,29 },{ 6057,30 },{ 6058,32 },{ 6061,1 },{ 6062,3 },{ 6064,25 },{ 6065,26 },{ 6066,28 },{ 6068,24 },{ 6069,28 },{ 6070,29 },{ 6073,1 },{ 6074,2 },{ 6077,4 },{ 6078,5 },{ 6080,114 },{ 6081,116 },{ 6082,115 },{ 6084,251 },{ 6085,251 },{ 6086,251 },{ 6096,254 },{ 6097,254 },{ 6098,254 },{ 6104,254 },{ 6105,254 },{ 6106,254 },{ 6188,254 },{ 6189,254 },{ 6190,254 },{ 6200,250 },{ 6201,250 },{ 6202,250 },{ 6204,243 },{ 6205,243 },{ 6206,243 },{ 6208,23 },{ 6209,25 },{ 6210,24 },{ 6213,1 },{ 6217,1 },{ 6228,148 },{ 6229,148 },{ 6230,148 },{ 6232,253 },{ 6233,253 },{ 6234,253 },{ 6240,254 },{ 6241,254 },{ 6242,254 },{ 6248,121 },{ 6249,121 },{ 6250,121 },{ 6252,3 },{ 6253,3 },{ 6254,3 },{ 6261,2 },{ 6262,1 },{ 6264,1 },{ 6265,3 },{ 6266,2 },{ 6268,22 },{ 6269,24 },{ 6270,23 },{ 6272,237 },{ 6273,237 },{ 6274,237 },{ 6276,250 },{ 6277,250 },{ 6278,250 },{ 6280,254 },{ 6281,254 },{ 6282,254 },{ 6284,252 },{ 6285,252 },{ 6286,252 },{ 6296,254 },{ 6297,254 },{ 6298,254 },{ 6368,254 },{ 6369,254 },{ 6370,254 },{ 6380,254 },{ 6381,254 },{ 6382,254 },{ 6392,254 },{ 6393,254 },{ 6394,254 },{ 6396,250 },{ 6397,250 },{ 6398,250 },{ 6404,92 },{ 6405,92 },{ 6406,92 },{ 6408,39 },{ 6409,39 },{ 6410,39 },{ 6412,78 },{ 6413,78 },{ 6414,78 },{ 6416,233 },{ 6417,233 },{ 6418,233 },{ 6428,252 },{ 6429,252 },{ 6430,252 },{ 6432,251 },{ 6433,251 },{ 6434,251 },{ 6440,254 },{ 6441,254 },{ 6442,254 },{ 6444,186 },{ 6445,186 },{ 6446,186 },{ 6448,59 },{ 6449,59 },{ 6450,59 },{ 6452,33 },{ 6453,33 },{ 6454,33 },{ 6456,63 },{ 6457,63 },{ 6458,63 },{ 6460,214 },{ 6461,214 },{ 6462,214 },{ 6464,248 },{ 6465,248 },{ 6466,248 },{ 6476,252 },{ 6477,252 },{ 6478,252 },{ 6488,253 },{ 6489,253 },{ 6490,253 },{ 6572,254 },{ 6573,254 },{ 6574,254 },{ 6576,253 },{ 6577,253 },{ 6578,253 },{ 6580,254 },{ 6581,254 },{ 6582,254 },{ 6584,254 },{ 6585,254 },{ 6586,254 },{ 6588,252 },{ 6589,252 },{ 6590,252 },{ 6596,251 },{ 6597,251 },{ 6598,251 },{ 6608,249 },{ 6609,249 },{ 6610,249 },{ 6612,250 },{ 6613,250 },{ 6614,250 },{ 6620,254 },{ 6621,254 },{ 6622,254 },{ 6628,253 },{ 6629,253 },{ 6630,253 },{ 6632,253 },{ 6633,253 },{ 6634,253 },{ 6636,253 },{ 6637,253 },{ 6638,253 },{ 6640,254 },{ 6641,254 },{ 6642,254 },{ 6656,254 },{ 6657,254 },{ 6658,254 },{ 6664,254 },{ 6665,254 },{ 6666,254 },{ 6668,252 },{ 6669,252 },{ 6670,252 },{ 6680,254 },{ 6681,254 },{ 6682,254 },{ 6764,253 },{ 6765,253 },{ 6766,253 },{ 6768,252 },{ 6769,252 },{ 6770,252 },{ 6772,252 },{ 6773,252 },{ 6774,252 },{ 6776,252 },{ 6777,252 },{ 6778,252 },{ 6780,252 },{ 6781,252 },{ 6782,252 },{ 6792,248 },{ 6793,248 },{ 6794,248 },{ 6796,254 },{ 6797,254 },{ 6798,254 },{ 6804,252 },{ 6805,252 },{ 6806,252 },{ 6808,251 },{ 6809,251 },{ 6810,251 },{ 6812,253 },{ 6813,253 },{ 6814,253 },{ 6816,247 },{ 6817,247 },{ 6818,247 },{ 6820,250 },{ 6821,250 },{ 6822,250 },{ 6828,250 },{ 6829,250 },{ 6830,250 },{ 6836,254 },{ 6837,254 },{ 6838,254 },{ 6840,252 },{ 6841,252 },{ 6842,252 },{ 6848,253 },{ 6849,253 },{ 6850,253 },{ 6852,254 },{ 6853,254 },{ 6854,254 },{ 6856,252 },{ 6857,252 },{ 6858,252 },{ 6860,253 },{ 6861,253 },{ 6862,253 },{ 6872,254 },{ 6873,254 },{ 6874,254 },{ 6956,254 },{ 6957,254 },{ 6958,254 },{ 6960,254 },{ 6961,254 },{ 6962,254 },{ 6976,254 },{ 6977,254 },{ 6978,254 },{ 6980,253 },{ 6981,253 },{ 6982,253 },{ 6988,253 },{ 6989,253 },{ 6990,253 },{ 6992,251 },{ 6993,251 },{ 6994,251 },{ 7004,254 },{ 7005,254 },{ 7006,254 },{ 7016,249 },{ 7017,249 },{ 7018,249 },{ 7020,254 },{ 7021,254 },{ 7022,254 },{ 7064,254 },{ 7065,254 },{ 7066,254 },{ 7172,252 },{ 7173,252 },{ 7174,252 },{ 7180,254 },{ 7181,254 },{ 7182,254 },{ 7188,250 },{ 7189,250 },{ 7190,250 },{ 7196,254 },{ 7197,254 },{ 7198,254 },{ 7200,252 },{ 7201,252 },{ 7202,252 },{ 7204,253 },{ 7205,253 },{ 7206,253 },{ 7212,254 },{ 7213,254 },{ 7214,254 },{ 7216,254 },{ 7217,254 },{ 7218,254 },{ 7220,251 },{ 7221,251 },{ 7222,251 },{ 7252,254 },{ 7253,254 },{ 7254,254 },{ 7368,250 },{ 7369,250 },{ 7370,250 },{ 7376,253 },{ 7377,253 },{ 7378,253 },{ 7380,254 },{ 7381,254 },{ 7382,254 },{ 7388,253 },{ 7389,253 },{ 7390,253 },{ 7392,252 },{ 7393,252 },{ 7394,252 },{ 7400,253 },{ 7401,253 },{ 7402,253 },{ 7408,254 },{ 7409,254 },{ 7410,254 },{ 7416,253 },{ 7417,253 },{ 7418,253 },{ 7424,254 },{ 7425,254 },{ 7426,254 },{ 7428,254 },{ 7429,254 },{ 7430,254 },{ 7432,254 },{ 7433,254 },{ 7434,254 },{ 7436,254 },{ 7437,254 },{ 7438,254 },{ 7440,253 },{ 7441,253 },{ 7442,253 },{ 7556,252 },{ 7557,252 },{ 7558,252 },{ 7564,251 },{ 7565,251 },{ 7566,251 },{ 7568,253 },{ 7569,253 },{ 7570,253 },{ 7576,252 },{ 7577,252 },{ 7578,252 },{ 7580,253 },{ 7581,253 },{ 7582,253 },{ 7588,251 },{ 7589,251 },{ 7590,251 },{ 7600,252 },{ 7601,252 },{ 7602,252 },{ 7604,254 },{ 7605,254 },{ 7606,254 },{ 7632,254 },{ 7633,254 },{ 7634,254 },{ 7644,252 },{ 7645,252 },{ 7646,252 },{ 7744,254 },{ 7745,254 },{ 7746,254 },{ 7748,254 },{ 7749,254 },{ 7750,254 },{ 7756,253 },{ 7757,253 },{ 7758,253 },{ 7760,254 },{ 7761,254 },{ 7762,254 },{ 7768,251 },{ 7769,251 },{ 7770,251 },{ 7784,254 },{ 7785,254 },{ 7786,254 },{ 7788,252 },{ 7789,252 },{ 7790,252 },{ 7796,252 },{ 7797,252 },{ 7798,252 },{ 7800,253 },{ 7801,253 },{ 7802,253 },{ 7936,254 },{ 7937,254 },{ 7938,254 },{ 7944,248 },{ 7945,248 },{ 7946,248 },{ 7948,254 },{ 7949,254 },{ 7950,254 },{ 7952,253 },{ 7953,253 },{ 7954,253 },{ 7960,253 },{ 7961,253 },{ 7962,253 },{ 7968,195 },{ 7969,195 },{ 7970,195 },{ 7972,138 },{ 7973,138 },{ 7974,138 },{ 7976,199 },{ 7977,199 },{ 7978,199 },{ 7980,248 },{ 7981,248 },{ 7982,248 },{ 7984,253 },{ 7985,253 },{ 7986,253 },{ 7988,253 },{ 7989,253 },{ 7990,253 },{ 7992,252 },{ 7993,252 },{ 7994,252 },{ 7996,254 },{ 7997,254 },{ 7998,254 },{ 8132,251 },{ 8133,251 },{ 8134,251 },{ 8140,251 },{ 8141,251 },{ 8142,251 },{ 8164,1 },{ 8165,1 },{ 8166,1 },{ 8180,254 },{ 8181,254 },{ 8182,254 },{ 8328,250 },{ 8329,250 },{ 8330,250 },{ 8332,254 },{ 8333,254 },{ 8334,254 },{ 8340,122 },{ 8341,122 },{ 8342,122 },{ 8348,3 },{ 8349,3 },{ 8350,3 },{ 8352,2 },{ 8353,2 },{ 8354,2 },{ 8360,35 },{ 8361,35 },{ 8362,35 },{ 8364,252 },{ 8365,252 },{ 8366,252 },{ 8376,254 },{ 8377,254 },{ 8378,254 },{ 8380,253 },{ 8381,253 },{ 8382,253 },{ 8512,254 },{ 8513,254 },{ 8514,254 },{ 8524,253 },{ 8525,253 },{ 8526,253 },{ 8528,252 },{ 8529,252 },{ 8530,252 },{ 8532,250 },{ 8533,250 },{ 8534,250 },{ 8536,65 },{ 8537,65 },{ 8538,65 },{ 8548,57 },{ 8549,57 },{ 8550,57 },{ 8552,224 },{ 8553,224 },{ 8554,224 },{ 8560,250 },{ 8561,250 },{ 8562,250 },{ 8712,250 },{ 8713,250 },{ 8714,250 },{ 8732,254 },{ 8733,254 },{ 8734,254 },{ 8748,250 },{ 8749,250 },{ 8750,250 },{ 8752,254 },{ 8753,254 },{ 8754,254 },{ 8756,253 },{ 8757,253 },{ 8758,253 },{ 8760,254 },{ 8761,254 },{ 8762,254 },{ 8904,252 },{ 8905,252 },{ 8906,252 },{ 8908,254 },{ 8909,254 },{ 8910,254 },{ 8912,252 },{ 8913,252 },{ 8914,252 },{ 8920,253 },{ 8921,253 },{ 8922,253 },{ 8924,253 },{ 8925,253 },{ 8926,253 },{ 8928,253 },{ 8929,253 },{ 8930,253 },{ 8932,253 },{ 8933,253 },{ 8934,253 },{ 8936,251 },{ 8937,251 },{ 8938,251 },{ 8948,252 },{ 8949,252 },{ 8950,252 },{ 8952,254 },{ 8953,254 },{ 8954,254 },{ 8956,254 },{ 8957,254 },{ 8958,254 },{ 9092,253 },{ 9093,253 },{ 9094,253 },{ 9104,253 },{ 9105,253 },{ 9106,253 },{ 9112,254 },{ 9113,254 },{ 9114,254 },{ 9124,254 },{ 9125,254 },{ 9126,254 },{ 9132,252 },{ 9133,252 },{ 9134,252 },{ 9136,252 },{ 9137,252 },{ 9138,252 },{ 9144,254 },{ 9145,254 },{ 9146,254 } };
	std::set<int> _black = { 704,705,706,828,829,830,896,897,898,900,901,902,1028,1029,1030,1076,1077,1078,1080,1081,1082,1092,1093,1094,1416,1417,1418,1460,1461,1462,1580,1581,1582,1652,1653,1654,1680,1681,1682,1804,1805,1806,1808,1809,1810,1816,1817,1818,1828,1829,1830,1836,1837,1838,1840,1841,1842,2004,2005,2006,2012,2013,2014,2016,2017,2018,2024,2025,2026,2032,2033,2034,2188,2189,2190,2192,2193,2194,2196,2197,2198,2216,2217,2218,2220,2221,2222,2384,2385,2386,2396,2397,2398,2404,2405,2406,2580,2581,2582,2584,2585,2586,2600,2601,2602,2604,2605,2606,2760,2761,2762,2764,2765,2766,2768,2769,2770,2772,2773,2774,2780,2781,2782,2784,2785,2786,2788,2789,2790,2796,2797,2798,2800,2801,2802,2956,2957,2958,2964,2965,2966,2968,2969,2970,2972,2973,2974,2976,2977,2978,2980,2981,2982,2984,2985,2986,2988,2989,2990,2996,2997,2998,3144,3145,3180,3181,3328,3329,3336,3337,3340,3341,3344,3345,3376,3377,3380,3381,3388,3389,3516,3517,3518,3524,3525,3568,3569,3576,3577,3580,3581,3584,3585,3586,3720,3721,3728,3729,3730,3768,3769,3896,3897,3898,3900,3901,3902,3904,3905,3952,3953,3964,3965,3972,3973,3974,4096,4097,4104,4105,4108,4109,4144,4145,4148,4149,4160,4161,4162,4164,4165,4166,4284,4285,4286,4292,4293,4294,4296,4297,4298,4336,4337,4338,4344,4345,4346,4348,4349,4350,4468,4469,4470,4476,4477,4478,4480,4481,4482,4492,4493,4494,4540,4541,4542,4548,4549,4550,4552,4553,4554,4664,4665,4666,4676,4678,4732,4734,4740,4741,4742,4860,4861,4862,4864,4866,4868,4870,4920,4924,4926,4928,4929,4930,5056,5057,5112,5113,5248,5249,5252,5304,5308,5309,5436,5437,5438,5440,5441,5500,5501,5504,5505,5506,5628,5630,5632,5696,5698,5820,5824,5840,5844,5845,5864,5884,5888,5890,6016,6020,6024,6028,6060,6072,6076,6212,6214,6216,6218,6220,6221,6222,6224,6225,6226,6256,6257,6258,6260,8148,8149,8150,8152,8153,8154,8156,8157,8158,8160,8161,8162,8168,8169,8170,8344,8345,8346,8356,8357,8358,8540,8541,8542,8544,8545,8546 };
#pragma endregion

	const int _size = 48 * 48 * 4;
	auto _pixels = (uint8_t*)malloc(_size * sizeof(uint8_t));
	if (!_pixels)
	{
		_logo_data.clear();
		_black.clear();
		logger.error("Could not allocate memory for texture of logo");
		return;
	}
	for (int i = 0; i < _size; ++i)
	{
		auto _iter = _logo_data.find(i);
		if (_iter != _logo_data.end())
		{
			_pixels[i] = _iter->second;
		}
		else
		{
			auto _b_iter = _black.find(i);
			if (_b_iter != _black.end())
			{
				//set black
				_pixels[i] = 0;
			}
			else
			{
				//set white
				_pixels[i] = 255;
			}
		}
	}
	_logo_data.clear();
	_black.clear();

	w_texture::default_texture = new w_texture();
	w_texture::default_texture->initialize(_gDevice, 48, 48, true);
	if (w_texture::default_texture->load_texture_from_memory_rgba(&_pixels[0]) == W_FAILED)
	{
		logger.error("Error on creating texture of logo from memory");
	}
	free(_pixels);
}

void w_graphics_device_manager::on_device_lost()
{
	logger.write(L"Graphics device is reseting");
	
	auto _windows_info = this->_pimp->get_output_windows_info();
	release();
	this->_pimp = new w_graphics_device_manager_pimp();
	initialize(_windows_info);
	
	logger.write(L"Graphics device reset successfully");
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
			V(W_FAILED, "getting dxgi device from d3d11device for graphics device: " + _gDevice->device_name + " ID:" + std::to_string(_gDevice->device_id) + , this->name, 2);
		}
		else
		{
			_dxgi_device->Trim();
		}
	}
#endif
}

void w_graphics_device_manager::on_window_resized(_In_ const uint32_t& pGraphicsDeviceIndex, _In_ const w_point& pNewSizeOfWindow)
{
	if (pGraphicsDeviceIndex >= this->graphics_devices.size()) return;

	auto _gDevice = this->graphics_devices.at(pGraphicsDeviceIndex);
	auto _window = &_gDevice->output_presentation_window;

	_window->width = static_cast<uint32_t>(pNewSizeOfWindow.x);
	_window->height =  static_cast<uint32_t>(pNewSizeOfWindow.y);

	_wait_for_previous_frame(_gDevice);

	_gDevice->_clean_swap_chain();
	this->_pimp->create_swap_chain(_gDevice);
}

void w_graphics_device_manager::_wait_for_previous_frame(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
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

#else
	vkDeviceWaitIdle(pGDevice->vk_device);
#endif
}

W_RESULT w_graphics_device_manager::prepare()
{
	if (!this->_pimp) return W_FAILED;
	
	auto _config = this->_pimp->get_graphics_device_manager_configs();
	if (_config.off_screen_mode) return W_PASSED;

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

        auto _output_window = &(_gDevice->output_presentation_window);

#ifdef __DX12__

        _wait_for_previous_frame(_gDevice, j);

#elif defined(__VULKAN__)
        auto _semaphore = _output_window->swap_chain_image_is_available_semaphore.get();
        auto _hr = vkAcquireNextImageKHR(_gDevice->vk_device,
            _output_window->vk_swap_chain,
            UINT64_MAX,
            *_semaphore,
            VK_NULL_HANDLE,
            &_output_window->swap_chain_image_index);

		if (_hr != VK_SUCCESS && _hr != VK_SUBOPTIMAL_KHR)
		{
			logger.error("error acquiring image of graphics device's swap chain : {}", i);
			release();
			std::exit(EXIT_FAILURE);
		}
#endif
    }

    return W_PASSED;
}

W_RESULT w_graphics_device_manager::present()
{
	if (!this->_pimp) return W_FAILED;

	auto _config = this->_pimp->get_graphics_device_manager_configs();
	if (_config.off_screen_mode) return W_PASSED;

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

        auto _present_window = &(_gDevice->output_presentation_window);

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
            return W_FAILED;
        }

#elif defined(__VULKAN__)
        VkPresentInfoKHR _present_info = {};

        _present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        _present_info.waitSemaphoreCount = 1;
        _present_info.pWaitSemaphores = _present_window->rendering_done_semaphore.get();
        _present_info.swapchainCount = 1;
        _present_info.pSwapchains = &_present_window->vk_swap_chain;
        _present_info.pImageIndices = &_present_window->swap_chain_image_index;

        auto _hr = vkQueuePresentKHR(_gDevice->vk_present_queue.queue,
            &_present_info);
        if (_hr)
        {
            if (_hr == VK_ERROR_OUT_OF_DATE_KHR || _hr == VK_SUBOPTIMAL_KHR)
            {
				//resize it agian
				w_point _new_win_size;
				_new_win_size.x = _present_window->width;
				_new_win_size.y = _present_window->height;
                on_window_resized(static_cast<uint32_t>(i), _new_win_size);
                return W_FAILED;
            }

            logger.error("error on presenting queue of graphics device: {}", _gDevice->get_info());
            release();
            std::exit(EXIT_FAILURE);
        }

        _hr = vkQueueWaitIdle(_gDevice->vk_present_queue.queue);
        if (_hr)
        {
            logger.error("error on wait idle queue of graphics device: {}", _gDevice->get_info());
            release();
            std::exit(EXIT_FAILURE);
        }
#endif
    }
    
    return W_PASSED;
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
        
#ifdef __DX12__
        //wait for all presentation windows to perform their tasks
		//for (size_t j = 0; j < _gDevice->output_presentation_windows.size(); ++j)
		//{
		//	_wait_for_previous_frame(_gDevice, j);
		//	//Present to avoid leak all references held by previous render
		//	present();
		//}
#endif
        
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
    
#if defined(__WIN32) && !defined(__PYTHON__)
	CoUninitialize();
#endif

    return _super::release();
}

#ifdef __VULKAN__
VkResult w_graphics_device_manager::memory_type_from_properties(
        VkPhysicalDeviceMemoryProperties pMemoryProperties,
        uint32_t pTypeBits,
        uint32_t pRequirementsFlags,
        uint32_t* pTypeIndex)
{
    // Search mem types to find first index with those properties
    for (uint32_t i = 0; i <  pMemoryProperties.memoryTypeCount; ++i)
    {
        if ((pTypeBits & 1) == 1) 
        {
            // Type is available, does it match user properties?
            if ((pMemoryProperties.memoryTypes[i].propertyFlags & pRequirementsFlags) == pRequirementsFlags)
            {
                *pTypeIndex =  i;
                return VkResult::VK_SUCCESS;
            }
        }
        pTypeBits >>= 1;
    }
    // No memory types matched, return failure
    return VkResult::VK_INCOMPLETE;
}

void w_graphics_device_manager::set_src_dst_masks_of_image_barrier(_Inout_ VkImageMemoryBarrier& pImageMemoryBarrier)
{
    //Set source access mask which controls actions that have to be finished on the old layout before it will be transitioned to the new layout
    switch (pImageMemoryBarrier.oldLayout)
    {
        default:
            break;
        case VK_IMAGE_LAYOUT_UNDEFINED:
            /*
                Image layout is undefined
                No flags required, listed only for completeness
            */
            pImageMemoryBarrier.srcAccessMask = 0;
            break;
            
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            /*
                Only valid as initial layout for linear images, preserves memory contents
                Make sure host writes have been finished
            */
            pImageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;
            
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            //Make sure any writes to the color buffer have been finished
            pImageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;
            
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            //Make sure any writes to the depth/stencil buffer have been finished
            pImageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;
            
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            //Make sure any reads from the image have been finished
            pImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;
            
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            // Image is a transfer destination
            // Make sure any writes to the image have been finished
            pImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
            
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            //Make sure any shader reads from the image have been finished
            pImageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
    }
    
    // Set destination access mask which controls the dependency for the new image layout
    switch (pImageMemoryBarrier.newLayout)
    {
        default:
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            /*
                Image will be used as a transfer destination
                Make sure any writes to the image have been finished
            */
            pImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
            
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            /*
                Image will be used as a transfer source
                Make sure any reads from the image have been finished
            */
            pImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;
            
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            //Make sure any writes to the color buffer have been finished
            pImageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;
            
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            //Make sure any writes to depth/stencil buffer have been finished
            pImageMemoryBarrier.dstAccessMask = pImageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;
            
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            //Make sure any writes to the image have been finished
            if (pImageMemoryBarrier.srcAccessMask == 0)
            {
                pImageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            }
            pImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
    }
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

std::shared_ptr<w_graphics_device> w_graphics_device_manager::get_graphics_device(_In_ const size_t& pGraphicsDeviceIndex) const
{
	return pGraphicsDeviceIndex < this->graphics_devices.size() ? this->graphics_devices.at(pGraphicsDeviceIndex) : nullptr;
}

const size_t w_graphics_device_manager::get_number_of_graphics_devices() const
{
	return this->graphics_devices.size();
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

#pragma endregion

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

#endif
