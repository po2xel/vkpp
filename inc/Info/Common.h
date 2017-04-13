#ifndef __VKPP_INFO_COMMON_H__
#define __VKPP_INFO_COMMON_H__



#include <cstring>
#include <cstdint>

#include <Type/Structure.h>
#include <Type/VkTrait.h>
#include <vector>
#include <Info/Flags.h>



#define DEFAULT_VK_API_VERSION VK_API_VERSION_1_0



#define DEFINE_CLASS_MEMBER(Class) \
Class(void) = default; \
\
explicit Class(const Class::VkType& aRhs) \
{ \
    std::memcpy(this, &aRhs, sizeof(Class)); \
} \
\
Class& operator=(const Class::VkType& aRhs) \
{ \
    std::memcpy(this, &aRhs, sizeof(Class)); \
    return *this; \
}



#define StaticSizeCheck(Class) static_assert(sizeof(Class) == sizeof(Class::VkType))



namespace vkpp
{



using Bool32            = VkBool32;
using DeviceSize        = VkDeviceSize;
using Result            = VkResult;
using ClearColorValue   = VkClearColorValue;


template <typename T>
using Array = std::vector<T>;



inline void ThrowIfFailed(VkResult aResult)
{
    if (aResult != VK_SUCCESS)
        throw;
}



enum class SharingMode
{
    eExclusive = VK_SHARING_MODE_EXCLUSIVE,
    eConcurrent = VK_SHARING_MODE_CONCURRENT
};



enum class DependencyFlagBits
{
    eByRegion = VK_DEPENDENCY_BY_REGION_BIT,
    eViewLocalKHX = VK_DEPENDENCY_VIEW_LOCAL_BIT_KHX,
    eDeviceGroupKHX = VK_DEPENDENCY_DEVICE_GROUP_BIT_KHX
};

using DependencyFlags = internal::Flags<DependencyFlagBits, VkDependencyFlags>;



}                    // End of namespace vkpp.



#endif             // __VKPP_INFO_COMMON_H__
