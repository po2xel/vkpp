#ifndef __VKPP_TYPE_QUEUE_H__
#define __VKPP_TYPE_QUEUE_H__



#include <initializer_list>

#include <Info/Common.h>
#include <Info/PipelineStage.h>

#include <Type/Fence.h>
#include <Type/Semaphore.h>
#include <Type/CommandBuffer.h>
#include <Type/Swapchain.h>



namespace vkpp
{



class SubmitInfo : public internal::VkTrait<SubmitInfo, VkSubmitInfo>
{
private:
    const internal::Structure sType = internal::Structure::eSubmitInfo;

    SubmitInfo(const Semaphore&, PipelineStageFlags&&, CommandBuffer&, Semaphore&) noexcept = delete;

    SubmitInfo& SetWaitSemaphore(const Semaphore&, PipelineStageFlags&&) noexcept = delete;
    SubmitInfo& SetCommandBuffer(CommandBuffer&&) noexcept = delete;
    SubmitInfo& SetSignalSemaphore(Semaphore&&) noexcept = delete;

public:
    const void*                 pNext{ nullptr };
    uint32_t                    waitSemaphoreCount{ 0 };
    const Semaphore*            pWaitSemaphores{ nullptr };
    const PipelineStageFlags*   pWaitDstStageMask{ nullptr };
    uint32_t                    commandBufferCount{ 0 };
    const CommandBuffer*        pCommandBuffers{ nullptr };
    uint32_t                    signalSemaphoreCount{ 0 };
    const Semaphore*            pSignalSemaphores{ nullptr };

    DEFINE_CLASS_MEMBER(SubmitInfo)

    SubmitInfo(uint32_t aWaitSemaphoreCount, const Semaphore* apWaitSemaphores, const PipelineStageFlags* apWaitDstStageMask,
        uint32_t aCommandBufferCount, const CommandBuffer* apCommandBuffers, uint32_t aSignalSemaphoreCount = 0, const Semaphore* apSignalSemaphores = nullptr) noexcept 
        : waitSemaphoreCount(aWaitSemaphoreCount), pWaitSemaphores(apWaitSemaphores), pWaitDstStageMask(apWaitDstStageMask),
          commandBufferCount(aCommandBufferCount), pCommandBuffers(apCommandBuffers), signalSemaphoreCount(aSignalSemaphoreCount), pSignalSemaphores(apSignalSemaphores)
    {}

    SubmitInfo(const Semaphore& aWaitSemaphore, const PipelineStageFlags& aWaitDstStageMask, const CommandBuffer& aCommandBuffer, const Semaphore& aSignalSemaphore) noexcept
        : SubmitInfo(1, aWaitSemaphore.AddressOf(), &aWaitDstStageMask, 1, aCommandBuffer.AddressOf(), 1, aSignalSemaphore.AddressOf())
    {}

    template <typename C, typename = EnableIfValueType<ValueType<C>, CommandBuffer>>
    explicit SubmitInfo(C&& aCommandBuffers) noexcept  : commandBufferCount(SizeOf<uint32_t>(aCommandBuffers)), pCommandBuffers(DataOf(aCommandBuffers))
    {
        StaticLValueRefAssert(C, aCommandBuffers);
    }

    SubmitInfo(const CommandBuffer& aCommandBuffer) noexcept : commandBufferCount(1), pCommandBuffers(aCommandBuffer.AddressOf())
    {}

    SubmitInfo& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    SubmitInfo& SetWaitSemaphores(uint32_t aWaitSemaphoreCount, const Semaphore* apWaitSemaphores, const PipelineStageFlags* apWaitDstStageMask) noexcept
    {
        waitSemaphoreCount  = aWaitSemaphoreCount;
        pWaitSemaphores     = apWaitSemaphores;
        pWaitDstStageMask   = apWaitDstStageMask;

        return *this;
    }

    SubmitInfo& SetWaitSemaphore(const Semaphore& aWaitSemaphore, const PipelineStageFlags& aWaitDstStageMask) noexcept
    {
        return SetWaitSemaphores(1, aWaitSemaphore.AddressOf(), &aWaitDstStageMask);
    }

    template <typename S, typename F, typename = EnableIfValueType<ValueType<S>, Semaphore, ValueType<F>, PipelineStageFlags>>
    SubmitInfo& SetWaitSemaphores(S&& aWaitSemaphores, F&& aWaitDstStageMasks)
    {
        static_assert(SizeOf<uint32_t>(aWaitSemaphores) <= SizeOf<uint32_t>(aWaitDstStageMasks));

        return SetWaitSemaphores(SizeOf<uint32_t>(aWaitSemaphores), DataOf(aWaitSemaphores), DataOf(aWaitDstStageMasks));
    }

    SubmitInfo& ClearWaitSemaphores(void) noexcept
    {
        return SetWaitSemaphores(0, nullptr, nullptr);
    }

    SubmitInfo& SetCommandBuffers(uint32_t aCommandBufferCount, const CommandBuffer* apCommandBuffers) noexcept
    {
        commandBufferCount  = aCommandBufferCount;
        pCommandBuffers     = apCommandBuffers;

        return *this;
    }

    SubmitInfo& SetCommandBuffer(const CommandBuffer& aCommandBuffer) noexcept
    {
        return SetCommandBuffers(1, aCommandBuffer.AddressOf());
    }

    template <typename C, typename = EnableIfValueType<ValueType<C>, CommandBuffer>>
    SubmitInfo& SetCommandBuffers(C&& aCommandBuffers) noexcept
    {
        StaticLValueRefAssert(C, aCommandBuffers);

        return SetCommandBuffers(SizeOf<uint32_t>(aCommandBuffers), DataOf(aCommandBuffers));
    }

    SubmitInfo& SetSignalSemaphores(uint32_t aSignalSemaphoreCount, const Semaphore* apSignalSemaphores) noexcept
    {
        signalSemaphoreCount = aSignalSemaphoreCount;
        pSignalSemaphores    = apSignalSemaphores;

        return *this;
    }

    SubmitInfo& SetSignalSemaphore(const Semaphore& aSignalSemaphore) noexcept
    {
        return SetSignalSemaphores(1, aSignalSemaphore.AddressOf());
    }

    template <typename S, typename = EnableIfValueType<ValueType<S>, Semaphore>>
    SubmitInfo& SetSignalSemaphores(S&& aSignalSemaphores) noexcept
    {
        StaticLValueRefAssert(S, aSignalSemaphores);

        return SetSignalSemaphores(SizeOf<uint32_t>(aSignalSemaphores), DataOf(aSignalSemaphores));
    }

    SubmitInfo& ClearSignalSemaphores(void) noexcept
    {
        return SetSignalSemaphores(0, nullptr);
    }
};

ConsistencyCheck(SubmitInfo, pNext, waitSemaphoreCount, pWaitSemaphores, pWaitDstStageMask, commandBufferCount, pCommandBuffers, signalSemaphoreCount, pSignalSemaphores)



namespace khr
{



class PresentInfo : public internal::VkTrait<PresentInfo, VkPresentInfoKHR>
{
private:
    const internal::Structure sType = internal::Structure::ePresent;

public:
    const void*         pNext{ nullptr };
    uint32_t            waitSemaphoreCount{ 0 };
    const Semaphore*    pWaitSemaphores{ nullptr };
    uint32_t            swapchainCount{ 0 };
    const Swapchain*    pSwapchains{ nullptr };
    const uint32_t*     pImageIndices{ nullptr };
    Result*             pResults{ nullptr };

    DEFINE_CLASS_MEMBER(PresentInfo)

    PresentInfo(uint32_t aWaitSemaphoreCount, const Semaphore* apWaitSemaphores,
        uint32_t aSwapchainCount, const Swapchain* apSwapchains, const uint32_t* apImageIndices, Result* apResults = nullptr) noexcept
    : waitSemaphoreCount(aWaitSemaphoreCount), pWaitSemaphores(apWaitSemaphores),
      swapchainCount(aSwapchainCount), pSwapchains(apSwapchains), pImageIndices(apImageIndices), pResults(apResults)
    {}

    PresentInfo& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    PresentInfo& SetWaitSemaphores(uint32_t aWaitSemaphoreCount, const Semaphore* apWaitSemaphores) noexcept
    {
        waitSemaphoreCount  = aWaitSemaphoreCount;
        pWaitSemaphores     = apWaitSemaphores;

        return *this;
    }

    template <typename S, typename = EnableIfValueType<ValueType<S>, Semaphore>>
    PresentInfo& SetWaitSemaphores(S&& aWaitSemaphores) noexcept
    {
        StaticLValueRefAssert(S, aWaitSemaphores);

        return SetWaitSemaphores(SizeOf<uint32_t>(aWaitSemaphores), DataOf(aWaitSemaphores));
    }

    PresentInfo& SetSwapchainImages(uint32_t aSwapchainCount, const Swapchain* apSwapchains, const uint32_t* apImageIndices, Result* apResults) noexcept
    {
        swapchainCount  = aSwapchainCount;
        pSwapchains     = apSwapchains;
        pImageIndices   = apImageIndices;
        pResults        = apResults;

        return *this;
    }

    // TODO: Container overload.
};

ConsistencyCheck(PresentInfo, pNext, waitSemaphoreCount, pWaitSemaphores, swapchainCount, pSwapchains, pImageIndices, pResults)



}                   // End of namespace khr.



class Queue : public internal::VkTrait<Queue, VkQueue>
{
private:
    VkQueue mQueue{ VK_NULL_HANDLE };

public:
    DEFINE_CLASS_MEMBER(Queue)

    Queue(std::nullptr_t) noexcept
    {}

    explicit Queue(VkQueue aQueue) noexcept : mQueue(aQueue)
    {}

    VkResult Wait(void) const
    {
        return vkQueueWaitIdle(mQueue);
    }

    void Submit(const SubmitInfo& aSubmit) const
    {
        return Submit(1, aSubmit.AddressOf());
    }

    void Submit(const SubmitInfo& aSubmit, const Fence& aFence) const
    {
        return Submit(1, aSubmit.AddressOf(), aFence);
    }

    void Submit(uint32_t aSubmitCount, const SubmitInfo* apSubmits) const
    {
        assert(aSubmitCount != 0 && apSubmits != nullptr);

        ThrowIfFailed(vkQueueSubmit(mQueue, aSubmitCount, &apSubmits[0], VK_NULL_HANDLE));
    }

    void Submit(uint32_t aSubmitCount, const SubmitInfo* apSubmits, const Fence& aFence) const
    {
        assert(aSubmitCount != 0 && apSubmits != nullptr);

        ThrowIfFailed(vkQueueSubmit(mQueue, aSubmitCount, &apSubmits[0], aFence));
    }

    template <typename T, typename = EnableIfValueType<ValueType<T>, SubmitInfo>>
    void Submit(T&& aSubmitInfos, const Fence& aFence) const
    {
        Submit(SizeOf<uint32_t>(aSubmitInfos), DataOf(aSubmitInfos), aFence);
    }

    void Submit(const std::initializer_list<SubmitInfo>& aSubmitInfos, const Fence& aFence) const
    {
        Submit(SizeOf<uint32_t>(aSubmitInfos), DataOf(aSubmitInfos), aFence);
    }

    void Present(const khr::PresentInfo& aPresentInfo) const
    {
        ThrowIfFailed(vkQueuePresentKHR(mQueue, &aPresentInfo));
    }
};

StaticSizeCheck(Queue)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_QUEUE_H__
