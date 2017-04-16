#ifndef __VKPP_TYPE_DESCRIPTOR_SET_H__
#define __VKPP_TYPE_DESCRIPTOR_SET_H__



#include <Info/Common.h>



namespace vkpp
{



class DescriptorSetLayout : public internal::VkTrait<DescriptorSetLayout, VkDescriptorSetLayout>
{
private:
    VkDescriptorSetLayout mDescriptorSetLayout{ VK_NULL_HANDLE };

public:
    DescriptorSetLayout(void) = default;

    DescriptorSetLayout(std::nullptr_t)
    {}

    explicit DescriptorSetLayout(VkDescriptorSetLayout aDescriptorSetLayout) : mDescriptorSetLayout(aDescriptorSetLayout)
    {}
};

StaticSizeCheck(DescriptorSetLayout);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_DESCRIPTOR_SET_H__
