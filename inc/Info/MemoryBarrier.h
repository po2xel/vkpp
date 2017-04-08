#ifndef __VKPP_INFO_MEMORY_BARRIER_H__
#define __VKPP_INFO_MEMORY_BARRIER_H__



#include <Info/Common.h>
#include <Info/Flags.h>
#include <Type/Image.h>
#include <Type/Buffer.h>



namespace vkpp
{



enum class AccessFlagBits
{
    eIndirectCommandRead            = VK_ACCESS_INDIRECT_COMMAND_READ_BIT,
    eIndexRead                      = VK_ACCESS_INDEX_READ_BIT,
    eVertexAttributeRead            = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
    eUniformRead                    = VK_ACCESS_UNIFORM_READ_BIT,
    eInputAttachmentRead            = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
    eShaderRead                     = VK_ACCESS_SHADER_READ_BIT,
    eShaderWrite                    = VK_ACCESS_SHADER_WRITE_BIT,
    eColorAttachmentRead            = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
    eColorAttachmentWrite           = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    eDepthStencilAttachmentRead     = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
    eDepthStencilAttachmentWrite    = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    eTransferRead                   = VK_ACCESS_TRANSFER_READ_BIT,
    eTransferWrite                  = VK_ACCESS_TRANSFER_WRITE_BIT,
    eHostRead                       = VK_ACCESS_HOST_READ_BIT,
    eHostWrite                      = VK_ACCESS_HOST_WRITE_BIT,
    eMemoryRead                     = VK_ACCESS_MEMORY_READ_BIT,
    eMemoryWrite                    = VK_ACCESS_MEMORY_WRITE_BIT,
    eCommandProcessReadNVX          = VK_ACCESS_COMMAND_PROCESS_READ_BIT_NVX,
    eCommandProcessWirteNVX         = VK_ACCESS_COMMAND_PROCESS_WRITE_BIT_NVX
};

using AccessFlags = internal::Flags<AccessFlagBits, VkAccessFlags>;



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



enum class ImageAspectFlagBits
{
    eColor = VK_IMAGE_ASPECT_COLOR_BIT,
    eDepth = VK_IMAGE_ASPECT_DEPTH_BIT,
    eStencil = VK_IMAGE_ASPECT_STENCIL_BIT,
    eMetaData = VK_IMAGE_ASPECT_METADATA_BIT
};

using ImageAspectFlags = internal::Flags<ImageAspectFlagBits, VkImageAspectFlags>;



struct ImageSubresourceRange : public internal::VkTrait<ImageSubresourceRange, VkImageSubresourceRange>
{
    ImageAspectFlags    aspectMask;
    uint32_t            baseMipLevel{ 0 };
    uint32_t            levelCount{ 0 };
    uint32_t            baseArrayLayer{ 0 };
    uint32_t            layerCount{ 0 };

    DEFINE_CLASS_MEMBER(ImageSubresourceRange)

    ImageSubresourceRange(uint32_t aBaseMipLevel, uint32_t aLevelCount, uint32_t aBaseArrayLayer, uint32_t aLayerCount, ImageAspectFlags aAspectMask = ImageAspectFlags())
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



class ImageMemoryBarrier : public internal::VkTrait<ImageMemoryBarrier, VkImageMemoryBarrier>
{
private:
    const internal::Structure sType = internal::Structure::eImageMemoryBarrier;

public:
    const void*             pNext{ nullptr };
    AccessFlags             srcAccessMask;
    AccessFlags             dstAccessMask;
    ImageLayout             oldLayout;
    ImageLayout             newLayout;
    uint32_t                srcQueueFamilyIndex{ 0 };
    uint32_t                dstQueueFamilyIndex{ 0 };
    Image                   image;
    ImageSubresourceRange   subresourceRange;

    DEFINE_CLASS_MEMBER(ImageMemoryBarrier)

    ImageMemoryBarrier(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessMask, const ImageLayout& aOldLayout, const ImageLayout& aNewLayout,
        uint32_t aSrcQueueFamilyIndex, uint32_t aDstQueueFamilyIndex, const Image& aImage, const ImageSubresourceRange& aSubresourceRange)
        : srcAccessMask(aSrcAccessMask), dstAccessMask(aDstAccessMask), oldLayout(aOldLayout), newLayout(aNewLayout),
        srcQueueFamilyIndex(aSrcQueueFamilyIndex), dstQueueFamilyIndex(aDstQueueFamilyIndex), image(aImage), subresourceRange(aSubresourceRange)
    {}

    ImageMemoryBarrier& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    ImageMemoryBarrier& SetAccessMask(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessMask)
    {
        srcAccessMask = aSrcAccessMask;
        dstAccessMask = aDstAccessMask;

        return *this;
    }

    ImageMemoryBarrier& SetLayout(const ImageLayout& aOldLayout, const ImageLayout& aNewLayout)
    {
        oldLayout = aOldLayout;
        newLayout = aNewLayout;

        return *this;
    }

    ImageMemoryBarrier& SetQueueFamilyIndex(uint32_t aSrcQueueFamilyIndex, uint32_t aDstQueueFamilyIndex)
    {
        srcQueueFamilyIndex = aSrcQueueFamilyIndex;
        dstQueueFamilyIndex = aDstQueueFamilyIndex;

        return *this;
    }

    ImageMemoryBarrier& SetImage(const Image& aImage, const ImageSubresourceRange& aSubresourceRange)
    {
        image               = aImage;
        subresourceRange    = aSubresourceRange;

        return *this;
    }
};

StaticSizeCheck(ImageMemoryBarrier);



class BufferMemoryBarrier : public internal::VkTrait<BufferMemoryBarrier, VkBufferMemoryBarrier>
{
private:
    const internal::Structure sType = internal::Structure::eBufferMemoryBarrier;

public:
    const void*     pNext{ nullptr };
    AccessFlags     srcAccessMask;
    AccessFlags     dstAccessMask;
    uint32_t        srcQueueFamilyIndex{ UINT32_MAX };
    uint32_t        dstQueueFamilyIndex{ UINT32_MAX };
    Buffer          buffer;
    DeviceSize      offset{ 0 };
    DeviceSize      size{ 0 };

    DEFINE_CLASS_MEMBER(BufferMemoryBarrier)

    BufferMemoryBarrier(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessFlags, uint32_t aSrcQueueFamilyIndex, uint32_t aDstQueueFamilyIndex,
        const Buffer& aBuffer, DeviceSize aOffset, DeviceSize aSize)
        : srcAccessMask(aSrcAccessMask), dstAccessMask(aDstAccessFlags), srcQueueFamilyIndex(aSrcQueueFamilyIndex), dstQueueFamilyIndex(aDstQueueFamilyIndex),
        buffer(aBuffer), offset(aOffset), size(aSize)
    {}

    BufferMemoryBarrier& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    BufferMemoryBarrier& SetAccessMask(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessMask)
    {
        srcAccessMask = aSrcAccessMask;
        dstAccessMask = aDstAccessMask;

        return *this;
    }

    BufferMemoryBarrier& SetFamilyIndex(uint32_t aSrcQueueFamilyIndex, uint32_t aDstQueueFamilyIndex)
    {
        srcQueueFamilyIndex = aSrcQueueFamilyIndex;
        dstQueueFamilyIndex = aDstQueueFamilyIndex;

        return *this;
    }

    BufferMemoryBarrier& SetBufferInfo(const Buffer& aBuffer, DeviceSize aOffset, DeviceSize aSize)
    {
        buffer  = aBuffer;
        offset  = aOffset;
        size    = aSize;

        return *this;
    }
};

StaticSizeCheck(BufferMemoryBarrier);



class MemoryBarrier : public internal::VkTrait<MemoryBarrier, VkMemoryBarrier>
{
private:
    const internal::Structure sType = internal::Structure::eMemoryBarrier;

public:
    const void*     pNext{ nullptr };
    AccessFlags     srcAccessMask;
    AccessFlags     dstAccessMask;

    DEFINE_CLASS_MEMBER(MemoryBarrier)

    MemoryBarrier(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessMask)
        : srcAccessMask(aSrcAccessMask), dstAccessMask(aDstAccessMask)
    {}

    MemoryBarrier& SetAccessMask(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessMask)
    {
        srcAccessMask = aSrcAccessMask;
        dstAccessMask = aDstAccessMask;

        return *this;
    }
};

StaticSizeCheck(MemoryBarrier);



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_MEMORY_BARRIER_H__
