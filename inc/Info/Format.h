#ifndef __VKPP_INTO_FORMAT_H__
#define __VKPP_INTO_FORMAT_H__



#include <Info/Common.h>



namespace vkpp
{



enum class Format
{
    eUndefined              = VK_FORMAT_UNDEFINED,

    eRG4uNormPack8          = VK_FORMAT_R4G4_UNORM_PACK8,
    eRGBA4uNormPack16       = VK_FORMAT_R4G4B4A4_UNORM_PACK16,
    eBGRA4uNormPack16       = VK_FORMAT_B4G4R4A4_UNORM_PACK16,
    eR5G6B5uNormPack16      = VK_FORMAT_R5G6B5_UNORM_PACK16,
    eB5G6R5uNormPack16      = VK_FORMAT_B5G6R5_UNORM_PACK16,
    eRGB5A1Pack16           = VK_FORMAT_R5G5B5A1_UNORM_PACK16,
    eBGR5A1Pack16           = VK_FORMAT_B5G5R5A1_UNORM_PACK16,
    eA1RGB5Pack16           = VK_FORMAT_A1R5G5B5_UNORM_PACK16,

    eR8uNorm                = VK_FORMAT_R8_UNORM,
    eR8sNorm                = VK_FORMAT_R8_SNORM,
    eR8uScaled              = VK_FORMAT_R8_USCALED,
    eR8sScaled              = VK_FORMAT_R8_SSCALED,
    eR8uInt                 = VK_FORMAT_R8_UINT,
    eR8sInt                 = VK_FORMAT_R8_SINT,
    eR8sRGB                 = VK_FORMAT_R8_SRGB,

    eRG8uNorm               = VK_FORMAT_R8G8_UNORM,
    eRG8sNorm               = VK_FORMAT_R8G8_SNORM,
    eRG8uScaled             = VK_FORMAT_R8G8_USCALED,
    eRG8sScaled             = VK_FORMAT_R8G8_SSCALED,
    eRG8uInt                = VK_FORMAT_R8G8_UINT,
    eRG8sInt                = VK_FORMAT_R8G8_SINT,
    eRG8sRGB                = VK_FORMAT_R8G8_SRGB,

    eRGB8uNorm              = VK_FORMAT_R8G8B8_UNORM,
    eRGB8sNorm              = VK_FORMAT_R8G8B8_SNORM,
    eRGB8uScaled            = VK_FORMAT_R8G8B8_USCALED,
    eRGB8sScaled            = VK_FORMAT_R8G8B8_SSCALED,
    eRGB8uInt               = VK_FORMAT_R8G8B8_UINT,
    eRGB8sInt               = VK_FORMAT_R8G8B8_SINT,
    eRGB8sRGB               = VK_FORMAT_R8G8B8_SRGB,

    eBGR8uNorm              = VK_FORMAT_B8G8R8_UNORM,
    eBGR8sNorm              = VK_FORMAT_B8G8R8_SNORM,
    eBGR8uScaled            = VK_FORMAT_B8G8R8_USCALED,
    eBGR8sScaled            = VK_FORMAT_B8G8R8_SSCALED,
    eBGR8uInt               = VK_FORMAT_B8G8R8_UINT,
    eBGR8sInt               = VK_FORMAT_B8G8R8_SINT,
    eBGR8sRGB               = VK_FORMAT_B8G8R8_SRGB,

    eRGBA8uNorm             = VK_FORMAT_R8G8B8A8_UNORM,
    eRGBA8sNorm             = VK_FORMAT_R8G8B8A8_SNORM,
    eRGBA8uScaled           = VK_FORMAT_R8G8B8A8_USCALED,
    eRGBA8sScaled           = VK_FORMAT_R8G8B8A8_SSCALED,
    eRGBA8uInt              = VK_FORMAT_R8G8B8A8_UINT,
    eRGBA8sInt              = VK_FORMAT_R8G8B8A8_SINT,
    eRGBA8sRGB              = VK_FORMAT_R8G8B8A8_SRGB,

    eBGRA8uNorm             = VK_FORMAT_B8G8R8A8_UNORM,
    eBGRA8sNorm             = VK_FORMAT_B8G8R8A8_SNORM,
    eBGRA8uScaled           = VK_FORMAT_B8G8R8A8_USCALED,
    eBGRA8sScaled           = VK_FORMAT_B8G8R8A8_SSCALED,
    eBGRA8uInt              = VK_FORMAT_B8G8R8A8_UINT,
    eBGRA8sInt              = VK_FORMAT_B8G8R8A8_SINT,
    eBGRA8sRGB              = VK_FORMAT_B8G8R8A8_SRGB,

    eABRG8uNormPack32       = VK_FORMAT_A8B8G8R8_UNORM_PACK32,
    eABRG8sNormPack32       = VK_FORMAT_A8B8G8R8_SNORM_PACK32,
    eABGR8uScaledPack32     = VK_FORMAT_A8B8G8R8_USCALED_PACK32,
    eABGR8sScaledPack32     = VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
    eABGR8uIntPack32        = VK_FORMAT_A8B8G8R8_UINT_PACK32,
    eABGR8sIntPack32        = VK_FORMAT_A8B8G8R8_SINT_PACK32,
    eABGR8sRGBPack32        = VK_FORMAT_A8B8G8R8_SRGB_PACK32,

    eA2RGB10uNormPack32     = VK_FORMAT_A2R10G10B10_UNORM_PACK32,
    eA2RGB10sNormPack32     = VK_FORMAT_A2R10G10B10_SNORM_PACK32,
    eA2RGB10uScaledPack32   = VK_FORMAT_A2R10G10B10_USCALED_PACK32,
    eA2RGB10sScaledPack32   = VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
    eA2RGB10uIntPack32      = VK_FORMAT_A2R10G10B10_UINT_PACK32,
    eA2RGB10sIntPack32      = VK_FORMAT_A2R10G10B10_SINT_PACK32,

    eA2BGR10uNormPack32     = VK_FORMAT_A2B10G10R10_UNORM_PACK32,
    eA2BGR10sNormPack32     = VK_FORMAT_A2B10G10R10_SNORM_PACK32,
    eA2BGR10uScaledPack32   = VK_FORMAT_A2B10G10R10_USCALED_PACK32,
    eA2BGR10sScaledPack32   = VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
    eA2BGR10uIntPack32      = VK_FORMAT_A2B10G10R10_UINT_PACK32,
    eA2BGR10sIntPack32      = VK_FORMAT_A2B10G10R10_SINT_PACK32,

    eR16uNorm               = VK_FORMAT_R16_UNORM,
    eR16sNorm               = VK_FORMAT_R16_SNORM,
    eR16uScaled             = VK_FORMAT_R16_USCALED,
    eR16sScaled             = VK_FORMAT_R16_SSCALED,
    eR16uInt                = VK_FORMAT_R16_UINT,
    eR16sInt                = VK_FORMAT_R16_SINT,
    eR16sFloat              = VK_FORMAT_R16_SFLOAT,

    eRG16uNorm              = VK_FORMAT_R16G16_UNORM,
    eRG16sNorm              = VK_FORMAT_R16G16_SNORM,
    eRG16uScaled            = VK_FORMAT_R16G16_USCALED,
    eRG16sScaled            = VK_FORMAT_R16G16_SSCALED,
    eRG16uInt               = VK_FORMAT_R16G16_UINT,
    eRG16sInt               = VK_FORMAT_R16G16_SINT,
    eRG16sFloat             = VK_FORMAT_R16G16_SFLOAT,

    eRGB16uNorm             = VK_FORMAT_R16G16B16_UNORM,
    eRGB16sNorm             = VK_FORMAT_R16G16B16_SNORM,
    eRGB16uScaled           = VK_FORMAT_R16G16B16_USCALED,
    eRGB16sScaled           = VK_FORMAT_R16G16B16_SSCALED,
    eRGB16uInt              = VK_FORMAT_R16G16B16_UINT,
    eRGB16sInt              = VK_FORMAT_R16G16B16_SINT,
    eRGB16sFloat            = VK_FORMAT_R16G16B16_SFLOAT,

    eRGBA16uNorm            = VK_FORMAT_R16G16B16A16_UNORM,
    eRGBA16sNorm            = VK_FORMAT_R16G16B16A16_SNORM,
    eRGBA16uScaled          = VK_FORMAT_R16G16B16A16_USCALED,
    eRGBA16sScaled          = VK_FORMAT_R16G16B16A16_SSCALED,
    eRGBA16uInt             = VK_FORMAT_R16G16B16A16_UINT,
    eRGBA16sInt             = VK_FORMAT_R16G16B16A16_SINT,
    eRGBA16sFloat           = VK_FORMAT_R16G16B16A16_SFLOAT,

    eR32uInt                = VK_FORMAT_R32_UINT,
    eR32sInt                = VK_FORMAT_R32_SINT,
    eR32sFloat              = VK_FORMAT_R32_SFLOAT,

    eRG32uInt               = VK_FORMAT_R32G32_UINT,
    eRG32sInt               = VK_FORMAT_R32G32_SINT,
    eRG32sFloat             = VK_FORMAT_R32G32_SFLOAT,

    eRGB32uInt              = VK_FORMAT_R32G32B32_UINT,
    eRGB32sInt              = VK_FORMAT_R32G32B32_SINT,
    eRGB32sFloat            = VK_FORMAT_R32G32B32_SFLOAT,

    eRGBA32uInt             = VK_FORMAT_R32G32B32A32_UINT,
    eRGBA32sInt             = VK_FORMAT_R32G32B32A32_SINT,
    eRGBA32sFloat           = VK_FORMAT_R32G32B32A32_SFLOAT,

    eR64uInt                = VK_FORMAT_R64_UINT,
    eR64sInt                = VK_FORMAT_R64_SINT,
    eR64sFloat              = VK_FORMAT_R64_SFLOAT,

    eRG64uInt               = VK_FORMAT_R64G64_UINT,
    eRG64sInt               = VK_FORMAT_R64G64_SINT,
    eRG64sFloat             = VK_FORMAT_R64G64_SFLOAT,

    eRGB64uInt              = VK_FORMAT_R64G64B64_UINT,
    eRGB64sInt              = VK_FORMAT_R64G64B64_SINT,
    eRGB64sFloat            = VK_FORMAT_R64G64B64_SFLOAT,

    eRGBA64uInt             = VK_FORMAT_R64G64B64A64_UINT,
    eRGBA64sInt             = VK_FORMAT_R64G64B64A64_SINT,
    eRGBA64sFloat           = VK_FORMAT_R64G64B64A64_SFLOAT,

    eB10GR11UFloatPack32    = VK_FORMAT_B10G11R11_UFLOAT_PACK32,
    eE5BGR9UFloatPack32     = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,

    eD16uNorm               = VK_FORMAT_D16_UNORM,
    eX8D24uNormPack32       = VK_FORMAT_X8_D24_UNORM_PACK32,
    eD32sFloat              = VK_FORMAT_D32_SFLOAT,
    eS8uInt                 = VK_FORMAT_S8_UINT,
    eD16uNormS8uInt         = VK_FORMAT_D16_UNORM_S8_UINT,
    eD24uNormS8uInt         = VK_FORMAT_D24_UNORM_S8_UINT,
    eD32sFloatS8uInt        = VK_FORMAT_D32_SFLOAT_S8_UINT,

    eBC1_RGBuNormBlock       = VK_FORMAT_BC1_RGB_UNORM_BLOCK,
    eBC1_RGBsRGBBlock        = VK_FORMAT_BC1_RGB_SRGB_BLOCK,
    eBC1_RGBAuNormBlock      = VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
    eBC1_RGBAsRGBBlock       = VK_FORMAT_BC1_RGBA_SRGB_BLOCK,

    eBC2_uNormBlock          = VK_FORMAT_BC2_UNORM_BLOCK,
    eBC2_sRGBBlock           = VK_FORMAT_BC2_SRGB_BLOCK,

    eBC3_uNormBlock          = VK_FORMAT_BC3_UNORM_BLOCK,
    eBC3_sRGBBlock           = VK_FORMAT_BC3_SRGB_BLOCK,

    eBC4_uNormBlock          = VK_FORMAT_BC4_UNORM_BLOCK,
    eBC4_sNormBlock          = VK_FORMAT_BC4_SNORM_BLOCK,

    eBC5_uNormBlock          = VK_FORMAT_BC5_UNORM_BLOCK,
    eBC5_sNormBlock          = VK_FORMAT_BC5_SNORM_BLOCK,

    eBC6_HUFloatBlock        = VK_FORMAT_BC6H_UFLOAT_BLOCK,
    eBC6_HsFloatBlock        = VK_FORMAT_BC6H_SFLOAT_BLOCK,

    eBC7_uNormBlock          = VK_FORMAT_BC7_UNORM_BLOCK,
    eBC7_sRGBBlock           = VK_FORMAT_BC7_SRGB_BLOCK,

    eETC2_RGB8uNormBlock     = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
    eETC2_RGB8sRGBBlock      = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
    eETC2_RGB8A1uNormBlock   = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
    eETC2_RGB8A1sRGBBlock    = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
    eETC2_RGBA8uNormBlock    = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
    eETC2_RGBA8sRGBBlock     = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,

    eEAC_R11uNormBlock       = VK_FORMAT_EAC_R11_UNORM_BLOCK,
    eEAC_R11sNormBlock       = VK_FORMAT_EAC_R11_SNORM_BLOCK,

    eEAC_RG11uNormBlock      = VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
    eEAC_RG11sNormBlock      = VK_FORMAT_EAC_R11G11_SNORM_BLOCK,

    eASTC_4x4uNormBlock      = VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
    eASTC_4x4sRGBBlock       = VK_FORMAT_ASTC_4x4_SRGB_BLOCK,

    eASTC_5x4uNormBlock      = VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
    eASTC_5x4sRGBBlock       = VK_FORMAT_ASTC_5x4_SRGB_BLOCK,

    eASTC_5x5uNormBlock      = VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
    eASTC_5x5sRGBBlock       = VK_FORMAT_ASTC_5x5_SRGB_BLOCK,

    eASTC_6x5uNormBlock      = VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
    eASTC_6x5sRGBBlock       = VK_FORMAT_ASTC_6x5_SRGB_BLOCK,

    eASTC_6x6uNormBlock      = VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
    eASTC_6x6sRGBBlock       = VK_FORMAT_ASTC_6x6_SRGB_BLOCK,

    eASTC_8x5uNormBlock      = VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
    eASTC_8x5sRGBBlock       = VK_FORMAT_ASTC_8x5_SRGB_BLOCK,

    eASTC_8x6uNormBlock      = VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
    eASTC_8x6sRGBBlock       = VK_FORMAT_ASTC_8x6_SRGB_BLOCK,

    eASTC_8x8uNormBlock      = VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
    eASTC_8x8sRGBBlock       = VK_FORMAT_ASTC_8x8_SRGB_BLOCK,

    eASTC_10x5uNormBlock     = VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
    eASTC_10x5sRGBBlock      = VK_FORMAT_ASTC_10x5_SRGB_BLOCK,

    eASTC_10x6uNormBlock     = VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
    eASTC_10x6sRGBBlock      = VK_FORMAT_ASTC_10x6_SRGB_BLOCK,

    eASTC_10x8uNormBlock     = VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
    eASTC_10x8sRGBBlock      = VK_FORMAT_ASTC_10x8_SRGB_BLOCK,

    eASTC_10x10uNormBlock    = VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
    eASTC_10x10sRGBBlock     = VK_FORMAT_ASTC_10x10_SRGB_BLOCK,

    eASTC_12x10uNormBlock    = VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
    eASTC_12x10sRGBBlock     = VK_FORMAT_ASTC_12x10_SRGB_BLOCK,

    eASTC_12x12uNormBlock    = VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
    eASTC_12x12sRGBBlock     = VK_FORMAT_ASTC_12x12_SRGB_BLOCK,

    ePVRTC1_2BPPuNormBlockIMG       = VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG,
    ePVRTC1_4BPPuNormBlockIMG       = VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG,

    ePVRTC2_2BPPuNormBlockIMG       = VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG,
    ePVRTC2_4BPPuNormBlockIMG       = VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG,

    ePVRTC1_2BPPsRGBBlockIMG        = VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG,
    ePVRTC1_4BPPsRGBBlockIMG        = VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG,

    ePVRTC2_2BPPsRGBBlockIMG        = VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG,
    ePVRTC2_4BPPsRGBBlockIMG        = VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG
};



enum class FormatFeatureFlagBits
{
    eSampledImage                   = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT,
    eStorageImage                   = VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT,
    eStorageImageAtomic             = VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT,
    eUniformTexelBuffer             = VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT,
    eStorageTexelBuffer             = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT,
    eStorageTexelBufferAtomic       = VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT,
    eVertexBuffer                   = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT,
    eColorAttachment                = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT,
    eColorAttachmentBlend           = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT,
    eDepthStencilAttachment         = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
    eBlitSrc                        = VK_FORMAT_FEATURE_BLIT_SRC_BIT,
    eBlitDst                        = VK_FORMAT_FEATURE_BLIT_DST_BIT,
    eSampledImageFilterLinear       = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT,
    eSampledImageFilterCubicIMG     = VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG,
    eTransferSrcKHR                 = VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR,
    eTransferDstKHR                 = VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR
};


VKPP_ENUM_BIT_MASK_FLAGS(FormatFeature)



struct FormatProperties : public internal::VkTrait<FormatProperties, VkFormatProperties>
{
    FormatFeatureFlags   linearTilingFeatures;
    FormatFeatureFlags   optimalTilingFeatures;
    FormatFeatureFlags   bufferFeatures;
};

ConsistencyCheck(FormatProperties, linearTilingFeatures, optimalTilingFeatures, bufferFeatures)



}                   // End of namespace vkpp.



#endif              // __VKPP_INTO_FORMAT_H__