#ifndef __VKPP_TYPE_IMAGE_H__
#define __VKPP_TYPE_IMAGE_H__



#include <cstring>          // std::memcpy

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

VKPP_ENUM_BIT_MASK_FLAGS(ImageCreate)



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



enum class Filter
{
    eNearest        = VK_FILTER_NEAREST,
    eLinear         = VK_FILTER_LINEAR,
    eCubicIMG       = VK_FILTER_CUBIC_IMG
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

    template <typename Q, typename = EnableIfValueType<Q, uint32_t>>
    ImageCreateInfo(ImageType aImageType, Format aFormat, const Extent3D& aExtent, uint32_t aMipLevels, uint32_t aArrayLayers,
        SampleCountFlagBits aSamples, ImageTiling aTiling, const ImageUsageFlags& aUsage, Q&& aQueueFamilyIndices,
        ImageLayout aInitialLayout, const ImageCreateFlags& aFlags = DefaultFlags)
        : ImageCreateInfo(aImageType, aFormat, aExtent, aMipLevels, aArrayLayers, aSamples, aTiling, aUsage,
          static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data(), aInitialLayout, aFlags)
    {
        StaticLValueRefAssert(Q, aQueueFamilyIndices);
    }

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

    template <typename Q, typename = EnableIfValueType<Q, uint32_t>>
    ImageCreateInfo& SetConcurrentMode(Q&& aQueueFamilyIndices)
    {
        StaticLValueRefAssert(Q, aQueueFamilyIndices);

        return SetConcurrentMode(static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data());
    }

    ImageCreateInfo& SetInitialLayout(const ImageLayout& aInitialLayout)
    {
        initialLayout = aInitialLayout;

        return *this;
    }
};

ConsistencyCheck(ImageCreateInfo, pNext, flags, imageType, format, samples, tiling, usage, extent, mipLevels, arrayLayers,
    sharingMode, queueFamilyIndexCount, pQueueFamilyIndices)



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

StaticSizeCheck(Image)



enum class ImageViewCreateFlagBits
{};

VKPP_ENUM_BIT_MASK_FLAGS(ImageViewCreate)



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

VKPP_ENUM_BIT_MASK_FLAGS(ImageAspect)



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

ConsistencyCheck(ComponentMapping, r, g, b, a)



struct ImageSubresource : public internal::VkTrait<ImageSubresource, VkImageSubresource>
{
    ImageAspectFlags    aspectMask;
    uint32_t            mipLevel{ 0 };
    uint32_t            arrayLayer{ 0 };

    DEFINE_CLASS_MEMBER(ImageSubresource)

    ImageSubresource(const ImageAspectFlags& aAspectMask, uint32_t aMipLevel, uint32_t aArrayLayer)
        : aspectMask(aAspectMask), mipLevel(aMipLevel), arrayLayer(aArrayLayer)
    {}

    ImageSubresource& SetAspectMask(const ImageAspectFlags& aAspectMask)
    {
        aspectMask = aAspectMask;

        return *this;
    }

    ImageSubresource& SetSubrange(uint32_t aMipLevel, uint32_t aArrayLayer)
    {
        mipLevel        = aMipLevel;
        arrayLayer      = aArrayLayer;

        return *this;
    }
};

ConsistencyCheck(ImageSubresource, aspectMask, mipLevel, arrayLayer)



struct SubresourceLayout : public internal::VkTrait<SubresourceLayout, VkSubresourceLayout>
{
    DeviceSize      offset{ 0 };
    DeviceSize      size{ 0 };
    DeviceSize      rowPitch{ 0 };
    DeviceSize      arrayPitch{ 0 };
    DeviceSize      depthPitch{ 0 };

    DEFINE_CLASS_MEMBER(SubresourceLayout)

    SubresourceLayout(DeviceSize aOffset, DeviceSize aSize, DeviceSize aRowPitch, DeviceSize aArrayPitch, DeviceSize aDepthPitch)
        : offset(aOffset), size(aSize), rowPitch(aRowPitch), arrayPitch(aArrayPitch), depthPitch(aDepthPitch)
    {}

    SubresourceLayout& SetOffset(DeviceSize aOffset)
    {
        offset = aOffset;

        return *this;
    }

    SubresourceLayout& SetSize(DeviceSize aSize)
    {
        size = aSize;

        return *this;
    }

    SubresourceLayout& SetPitch(DeviceSize aRowPitch, DeviceSize aArrayPitch, DeviceSize aDepthPitch)
    {
        rowPitch        = aRowPitch;
        arrayPitch      = aArrayPitch;
        depthPitch      = aDepthPitch;

        return *this;
    }
};

ConsistencyCheck(SubresourceLayout, offset, size, rowPitch, arrayPitch, depthPitch)



struct ImageSubresourceLayers : public internal::VkTrait<ImageSubresourceLayers, VkImageSubresourceLayers>
{
    ImageAspectFlags    aspectMask;
    uint32_t            mipLevel{ 0 };
    uint32_t            baseArrayLayer{ 0 };
    uint32_t            layerCount{ 0 };

    DEFINE_CLASS_MEMBER(ImageSubresourceLayers)

    ImageSubresourceLayers(const ImageAspectFlags& aAspectMask, uint32_t aMipLevel, uint32_t aBaseArrayLayer, uint32_t aLayerCount)
        : aspectMask(aAspectMask), mipLevel(aMipLevel), baseArrayLayer(aBaseArrayLayer), layerCount(aLayerCount)
    {}

    ImageSubresourceLayers& SetAspectMask(const ImageAspectFlags& aAspectMask)
    {
        aspectMask = aAspectMask;

        return *this;
    }

    ImageSubresourceLayers& SetMipLevel(uint32_t aMipLevel)
    {
        mipLevel = aMipLevel;

        return *this;
    }

    ImageSubresourceLayers& SetArrayLayer(uint32_t aBaseArrayLayer, uint32_t aLayerCount)
    {
        baseArrayLayer  = aBaseArrayLayer;
        layerCount      = aLayerCount;

        return *this;
    }
};

ConsistencyCheck(ImageSubresourceLayers, aspectMask, mipLevel, baseArrayLayer, layerCount)



struct ImageCopy : public internal::VkTrait<ImageCopy, VkImageCopy>
{
    ImageSubresourceLayers  srcSubresource;
    Offset3D                srcOffset;
    ImageSubresourceLayers  dstSubresource;
    Offset3D                dstOffset;
    Extent3D                extent;

    DEFINE_CLASS_MEMBER(ImageCopy)

    ImageCopy(const ImageSubresourceLayers& aSrcSubresource, const Offset3D& aSrcOffset, const ImageSubresourceLayers& aDstSubresource, const Offset3D& aDstOffset, const Extent3D& aExtent)
        : srcSubresource(aSrcSubresource), srcOffset(aSrcOffset), dstSubresource(aDstSubresource), dstOffset(aDstOffset), extent(aExtent)
    {}

    ImageCopy& SetSubresourceLayers(const ImageSubresourceLayers& aSrcSubresource, const ImageSubresourceLayers& aDstSubResource)
    {
        srcSubresource = aSrcSubresource;
        dstSubresource = aDstSubResource;

        return *this;
    }

    ImageCopy& SetOffset(const Offset3D& aSrcOffset, const Offset3D& aDstOffset)
    {
        srcOffset   = aSrcOffset;
        dstOffset   = aDstOffset;

        return *this;
    }

    ImageCopy& SetExtent(const Extent3D& aExtent)
    {
        extent  = aExtent;

        return *this;
    }
};

ConsistencyCheck(ImageCopy, srcSubresource, srcOffset, dstSubresource, dstOffset, extent)



struct ImageBlit : public internal::VkTrait<ImageBlit, VkImageBlit>
{
    ImageSubresourceLayers      srcSubresource;
    Offset3D                    srcOffsets[2];
    ImageSubresourceLayers      dstSubresource;
    Offset3D                    dstOffsets[2];
    
    DEFINE_CLASS_MEMBER(ImageBlit)

    ImageBlit(const ImageSubresourceLayers& aSrcSubresoure, const std::array<Offset3D, 2>& aSrcOffsets, const ImageSubresourceLayers& aDstSubresource, const std::array<Offset3D, 2>& aDstOffsets)
        : srcSubresource(aSrcSubresoure), srcOffsets{aSrcOffsets[0], aSrcOffsets[1]}, dstSubresource(aDstSubresource), dstOffsets{aDstOffsets[0], aDstOffsets[1]}
    {}

    ImageBlit& SetSubresourceLayers(const ImageSubresourceLayers& aSrcSubresource, const ImageSubresourceLayers& aDstSubresource)
    {
        srcSubresource  = aSrcSubresource;
        dstSubresource  = aDstSubresource;

        return *this;
    }

    ImageBlit& SetOffsets(const std::array<Offset3D, 2>& aSrcOffsets, const std::array<Offset3D, 2>& aDstOffsets)
    {
        std::memcpy(srcOffsets, aSrcOffsets.data(), aSrcOffsets.size());
        std::memcpy(dstOffsets, aDstOffsets.data(), aDstOffsets.size());

        return *this;
    }
};

ConsistencyCheck(ImageBlit, srcSubresource, srcOffsets, dstSubresource, dstOffsets)



struct ImageResolve : public internal::VkTrait<ImageResolve, VkImageResolve>
{
    ImageSubresourceLayers      srcSubresource;
    Offset3D                    srcOffset;
    ImageSubresourceLayers      dstSubresource;
    Offset3D                    dstOffset;
    Extent3D                    extent;

    DEFINE_CLASS_MEMBER(ImageResolve)

    ImageResolve(const ImageSubresourceLayers& aSrcSubresource, const Offset3D& aSrcOffset, const ImageSubresourceLayers& aDstSubresource, const Offset3D& aDstOffset, const Extent3D& aExtent)
        : srcSubresource(aSrcSubresource), srcOffset(aSrcOffset), dstSubresource(aDstSubresource), dstOffset(aDstOffset), extent(aExtent)
    {}

    ImageResolve& SetSubresourceLayers(const ImageSubresourceLayers& aSrcSubresource, const ImageSubresourceLayers& aDstSubresource)
    {
        srcSubresource = aSrcSubresource;
        dstSubresource = aDstSubresource;

        return *this;
    }

    ImageResolve& SetOffset(const Offset3D& aSrcOffset, const Offset3D& aDstOffset)
    {
        srcOffset       = aSrcOffset;
        dstOffset       = aDstOffset;

        return *this;
    }

    ImageResolve& SetExtent(const Extent3D& aExtent)
    {
        extent = aExtent;

        return *this;
    }
};

ConsistencyCheck(ImageResolve, srcSubresource, srcOffset, dstSubresource, dstOffset, extent)



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

ConsistencyCheck(ImageSubresourceRange, aspectMask, baseMipLevel, levelCount, baseArrayLayer, layerCount)



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

ConsistencyCheck(ImageViewCreateInfo, pNext, flags, image, viewType, format, components, subresourceRange)



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

StaticSizeCheck(ImageView)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_IMAGE_H__
