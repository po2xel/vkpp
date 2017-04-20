#ifndef __VKPP_TYPE_LOGICAL_DEVICE_H__
#define __VKPP_TYPE_LOGICAL_DEVICE_H__



#include <Info/Common.h>
#include <Info/CommandBufferAllocateInfo.h>
#include <Info/PipelineStage.h>
#include <Info/PhysicalDeviceFeatures.h>
#include <Info/MemoryRequirements.h>

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
#include <Type/Memory.h>
#include <Type/Buffer.h>



namespace vkpp
{



enum class QueueCreateFlagBits
{};

using QueueCreateFlags = internal::Flags<QueueCreateFlagBits, VkDeviceQueueCreateFlags>;



class QueueCreateInfo : public internal::VkTrait<QueueCreateInfo, VkDeviceQueueCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eQueue;

public:
    const void*         pNext{ nullptr };
    QueueCreateFlags    flags;
    uint32_t            queueFamilyIndex{ 0 };
    uint32_t            queueCount{ 0 };
    const float*        pQueuePriorities{ nullptr };

    DEFINE_CLASS_MEMBER(QueueCreateInfo)

    QueueCreateInfo(uint32_t aQueueFamilyIndex, uint32_t aQueueCount, const float* apQueuePriorities)
        : queueFamilyIndex(aQueueFamilyIndex), queueCount(aQueueCount), pQueuePriorities(apQueuePriorities)
    {}

    QueueCreateInfo(uint32_t aQueueFamilyIndex, const std::vector<float>& aQueuePriorities)
        : QueueCreateInfo(aQueueFamilyIndex, static_cast<uint32_t>(aQueuePriorities.size()), aQueuePriorities.data())
    {}

    template <std::size_t P>
    QueueCreateInfo(uint32_t aQueueFamilyIndex, const std::array<float, P>& aQueuePriorities)
        : QueueCreateInfo(aQueueFamilyIndex, static_cast<uint32_t>(aQueuePriorities.size()), aQueuePriorities.data())
    {}

    QueueCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    QueueCreateInfo& SetFamilyIndex(uint32_t aQueueFamilyIndex)
    {
        queueFamilyIndex = aQueueFamilyIndex;

        return *this;
    }

    QueueCreateInfo& SetQueuePriorities(uint32_t aQueueCount, const float* apQueuePriorities)
    {
        queueCount = aQueueCount;
        pQueuePriorities = apQueuePriorities;

        return *this;
    }

    QueueCreateInfo& SetQueuePriorities(const std::vector<float>& aQueuePriorities)
    {
        return SetQueuePriorities(static_cast<uint32_t>(aQueuePriorities.size()), aQueuePriorities.data());
    }

    template <std::size_t P>
    QueueCreateInfo& SetQueuePriorities(const std::array<float, P>& aQueuePriorities)
    {
        return SetQueuePriorities(static_cast<uint32_t>(aQueuePriorities.size()), aQueuePriorities.data())
    }
};

StaticSizeCheck(QueueCreateInfo);



enum class DeviceCreateFlagBits
{};

using DeviceCreateFlags = internal::Flags<DeviceCreateFlagBits, VkDeviceCreateFlags>;



class LogicalDeviceCreateInfo : public internal::VkTrait<LogicalDeviceCreateInfo, VkDeviceCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eDevice;

public:
    const void*                             pNext{ nullptr };
    DeviceCreateFlags                       flags;
    uint32_t                                queueCreateInfoCount{ 0 };
    const QueueCreateInfo*                  pQueueCreateInfo{ nullptr };

private:
    [[deprecated]] uint32_t                 enabledLayerCount{ 0 };
    [[deprecated]] const char* const*       ppEnabledLayerNames{ nullptr };

public:
    uint32_t                                enabledExtensionCount{ 0 };
    const char* const*                      ppEnabledExtensionNames{ nullptr };
    const PhysicalDeviceFeatures*           pEnabledFeatures{ nullptr };

    DEFINE_CLASS_MEMBER(LogicalDeviceCreateInfo)

    LogicalDeviceCreateInfo(uint32_t aQueueCreateInfoCount, const QueueCreateInfo* apQueueCreateInfos,
        uint32_t aEnalbedExtensionCount, const char* const* appEnabledExtensionNames, const PhysicalDeviceFeatures* apEnabledFeatures)
        : queueCreateInfoCount(aQueueCreateInfoCount), pQueueCreateInfo(apQueueCreateInfos),
        enabledExtensionCount(aEnalbedExtensionCount), ppEnabledExtensionNames(appEnabledExtensionNames), pEnabledFeatures(apEnabledFeatures)
    {}

    LogicalDeviceCreateInfo(const std::vector<QueueCreateInfo>& aQueueCreateInfos, const std::vector<const char*>& aEnabledExtensions,
        const PhysicalDeviceFeatures& aEnabledFeatures)
        : LogicalDeviceCreateInfo(static_cast<uint32_t>(aQueueCreateInfos.size()), aQueueCreateInfos.data(),
            static_cast<uint32_t>(aEnabledExtensions.size()), aEnabledExtensions.data(),
            &aEnabledFeatures)
    {}

    template <std::size_t Q, std::size_t E>
    LogicalDeviceCreateInfo(const std::array<QueueCreateInfo, Q>& aQueueCreateInfos, const std::array<const char*, E>& aEnabledExtensions,
        const PhysicalDeviceFeatures& aEnabledFeatures)
        : LogicalDeviceCreateInfo(static_cast<uint32_t>(aQueueCreateInfos.size()), aQueueCreateInfos.data(),
            static_cast<uint32_t>(aEnabledExtensions.size()), aEnabledExtensions.data(),
            &aEnabledFeatures)
    {}

    LogicalDeviceCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    LogicalDeviceCreateInfo& SetFlags(DeviceCreateFlags aFlags)
    {
        flags = aFlags;

        return *this;
    }

    LogicalDeviceCreateInfo& SetQueueCreateInfo(const QueueCreateInfo& aQueueCreateInfo)
    {
        pQueueCreateInfo = aQueueCreateInfo.AddressOf();

        return *this;
    }

    LogicalDeviceCreateInfo& SetEnabledExtensions(uint32_t aEnabledExtensionCount, const char* const* appEnabledExtensionNames)
    {
        enabledExtensionCount = aEnabledExtensionCount;
        ppEnabledExtensionNames = appEnabledExtensionNames;

        return *this;
    }

    LogicalDeviceCreateInfo& SetEnabledExtensions(const std::vector<const char*>& aEnabledExtensionNames)
    {
        return SetEnabledExtensions(static_cast<uint32_t>(aEnabledExtensionNames.size()), aEnabledExtensionNames.data());
    }

    template <std::size_t E>
    LogicalDeviceCreateInfo& SetEnabledExtensions(const std::array<const char*, E>& aEnabledExtensionNames)
    {
        return SetEnabledExtensions(static_cast<uint32_t>(aEnabledExtensionNames.size()), aEnabledExtensionNames.data());
    }
};

StaticSizeCheck(LogicalDeviceCreateInfo);



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

    MemoryRequirements GetBufferMemoryRequirements(const Buffer& aBuffer) const
    {
        MemoryRequirements lMemoryRequirements;
        vkGetBufferMemoryRequirements(mDevice, aBuffer, &lMemoryRequirements);

        return lMemoryRequirements;
    }

    MemoryRequirements GetImageMemoryRequirements(const Image& aImage) const
    {
        MemoryRequirements lMemoryRequirements;
        vkGetImageMemoryRequirements(mDevice, aImage, &lMemoryRequirements);

        return lMemoryRequirements;
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

    Fence CreateFence(const FenceCreateInfo& aFenceCreateInfo) const
    {
        Fence lFence;
        ThrowIfFailed(vkCreateFence(mDevice, &aFenceCreateInfo, nullptr, &lFence));

        return lFence;
    }

    Fence CreateFence(const FenceCreateInfo& aFenceCreateInfo, const AllocationCallbacks& aAllocator) const
    {
        Fence lFence;
        ThrowIfFailed(vkCreateFence(mDevice, &aFenceCreateInfo, &aAllocator, &lFence));

        return lFence;
    }

    void DestroyFence(const Fence& aFence) const
    {
        vkDestroyFence(mDevice, aFence, nullptr);
    }

    void DestroyFence(const Fence& aFence, const AllocationCallbacks& aAllocator) const
    {
        vkDestroyFence(mDevice, aFence, &aAllocator);
    }

    VkResult GetFenceStatus(const Fence& aFence) const
    {
        return vkGetFenceStatus(mDevice, aFence);
    }

    void ResetFence(const Fence& apFences) const
    {
        ThrowIfFailed(vkResetFences(mDevice, 1, &apFences));
    }

    void ResetFences(uint32_t aFenceCount, const Fence* apFences) const
    {
        assert(aFenceCount != 0 && apFences != nullptr);

        ThrowIfFailed(vkResetFences(mDevice, aFenceCount, &apFences[0]));
    }

    void ResetFences(const std::vector<Fence>& aFences) const
    {
        return ResetFences(static_cast<uint32_t>(aFences.size()), aFences.data());
    }

    template <std::size_t F>
    void ResetFences(const std::array<Fence, F>& aFences) const
    {
        return ResetFences(static_cast<uint32_t>(aFences.size()), aFences.data());
    }

    void WaitForFence(const Fence& aFence, bool aWaitAll, uint64_t aTimeout) const
    {
        ThrowIfFailed(vkWaitForFences(mDevice, 1, &aFence, aWaitAll, aTimeout));
    }

    void WaitForFences(uint32_t aFenceCount, const Fence* apFences, bool aWaitAll, uint64_t aTimeout) const
    {
        assert(aFenceCount != 0 && apFences != nullptr);

        ThrowIfFailed(vkWaitForFences(mDevice, aFenceCount, &apFences[0], aWaitAll, aTimeout));
    }

    void WaitForFences(const std::vector<Fence>& aFences, bool aWaitAll, uint64_t aTimeout) const
    {
        return WaitForFences(static_cast<uint32_t>(aFences.size()), aFences.data(), aWaitAll, aTimeout);
    }

    template <std::size_t F>
    void WaitForFences(const std::array<Fence, F>& aFences, bool aWaitAll, uint64_t aTimeout) const
    {
        return WaitForFences(static_cast<uint32_t>(aFences.size()), aFences.data(), aWaitAll, aTimeout);
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

    uint32_t AcquireNextImage(const khr::Swapchain& aSwapchain, uint64_t aTimeout, const Semaphore& aSemaphore, const Fence& aFence) const
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

    void FreeCommandBuffer(const CommandPool& aCommandPool, const CommandBuffer& aCommandBuffer) const
    {
        vkFreeCommandBuffers(mDevice, aCommandPool, 1, &aCommandBuffer);
    }

    void FreeCommandBuffers(const CommandPool& aCommandPool, uint32_t aCommandBufferCount, const CommandBuffer* apCmdBuffers) const
    {
        assert(aCommandBufferCount != 0 && apCmdBuffers != nullptr);

        vkFreeCommandBuffers(mDevice, aCommandPool, aCommandBufferCount, &apCmdBuffers[0]);
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

    Buffer CreateBuffer(const BufferCreateInfo& aBufferCreateInfo) const
    {
        Buffer lBuffer;
        ThrowIfFailed(vkCreateBuffer(mDevice, &aBufferCreateInfo, nullptr, &lBuffer));

        return lBuffer;
    }

    Buffer CreateBuffer(const BufferCreateInfo& aBufferCreateInfo, const AllocationCallbacks& aAllocator) const
    {
        Buffer lBuffer;
        ThrowIfFailed(vkCreateBuffer(mDevice, &aBufferCreateInfo, &aAllocator, &lBuffer));

        return lBuffer;
    }

    void DestroyBuffer(const Buffer& aBuffer) const
    {
        vkDestroyBuffer(mDevice, aBuffer, nullptr);
    }

    void DestroyBuffer(const Buffer& aBuffer, const AllocationCallbacks& aAllocator) const
    {
        vkDestroyBuffer(mDevice, aBuffer, &aAllocator);
    }

    BufferView CreateBufferView(const BufferViewCreateInfo& aBufferViewCreateInfo) const
    {
        BufferView lBufferView;
        ThrowIfFailed(vkCreateBufferView(mDevice, &aBufferViewCreateInfo, nullptr, &lBufferView));

        return lBufferView;
    }

    BufferView CreateBufferView(const BufferViewCreateInfo& aBufferViewCreateInfo, const AllocationCallbacks& aAllocator) const
    {
        BufferView lBufferView;
        ThrowIfFailed(vkCreateBufferView(mDevice, &aBufferViewCreateInfo, &aAllocator, &lBufferView));

        return lBufferView;
    }

    void DestroyBufferView(const BufferView& aBufferView) const
    {
        vkDestroyBufferView(mDevice, aBufferView, nullptr);
    }

    void DestroyBufferView(const BufferView& aBufferView, const AllocationCallbacks& aAllocator) const
    {
        vkDestroyBufferView(mDevice, aBufferView, &aAllocator);
    }

    Image CreateImage(const ImageCreateInfo& aImageCreateInfo) const
    {
        Image lImage;
        ThrowIfFailed(vkCreateImage(mDevice, &aImageCreateInfo, nullptr, &lImage));

        return lImage;
    }

    Image CreateImage(const ImageCreateInfo& aImageCreateInfo, const AllocationCallbacks& aAllocator) const
    {
        Image lImage;
        ThrowIfFailed(vkCreateImage(mDevice, &aImageCreateInfo, &aAllocator, &lImage));

        return lImage;
    }

    void DestroyImage(const Image& aImage) const
    {
        vkDestroyImage(mDevice, aImage, nullptr);
    }

    void DestroyImage(const Image& aImage, const AllocationCallbacks& aAllocator) const
    {
        vkDestroyImage(mDevice, aImage, &aAllocator);
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

    DeviceMemory AllocateMemory(const MemoryAllocateInfo& aMemoryAllocationInfo) const
    {
        DeviceMemory lDeviceMemory;
        ThrowIfFailed(vkAllocateMemory(mDevice, &aMemoryAllocationInfo, nullptr, &lDeviceMemory));

        return lDeviceMemory;
    }

    DeviceMemory AllocateMemory(const MemoryAllocateInfo& aMemoryAllocationInfo, const AllocationCallbacks& aAllocator) const
    {
        DeviceMemory lDeviceMemory;
        ThrowIfFailed(vkAllocateMemory(mDevice, &aMemoryAllocationInfo, &aAllocator, &lDeviceMemory));

        return lDeviceMemory;
    }

    void FreeMemory(const DeviceMemory& aDeviceMemory) const
    {
        vkFreeMemory(mDevice, aDeviceMemory, nullptr);
    }

    void FreeMemory(const DeviceMemory& aDeviceMemory, const AllocationCallbacks& aAllocator) const
    {
        vkFreeMemory(mDevice, aDeviceMemory, &aAllocator);
    }

    void BindBufferMemory(const Buffer& aBuffer, const DeviceMemory& aDeviceMemory, DeviceSize aMemoryOffset = 0) const
    {
        ThrowIfFailed(vkBindBufferMemory(mDevice, aBuffer, aDeviceMemory, aMemoryOffset));
    }

    void BindImageMemory(const Image& aImage, const DeviceMemory& aDeviceMemory, DeviceSize aMemoryOffset = 0) const
    {
        ThrowIfFailed(vkBindImageMemory(mDevice, aImage, aDeviceMemory, aMemoryOffset));
    }

    void* MapMemory(const DeviceMemory& aDeviceMemory, DeviceSize aOffset, DeviceSize aSize, const MemoryMapFlags& aFlags = DefaultFlags) const
    {
        void* lpHostData{ nullptr };

        ThrowIfFailed(vkMapMemory(mDevice, aDeviceMemory, aOffset, aSize, aFlags, &lpHostData));

        assert(lpHostData != nullptr);

        return lpHostData;
    }

    void UnmapMemory(const DeviceMemory& aDeviceMemory) const
    {
        vkUnmapMemory(mDevice, aDeviceMemory);
    }

    void FlushMappedMemoryRange(const MappedMemoryRange& aMappedMemoryRange) const
    {
        ThrowIfFailed(vkFlushMappedMemoryRanges(mDevice, 1, &aMappedMemoryRange));
    }

    void FlushMappedMemoryRanges(uint32_t aMemoryRangeCount, const MappedMemoryRange* aMappedMemoryRanges) const
    {
        assert(aMemoryRangeCount != 0 && aMappedMemoryRanges != nullptr);

        ThrowIfFailed(vkFlushMappedMemoryRanges(mDevice, aMemoryRangeCount, &aMappedMemoryRanges[0]));
    }

    void FlushMappedMemoryRanges(const std::vector<MappedMemoryRange>& aMappedMemoryRanges) const
    {
        FlushMappedMemoryRanges(static_cast<uint32_t>(aMappedMemoryRanges.size()), aMappedMemoryRanges.data());
    }

    template <std::size_t M>
    void FlushMappedMemoryRanges(const std::array<MappedMemoryRange, M>& aMappedMemoryRanges) const
    {
        FlushMappedMemoryRanges(static_cast<uint32_t>(aMappedMemoryRanges.size()), aMappedMemoryRanges.data());
    }

    void InvalidateMappedMemoryRanges(uint32_t aMemoryRangeCount, const MappedMemoryRange* apMappedMemoryRanges) const
    {
        assert(aMemoryRangeCount != 0 && apMappedMemoryRanges != nullptr);

        ThrowIfFailed(vkInvalidateMappedMemoryRanges(mDevice, aMemoryRangeCount, &apMappedMemoryRanges[0]));
    }

    void InvalidateMappedMemoryRanges(const std::vector<MappedMemoryRange>& aMappedMemoryRanges) const
    {
        return InvalidateMappedMemoryRanges(static_cast<uint32_t>(aMappedMemoryRanges.size()), aMappedMemoryRanges.data());
    }

    template <std::size_t M>
    void InvalidateMappedMemoryRanges(const std::array<MappedMemoryRange, M>& aMappedMemoryRanges) const
    {
        return InvalidateMappedMemoryRanges(static_cast<uint32_t>(aMappedMemoryRanges.size()), aMappedMemoryRanges.data());
    }

    VkResult Wait(void) const
    {
        return vkDeviceWaitIdle(mDevice);
    }
};



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_LOGICAL_DEVICE_H__
