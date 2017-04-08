#ifndef __VKPP_TYPE_QUEUE_H__
#define __VKPP_TYPE_QUEUE_H__



#include <vector>
#include <array>

#include <Info/Common.h>
#include <Info/SubmitInfo.h>
#include <Info/PresentInfo.h>

#include <Type/Fence.h>



namespace vkpp
{



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

    void Submit(uint32_t aSubmitCount, const SubmitInfo* apSubmits)
    {
        ThrowIfFailed(vkQueueSubmit(mQueue, aSubmitCount, &apSubmits[0], nullptr));
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
