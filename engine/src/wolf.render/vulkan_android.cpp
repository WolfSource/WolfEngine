#include "vulkan_android.h"

#if defined(__ANDROID)

#include <android/log.h>
#include <dlfcn.h>

static void* _lib_vulkan = nullptr;

PFN_vkCreateInstance android::vkCreateInstance = nullptr;
PFN_vkGetDeviceProcAddr android::vkGetDeviceProcAddr = nullptr;
PFN_vkGetInstanceProcAddr android::vkGetInstanceProcAddr = nullptr;
PFN_vkCreateDevice android::vkCreateDevice = nullptr;
PFN_vkEnumeratePhysicalDevices android::vkEnumeratePhysicalDevices = nullptr;
PFN_vkGetPhysicalDeviceProperties android::vkGetPhysicalDeviceProperties = nullptr;
PFN_vkEnumerateDeviceExtensionProperties android::vkEnumerateDeviceExtensionProperties = nullptr;
PFN_vkEnumerateDeviceLayerProperties android::vkEnumerateDeviceLayerProperties = nullptr;
PFN_vkGetPhysicalDeviceFormatProperties android::vkGetPhysicalDeviceFormatProperties = nullptr;
PFN_vkGetPhysicalDeviceFeatures android::vkGetPhysicalDeviceFeatures = nullptr;
PFN_vkGetPhysicalDeviceQueueFamilyProperties android::vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
PFN_vkGetPhysicalDeviceMemoryProperties android::vkGetPhysicalDeviceMemoryProperties = nullptr;
PFN_vkEnumerateInstanceExtensionProperties android::vkEnumerateInstanceExtensionProperties = nullptr;
PFN_vkEnumerateInstanceLayerProperties android::vkEnumerateInstanceLayerProperties = nullptr;
PFN_vkCmdPipelineBarrier android::vkCmdPipelineBarrier = nullptr;
PFN_vkCreateShaderModule android::vkCreateShaderModule = nullptr;
PFN_vkCreateBuffer android::vkCreateBuffer = nullptr;
PFN_vkGetBufferMemoryRequirements android::vkGetBufferMemoryRequirements = nullptr;
PFN_vkMapMemory android::vkMapMemory = nullptr;
PFN_vkUnmapMemory android::vkUnmapMemory = nullptr;
PFN_vkFlushMappedMemoryRanges android::vkFlushMappedMemoryRanges = nullptr;
PFN_vkInvalidateMappedMemoryRanges android::vkInvalidateMappedMemoryRanges = nullptr;
PFN_vkBindBufferMemory android::vkBindBufferMemory = nullptr;
PFN_vkDestroyBuffer android::vkDestroyBuffer = nullptr;
PFN_vkAllocateMemory android::vkAllocateMemory = nullptr;
PFN_vkBindImageMemory android::vkBindImageMemory = nullptr;
PFN_vkGetImageSubresourceLayout android::vkGetImageSubresourceLayout = nullptr;
PFN_vkCmdCopyBuffer android::vkCmdCopyBuffer = nullptr;
PFN_vkCmdCopyBufferToImage android::vkCmdCopyBufferToImage = nullptr;
PFN_vkCmdCopyImage android::vkCmdCopyImage = nullptr;
PFN_vkCmdBlitImage android::vkCmdBlitImage = nullptr;
PFN_vkCmdClearAttachments android::vkCmdClearAttachments = nullptr;
PFN_vkCreateSampler android::vkCreateSampler = nullptr;
PFN_vkDestroySampler android::vkDestroySampler = nullptr;
PFN_vkDestroyImage android::vkDestroyImage = nullptr;
PFN_vkFreeMemory android::vkFreeMemory = nullptr;
PFN_vkCreateRenderPass android::vkCreateRenderPass = nullptr;
PFN_vkCmdBeginRenderPass android::vkCmdBeginRenderPass = nullptr;
PFN_vkCmdEndRenderPass android::vkCmdEndRenderPass = nullptr;
PFN_vkCmdExecuteCommands android::vkCmdExecuteCommands = nullptr;
PFN_vkCreateImage android::vkCreateImage = nullptr;
PFN_vkGetImageMemoryRequirements android::vkGetImageMemoryRequirements = nullptr;
PFN_vkCreateImageView android::vkCreateImageView = nullptr;
PFN_vkDestroyImageView android::vkDestroyImageView = nullptr;
PFN_vkCreateSemaphore android::vkCreateSemaphore = nullptr;
PFN_vkDestroySemaphore android::vkDestroySemaphore = nullptr;
PFN_vkCreateFence android::vkCreateFence = nullptr;
PFN_vkDestroyFence android::vkDestroyFence = nullptr;
PFN_vkWaitForFences android::vkWaitForFences = nullptr;
PFN_vkResetFences android::vkResetFences = nullptr;
PFN_vkCreateCommandPool android::vkCreateCommandPool = nullptr;
PFN_vkDestroyCommandPool android::vkDestroyCommandPool = nullptr;
PFN_vkAllocateCommandBuffers android::vkAllocateCommandBuffers = nullptr;
PFN_vkBeginCommandBuffer android::vkBeginCommandBuffer = nullptr;
PFN_vkEndCommandBuffer android::vkEndCommandBuffer = nullptr;
PFN_vkGetDeviceQueue android::vkGetDeviceQueue = nullptr;
PFN_vkQueueSubmit android::vkQueueSubmit = nullptr;
PFN_vkQueueWaitIdle android::vkQueueWaitIdle = nullptr;
PFN_vkDeviceWaitIdle android::vkDeviceWaitIdle = nullptr;
PFN_vkCreateFramebuffer android::vkCreateFramebuffer = nullptr;
PFN_vkCreatePipelineCache android::vkCreatePipelineCache = nullptr;
PFN_vkCreatePipelineLayout android::vkCreatePipelineLayout = nullptr;
PFN_vkCreateGraphicsPipelines android::vkCreateGraphicsPipelines = nullptr;
PFN_vkCreateComputePipelines android::vkCreateComputePipelines = nullptr;
PFN_vkCreateDescriptorPool android::vkCreateDescriptorPool = nullptr;
PFN_vkCreateDescriptorSetLayout android::vkCreateDescriptorSetLayout = nullptr;
PFN_vkAllocateDescriptorSets android::vkAllocateDescriptorSets = nullptr;
PFN_vkUpdateDescriptorSets android::vkUpdateDescriptorSets = nullptr;
PFN_vkCmdBindDescriptorSets android::vkCmdBindDescriptorSets = nullptr;
PFN_vkCmdBindPipeline android::vkCmdBindPipeline = nullptr;
PFN_vkCmdBindVertexBuffers android::vkCmdBindVertexBuffers = nullptr;
PFN_vkCmdBindIndexBuffer android::vkCmdBindIndexBuffer = nullptr;
PFN_vkCmdSetViewport android::vkCmdSetViewport = nullptr;
PFN_vkCmdSetScissor android::vkCmdSetScissor = nullptr;
PFN_vkCmdSetLineWidth android::vkCmdSetLineWidth = nullptr;
PFN_vkCmdSetDepthBias android::vkCmdSetDepthBias = nullptr;
PFN_vkCmdPushConstants android::vkCmdPushConstants = nullptr;
PFN_vkCmdDrawIndexed android::vkCmdDrawIndexed = nullptr;
PFN_vkCmdDraw android::vkCmdDraw = nullptr;
PFN_vkCmdDrawIndexedIndirect android::vkCmdDrawIndexedIndirect = nullptr;
PFN_vkCmdDrawIndirect android::vkCmdDrawIndirect = nullptr;
PFN_vkCmdDispatch android::vkCmdDispatch = nullptr;
PFN_vkDestroyPipeline android::vkDestroyPipeline = nullptr;
PFN_vkDestroyPipelineLayout android::vkDestroyPipelineLayout = nullptr;
PFN_vkDestroyDescriptorSetLayout android::vkDestroyDescriptorSetLayout = nullptr;
PFN_vkDestroyDevice android::vkDestroyDevice = nullptr;
PFN_vkDestroyInstance android::vkDestroyInstance = nullptr;
PFN_vkDestroyDescriptorPool android::vkDestroyDescriptorPool = nullptr;
PFN_vkFreeCommandBuffers android::vkFreeCommandBuffers = nullptr;
PFN_vkDestroyRenderPass android::vkDestroyRenderPass = nullptr;
PFN_vkDestroyFramebuffer android::vkDestroyFramebuffer = nullptr;
PFN_vkDestroyShaderModule android::vkDestroyShaderModule = nullptr;
PFN_vkDestroyPipelineCache android::vkDestroyPipelineCache = nullptr;
PFN_vkCreateQueryPool android::vkCreateQueryPool = nullptr;
PFN_vkDestroyQueryPool android::vkDestroyQueryPool = nullptr;
PFN_vkGetQueryPoolResults android::vkGetQueryPoolResults = nullptr;
PFN_vkCmdBeginQuery android::vkCmdBeginQuery = nullptr;
PFN_vkCmdEndQuery android::vkCmdEndQuery = nullptr;
PFN_vkCmdResetQueryPool android::vkCmdResetQueryPool = nullptr;
PFN_vkCmdCopyQueryPoolResults android::vkCmdCopyQueryPoolResults = nullptr;

PFN_vkCreateAndroidSurfaceKHR android::vkCreateAndroidSurfaceKHR = nullptr;
PFN_vkDestroySurfaceKHR android::vkDestroySurfaceKHR = nullptr;

PFN_vkDestroySwapchainKHR android::vkDestroySwapchainKHR = nullptr;

PFN_vkGetPhysicalDeviceSurfaceSupportKHR android::vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR android::vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR android::vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR android::vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;

PFN_vkCreateSwapchainKHR android::vkCreateSwapchainKHR = nullptr;
PFN_vkGetSwapchainImagesKHR android::vkGetSwapchainImagesKHR = nullptr;

//Dynamically load vulkan library and base function pointers
bool android::vk_load_library()
{
	__android_log_print(ANDROID_LOG_INFO, "wolf.engine", "loading libvulkan.so.\n");

	//load vulkan library
	_lib_vulkan = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
	if (!_lib_vulkan)
	{
		__android_log_print(ANDROID_LOG_INFO, "wolf.engine", "could not load vulkan library: %s!\n", dlerror());
		return false;
	}

	using namespace android;

	vkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(dlsym(_lib_vulkan, "vkEnumerateInstanceExtensionProperties"));
	vkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(dlsym(_lib_vulkan, "vkEnumerateInstanceLayerProperties"));
	vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(dlsym(_lib_vulkan, "vkCreateInstance"));
	vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(_lib_vulkan, "vkGetInstanceProcAddr"));
	vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(dlsym(_lib_vulkan, "vkGetDeviceProcAddr"));

	return true;
}

//Load instance based Vulkan function pointers
void android::vk_load_functions_of_instance(VkInstance pInstance)
{
	__android_log_print(ANDROID_LOG_INFO, "wolf.engine", "Loading instance based function pointers.\n");

	using namespace android;

	vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(vkGetInstanceProcAddr(pInstance, "vkEnumeratePhysicalDevices"));
	vkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(vkGetInstanceProcAddr(pInstance, "vkGetPhysicalDeviceProperties"));
	vkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(vkGetInstanceProcAddr(pInstance, "vkGetPhysicalDeviceMemoryProperties"));
	vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(vkGetInstanceProcAddr(pInstance, "vkGetPhysicalDeviceQueueFamilyProperties"));

	vkCreateAndroidSurfaceKHR = reinterpret_cast<PFN_vkCreateAndroidSurfaceKHR>(vkGetInstanceProcAddr(pInstance, "vkCreateAndroidSurfaceKHR"));

	vkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(vkGetInstanceProcAddr(pInstance, "vkCreateDevice"));
	
	/*vkEnumerateDeviceLayerProperties = reinterpret_cast<PFN_vkEnumerateDeviceLayerProperties>(vkGetInstanceProcAddr(pInstance, "vkEnumerateDeviceLayerProperties"));
	vkEnumerateDeviceExtensionProperties = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(vkGetInstanceProcAddr(pInstance, "vkEnumerateDeviceExtensionProperties"));
	vkGetPhysicalDeviceFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(vkGetInstanceProcAddr(pInstance, "vkGetPhysicalDeviceFeatures"));
	vkGetPhysicalDeviceFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties>(vkGetInstanceProcAddr(pInstance, "vkGetPhysicalDeviceFormatProperties"));

	vkCmdPipelineBarrier = reinterpret_cast<PFN_vkCmdPipelineBarrier>(vkGetInstanceProcAddr(pInstance, "vkCmdPipelineBarrier"));
	vkCreateShaderModule = reinterpret_cast<PFN_vkCreateShaderModule>(vkGetInstanceProcAddr(pInstance, "vkCreateShaderModule"));

	vkCreateBuffer = reinterpret_cast<PFN_vkCreateBuffer>(vkGetInstanceProcAddr(pInstance, "vkCreateBuffer"));
	vkGetBufferMemoryRequirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(vkGetInstanceProcAddr(pInstance, "vkGetBufferMemoryRequirements"));
	vkMapMemory = reinterpret_cast<PFN_vkMapMemory>(vkGetInstanceProcAddr(pInstance, "vkMapMemory"));
	vkUnmapMemory = reinterpret_cast<PFN_vkUnmapMemory>(vkGetInstanceProcAddr(pInstance, "vkUnmapMemory"));
	vkFlushMappedMemoryRanges = reinterpret_cast<PFN_vkFlushMappedMemoryRanges>(vkGetInstanceProcAddr(pInstance, "vkFlushMappedMemoryRanges"));
	vkInvalidateMappedMemoryRanges = reinterpret_cast<PFN_vkInvalidateMappedMemoryRanges>(vkGetInstanceProcAddr(pInstance, "vkInvalidateMappedMemoryRanges"));
	vkBindBufferMemory = reinterpret_cast<PFN_vkBindBufferMemory>(vkGetInstanceProcAddr(pInstance, "vkBindBufferMemory"));
	vkDestroyBuffer = reinterpret_cast<PFN_vkDestroyBuffer>(vkGetInstanceProcAddr(pInstance, "vkDestroyBuffer"));

	vkAllocateMemory = reinterpret_cast<PFN_vkAllocateMemory>(vkGetInstanceProcAddr(pInstance, "vkAllocateMemory"));
	vkFreeMemory = reinterpret_cast<PFN_vkFreeMemory>(vkGetInstanceProcAddr(pInstance, "vkFreeMemory"));
	vkCreateRenderPass = reinterpret_cast<PFN_vkCreateRenderPass>(vkGetInstanceProcAddr(pInstance, "vkCreateRenderPass"));
	vkCmdBeginRenderPass = reinterpret_cast<PFN_vkCmdBeginRenderPass>(vkGetInstanceProcAddr(pInstance, "vkCmdBeginRenderPass"));
	vkCmdEndRenderPass = reinterpret_cast<PFN_vkCmdEndRenderPass>(vkGetInstanceProcAddr(pInstance, "vkCmdEndRenderPass"));
	vkCmdExecuteCommands = reinterpret_cast<PFN_vkCmdExecuteCommands>(vkGetInstanceProcAddr(pInstance, "vkCmdExecuteCommands"));*/

	vkCreateImage = reinterpret_cast<PFN_vkCreateImage>(vkGetInstanceProcAddr(pInstance, "vkCreateImage"));
	vkGetImageMemoryRequirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(vkGetInstanceProcAddr(pInstance, "vkGetImageMemoryRequirements"));
	vkCreateImageView = reinterpret_cast<PFN_vkCreateImageView>(vkGetInstanceProcAddr(pInstance, "vkCreateImageView"));
	vkDestroyImageView = reinterpret_cast<PFN_vkDestroyImageView>(vkGetInstanceProcAddr(pInstance, "vkDestroyImageView"));
	vkBindImageMemory = reinterpret_cast<PFN_vkBindImageMemory>(vkGetInstanceProcAddr(pInstance, "vkBindImageMemory"));
	vkGetImageSubresourceLayout = reinterpret_cast<PFN_vkGetImageSubresourceLayout>(vkGetInstanceProcAddr(pInstance, "vkGetImageSubresourceLayout"));
	vkCmdCopyImage = reinterpret_cast<PFN_vkCmdCopyImage>(vkGetInstanceProcAddr(pInstance, "vkCmdCopyImage"));
	vkCmdBlitImage = reinterpret_cast<PFN_vkCmdBlitImage>(vkGetInstanceProcAddr(pInstance, "vkCmdBlitImage"));
	vkDestroyImage = reinterpret_cast<PFN_vkDestroyImage>(vkGetInstanceProcAddr(pInstance, "vkDestroyImage"));

	/*vkCmdClearAttachments = reinterpret_cast<PFN_vkCmdClearAttachments>(vkGetInstanceProcAddr(pInstance, "vkCmdClearAttachments"));

	vkCmdCopyBuffer = reinterpret_cast<PFN_vkCmdCopyBuffer>(vkGetInstanceProcAddr(pInstance, "vkCmdCopyBuffer"));
	vkCmdCopyBufferToImage = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(vkGetInstanceProcAddr(pInstance, "vkCmdCopyBufferToImage"));

	vkCreateSampler = reinterpret_cast<PFN_vkCreateSampler>(vkGetInstanceProcAddr(pInstance, "vkCreateSampler"));
	vkDestroySampler = reinterpret_cast<PFN_vkDestroySampler>(vkGetInstanceProcAddr(pInstance, "vkDestroySampler"));;

	vkCreateSemaphore = reinterpret_cast<PFN_vkCreateSemaphore>(vkGetInstanceProcAddr(pInstance, "vkCreateSemaphore"));
	vkDestroySemaphore = reinterpret_cast<PFN_vkDestroySemaphore>(vkGetInstanceProcAddr(pInstance, "vkDestroySemaphore"));

	vkCreateFence = reinterpret_cast<PFN_vkCreateFence>(vkGetInstanceProcAddr(pInstance, "vkCreateFence"));
	vkDestroyFence = reinterpret_cast<PFN_vkDestroyFence>(vkGetInstanceProcAddr(pInstance, "vkDestroyFence"));
	vkWaitForFences = reinterpret_cast<PFN_vkWaitForFences>(vkGetInstanceProcAddr(pInstance, "vkWaitForFences"));
	vkResetFences = reinterpret_cast<PFN_vkResetFences>(vkGetInstanceProcAddr(pInstance, "vkResetFences"));;

	vkCreateCommandPool = reinterpret_cast<PFN_vkCreateCommandPool>(vkGetInstanceProcAddr(pInstance, "vkCreateCommandPool"));
	vkDestroyCommandPool = reinterpret_cast<PFN_vkDestroyCommandPool>(vkGetInstanceProcAddr(pInstance, "vkDestroyCommandPool"));;

	vkAllocateCommandBuffers = reinterpret_cast<PFN_vkAllocateCommandBuffers>(vkGetInstanceProcAddr(pInstance, "vkAllocateCommandBuffers"));
	vkBeginCommandBuffer = reinterpret_cast<PFN_vkBeginCommandBuffer>(vkGetInstanceProcAddr(pInstance, "vkBeginCommandBuffer"));
	vkEndCommandBuffer = reinterpret_cast<PFN_vkEndCommandBuffer>(vkGetInstanceProcAddr(pInstance, "vkEndCommandBuffer"));

	vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(vkGetInstanceProcAddr(pInstance, "vkGetDeviceQueue"));
	vkQueueSubmit = reinterpret_cast<PFN_vkQueueSubmit>(vkGetInstanceProcAddr(pInstance, "vkQueueSubmit"));
	vkQueueWaitIdle = reinterpret_cast<PFN_vkQueueWaitIdle>(vkGetInstanceProcAddr(pInstance, "vkQueueWaitIdle"));

	vkDeviceWaitIdle = reinterpret_cast<PFN_vkDeviceWaitIdle>(vkGetInstanceProcAddr(pInstance, "vkDeviceWaitIdle"));

	vkCreateFramebuffer = reinterpret_cast<PFN_vkCreateFramebuffer>(vkGetInstanceProcAddr(pInstance, "vkCreateFramebuffer"));

	vkCreatePipelineCache = reinterpret_cast<PFN_vkCreatePipelineCache>(vkGetInstanceProcAddr(pInstance, "vkCreatePipelineCache"));
	vkCreatePipelineLayout = reinterpret_cast<PFN_vkCreatePipelineLayout>(vkGetInstanceProcAddr(pInstance, "vkCreatePipelineLayout"));
	vkCreateGraphicsPipelines = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(vkGetInstanceProcAddr(pInstance, "vkCreateGraphicsPipelines"));
	vkCreateComputePipelines = reinterpret_cast<PFN_vkCreateComputePipelines>(vkGetInstanceProcAddr(pInstance, "vkCreateComputePipelines"));

	vkCreateDescriptorPool = reinterpret_cast<PFN_vkCreateDescriptorPool>(vkGetInstanceProcAddr(pInstance, "vkCreateDescriptorPool"));
	vkCreateDescriptorSetLayout = reinterpret_cast<PFN_vkCreateDescriptorSetLayout>(vkGetInstanceProcAddr(pInstance, "vkCreateDescriptorSetLayout"));

	vkAllocateDescriptorSets = reinterpret_cast<PFN_vkAllocateDescriptorSets>(vkGetInstanceProcAddr(pInstance, "vkAllocateDescriptorSets"));
	vkUpdateDescriptorSets = reinterpret_cast<PFN_vkUpdateDescriptorSets>(vkGetInstanceProcAddr(pInstance, "vkUpdateDescriptorSets"));

	vkCmdBindDescriptorSets = reinterpret_cast<PFN_vkCmdBindDescriptorSets>(vkGetInstanceProcAddr(pInstance, "vkCmdBindDescriptorSets"));
	vkCmdBindPipeline = reinterpret_cast<PFN_vkCmdBindPipeline>(vkGetInstanceProcAddr(pInstance, "vkCmdBindPipeline"));
	vkCmdBindVertexBuffers = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(vkGetInstanceProcAddr(pInstance, "vkCmdBindVertexBuffers"));
	vkCmdBindIndexBuffer = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(vkGetInstanceProcAddr(pInstance, "vkCmdBindIndexBuffer"));

	vkCmdSetViewport = reinterpret_cast<PFN_vkCmdSetViewport>(vkGetInstanceProcAddr(pInstance, "vkCmdSetViewport"));
	vkCmdSetScissor = reinterpret_cast<PFN_vkCmdSetScissor>(vkGetInstanceProcAddr(pInstance, "vkCmdSetScissor"));
	vkCmdSetLineWidth = reinterpret_cast<PFN_vkCmdSetLineWidth>(vkGetInstanceProcAddr(pInstance, "vkCmdSetLineWidth"));
	vkCmdSetDepthBias = reinterpret_cast<PFN_vkCmdSetDepthBias>(vkGetInstanceProcAddr(pInstance, "vkCmdSetDepthBias"));
	vkCmdPushConstants = reinterpret_cast<PFN_vkCmdPushConstants>(vkGetInstanceProcAddr(pInstance, "vkCmdPushConstants"));;

	vkCmdDrawIndexed = reinterpret_cast<PFN_vkCmdDrawIndexed>(vkGetInstanceProcAddr(pInstance, "vkCmdDrawIndexed"));
	vkCmdDraw = reinterpret_cast<PFN_vkCmdDraw>(vkGetInstanceProcAddr(pInstance, "vkCmdDraw"));
	vkCmdDrawIndexedIndirect = reinterpret_cast<PFN_vkCmdDrawIndexedIndirect>(vkGetInstanceProcAddr(pInstance, "vkCmdDrawIndexedIndirect"));
	vkCmdDrawIndirect = reinterpret_cast<PFN_vkCmdDrawIndirect>(vkGetInstanceProcAddr(pInstance, "vkCmdDrawIndirect"));
	vkCmdDispatch = reinterpret_cast<PFN_vkCmdDispatch>(vkGetInstanceProcAddr(pInstance, "vkCmdDispatch"));

	vkDestroyPipeline = reinterpret_cast<PFN_vkDestroyPipeline>(vkGetInstanceProcAddr(pInstance, "vkDestroyPipeline"));
	vkDestroyPipelineLayout = reinterpret_cast<PFN_vkDestroyPipelineLayout>(vkGetInstanceProcAddr(pInstance, "vkDestroyPipelineLayout"));;
	vkDestroyDescriptorSetLayout = reinterpret_cast<PFN_vkDestroyDescriptorSetLayout>(vkGetInstanceProcAddr(pInstance, "vkDestroyDescriptorSetLayout"));
	vkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(vkGetInstanceProcAddr(pInstance, "vkDestroyDevice"));
	vkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(vkGetInstanceProcAddr(pInstance, "vkDestroyInstance"));
	vkDestroyDescriptorPool = reinterpret_cast<PFN_vkDestroyDescriptorPool>(vkGetInstanceProcAddr(pInstance, "vkDestroyDescriptorPool"));
	vkFreeCommandBuffers = reinterpret_cast<PFN_vkFreeCommandBuffers>(vkGetInstanceProcAddr(pInstance, "vkFreeCommandBuffers"));
	vkDestroyRenderPass = reinterpret_cast<PFN_vkDestroyRenderPass>(vkGetInstanceProcAddr(pInstance, "vkDestroyRenderPass"));
	vkDestroyFramebuffer = reinterpret_cast<PFN_vkDestroyFramebuffer>(vkGetInstanceProcAddr(pInstance, "vkDestroyFramebuffer"));
	vkDestroyShaderModule = reinterpret_cast<PFN_vkDestroyShaderModule>(vkGetInstanceProcAddr(pInstance, "vkDestroyShaderModule"));
	vkDestroyPipelineCache = reinterpret_cast<PFN_vkDestroyPipelineCache>(vkGetInstanceProcAddr(pInstance, "vkDestroyPipelineCache"));

	vkCreateQueryPool = reinterpret_cast<PFN_vkCreateQueryPool>(vkGetInstanceProcAddr(pInstance, "vkCreateQueryPool"));
	vkDestroyQueryPool = reinterpret_cast<PFN_vkDestroyQueryPool>(vkGetInstanceProcAddr(pInstance, "vkDestroyQueryPool"));
	vkGetQueryPoolResults = reinterpret_cast<PFN_vkGetQueryPoolResults>(vkGetInstanceProcAddr(pInstance, "vkGetQueryPoolResults"));

	vkCmdBeginQuery = reinterpret_cast<PFN_vkCmdBeginQuery>(vkGetInstanceProcAddr(pInstance, "vkCmdBeginQuery"));
	vkCmdEndQuery = reinterpret_cast<PFN_vkCmdEndQuery>(vkGetInstanceProcAddr(pInstance, "vkCmdEndQuery"));
	vkCmdResetQueryPool = reinterpret_cast<PFN_vkCmdResetQueryPool>(vkGetInstanceProcAddr(pInstance, "vkCmdResetQueryPool"));
	vkCmdCopyQueryPoolResults = reinterpret_cast<PFN_vkCmdCopyQueryPoolResults>(vkGetInstanceProcAddr(pInstance, "vkCmdCopyQueryPoolResults"));

	vkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(vkGetInstanceProcAddr(pInstance, "vkDestroySurfaceKHR"));

	vkDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(vkGetInstanceProcAddr(pInstance, "vkDestroySwapchainKHR"));*/

	vkGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(vkGetInstanceProcAddr(pInstance, "vkGetPhysicalDeviceSurfaceSupportKHR"));
	vkGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(vkGetInstanceProcAddr(pInstance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(vkGetInstanceProcAddr(pInstance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
	vkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(vkGetInstanceProcAddr(pInstance, "vkGetPhysicalDeviceSurfacePresentModesKHR"));

	/*vkCmdClearColorImage = reinterpret_cast<PFN_vkCmdClearColorImage>(vkGetInstanceProcAddr(pInstance, "vkCmdClearColorImage"));
	vkAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(vkGetInstanceProcAddr(pInstance, "vkAcquireNextImageKHR"));
	vkQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(vkGetInstanceProcAddr(pInstance, "vkQueuePresentKHR"));*/
}

void android::vk_load_functions_of_device(VkDevice pDevice)
{
	vkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(vkGetDeviceProcAddr(pDevice, "vkCreateSwapchainKHR"));
	vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(vkGetDeviceProcAddr(pDevice, "vkGetSwapchainImagesKHR"));
}

void android::vk_free_library()
{
	dlclose(_lib_vulkan);
	_lib_vulkan = nullptr;

	using namespace android;

	vkCreateInstance = nullptr;
	vkGetDeviceProcAddr = nullptr;
	vkGetInstanceProcAddr = nullptr;
	vkCreateDevice = nullptr;
	vkEnumeratePhysicalDevices = nullptr;
	vkGetPhysicalDeviceProperties = nullptr;
	vkEnumerateDeviceExtensionProperties = nullptr;
	vkEnumerateDeviceLayerProperties = nullptr;
	vkGetPhysicalDeviceFormatProperties = nullptr;
	vkGetPhysicalDeviceFeatures = nullptr;
	vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
	vkGetPhysicalDeviceMemoryProperties = nullptr;
	vkEnumerateInstanceExtensionProperties = nullptr;
	vkEnumerateInstanceLayerProperties = nullptr;
	vkCmdPipelineBarrier = nullptr;
	vkCreateShaderModule = nullptr;
	vkCreateBuffer = nullptr;
	vkGetBufferMemoryRequirements = nullptr;
	vkMapMemory = nullptr;
	vkUnmapMemory = nullptr;
	vkFlushMappedMemoryRanges = nullptr;
	vkInvalidateMappedMemoryRanges = nullptr;
	vkBindBufferMemory = nullptr;
	vkDestroyBuffer = nullptr;
	vkAllocateMemory = nullptr;
	vkBindImageMemory = nullptr;
	vkGetImageSubresourceLayout = nullptr;
	vkCmdCopyBuffer = nullptr;
	vkCmdCopyBufferToImage = nullptr;
	vkCmdCopyImage = nullptr;
	vkCmdBlitImage = nullptr;
	vkCmdClearAttachments = nullptr;
	vkCreateSampler = nullptr;
	vkDestroySampler = nullptr;
	vkDestroyImage = nullptr;
	vkFreeMemory = nullptr;
	vkCreateRenderPass = nullptr;
	vkCmdBeginRenderPass = nullptr;
	vkCmdEndRenderPass = nullptr;
	vkCmdExecuteCommands = nullptr;
	vkCreateImage = nullptr;
	vkGetImageMemoryRequirements = nullptr;
	vkCreateImageView = nullptr;
	vkDestroyImageView = nullptr;
	vkCreateSemaphore = nullptr;
	vkDestroySemaphore = nullptr;
	vkCreateFence = nullptr;
	vkDestroyFence = nullptr;
	vkWaitForFences = nullptr;
	vkResetFences = nullptr;
	vkCreateCommandPool = nullptr;
	vkDestroyCommandPool = nullptr;
	vkAllocateCommandBuffers = nullptr;
	vkBeginCommandBuffer = nullptr;
	vkEndCommandBuffer = nullptr;
	vkGetDeviceQueue = nullptr;
	vkQueueSubmit = nullptr;
	vkQueueWaitIdle = nullptr;
	vkDeviceWaitIdle = nullptr;
	vkCreateFramebuffer = nullptr;
	vkCreatePipelineCache = nullptr;
	vkCreatePipelineLayout = nullptr;
	vkCreateGraphicsPipelines = nullptr;
	vkCreateComputePipelines = nullptr;
	vkCreateDescriptorPool = nullptr;
	vkCreateDescriptorSetLayout = nullptr;
	vkAllocateDescriptorSets = nullptr;
	vkUpdateDescriptorSets = nullptr;
	vkCmdBindDescriptorSets = nullptr;
	vkCmdBindPipeline = nullptr;
	vkCmdBindVertexBuffers = nullptr;
	vkCmdBindIndexBuffer = nullptr;
	vkCmdSetViewport = nullptr;
	vkCmdSetScissor = nullptr;
	vkCmdSetLineWidth = nullptr;
	vkCmdSetDepthBias = nullptr;
	vkCmdPushConstants = nullptr;
	vkCmdDrawIndexed = nullptr;
	vkCmdDraw = nullptr;
	vkCmdDrawIndexedIndirect = nullptr;
	vkCmdDrawIndirect = nullptr;
	vkCmdDispatch = nullptr;
	vkDestroyPipeline = nullptr;
	vkDestroyPipelineLayout = nullptr;
	vkDestroyDescriptorSetLayout = nullptr;
	vkDestroyDevice = nullptr;
	vkDestroyInstance = nullptr;
	vkDestroyDescriptorPool = nullptr;
	vkFreeCommandBuffers = nullptr;
	vkDestroyRenderPass = nullptr;
	vkDestroyFramebuffer = nullptr;
	vkDestroyShaderModule = nullptr;
	vkDestroyPipelineCache = nullptr;
	vkCreateQueryPool = nullptr;
	vkDestroyQueryPool = nullptr;
	vkGetQueryPoolResults = nullptr;
	vkCmdBeginQuery = nullptr;
	vkCmdEndQuery = nullptr;
	vkCmdResetQueryPool = nullptr;
	vkCmdCopyQueryPoolResults = nullptr;
	vkCreateAndroidSurfaceKHR = nullptr;
	vkDestroySurfaceKHR = nullptr;
	vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;

	vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
	vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;

	vkCreateSwapchainKHR = nullptr;
	vkGetSwapchainImagesKHR = nullptr;
}

#endif
