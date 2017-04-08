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
