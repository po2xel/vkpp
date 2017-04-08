#ifndef __VKPP_INFO_PRESENT_INFO_H__
#define __VKPP_INFO_PRESENT_INFO_H__


#include <vector>
#include <array>

#include <Info/Common.h>
#include <Type/Semaphore.h>
#include <Type/Swapchain.h>



namespace vkpp::khr
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
        waitSemaphoreCount  = aWaitSemaphoreCount;
        pWaitSemaphores     = apWaitSemaphores;

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
        swapchainCount  = aSwapchainCount;
        pSwapchains     = apSwapchains;
        pImageIndices   = apImageIndices;
        pResults        = apResults;

        return *this;
    }
};



}                   // End of namespace vkpp::khr.



#endif              // __VKPP_INFO_PRESENT_INFO_H__
