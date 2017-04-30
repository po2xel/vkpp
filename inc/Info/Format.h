#ifndef __VKPP_INTO_FORMAT_H__
#define __VKPP_INTO_FORMAT_H__



#include <vulkan/vulkan.h>



namespace vkpp
{



enum class Format
{
    eUndefined           = VK_FORMAT_UNDEFINED,

    eRGBA8Unorm          = VK_FORMAT_R8G8B8A8_UNORM,
    eRGBA8sRGB           = VK_FORMAT_R8G8B8A8_SRGB,

    eBGRA8Unorm          = VK_FORMAT_B8G8R8A8_UNORM,
    eBGRA8sRGB           = VK_FORMAT_B8G8R8A8_SRGB,

    eR32UInt            = VK_FORMAT_R32_UINT,
    eR32SInt            = VK_FORMAT_R32_SINT,
    eR32SFloat          = VK_FORMAT_R32_SFLOAT,

    eRG32UInt           = VK_FORMAT_R32G32_UINT,
    eRG32SInt           = VK_FORMAT_R32G32_SINT,
    eRG32SFloat         = VK_FORMAT_R32G32_SFLOAT,

    eRGB32UInt          = VK_FORMAT_R32G32B32_UINT,
    eRGB32SInt          = VK_FORMAT_R32G32B32_SINT,
    eRGB32SFloat        = VK_FORMAT_R32G32B32_SFLOAT,

    eRGBA32UInt         = VK_FORMAT_R32G32B32A32_UINT,
    eRGBA32SInt         = VK_FORMAT_R32G32B32A32_SINT,
    eRGBA32SFloat       = VK_FORMAT_R32G32B32A32_SFLOAT
};



}                   // End of namespace vkpp.



#endif              // __VKPP_INTO_FORMAT_H__
