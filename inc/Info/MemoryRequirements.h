#ifndef __VKPP_INFO_MEMORY_REQUIREMENTS_H__
#define __VKPP_INFO_MEMORY_REQUIREMENTS_H__



#include <Info/Common.h>



namespace vkpp
{



struct MemoryRequirements : public internal::VkTrait<MemoryRequirements, VkMemoryRequirements>
{
    DeviceSize      size{ 0 };
    DeviceSize      alignment{ 0 };
    uint32_t        memoryTypeBits{ 0 };
};

ConsistencyCheck(MemoryRequirements, size, alignment, memoryTypeBits)



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_MEMORY_REQUIREMENTS_H__
