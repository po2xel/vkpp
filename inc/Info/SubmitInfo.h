#ifndef __VKPP_INFO_SUBMIT_INFO_H__
#define __VKPP_INFO_SUBMIT_INFO_H__



#include <vector>
#include <array>

#include <Info/Common.h>
#include <Info/PipelineStage.h>

#include <Type/Semaphore.h>
#include <Type/CommandBuffer.h>



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



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_SUBMIT_INFO_H__
