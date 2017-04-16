#ifndef __VKPP_TYPE_STRUCTURE_H__
#define __VKPP_TYPE_STRUCTURE_H__



#include <vulkan/vulkan.h>



namespace vkpp::internal
{



enum class Structure
{
    eApplication                = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    eInstance                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    eQueue                      = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    eDevice                     = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    eSubmitInfo                 = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    eSemaphore                  = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    eBuffer                     = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    eBufferView                 = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
    eImage                      = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    eImageView                  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    eShaderModule               = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    ePipelineCache              = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
    ePipelineShaderStage        = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    ePipelineVertexInputState   = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    ePipelineInputAssemblyState = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    ePipelineTessellationState  = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
    ePipelineViewportState      = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    ePipelineRasterizationState = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    ePipelineMultisampleState   = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    ePipelineDepthStencilState  = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    ePipelineColorBlendState    = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    ePipelineDynamicState       = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    ePipelineGraphics           = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    ePipelineCompute            = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
    ePipelineLayout             = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    eFrameBuffer                = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
    eRenderPass                 = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
    eCommandPool                = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    eCommandBufferAllocate      = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    eCommandBufferInheritance   = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
    eCommandBufferBegin         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    eRenderPassBegin            = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    eBufferMemoryBarrier        = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
    eImageMemoryBarrier         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    eMemoryBarrier              = VK_STRUCTURE_TYPE_MEMORY_BARRIER,

    eSwapchain                  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    ePresent                    = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    eWin32Surface               = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
    eDebugReportCallback        = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
};



}                    // End of namespace vkpp::type



#endif             // __VKPP_TYPE_STRUCTURE_H__
