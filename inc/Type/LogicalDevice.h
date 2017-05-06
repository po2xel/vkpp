#ifndef __VKPP_TYPE_LOGICAL_DEVICE_H__
#define __VKPP_TYPE_LOGICAL_DEVICE_H__



#include <Info/Common.h>
#include <Info/CommandBufferAllocateInfo.h>
#include <Info/PipelineStage.h>
#include <Info/PhysicalDeviceFeatures.h>
#include <Info/MemoryRequirements.h>

#include <Type/PhysicalDevice.h>
#include <Type/AllocationCallbacks.h>
#include <Type/Queue.h>
#include <Type/Semaphore.h>
#include <Type/Fence.h>
#include <Type/Swapchain.h>
#include <Type/Image.h>
#include <Type/Sampler.h>
#include <Type/CommandPool.h>
#include <Type/CommandBuffer.h>
#include <Type/Framebuffer.h>
#include <Type/ShaderModule.h>
#include <Type/DescriptorSet.h>
#include <Type/GraphicsPipeline.h>
#include <Type/Memory.h>
#include <Type/Buffer.h>
#include <Type/DescriptorPool.h>



namespace vkpp
{



enum class DeviceQueueCreateFlagBits
{};

VKPP_ENUM_BIT_MASK_FLAGS(DeviceQueueCreate)



class QueueCreateInfo : public internal::VkTrait<QueueCreateInfo, VkDeviceQueueCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eQueue;

public:
    const void*                 pNext{ nullptr };
    DeviceQueueCreateFlags      flags;
    uint32_t                    queueFamilyIndex{ 0 };
    uint32_t                    queueCount{ 0 };
    const float*                pQueuePriorities{ nullptr };

    DEFINE_CLASS_MEMBER(QueueCreateInfo)

    QueueCreateInfo(uint32_t aQueueFamilyIndex, uint32_t aQueueCount, const float* apQueuePriorities, const DeviceQueueCreateFlags& aFlags = DefaultFlags)
        :flags(aFlags), queueFamilyIndex(aQueueFamilyIndex), queueCount(aQueueCount), pQueuePriorities(apQueuePriorities)
    {}

    template <typename P, typename = EnableIfValueType<ValueType<P>, float>>
    QueueCreateInfo(uint32_t aQueueFamilyIndex,P&& aQueuePriorities, const DeviceQueueCreateFlags& aFlags = DefaultFlags)
        : QueueCreateInfo(aQueueFamilyIndex, static_cast<uint32_t>(aQueuePriorities.size()), aQueuePriorities.data(), aFlags)
    {
        StaticLValueRefAssert(P, aQueuePriorities);
    }

    QueueCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    QueueCreateInfo& SetFlags(const DeviceQueueCreateFlags& aFlags)
    {
        flags = aFlags;

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

    template <typename P, typename = EnableIfValueType<ValueType<P>, float>>
    QueueCreateInfo& SetQueuePriorities(P&& aQueuePriorities)
    {
        StaticLValueRefAssert(P, aQueuePriorities);

        return SetQueuePriorities(static_cast<uint32_t>(aQueuePriorities.size()), aQueuePriorities.data());
    }
};

ConsistencyCheck(QueueCreateInfo, pNext, flags, queueFamilyIndex, queueCount, pQueuePriorities)



enum class DeviceCreateFlagBits
{};

VKPP_ENUM_BIT_MASK_FLAGS(DeviceCreate)



class LogicalDeviceCreateInfo : public internal::VkTrait<LogicalDeviceCreateInfo, VkDeviceCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eDevice;

public:
    const void*                             pNext{ nullptr };
    DeviceCreateFlags                       flags;
    uint32_t                                queueCreateInfoCount{ 0 };
    const QueueCreateInfo*                  pQueueCreateInfos{ nullptr };

private:
    [[deprecated]] uint32_t                 enabledLayerCount{ 0 };
    [[deprecated]] const char* const*       ppEnabledLayerNames{ nullptr };

public:
    uint32_t                                enabledExtensionCount{ 0 };
    const char* const*                      ppEnabledExtensionNames{ nullptr };
    const PhysicalDeviceFeatures*           pEnabledFeatures{ nullptr };

    DEFINE_CLASS_MEMBER(LogicalDeviceCreateInfo)

    LogicalDeviceCreateInfo(uint32_t aQueueCreateInfoCount, const QueueCreateInfo* apQueueCreateInfos,
        uint32_t aEnalbedExtensionCount, const char* const* appEnabledExtensionNames, const PhysicalDeviceFeatures* apEnabledFeatures, const DeviceCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), queueCreateInfoCount(aQueueCreateInfoCount), pQueueCreateInfos(apQueueCreateInfos),
          enabledExtensionCount(aEnalbedExtensionCount), ppEnabledExtensionNames(appEnabledExtensionNames), pEnabledFeatures(apEnabledFeatures)
    {}

    template <typename Q, typename E, typename = EnableIfValueType<ValueType<Q>, QueueCreateInfo, ValueType<E>, const char*>>
    LogicalDeviceCreateInfo(Q&& aQueueCreateInfos, E&& aEnabledExtensions, const PhysicalDeviceFeatures* apEnabledFeatures = nullptr, const DeviceCreateFlags& aFlags = DefaultFlags)
        : LogicalDeviceCreateInfo(static_cast<uint32_t>(aQueueCreateInfos.size()), aQueueCreateInfos.data(),
          static_cast<uint32_t>(aEnabledExtensions.size()), aEnabledExtensions.data(), apEnabledFeatures, aFlags)
    {
        StaticLValueRefAssert(Q, aQueueCreateInfos);
        StaticLValueRefAssert(E, aEnabledExtensions);
    }

    LogicalDeviceCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    LogicalDeviceCreateInfo& SetFlags(const DeviceCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    LogicalDeviceCreateInfo& SetQueueCreateInfo(uint32_t aQueueCreateInfoCountconst, const QueueCreateInfo* apQueueCreateInfos)
    {
        queueCreateInfoCount    = aQueueCreateInfoCountconst;
        pQueueCreateInfos       = apQueueCreateInfos;

        return *this;
    }

    template <typename Q, typename = EnableIfValueType<ValueType<Q>, QueueCreateInfo>>
    LogicalDeviceCreateInfo& SetQueueCreateInfo(Q&& aQueueCreateInfos)
    {
        StaticLValueRefAssert(Q, aQueueCreateInfos);

        return SetQueueCreateInfo(static_cast<uint32_t>(aQueueCreateInfos.size()), aQueueCreateInfos.data());
    }

    LogicalDeviceCreateInfo& SetEnabledExtensions(uint32_t aEnabledExtensionCount, const char* const* appEnabledExtensionNames)
    {
        enabledExtensionCount   = aEnabledExtensionCount;
        ppEnabledExtensionNames = appEnabledExtensionNames;

        return *this;
    }

    template <typename E, typename = EnableIfValueType<ValueType<E>, const char*>>
    LogicalDeviceCreateInfo& SetEnabledExtensions(E&& aEnabledExtensionNames)
    {
        StaticLValueRefAssert(E, aEnabledExtensionNames);

        return SetEnabledExtensions(static_cast<uint32_t>(aEnabledExtensionNames.size()), aEnabledExtensionNames.data());
    }
};

ConsistencyCheck(LogicalDeviceCreateInfo, pNext, flags, queueCreateInfoCount, pQueueCreateInfos, enabledExtensionCount, ppEnabledExtensionNames, pEnabledFeatures)



class LogicalDevice : public internal::VkTrait<LogicalDevice, VkDevice>
{
private:
   VkDevice mDevice{ VK_NULL_HANDLE };

public:
    DEFINE_CLASS_MEMBER(LogicalDevice)

    LogicalDevice(std::nullptr_t) noexcept
    {}

    LogicalDevice(LogicalDevice&& aLogicalDevice) noexcept : mDevice(std::move(aLogicalDevice.mDevice))
    {}

    LogicalDevice(const PhysicalDevice& aPhysicalDevice, const LogicalDeviceCreateInfo& aLogicalDeviceCreateInfo)
    {
        Reset(aPhysicalDevice, aLogicalDeviceCreateInfo);
    }

    LogicalDevice& operator=(LogicalDevice&& aLogicalDevice) noexcept
    {
        mDevice = std::move(aLogicalDevice.mDevice);

        return *this;
    }

    template <typename T = DefaultAllocationCallbacks>
    void Reset(const T& aAllocator = DefaultAllocator)
    {
        vkDestroyDevice(mDevice, &aAllocator);
        mDevice = VK_NULL_HANDLE;
    }

    template <typename T = DefaultAllocationCallbacks>
    void Reset(const PhysicalDevice& aPhysicalDevice, const LogicalDeviceCreateInfo& aLogicalDeviceCreateInfo, const T& aAllocator = DefaultAllocator)
    {
        ThrowIfFailed(vkCreateDevice(aPhysicalDevice, &aLogicalDeviceCreateInfo, &aAllocator, &mDevice));
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

    template <typename T = DefaultAllocationCallbacks>
    Semaphore CreateSemaphore(const SemaphoreCreateInfo& aSemaphoreCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        Semaphore lSemaphore;
        ThrowIfFailed(vkCreateSemaphore(mDevice, &aSemaphoreCreateInfo, &aAllocator, &lSemaphore));

        return lSemaphore;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroySemaphore(const Semaphore& aSemaphore, const T& aAllocator = DefaultAllocator) const
    {
        vkDestroySemaphore(mDevice, aSemaphore, &aAllocator);
    }

    template <typename T = DefaultAllocationCallbacks>
    Fence CreateFence(const FenceCreateInfo& aFenceCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        Fence lFence;
        ThrowIfFailed(vkCreateFence(mDevice, &aFenceCreateInfo, &aAllocator, &lFence));

        return lFence;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroyFence(const Fence& aFence, const T& aAllocator = DefaultAllocator) const
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

    void WaitForFence(const Fence& aFence, bool aWaitAll = false, uint64_t aTimeout = UINT64_MAX) const
    {
        ThrowIfFailed(vkWaitForFences(mDevice, 1, &aFence, aWaitAll, aTimeout));
    }

    void WaitForFences(uint32_t aFenceCount, const Fence* apFences, bool aWaitAll = false, uint64_t aTimeout = UINT64_MAX) const
    {
        assert(aFenceCount != 0 && apFences != nullptr);

        ThrowIfFailed(vkWaitForFences(mDevice, aFenceCount, &apFences[0], aWaitAll, aTimeout));
    }

    void WaitForFences(const std::vector<Fence>& aFences, bool aWaitAll = false, uint64_t aTimeout = UINT64_MAX) const
    {
        return WaitForFences(static_cast<uint32_t>(aFences.size()), aFences.data(), aWaitAll, aTimeout);
    }

    template <std::size_t F>
    void WaitForFences(const std::array<Fence, F>& aFences, bool aWaitAll = false, uint64_t aTimeout = UINT64_MAX) const
    {
        return WaitForFences(static_cast<uint32_t>(aFences.size()), aFences.data(), aWaitAll, aTimeout);
    }

    template <typename T = DefaultAllocationCallbacks>
    khr::Swapchain CreateSwapchain(const khr::SwapchainCreateInfo& aSwapchainCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        khr::Swapchain lSwapchain;
        ThrowIfFailed(vkCreateSwapchainKHR(mDevice, &aSwapchainCreateInfo, &aAllocator, &lSwapchain));

        return lSwapchain;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroySwapchain(const khr::Swapchain& aSwapchain, const T& aAllocator = DefaultAllocator) const
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

    template <typename T = DefaultAllocationCallbacks>
    CommandPool CreateCommandPool(const CommandPoolCreateInfo& aCommandPoolCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        CommandPool lCommandPool;
        ThrowIfFailed(vkCreateCommandPool(mDevice, &aCommandPoolCreateInfo, &aAllocator, &lCommandPool));

        return lCommandPool;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroyCommandPool(const CommandPool& aCommandPool, const T& aAllocator = DefaultAllocator) const
    {
        vkDestroyCommandPool(mDevice, aCommandPool, &aAllocator);
    }

    void ResetCommandPool(const CommandPool& aCommandPool, const CommandPoolResetFlags& aFlags) const
    {
        ThrowIfFailed(vkResetCommandPool(mDevice, aCommandPool, static_cast<VkCommandPoolResetFlags>(aFlags)));
    }

    CommandBuffer AllocateCommandBuffer(const CommandBufferAllocateInfo& aCommandBufferAllocateInfo) const
    {
        assert(aCommandBufferAllocateInfo.commandBufferCount == 1);

        CommandBuffer lCommandBuffer;
        ThrowIfFailed(vkAllocateCommandBuffers(mDevice, &aCommandBufferAllocateInfo, &lCommandBuffer));

        return lCommandBuffer;
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

    template <typename T = DefaultAllocationCallbacks>
    DescriptorPool CreateDescriptorPool(const DescriptorPoolCreateInfo& aDescriptorPoolCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        DescriptorPool lDescriptorPool;
        ThrowIfFailed(vkCreateDescriptorPool(mDevice, &aDescriptorPoolCreateInfo, &aAllocator, &lDescriptorPool));

        return lDescriptorPool;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroyDescriptorPool(const DescriptorPool& aDescriptorPool, const T& aAllocator = DefaultAllocator) const
    {
        vkDestroyDescriptorPool(mDevice, aDescriptorPool, &aAllocator);
    }

    void ResetDescriptorPool(const DescriptorPool& aDescriptorPool, const DescriptorPoolResetFlags& aFlags = DefaultFlags) const
    {
        ThrowIfFailed(vkResetDescriptorPool(mDevice, aDescriptorPool, aFlags));
    }

    DescriptorSet AllocateDescriptorSet(const DescriptorSetAllocateInfo& aDescriptorSetAllocateInfo) const
    {
        assert(aDescriptorSetAllocateInfo.descriptorSetCount == 1);

        DescriptorSet lDescriptorSet;
        ThrowIfFailed(vkAllocateDescriptorSets(mDevice, &aDescriptorSetAllocateInfo, &lDescriptorSet));

        return lDescriptorSet;
    }

    std::vector<DescriptorSet> AllocateDescriptorSets(const DescriptorSetAllocateInfo& aDescriptorSetAllocateInfo) const
    {
        std::vector<DescriptorSet> lDescriptorSets(aDescriptorSetAllocateInfo.descriptorSetCount);
        ThrowIfFailed(vkAllocateDescriptorSets(mDevice, &aDescriptorSetAllocateInfo, &lDescriptorSets[0]));

        return lDescriptorSets;
    }

    void FreeDescriptorSet(const DescriptorPool& aDescriptorPool, const DescriptorSet& aDescriptorSet) const
    {
        assert(aDescriptorPool);

        vkFreeDescriptorSets(mDevice, aDescriptorPool, 1, &aDescriptorSet);
    }

    void FreeDescriptorSets(const DescriptorPool& aDescriptorPool, uint32_t aDescriptorSetCount, const DescriptorSet* apDescriptorSets) const
    {
        assert(aDescriptorPool);
        assert(aDescriptorSetCount != 0 && apDescriptorSets != nullptr);

        vkFreeDescriptorSets(mDevice, aDescriptorPool, aDescriptorSetCount, &apDescriptorSets[0]);
    }

    void FreeDescriptorSets(const DescriptorPool& aDescriptorPool, const std::vector<DescriptorSet>& aDescriptorSets) const
    {
        FreeDescriptorSets(aDescriptorPool, static_cast<uint32_t>(aDescriptorSets.size()), aDescriptorSets.data());
    }

    void UpdateDescriptorSet(const WriteDescriptorSetInfo& aDescriptorWrite) const
    {
        UpdateDescriptorSets(1, aDescriptorWrite.AddressOf());
    }

    void UpdateDescriptorSet(const CopyDescriptorSetInfo& aDescriptorCopy) const
    {
        UpdateDescriptorSets(0, nullptr, 1, aDescriptorCopy.AddressOf());
    }

    void UpdateDescriptorSets(uint32_t aDescriptorWriteCount, const WriteDescriptorSetInfo* apDescriptorWrites, uint32_t aDescriptorCopyCount = 0, const CopyDescriptorSetInfo* apDescriptorCopies = nullptr) const
    {
        vkUpdateDescriptorSets(mDevice, aDescriptorWriteCount, &apDescriptorWrites[0], aDescriptorCopyCount, &apDescriptorCopies[0]);
    }

    void UpdateDescriptorSets(const std::vector<WriteDescriptorSetInfo>& aDescriptorWrites, const std::vector<CopyDescriptorSetInfo>& aDescriptorCopies) const
    {
        UpdateDescriptorSets(static_cast<uint32_t>(aDescriptorWrites.size()), aDescriptorWrites.data(), static_cast<uint32_t>(aDescriptorCopies.size()), aDescriptorCopies.data());
    }

    template <std::size_t W, std::size_t C>
    void UpdateDescriptorSets(const std::array<WriteDescriptorSetInfo, W>& aDescriptorWrites, const std::array<CopyDescriptorSetInfo, C>& aDescriptorCopies)
    {
        UpdateDescriptorSets(static_cast<uint32_t>(aDescriptorWrites.size()), aDescriptorWrites.data(), static_cast<uint32_t>(aDescriptorCopies.size()), aDescriptorCopies.data());
    }

    template <typename T = DefaultAllocationCallbacks>
    RenderPass CreateRenderPass(const RenderPassCreateInfo& aRenderPassCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        RenderPass lRenderPass;
        ThrowIfFailed(vkCreateRenderPass(mDevice, &aRenderPassCreateInfo, &aAllocator, &lRenderPass));

        return lRenderPass;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroyRenderPass(const RenderPass& aRenderPass, const T& aAllocator = DefaultAllocator) const
    {
        vkDestroyRenderPass(mDevice, aRenderPass, &aAllocator);
    }

    template <typename T = DefaultAllocationCallbacks>
    Buffer CreateBuffer(const BufferCreateInfo& aBufferCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        Buffer lBuffer;
        ThrowIfFailed(vkCreateBuffer(mDevice, &aBufferCreateInfo, &aAllocator, &lBuffer));

        return lBuffer;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroyBuffer(const Buffer& aBuffer, const T& aAllocator = DefaultAllocator) const
    {
        vkDestroyBuffer(mDevice, aBuffer, &aAllocator);
    }

    template <typename T = DefaultAllocationCallbacks>
    BufferView CreateBufferView(const BufferViewCreateInfo& aBufferViewCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        BufferView lBufferView;
        ThrowIfFailed(vkCreateBufferView(mDevice, &aBufferViewCreateInfo, &aAllocator, &lBufferView));

        return lBufferView;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroyBufferView(const BufferView& aBufferView, const T& aAllocator = DefaultAllocator) const
    {
        vkDestroyBufferView(mDevice, aBufferView, &aAllocator);
    }

    template <typename T = DefaultAllocationCallbacks>
    Image CreateImage(const ImageCreateInfo& aImageCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        Image lImage;
        ThrowIfFailed(vkCreateImage(mDevice, &aImageCreateInfo, &aAllocator, &lImage));

        return lImage;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroyImage(const Image& aImage, const T& aAllocator = DefaultAllocator) const
    {
        assert(aImage);

        vkDestroyImage(mDevice, aImage, &aAllocator);
    }

    template <typename T = DefaultAllocationCallbacks>
    ImageView CreateImageView(const ImageViewCreateInfo& aImageViewCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        ImageView lImageView;
        ThrowIfFailed(vkCreateImageView(mDevice, &aImageViewCreateInfo, &aAllocator, &lImageView));

        return lImageView;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroyImageView(const ImageView& aImageView, const T& aAllocator = DefaultAllocator) const
    {
        vkDestroyImageView(mDevice, aImageView, &aAllocator);
    }

    SubresourceLayout GetImageSubresourceLayout(const Image& aImage, const ImageSubresource& aSubresource) const
    {
        SubresourceLayout lSubresourceLayout;
        vkGetImageSubresourceLayout(mDevice, aImage, &aSubresource, &lSubresourceLayout);

        return lSubresourceLayout;
    }

    template <typename T = DefaultAllocationCallbacks>
    Sampler CreateSampler(const SamplerCreateInfo& aSamplerCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        Sampler lSampler;
        ThrowIfFailed(vkCreateSampler(mDevice, &aSamplerCreateInfo, &aAllocator, &lSampler));

        return lSampler;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroySampler(const Sampler& aSampler, const T& aAllocator = DefaultAllocator) const
    {
        vkDestroySampler(mDevice, aSampler, &aAllocator);
    }

    template <typename T = DefaultAllocationCallbacks>
    Framebuffer CreateFramebuffer(const FramebufferCreateInfo& aFramebufferCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        Framebuffer lFramebuffer;
        ThrowIfFailed(vkCreateFramebuffer(mDevice, &aFramebufferCreateInfo, &aAllocator, &lFramebuffer));

        return lFramebuffer;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroyFramebuffer(const Framebuffer& aFramebuffer, const T& aAllocator = DefaultAllocator) const
    {
        vkDestroyFramebuffer(mDevice, aFramebuffer, &aAllocator);
    }

    template <typename T = DefaultAllocationCallbacks>
    ShaderModule CreateShaderModule(const ShaderModuleCreateInfo& aShaderModuleCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        ShaderModule lShaderModule;
        ThrowIfFailed(vkCreateShaderModule(mDevice, &aShaderModuleCreateInfo, &aAllocator, &lShaderModule));

        return lShaderModule;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroyShaderModule(const ShaderModule& aShaderModule, const T& aAllocator = DefaultAllocator) const
    {
        vkDestroyShaderModule(mDevice, aShaderModule, &aAllocator);
    }

    template <typename T = DefaultAllocationCallbacks>
    DescriptorSetLayout CreateDescriptorSetLayout(const DescriptorSetLayoutCreateInfo& aDescriptorSetLayoutCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        DescriptorSetLayout lDescriptorSetLayout;
        ThrowIfFailed(vkCreateDescriptorSetLayout(mDevice, &aDescriptorSetLayoutCreateInfo, &aAllocator, &lDescriptorSetLayout));

        return lDescriptorSetLayout;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroyDescriptorSetLayout(const DescriptorSetLayout& aDescriptorSetLayout, const T& aAllocator = DefaultAllocator) const
    {
        vkDestroyDescriptorSetLayout(mDevice, aDescriptorSetLayout, &aAllocator);
    }

    template <typename T = DefaultAllocationCallbacks>
    PipelineLayout CreatePipelineLayout(const PipelineLayoutCreateInfo& aPipelineLayoutCreateInfo, const T& aAllocator = DefaultAllocator) const
    {
        PipelineLayout lPipelineLayout;
        ThrowIfFailed(vkCreatePipelineLayout(mDevice, &aPipelineLayoutCreateInfo, &aAllocator, &lPipelineLayout));

        return lPipelineLayout;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroyPipelineLayout(const PipelineLayout& aPipelineLayout, const T& aAllocator = DefaultAllocator) const
    {
        vkDestroyPipelineLayout(mDevice, aPipelineLayout, &aAllocator);
    }

    Pipeline CreateGraphicsPipeline(uint32_t aCreateInfoCount, const GraphicsPipelineCreateInfo* apGraphicsPipelineCraeteInfos) const
    {
        return CreateGraphicsPipeline(VK_NULL_HANDLE, aCreateInfoCount, apGraphicsPipelineCraeteInfos);
    }

    template <typename T = DefaultAllocationCallbacks>
    Pipeline CreateGraphicsPipeline(const PipelineCache& aPipelineCache, uint32_t aCreateInfoCount,
        const GraphicsPipelineCreateInfo* apGraphicsPipelineCraeteInfos, const T& aAllocator = DefaultAllocator) const
    {
        assert(aCreateInfoCount > 0 && apGraphicsPipelineCraeteInfos != nullptr);

        Pipeline lPipeline;
        ThrowIfFailed(vkCreateGraphicsPipelines(mDevice, aPipelineCache, aCreateInfoCount, &apGraphicsPipelineCraeteInfos[0], &aAllocator, &lPipeline));

        return lPipeline;
    }

    template <typename T = DefaultAllocationCallbacks>
    void DestroyPipeline(const Pipeline& aPipeline, const T& aAllocator = DefaultAllocator) const
    {
        vkDestroyPipeline(mDevice, aPipeline, &aAllocator);
    }

    template <typename T = DefaultAllocationCallbacks>
    DeviceMemory AllocateMemory(const MemoryAllocateInfo& aMemoryAllocationInfo, const T& aAllocator = DefaultAllocator) const
    {
        DeviceMemory lDeviceMemory;
        ThrowIfFailed(vkAllocateMemory(mDevice, &aMemoryAllocationInfo, &aAllocator, &lDeviceMemory));

        return lDeviceMemory;
    }

    template <typename T = DefaultAllocationCallbacks>
    void FreeMemory(const DeviceMemory& aDeviceMemory, const T& aAllocator = DefaultAllocator) const
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

StaticSizeCheck(LogicalDevice)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_LOGICAL_DEVICE_H__
