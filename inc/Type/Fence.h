#ifndef __VKPP_TYPE_FENCE_H__
#define __VKPP_TYPE_FENCE_H__



#include <Info/Common.h>



namespace vkpp
{



class Fence : public internal::VkTrait<Fence, VkFence>
{
private:
    VkFence mFence{ VK_NULL_HANDLE };

public:
    Fence(void) = default;

    Fence(std::nullptr_t)
    {}

    explicit Fence(VkFence aFence) : mFence(aFence)
    {}
};

StaticSizeCheck(Fence);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_FENCE_H__
