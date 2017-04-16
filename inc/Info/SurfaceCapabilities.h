#ifndef __VKPP_INFO_SURFACE_CAPABILITIES_H__
#define __VKPP_INFO_SURFACE_CAPABILITIES_H__



#include <Info/Common.h>
#include <Info/Flags.h>



namespace vkpp
{



enum class ImageUsageFlagBits
{
    eTransferSrc            = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
    eTransferDst            = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    eSampled                = VK_IMAGE_USAGE_SAMPLED_BIT,
    eStorage                = VK_IMAGE_USAGE_STORAGE_BIT,
    eColorAttachment        = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    eDepthStencilAttachment = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    eTransientAttachment    = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
    eInputAttachment        = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
};

using ImageUsageFlags = internal::Flags<ImageUsageFlagBits, VkImageUsageFlags>;



}



namespace vkpp::khr
{



enum class SurfaceTransformFlagBits
{
    eIdentity                   = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
    eRotate90                   = VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR,
    eRotate180                  = VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR,
    eRotate270                  = VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR,
    eHorizontalMirror           = VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR,
    eHorizontalMirrorRotate90   = VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR,
    eHorizontalMirrorRotate180  = VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR,
    eHorizontalMirrorRotate270  = VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR,
    eInherit                    = VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR
};

using SurfaceTransformFlags = internal::Flags<SurfaceTransformFlagBits, VkSurfaceTransformFlagsKHR>;



enum class CompositeAlphaFlagBits
{
    eOpaque         = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    ePreMultiplied  = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
    ePostMultiplied = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
    eInherit        = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
};

using CompositeAlphaFlags = internal::Flags<CompositeAlphaFlagBits, VkCompositeAlphaFlagsKHR>;



struct SurfaceCapabilities : public internal::VkTrait<SurfaceCapabilities, VkSurfaceCapabilitiesKHR>
{
    uint32_t                    minImageCount{ 0 };
    uint32_t                    maxImageCount{ 0 };
    Extent2D                    currentExtent;
    Extent2D                    minImageExtent;
    Extent2D                    maxImageExtent;
    uint32_t                    maxImageArrayLayers{ 0 };
    SurfaceTransformFlags       supportedTransforms;
    SurfaceTransformFlagBits    currentTransform;
    CompositeAlphaFlags         supportedCompositeAlpha;
    ImageUsageFlags             supportedUsageFlags;

    DEFINE_CLASS_MEMBER(SurfaceCapabilities)
};

StaticSizeCheck(SurfaceCapabilities);



}                   // End of namespace vkpp::khr.



#endif              // __VKPP_INFO_SURFACE_CAPABILITIES_H__
