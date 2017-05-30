#ifndef __VKPP_TYPE_MEMORY_BARRIER_H__
#define __VKPP_TYPE_MEMORY_BARRIER_H__



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

VKPP_ENUM_BIT_MASK_FLAGS(Access)



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
    uint32_t                srcQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };
    uint32_t                dstQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };
    Image                   image;
    ImageSubresourceRange   subresourceRange;

    DEFINE_CLASS_MEMBER(ImageMemoryBarrier)

    ImageMemoryBarrier(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessMask, const ImageLayout& aOldLayout, const ImageLayout& aNewLayout,
        const Image& aImage, const ImageSubresourceRange& aSubresourceRange,
        uint32_t aSrcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, uint32_t aDstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED) noexcept
        : srcAccessMask(aSrcAccessMask), dstAccessMask(aDstAccessMask), oldLayout(aOldLayout), newLayout(aNewLayout),
        srcQueueFamilyIndex(aSrcQueueFamilyIndex), dstQueueFamilyIndex(aDstQueueFamilyIndex), image(aImage), subresourceRange(aSubresourceRange)
    {}

    ImageMemoryBarrier& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    ImageMemoryBarrier& SetAccessMask(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessMask) noexcept
    {
        srcAccessMask = aSrcAccessMask;
        dstAccessMask = aDstAccessMask;

        return *this;
    }

    ImageMemoryBarrier& SetLayout(const ImageLayout& aOldLayout, const ImageLayout& aNewLayout) noexcept
    {
        oldLayout = aOldLayout;
        newLayout = aNewLayout;

        return *this;
    }

    ImageMemoryBarrier& SetQueueFamilyIndex(uint32_t aSrcQueueFamilyIndex, uint32_t aDstQueueFamilyIndex) noexcept
    {
        srcQueueFamilyIndex = aSrcQueueFamilyIndex;
        dstQueueFamilyIndex = aDstQueueFamilyIndex;

        return *this;
    }

    ImageMemoryBarrier& SetImage(const Image& aImage, const ImageSubresourceRange& aSubresourceRange) noexcept
    {
        image               = aImage;
        subresourceRange    = aSubresourceRange;

        return *this;
    }
};

ConsistencyCheck(ImageMemoryBarrier, pNext, srcAccessMask, dstAccessMask, oldLayout, newLayout, srcQueueFamilyIndex, dstQueueFamilyIndex, image, subresourceRange)



class BufferMemoryBarrier : public internal::VkTrait<BufferMemoryBarrier, VkBufferMemoryBarrier>
{
private:
    const internal::Structure sType = internal::Structure::eBufferMemoryBarrier;

public:
    const void*     pNext{ nullptr };
    AccessFlags     srcAccessMask;
    AccessFlags     dstAccessMask;
    uint32_t        srcQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };
    uint32_t        dstQueueFamilyIndex{ VK_QUEUE_FAMILY_IGNORED };
    Buffer          buffer;
    DeviceSize      offset{ 0 };
    DeviceSize      size{ 0 };

    DEFINE_CLASS_MEMBER(BufferMemoryBarrier)

    BufferMemoryBarrier(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessFlags, uint32_t aSrcQueueFamilyIndex, uint32_t aDstQueueFamilyIndex,
        const Buffer& aBuffer, DeviceSize aOffset, DeviceSize aSize) noexcept
        : srcAccessMask(aSrcAccessMask), dstAccessMask(aDstAccessFlags), srcQueueFamilyIndex(aSrcQueueFamilyIndex), dstQueueFamilyIndex(aDstQueueFamilyIndex),
        buffer(aBuffer), offset(aOffset), size(aSize)
    {}

    BufferMemoryBarrier& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    BufferMemoryBarrier& SetAccessMask(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessMask) noexcept
    {
        srcAccessMask = aSrcAccessMask;
        dstAccessMask = aDstAccessMask;

        return *this;
    }

    BufferMemoryBarrier& SetFamilyIndex(uint32_t aSrcQueueFamilyIndex, uint32_t aDstQueueFamilyIndex) noexcept
    {
        srcQueueFamilyIndex = aSrcQueueFamilyIndex;
        dstQueueFamilyIndex = aDstQueueFamilyIndex;

        return *this;
    }

    BufferMemoryBarrier& SetBufferInfo(const Buffer& aBuffer, DeviceSize aOffset, DeviceSize aSize) noexcept
    {
        buffer  = aBuffer;
        offset  = aOffset;
        size    = aSize;

        return *this;
    }
};

ConsistencyCheck(BufferMemoryBarrier, pNext, srcAccessMask, dstAccessMask, srcQueueFamilyIndex, dstQueueFamilyIndex, buffer, offset, size)



class MemoryBarrier : public internal::VkTrait<MemoryBarrier, VkMemoryBarrier>
{
private:
    const internal::Structure sType = internal::Structure::eMemoryBarrier;

public:
    const void*     pNext{ nullptr };
    AccessFlags     srcAccessMask;
    AccessFlags     dstAccessMask;

    DEFINE_CLASS_MEMBER(MemoryBarrier)

    constexpr MemoryBarrier(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessMask) noexcept
        : srcAccessMask(aSrcAccessMask), dstAccessMask(aDstAccessMask)
    {}

    MemoryBarrier& SetNext(const void* apNext) noexcept
    {
        pNext   = apNext;

        return *this;
    }

    MemoryBarrier& SetAccessMask(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessMask) noexcept
    {
        srcAccessMask = aSrcAccessMask;
        dstAccessMask = aDstAccessMask;

        return *this;
    }
};

ConsistencyCheck(MemoryBarrier, pNext, srcAccessMask, dstAccessMask)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_MEMORY_BARRIER_H__
