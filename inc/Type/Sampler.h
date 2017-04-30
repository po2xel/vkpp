#ifndef __VKPP_TYPE_SAMPLER_H__
#define __VKPP_TYPE_SAMPLER_H__



#include <Info/Common.h>



namespace vkpp
{



class Sampler : public internal::VkTrait<Sampler, VkSampler>
{
private:
    VkSampler mSampler{ VK_NULL_HANDLE };

public:
    Sampler(void) = default;

    Sampler(std::nullptr_t)
    {}

    explicit Sampler(VkSampler aSampler) : mSampler(aSampler)
    {}
};

StaticSizeCheck(Sampler)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_SAMPLER_H__
