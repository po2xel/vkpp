#ifndef __VKPP_INTO_FORMAT_H__
#define __VKPP_INTO_FORMAT_H__



#include <vulkan/vulkan.h>



namespace vkpp
{



enum class Format
{
    eUndefined              = VK_FORMAT_UNDEFINED,

    eR8G8B8A8Unorm          = VK_FORMAT_R8G8B8A8_UNORM,
    eR8G8B8A8sRGB           = VK_FORMAT_R8G8B8A8_SRGB,

    eB8G8R8A8Unorm          = VK_FORMAT_B8G8R8A8_UNORM,
    eB8G8R8A8sRGB           = VK_FORMAT_B8G8R8A8_SRGB,

    eR32G32B32A32SFloat     = VK_FORMAT_R32G32B32A32_SFLOAT
};



}                   // End of namespace vkpp.



#endif              // __VKPP_INTO_FORMAT_H__
