#ifndef __VKPP_TYPE_LOGICAL_DEVICE_H__
#define __VKPP_TYPE_LOGICAL_DEVICE_H__



#include <utility>

#include <Info/Common.h>
#include <Info/LogicalDeviceInfo.h>
#include <Info/BufferCreateInfo.h>
#include <Info/SemaphoreCreateInfo.h>
#include <Info/SwapchainCreateInfo.h>
#include <Info/CommandPoolCreateInfo.h>
#include <Info/CommandBufferAllocateInfo.h>

#include <Type/VkDeleter.h>
#include <Type/AllocationCallbacks.h>
#include <Type/Queue.h>
#include <Type/Semaphore.h>
#include <Type/Fence.h>
#include <Type/Swapchain.h>
#include <Type/Image.h>
#include <Type/CommandPool.h>
#include <Type/CommandBuffer.h>



namespace vkpp
{



class LogicalDevice
{
private:
    internal::VkDeleter<VkDevice> mDevice{ vkDestroyDevice };

public:
    LogicalDevice(void) noexcept = default;

    LogicalDevice(std::nullptr_t) noexcept
    {}

    LogicalDevice(VkDevice aLogicalDevice)
    {
        mDevice = aLogicalDevice;
    }

    LogicalDevice(LogicalDevice&& aLogicalDevice) noexcept : mDevice(std::move(aLogicalDevice.mDevice))
    {}

    LogicalDevice& operator=(LogicalDevice&& aLogicalDevice) noexcept
    {
        mDevice = std::move(aLogicalDevice.mDevice);

        return *this;
    }

    Queue GetQueue(uint32_t aQueueFamilyIndex, uint32_t aQueueIndex) const
    {
        Queue lQueue;
        vkGetDeviceQueue(mDevice, aQueueFamilyIndex, aQueueIndex, &lQueue);

        return lQueue;
    }

    Semaphore CreateSemaphore(const SemaphoreCreateInfo& aSemaphoreCreateInfo) const
    {
        Semaphore lSemaphore;
        ThrowIfFailed(vkCreateSemaphore(mDevice, &aSemaphoreCreateInfo, nullptr, &lSemaphore));

        return lSemaphore;
    }

    Semaphore CreateSemaphore(const SemaphoreCreateInfo& aSemaphoreCreateInfo, const AllocationCallbacks& aAllocator) const
    {
        Semaphore lSemaphore;
        ThrowIfFailed(vkCreateSemaphore(mDevice, &aSemaphoreCreateInfo, &aAllocator, &lSemaphore));

        return lSemaphore;
    }

    void DestroySemaphore(const Semaphore& aSemaphore) const
    {
        vkDestroySemaphore(mDevice, aSemaphore, nullptr);
    }

    void DestroySemaphore(const Semaphore& aSemaphore, const AllocationCallbacks& aAllocator) const
    {
        vkDestroySemaphore(mDevice, aSemaphore, &aAllocator);
    }

    khr::Swapchain CreateSwapchain(const khr::SwapchainCreateInfo& aSwapchainCreateInfo) const
    {
        khr::Swapchain lSwapchain;
        ThrowIfFailed(vkCreateSwapchainKHR(mDevice, &aSwapchainCreateInfo, nullptr, &lSwapchain));

        return lSwapchain;
    }

    khr::Swapchain CreateSwapchain(const khr::SwapchainCreateInfo& aSwapchainCreateInfo, const AllocationCallbacks& aAllocator) const
    {
        khr::Swapchain lSwapchain;
        ThrowIfFailed(vkCreateSwapchainKHR(mDevice, &aSwapchainCreateInfo, &aAllocator, &lSwapchain));

        return lSwapchain;
    }

    void DestroySwapchain(const khr::Swapchain& aSwapchain) const
    {
        vkDestroySwapchainKHR(mDevice, aSwapchain, nullptr);
    }

    void DestroySwapchain(const khr::Swapchain& aSwapchain, const AllocationCallbacks& aAllocator) const
    {
        vkDestroySwapchainKHR(mDevice, aSwapchain, &aAllocator);
    }

    std::vector<Image> GetSwapchainImages(const khr::Swapchain& aSwapchain) const
    {
        uint32_t lSwapchainImageCount{ 0 };
        ThrowIfFailed(vkGetSwapchainImagesKHR(mDevice, aSwapchain, &lSwapchainImageCount, nullptr));

        std::vector<Image> lSwapchainImages(lSwapchainImageCount);
        ThrowIfFailed(vkGetSwapchainImagesKHR(mDevice, aSwapchain, &lSwapchainImageCount, &lSwapchainImages[0]));

        return lSwapchainImages;
    }

    uint32_t AcquireNextImage(const khr::Swapchain& aSwapchain, uint32_t aTimeout, const Semaphore& aSemaphore, const Fence& aFence) const
    {
        uint32_t lImageIndex;
        // TODO: Error handling.
        ThrowIfFailed(vkAcquireNextImageKHR(mDevice, aSwapchain, aTimeout, aSemaphore, aFence, &lImageIndex));

        return lImageIndex;
    }

    CommandPool CreateCommandPool(const CommandPoolCreateInfo& aCommandPoolCreateInfo) const
    {
        CommandPool lCommandPool;
        ThrowIfFailed(vkCreateCommandPool(mDevice, &aCommandPoolCreateInfo, nullptr, &lCommandPool));

        return lCommandPool;
    }

    CommandPool CreateCommandPool(const CommandPoolCreateInfo& aCommandPoolCreateInfo, const AllocationCallbacks& aAllocator) const
    {
        CommandPool lCommandPool;
        ThrowIfFailed(vkCreateCommandPool(mDevice, &aCommandPoolCreateInfo, &aAllocator, &lCommandPool));

        return lCommandPool;
    }

    void DestroyCommandPool(const CommandPool& aCmdPool) const
    {
        vkDestroyCommandPool(mDevice, aCmdPool, nullptr);
    }

    std::vector<CommandBuffer> AllocateCommandBuffers(const CommandBufferAllocateInfo& aCommandBufferAllocateInfo) const
    {
        std::vector<CommandBuffer> lCommandBuffers(aCommandBufferAllocateInfo.commandBufferCount);
        ThrowIfFailed(vkAllocateCommandBuffers(mDevice, &aCommandBufferAllocateInfo, &lCommandBuffers[0]));

        return lCommandBuffers;
    }

    void FreeCommandBuffers(const CommandPool& aCmdPool, uint32_t aCmdBufferCount, const CommandBuffer* apCmdBuffers) const
    {
        vkFreeCommandBuffers(mDevice, aCmdPool, aCmdBufferCount, &apCmdBuffers[0]);
    }

    void FreeCommandBuffers(const CommandPool& aCommandPool, const std::vector<CommandBuffer>& aCommandBuffers) const
    {
        return FreeCommandBuffers(aCommandPool, static_cast<uint32_t>(aCommandBuffers.size()), aCommandBuffers.data());
    }

    RenderPass CreateRenderPass(const RenderPassCreateInfo& aRenderPassCreateInfo) const
    {
        RenderPass lRenderPass;
        ThrowIfFailed(vkCreateRenderPass(mDevice, &aRenderPassCreateInfo, nullptr, &lRenderPass));

        return lRenderPass;
    }

    RenderPass CreateRenderPass(const RenderPassCreateInfo& aRenderPassCreateInfo, const AllocationCallbacks& aAllocator) const
    {
        RenderPass lRenderPass;
        ThrowIfFailed(vkCreateRenderPass(mDevice, &aRenderPassCreateInfo, &aAllocator, &lRenderPass));

        return lRenderPass;
    }

    void DestroyRenderPass(const RenderPass& aRenderPass) const
    {
        vkDestroyRenderPass(mDevice, aRenderPass, nullptr);
    }

    void DestroyRenderPass(const RenderPass& aRenderPass, const AllocationCallbacks& aAllocator) const
    {
        vkDestroyRenderPass(mDevice, aRenderPass, &aAllocator);
    }

    VkResult Wait(void) const
    {
        return vkDeviceWaitIdle(mDevice);
    }
};



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_LOGICAL_DEVICE_H__
