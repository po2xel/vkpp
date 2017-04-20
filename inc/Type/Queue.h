#ifndef __VKPP_TYPE_QUEUE_H__
#define __VKPP_TYPE_QUEUE_H__



#include <vector>
#include <array>

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
        uint32_t aCommandBufferCount, const CommandBuffer* apCommandBuffers, uint32_t aSignalSemaphoreCount, const Semaphore* apSignalSemaphores)
        : waitSemaphoreCount(aWaitSemaphoreCount), pWaitSemaphores(apWaitSemaphores), pWaitDstStageMask(apWaitDstStageMask),
        commandBufferCount(aCommandBufferCount), pCommandBuffers(apCommandBuffers), signalSemaphoreCount(aSignalSemaphoreCount), pSignalSemaphores(apSignalSemaphores)
    {}

    SubmitInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    SubmitInfo& SetWaitSemaphores(uint32_t aWaitSemaphoreCount, const Semaphore* apWaitSemaphores, const PipelineStageFlags* apWaitDstStageMask)
    {
        waitSemaphoreCount  = aWaitSemaphoreCount;
        pWaitSemaphores     = apWaitSemaphores;
        pWaitDstStageMask   = apWaitDstStageMask;

        return *this;
    }

    SubmitInfo& SetCommandBuffers(uint32_t aCommandBufferCount, const CommandBuffer* apCommandBuffers)
    {
        commandBufferCount  = aCommandBufferCount;
        pCommandBuffers     = apCommandBuffers;

        return *this;
    }

    SubmitInfo& SetCommandBuffers(const std::vector<CommandBuffer>& aCommandBuffers)
    {
        return SetCommandBuffers(static_cast<uint32_t>(aCommandBuffers.size()), aCommandBuffers.data());
    }

    template <std::size_t C>
    SubmitInfo& SetCommandBuffers(const std::array<CommandBuffer, C>& aCommandBuffers)
    {
        return SetCommandBuffers(static_cast<uint32_t>(aCommandBuffers.size()), aCommandBuffers.data());
    }

    SubmitInfo& SetSignalSemaphores(uint32_t aSignalSemaphoreCount, const Semaphore* apSignalSemaphores)
    {
        signalSemaphoreCount = aSignalSemaphoreCount;
        pSignalSemaphores    = apSignalSemaphores;

        return *this;
    }

    SubmitInfo& SetSignalSemaphores(const std::vector<Semaphore>& aSignalSemaphores)
    {
        return SetSignalSemaphores(static_cast<uint32_t>(aSignalSemaphores.size()), aSignalSemaphores.data());
    }

    template <std::size_t S>
    SubmitInfo& SetSignalSemaphores(const std::array<Semaphore, S>& aSignalSemaphores)
    {
        return SetSignalSemaphores(static_cast<uint32_t>(aSignalSemaphores.size()), aSignalSemaphores.data());
    }
};

StaticSizeCheck(SubmitInfo);



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
            uint32_t aSwapchainCount, const Swapchain* apSwapchains, const uint32_t* apImageIndices, Result* apResults)
        : waitSemaphoreCount(aWaitSemaphoreCount), pWaitSemaphores(apWaitSemaphores),
        swapchainCount(aSwapchainCount), pSwapchains(apSwapchains), pImageIndices(apImageIndices), pResults(apResults)
    {}

    PresentInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    PresentInfo& SetWaitSemaphores(uint32_t aWaitSemaphoreCount, const Semaphore* apWaitSemaphores)
    {
        waitSemaphoreCount = aWaitSemaphoreCount;
        pWaitSemaphores = apWaitSemaphores;

        return *this;
    }

    PresentInfo& SetWaitSemaphores(const std::vector<Semaphore>& aWaitSemaphores)
    {
        return SetWaitSemaphores(static_cast<uint32_t>(aWaitSemaphores.size()), aWaitSemaphores.data());
    }

    template <std::size_t S>
    PresentInfo& SetWaitSemaphores(const std::array<Semaphore, S>& aWaitSemaphores)
    {
        return SetWaitSemaphores(static_cast<uint32_t>(aWaitSemaphores.size()), aWaitSemaphores.data());
    }

    PresentInfo& SetSwapchainImages(uint32_t aSwapchainCount, const Swapchain* apSwapchains, const uint32_t* apImageIndices, Result* apResults)
    {
        swapchainCount = aSwapchainCount;
        pSwapchains = apSwapchains;
        pImageIndices = apImageIndices;
        pResults = apResults;

        return *this;
    }
};



}                   // End of namespace khr.



class Queue : public internal::VkTrait<Queue, VkQueue>
{
private:
    VkQueue mQueue{ VK_NULL_HANDLE };

public:
    DEFINE_CLASS_MEMBER(Queue)

    Queue(std::nullptr_t)
    {}

    explicit Queue(VkQueue aQueue) : mQueue(aQueue)
    {}

    VkResult Wait(void) const
    {
        return vkQueueWaitIdle(mQueue);
    }

    void Submit(uint32_t aSubmitCount, const SubmitInfo* apSubmits) const
    {
        ThrowIfFailed(vkQueueSubmit(mQueue, aSubmitCount, &apSubmits[0], VK_NULL_HANDLE));
    }

    void Submit(uint32_t aSubmitCount, const SubmitInfo* apSubmits, const Fence& aFence) const
    {
        ThrowIfFailed(vkQueueSubmit(mQueue, aSubmitCount, &apSubmits[0], aFence));
    }

    void Submit(const std::vector<SubmitInfo>& aSubmitInfos, const Fence& aFence) const
    {
        ThrowIfFailed(vkQueueSubmit(mQueue, static_cast<uint32_t>(aSubmitInfos.size()), &aSubmitInfos[0], aFence));
    }

    template <std::size_t S>
    void Submit(const std::array<SubmitInfo, S>& aSubmitInfos, const Fence& aFence) const
    {
        ThrowIfFailed(vkQueueSubmit(mQueue, static_cast<uint32_t>(aSubmitInfos.size()), &aSubmitInfos[0], aFence));
    }

    void Present(const khr::PresentInfo& aPresentInfo) const
    {
        ThrowIfFailed(vkQueuePresentKHR(mQueue, &aPresentInfo));
    }
};

StaticSizeCheck(Queue);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_QUEUE_H__
