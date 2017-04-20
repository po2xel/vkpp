#ifndef __VKPP_TYPE_IMAGE_H__
#define __VKPP_TYPE_IMAGE_H__



#include <Info/Common.h>
#include <Info/Flags.h>




namespace vkpp
{



enum class ImageLayout
{
    eUndefined                      = VK_IMAGE_LAYOUT_UNDEFINED,
    eGeneral                        = VK_IMAGE_LAYOUT_GENERAL,
    eColorAttachmentOptimal         = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    eDepthStencilAttachmentOptimal  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    eDepthStencilReadOnlyOptimal    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    eShaderReadOnlyOptimal          = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    eTransferSrcOptimal             = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    eTransferDstOptimal             = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    ePreinitialized                 = VK_IMAGE_LAYOUT_PREINITIALIZED,
    ePresentSrcKHR                  = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
};



enum class ImageCreateFlagBits
{
    eSparseBinding                  = VK_IMAGE_CREATE_SPARSE_BINDING_BIT,
    eSparseResidency                = VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT,
    eSparseAliased                  = VK_IMAGE_CREATE_SPARSE_ALIASED_BIT,
    eMultableFormat                 = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT,
    eCubeCompatible                 = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
    eBindSFRKHX                     = VK_IMAGE_CREATE_BIND_SFR_BIT_KHX,
    e2DArrayCompatibleKHR           = VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT_KHR
};

using ImageCreateFlags = internal::Flags<ImageCreateFlagBits, VkImageCreateFlags>;



enum class ImageType
{
    e1D     = VK_IMAGE_VIEW_TYPE_1D,
    e2D     = VK_IMAGE_VIEW_TYPE_2D,
    e3D     = VK_IMAGE_VIEW_TYPE_3D
};



enum class ImageTiling
{
    eOptimal    = VK_IMAGE_TILING_OPTIMAL,
    eLinear     = VK_IMAGE_TILING_LINEAR
};



class ImageCreateInfo : public internal::VkTrait<ImageCreateInfo, VkImageCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eImage;

public:
    const void*             pNext{ nullptr };
    ImageCreateFlags        flags;
    ImageType               imageType{ ImageType::e2D };
    Format                  format{ Format::eUndefined };
    Extent3D                extent;
    uint32_t                mipLevels{ 0 };
    uint32_t                arrayLayers{ 0 };
    SampleCountFlagBits     samples{ SampleCountFlagBits::e1 };
    ImageTiling             tiling{ ImageTiling::eOptimal };
    ImageUsageFlags         usage;
    SharingMode             sharingMode{ SharingMode::eExclusive };
    uint32_t                queueFamilyIndexCount{ 0 };
    const uint32_t*         pQueueFamilyIndices{ nullptr };
    ImageLayout             initialLayout{ ImageLayout::eUndefined };

    DEFINE_CLASS_MEMBER(ImageCreateInfo)

    ImageCreateInfo(ImageType aImageType, Format aFormat, const Extent3D& aExtent, uint32_t aMipLevels, uint32_t aArrayLayers,
        SampleCountFlagBits aSamples, ImageTiling aTiling, const ImageUsageFlags& aUsage, ImageLayout aInitialLayout, const ImageCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), imageType(aImageType), format(aFormat), extent(aExtent), mipLevels(aMipLevels), arrayLayers(aArrayLayers), samples(aSamples), tiling(aTiling),
          usage(aUsage), initialLayout(aInitialLayout)
    {}

    ImageCreateInfo(ImageType aImageType, Format aFormat, const Extent3D& aExtent, uint32_t aMipLevels, uint32_t aArrayLayers,
        SampleCountFlagBits aSamples, ImageTiling aTiling, const ImageUsageFlags& aUsage, uint32_t aQueueFamilyIndexCount, const uint32_t* apQueueFamilyIndices,
        ImageLayout aInitialLayout, const ImageCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), imageType(aImageType), format(aFormat), extent(aExtent), mipLevels(aMipLevels), arrayLayers(aArrayLayers), samples(aSamples), tiling(aTiling),
          usage(aUsage), sharingMode(SharingMode::eConcurrent), queueFamilyIndexCount(aQueueFamilyIndexCount), pQueueFamilyIndices(apQueueFamilyIndices), initialLayout(aInitialLayout)
    {
        assert(queueFamilyIndexCount != 0 && pQueueFamilyIndices != nullptr);
    }

    ImageCreateInfo(ImageType aImageType, Format aFormat, const Extent3D& aExtent, uint32_t aMipLevels, uint32_t aArrayLayers,
        SampleCountFlagBits aSamples, ImageTiling aTiling, const ImageUsageFlags& aUsage, const std::vector<uint32_t>& aQueueFamilyIndices,
        ImageLayout aInitialLayout, const ImageCreateFlags& aFlags = DefaultFlags)
        : ImageCreateInfo(aImageType, aFormat, aExtent, aMipLevels, aArrayLayers, aSamples, aTiling, aUsage,
          static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data(), aInitialLayout, aFlags)
    {}

    template <std::size_t Q>
    ImageCreateInfo(ImageType aImageType, Format aFormat, const Extent3D& aExtent, uint32_t aMipLevels, uint32_t aArrayLayers,
        SampleCountFlagBits aSamples, ImageTiling aTiling, const ImageUsageFlags& aUsage, const std::array<uint32_t, Q>& aQueueFamilyIndices,
        ImageLayout aInitialLayout, const ImageCreateFlags& aFlags = DefaultFlags)
        : ImageCreateInfo(aImageType, aFormat, aExtent, aMipLevels, aArrayLayers, aSamples, aTiling, aUsage,
          static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data(), aInitialLayout, aFlags)
    {}

    ImageCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    ImageCreateInfo& SetFlags(const ImageCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    ImageCreateInfo& SetImageType(ImageType aImageType, Format aFormat, SampleCountFlagBits aSamples, ImageTiling aTiling, const ImageUsageFlags& aUsage)
    {
        imageType   = aImageType;
        format      = aFormat;
        samples     = aSamples;
        tiling      = aTiling;
        usage       = aUsage;

        return *this;
    }

    ImageCreateInfo& SetImageExtent(const Extent3D& aExtent, uint32_t aMipLevels, uint32_t aArrayLayers)
    {
        extent      = aExtent;
        mipLevels   = aMipLevels;
        arrayLayers = aArrayLayers;

        return *this;
    }

    ImageCreateInfo& SetExclusiveMode(void)
    {
        sharingMode             = SharingMode::eExclusive;
        queueFamilyIndexCount   = 0;
        pQueueFamilyIndices     = nullptr;

        return *this;
    }

    ImageCreateInfo& SetConcurrentMode(uint32_t aQueueFamilyIndexCount, const uint32_t* apQueueFamilyIndices)
    {
        assert(aQueueFamilyIndexCount != 0 && apQueueFamilyIndices != nullptr);

        sharingMode             = SharingMode::eConcurrent;
        queueFamilyIndexCount   = aQueueFamilyIndexCount;
        pQueueFamilyIndices     = apQueueFamilyIndices;

        return *this;
    }

    ImageCreateInfo& SetConcurrentMode(const std::vector<uint32_t>& aQueueFamilyIndices)
    {
        return SetConcurrentMode(static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data());
    }

    template <std::size_t Q>
    ImageCreateInfo& SetConcurrentMode(const std::array<uint32_t, Q>& aQueueFamilyIndices)
    {
        return SetConcurrentMode(static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data());
    }
};

StaticSizeCheck(ImageCreateInfo);



class Image : public internal::VkTrait<Image, VkImage>
{
private:
    VkImage mImage{ VK_NULL_HANDLE };

public:
    Image(void) = default;

    Image(std::nullptr_t)
    {}

    explicit Image(VkImage aImage) : mImage(aImage)
    {}
};

StaticSizeCheck(Image);



enum class ImageViewCreateFlagBits
{};

using ImageViewCreateFlags = internal::Flags<ImageViewCreateFlagBits, VkImageViewCreateFlags>;



enum class ImageViewType
{
    e1D         = VK_IMAGE_VIEW_TYPE_1D,
    e2D         = VK_IMAGE_VIEW_TYPE_2D,
    e3D         = VK_IMAGE_VIEW_TYPE_3D,
    eCube       = VK_IMAGE_VIEW_TYPE_CUBE,
    e1DArray    = VK_IMAGE_VIEW_TYPE_1D_ARRAY,
    e2DArray    = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
    eCubeArray  = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY
};



enum class ComponentSwizzle
{
    eIdentity       = VK_COMPONENT_SWIZZLE_IDENTITY,
    eZero           = VK_COMPONENT_SWIZZLE_ZERO,
    eOne            = VK_COMPONENT_SWIZZLE_ONE,
    eR              = VK_COMPONENT_SWIZZLE_R,
    eG              = VK_COMPONENT_SWIZZLE_G,
    eB              = VK_COMPONENT_SWIZZLE_B,
    eA              = VK_COMPONENT_SWIZZLE_A
};



enum class ImageAspectFlagBits
{
    eColor      = VK_IMAGE_ASPECT_COLOR_BIT,
    eDepth      = VK_IMAGE_ASPECT_DEPTH_BIT,
    eStencil    = VK_IMAGE_ASPECT_STENCIL_BIT,
    eMetaData   = VK_IMAGE_ASPECT_METADATA_BIT
};

using ImageAspectFlags = internal::Flags<ImageAspectFlagBits, VkImageAspectFlags>;



struct ComponentMapping : public internal::VkTrait<ComponentMapping, VkComponentMapping>
{
    ComponentSwizzle r{ ComponentSwizzle::eIdentity };
    ComponentSwizzle g{ ComponentSwizzle::eIdentity };
    ComponentSwizzle b{ ComponentSwizzle::eIdentity };
    ComponentSwizzle a{ ComponentSwizzle::eIdentity };

    DEFINE_CLASS_MEMBER(ComponentMapping)

    ComponentMapping(ComponentSwizzle aR, ComponentSwizzle aG, ComponentSwizzle aB, ComponentSwizzle aA)
        : r(aR), g(aG), b(aB), a(aA)
    {}

    ComponentMapping& SetSwizzles(ComponentSwizzle aR = ComponentSwizzle::eIdentity, ComponentSwizzle aG = ComponentSwizzle::eIdentity,
        ComponentSwizzle aB = ComponentSwizzle::eIdentity, ComponentSwizzle aA = ComponentSwizzle::eIdentity)
    {
        r = aR;
        g = aG;
        b = aB;
        a = aA;

        return *this;
    }
};



struct ImageSubresourceRange : public internal::VkTrait<ImageSubresourceRange, VkImageSubresourceRange>
{
    ImageAspectFlags    aspectMask;
    uint32_t            baseMipLevel{ 0 };
    uint32_t            levelCount{ 0 };
    uint32_t            baseArrayLayer{ 0 };
    uint32_t            layerCount{ 0 };

    DEFINE_CLASS_MEMBER(ImageSubresourceRange)

    ImageSubresourceRange(ImageAspectFlags aAspectMask, uint32_t aBaseMipLevel, uint32_t aLevelCount, uint32_t aBaseArrayLayer, uint32_t aLayerCount)
    : aspectMask(aAspectMask), baseMipLevel(aBaseMipLevel), levelCount(aLevelCount), baseArrayLayer(aBaseArrayLayer), layerCount(aLayerCount)
    {}

    ImageSubresourceRange& SetAspectMask(const ImageAspectFlags& aAspectMask)
    {
        aspectMask = aAspectMask;

        return *this;
    }

    ImageSubresourceRange& SetMips(uint32_t aBaseMipLevel, uint32_t aLevelCount)
    {
        baseMipLevel = aBaseMipLevel;
        levelCount = aLevelCount;

        return *this;
    }

    ImageSubresourceRange& SetArrayLayers(uint32_t aBaseArrayLayer, uint32_t aLayerCount)
    {
        baseArrayLayer = aBaseArrayLayer;
        layerCount = aLayerCount;

        return *this;
    }
};

StaticSizeCheck(ImageSubresourceRange);



class ImageViewCreateInfo : public internal::VkTrait<ImageViewCreateInfo, VkImageViewCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eImageView;

public:
    const void*             pNext{ nullptr };
    ImageViewCreateFlags    flags;
    Image                   image;
    ImageViewType           viewType{ ImageViewType::e1D };
    Format                  format{ Format::eUndefined };
    ComponentMapping        components;
    ImageSubresourceRange   subresourceRange;

    DEFINE_CLASS_MEMBER(ImageViewCreateInfo)

    ImageViewCreateInfo(const Image& aImage, ImageViewType aImageViewType, Format aFormat, const ComponentMapping& aComponents,
        const ImageSubresourceRange& aSubresourceRange, const ImageViewCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), image(aImage), viewType(aImageViewType), format(aFormat), components(aComponents), subresourceRange(aSubresourceRange)
    {}

    ImageViewCreateInfo& SetFlags(const ImageViewCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    ImageViewCreateInfo& SetImage(const Image& aImage, ImageViewType aViewType, Format aFormat)
    {
        image       = aImage;
        viewType    = aViewType;
        format      = aFormat;

        return *this;
    }

    ImageViewCreateInfo& SetImageComponents(const ComponentMapping& aComponents)
    {
        components  = aComponents;

        return *this;
    }

    ImageViewCreateInfo& SetSubresourceRange(const ImageSubresourceRange& aSubresourceRange)
    {
        subresourceRange = aSubresourceRange;

        return *this;
    }
};

StaticSizeCheck(ImageViewCreateInfo);



class ImageView : public internal::VkTrait<ImageView, VkImageView>
{
private:
    VkImageView mImageView{ VK_NULL_HANDLE };

public:
    ImageView(void) noexcept = default;

    ImageView(std::nullptr_t)
    {}

    explicit ImageView(VkImageView aImageView) : mImageView(aImageView)
    {}
};

StaticSizeCheck(ImageView);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_IMAGE_H__
