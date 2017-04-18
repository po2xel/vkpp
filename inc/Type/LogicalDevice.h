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
#include <Info/PipelineStage.h>

#include <Type/VkDeleter.h>
#include <Type/AllocationCallbacks.h>
#include <Type/Queue.h>
#include <Type/Semaphore.h>
#include <Type/Fence.h>
#include <Type/Swapchain.h>
#include <Type/Image.h>
#include <Type/CommandPool.h>
#include <Type/CommandBuffer.h>
#include <Type/FrameBuffer.h>
#include <Type/ShaderModule.h>
#include <Type/GraphicsPipeline.h>



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

    ImageView CreateImageView(const ImageViewCreateInfo& aImageViewCreateInfo) const
    {
        ImageView lImageView;
        ThrowIfFailed(vkCreateImageView(mDevice, &aImageViewCreateInfo, nullptr, &lImageView));

        return lImageView;
    }

    ImageView CreateImageView(const ImageViewCreateInfo& aImageViewCreateInfo, const AllocationCallbacks& aAllocator) const
    {
        ImageView lImageView;
        ThrowIfFailed(vkCreateImageView(mDevice, &aImageViewCreateInfo, &aAllocator, &lImageView));

        return lImageView;
    }

    void DestroyImageView(const ImageView& aImageView) const
    {
        vkDestroyImageView(mDevice, aImageView, nullptr);
    }

    void DestroyImageView(const ImageView& aImageView, const AllocationCallbacks& aAllocator) const
    {
        vkDestroyImageView(mDevice, aImageView, &aAllocator);
    }

    FrameBuffer CreateFrameBuffer(const FrameBufferCreateInfo& aFrameBufferCreateInfo) const
    {
        FrameBuffer lFrameBuffer;
        ThrowIfFailed(vkCreateFramebuffer(mDevice, &aFrameBufferCreateInfo, nullptr, &lFrameBuffer));

        return lFrameBuffer;
    }

    FrameBuffer CreateFrameBuffer(const FrameBufferCreateInfo& aFrameBufferCreateInfo, const AllocationCallbacks& aAllocator) const
    {
        FrameBuffer lFrameBuffer;
        ThrowIfFailed(vkCreateFramebuffer(mDevice, &aFrameBufferCreateInfo, &aAllocator, &lFrameBuffer));

        return lFrameBuffer;
    }

    void DestroyFrameBuffer(const FrameBuffer& aFrameBuffer) const
    {
        vkDestroyFramebuffer(mDevice, aFrameBuffer, nullptr);
    }

    void DestroyFrameBuffer(const FrameBuffer& aFrameBuffer, const AllocationCallbacks& aAllocator) const
    {
        vkDestroyFramebuffer(mDevice, aFrameBuffer, &aAllocator);
    }

    ShaderModule CreateShaderModule(const ShaderModuleCreateInfo& aShaderModuleCreateInfo) const
    {
        ShaderModule lShaderModule;
        ThrowIfFailed(vkCreateShaderModule(mDevice, &aShaderModuleCreateInfo, nullptr, &lShaderModule));

        return lShaderModule;
    }

    ShaderModule CreateShaderModule(const ShaderModuleCreateInfo& aShaderModuleCreateInfo, const AllocationCallbacks& aAllocator) const
    {
        ShaderModule lShaderModule;
        ThrowIfFailed(vkCreateShaderModule(mDevice, &aShaderModuleCreateInfo, &aAllocator, &lShaderModule));

        return lShaderModule;
    }

    void DestroyShaderModule(const ShaderModule& aShaderModule) const
    {
        vkDestroyShaderModule(mDevice, aShaderModule, nullptr);
    }

    void DestroyShaderModule(const ShaderModule& aShaderModule, const AllocationCallbacks& aAllocator) const
    {
        vkDestroyShaderModule(mDevice, aShaderModule, &aAllocator);
    }

    PipelineLayout CreatePipelineLayout(const PipelineLayoutCreateInfo& aPipelineLayoutCreateInfo) const
    {
        PipelineLayout lPipelineLayout;
        ThrowIfFailed(vkCreatePipelineLayout(mDevice, &aPipelineLayoutCreateInfo, nullptr, &lPipelineLayout));

        return lPipelineLayout;
    }

    PipelineLayout CreatePipelineLayout(const PipelineLayoutCreateInfo& aPipelineLayoutCreateInfo, const AllocationCallbacks& aAllocator) const
    {
        PipelineLayout lPipelineLayout;
        ThrowIfFailed(vkCreatePipelineLayout(mDevice, &aPipelineLayoutCreateInfo, &aAllocator, &lPipelineLayout));

        return lPipelineLayout;
    }

    void DestroyPipelineLayout(const PipelineLayout& aPipelineLayout) const
    {
        vkDestroyPipelineLayout(mDevice, aPipelineLayout, nullptr);
    }

    void DestroyPipelineLayout(const PipelineLayout& aPipelineLayout, const AllocationCallbacks& aAllocator) const
    {
        vkDestroyPipelineLayout(mDevice, aPipelineLayout, &aAllocator);
    }

    Pipeline CreateGraphicsPipeline(const PipelineCache& aPipelineCache, uint32_t aCreateInfoCount, const GraphicsPipelineCreateInfo* apGraphicsPipelineCraeteInfos) const
    {
        assert(aCreateInfoCount > 0 && apGraphicsPipelineCraeteInfos != nullptr);

        Pipeline lPipeline;
        ThrowIfFailed(vkCreateGraphicsPipelines(mDevice, aPipelineCache, aCreateInfoCount, &apGraphicsPipelineCraeteInfos[0], nullptr, &lPipeline));

        return lPipeline;
    }

    Pipeline CreateGraphicsPipeline(const PipelineCache& aPipelineCache, uint32_t aCreateInfoCount,
        const GraphicsPipelineCreateInfo* apGraphicsPipelineCraeteInfos, const AllocationCallbacks& aAllocator) const
    {
        assert(aCreateInfoCount > 0 && apGraphicsPipelineCraeteInfos != nullptr);

        Pipeline lPipeline;
        ThrowIfFailed(vkCreateGraphicsPipelines(mDevice, aPipelineCache, aCreateInfoCount, &apGraphicsPipelineCraeteInfos[0], &aAllocator, &lPipeline));

        return lPipeline;
    }

    void DestroyPipeline(const Pipeline& aPipeline) const
    {
        vkDestroyPipeline(mDevice, aPipeline, nullptr);
    }

    void DestroyPipeline(const Pipeline& aPipeline, const AllocationCallbacks& aAllocator) const
    {
        vkDestroyPipeline(mDevice, aPipeline, &aAllocator);
    }

    VkResult Wait(void) const
    {
        return vkDeviceWaitIdle(mDevice);
    }
};



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_LOGICAL_DEVICE_H__
