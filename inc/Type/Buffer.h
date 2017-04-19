#ifndef __VKPP_TYPE_BUFFER_H__
#define __VKPP_TYPE_BUFFER_H__



#include <Info/Common.h>



namespace vkpp
{



enum class BufferCreateFlagBits
{
    eSparseBinding      = VK_BUFFER_CREATE_SPARSE_BINDING_BIT,
    eSparseResidency    = VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT,
    eSparseAliased      = VK_BUFFER_CREATE_SPARSE_ALIASED_BIT
};

using BufferCreateFlags = internal::Flags<BufferCreateFlagBits, VkBufferCreateFlags>;



enum class BufferUsageFlagBits
{
    eTransferSrc        = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    eTransferDst        = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    eUniformTexelBuffer = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
    eStorageTexelBuffer = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
    eUniformBuffer      = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    eStorageBuffer      = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    eIndexBuffer        = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    eVertexBuffer       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    eIndirectBuffer     = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
};

using BufferUsageFlags = internal::Flags<BufferUsageFlagBits, VkBufferUsageFlags>;



class BufferCreateInfo : public internal::VkTrait<BufferCreateInfo, VkBufferCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eBuffer;

public:
    const void*         pNext{ nullptr };
    BufferCreateFlags   flags;
    DeviceSize          size{ 0 };
    BufferUsageFlags    usage;
    SharingMode         sharingMode{ SharingMode::eExclusive };
    uint32_t            queueFamilyIndexCount{ 0 };
    const uint32_t*     pQueueFamilyIndices{ nullptr };

    DEFINE_CLASS_MEMBER(BufferCreateInfo)

    BufferCreateInfo(BufferCreateFlags aFlags, DeviceSize aSize, BufferUsageFlags aUsage, SharingMode aSharingMode,
        uint32_t aQueueFamilyIndexCount, const uint32_t* apQueueFamilyIndices)
        : flags(aFlags), size(aSize), usage(aUsage), sharingMode(aSharingMode), queueFamilyIndexCount(aQueueFamilyIndexCount), pQueueFamilyIndices(apQueueFamilyIndices)
    {}

    BufferCreateInfo(BufferCreateFlags aFlags, DeviceSize aSize, BufferUsageFlags aUsage, SharingMode aSharingMode, const std::vector<uint32_t>& aQueueFamilyIndices)
        : BufferCreateInfo(aFlags, aSize, aUsage, aSharingMode, static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data())
    {}

    template <std::size_t Q>
    BufferCreateInfo(BufferCreateFlags aFlags, DeviceSize aSize, BufferUsageFlags aUsage, SharingMode aSharingMode, const std::array<uint32_t, Q>& aQueueFamilyIndices)
        : BufferCreateInfo(aFlags, aSize, aUsage, aSharingMode, static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data())
    {}

    BufferCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    BufferCreateInfo& SetFlags(BufferCreateFlags aFlags)
    {
        flags = aFlags;

        return *this;
    }

    BufferCreateInfo& SetSize(DeviceSize aSize)
    {
        size = aSize;

        return *this;
    }

    BufferCreateInfo& SetUsage(BufferUsageFlags aUsage)
    {
        usage = aUsage;

        return *this;
    }

    BufferCreateInfo& SetSharingMode(SharingMode aSharingMode)
    {
        sharingMode = aSharingMode;

        return *this;
    }

    BufferCreateInfo& SetQueueFamilyIndices(uint32_t aQueueFamilyIndexCount, const uint32_t* apQueueFamilyIndices)
    {
        queueFamilyIndexCount = aQueueFamilyIndexCount;
        pQueueFamilyIndices = apQueueFamilyIndices;

        return *this;
    }

    BufferCreateInfo& SetQueueFamilyIndices(const std::vector<uint32_t>& aQueueFamilyIndices)
    {
        return SetQueueFamilyIndices(static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data());
    }

    template <std::size_t Q>
    BufferCreateInfo& SetQueueFamilyIndices(const std::array<uint32_t, Q>& aQueueFamilyIndices)
    {
        return SetQueueFamilyIndices(static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data());
    }
};

StaticSizeCheck(BufferCreateInfo);



class Buffer : public internal::VkTrait<Buffer, VkBuffer>
{
private:
    VkBuffer mBuffer{ VK_NULL_HANDLE };

public:
    Buffer(void) = default;

    Buffer(std::nullptr_t)
    {}

    explicit Buffer(VkBuffer aBuffer) : mBuffer(aBuffer)
    {}
};

StaticSizeCheck(Buffer);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_BUFFER_H__
