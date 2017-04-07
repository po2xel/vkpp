#ifndef __VKPP_TYPE_SWAPCHAIN_H__
#define __VKPP_TYPE_SWAPCHAIN_H__



#include <Info/Common.h>



namespace vkpp::khr
{



class Swapchain : public internal::VkTrait<Swapchain, VkSwapchainKHR>
{
private:
    VkSwapchainKHR mSwpchain{ VK_NULL_HANDLE };

public:
    Swapchain(void) = default;

    Swapchain(std::nullptr_t)
    {}

    explicit Swapchain(VkSwapchainKHR aSwapchain) : mSwpchain(aSwapchain)
    {}
};



}                   // End of namespace vkpp::khr.



#endif              // __VKPP_TYPE_SWAPCHAIN_H__
