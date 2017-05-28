#ifndef __VKPP_TYPE_BUFFER_H__
#define __VKPP_TYPE_BUFFER_H__



#include <Info/Common.h>



namespace vkpp
{



enum class IndexType
{
    eUInt16     = VK_INDEX_TYPE_UINT16,
    eUInt32     = VK_INDEX_TYPE_UINT32
};



enum class BufferCreateFlagBits
{
    eSparseBinding      = VK_BUFFER_CREATE_SPARSE_BINDING_BIT,
    eSparseResidency    = VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT,
    eSparseAliased      = VK_BUFFER_CREATE_SPARSE_ALIASED_BIT
};

VKPP_ENUM_BIT_MASK_FLAGS(BufferCreate)



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

VKPP_ENUM_BIT_MASK_FLAGS(BufferUsage)



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

    constexpr BufferCreateInfo(DeviceSize aSize, const BufferUsageFlags& aUsage, const BufferCreateFlags& aFlags = DefaultFlags) noexcept
        : flags(aFlags), size(aSize), usage(aUsage)
    {}

    constexpr BufferCreateInfo(DeviceSize aSize, const BufferUsageFlags& aUsage, uint32_t aQueueFamilyIndexCount, const uint32_t* apQueueFamilyIndices, const BufferCreateFlags& aFlags = DefaultFlags) noexcept
        : flags(aFlags), size(aSize), usage(aUsage), sharingMode(SharingMode::eConcurrent), queueFamilyIndexCount(aQueueFamilyIndexCount), pQueueFamilyIndices(apQueueFamilyIndices)
    {
        assert(aQueueFamilyIndexCount != 0 && apQueueFamilyIndices != nullptr);
    }

    template <typename Q, typename = EnableIfValueType<ValueType<Q>, uint32_t>>
    constexpr BufferCreateInfo(DeviceSize aSize, const BufferUsageFlags& aUsage, Q&& aQueueFamilyIndices, const BufferCreateFlags& aFlags = DefaultFlags) noexcept
        : BufferCreateInfo(aSize, aUsage, static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data(), aFlags)
    {
        StaticLValueRefAssert(Q, aQueueFamilyIndices);
    }

    BufferCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    BufferCreateInfo& SetFlags(const BufferCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    BufferCreateInfo& SetSize(DeviceSize aSize)
    {
        size = aSize;

        return *this;
    }

    BufferCreateInfo& SetUsage(const BufferUsageFlags& aUsage)
    {
        usage = aUsage;

        return *this;
    }

    BufferCreateInfo& SetExclusiveMode(void)
    {
        sharingMode             = SharingMode::eExclusive;
        queueFamilyIndexCount   = 0;
        pQueueFamilyIndices     = nullptr;

        return *this;
    }

    BufferCreateInfo& SetConcurrentMode(uint32_t aQueueFamilyIndexCount, const uint32_t* apQueueFamilyIndices)
    {
        assert(aQueueFamilyIndexCount != 0 && apQueueFamilyIndices != nullptr);

        sharingMode             = SharingMode::eConcurrent;
        queueFamilyIndexCount   = aQueueFamilyIndexCount;
        pQueueFamilyIndices     = apQueueFamilyIndices;

        return *this;
    }

    template <typename Q, typename = EnableIfValueType<ValueType<Q>, uint32_t>>
    BufferCreateInfo& SetConcurrentMode(Q&& aQueueFamilyIndices)
    {
        StaticLValueRefAssert(Q, aQueueFamilyIndices);

        return SetConcurrentMode(static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data());
    }
};

ConsistencyCheck(BufferCreateInfo, pNext, flags, size, usage, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices)



struct BufferCopy : public internal::VkTrait<BufferCopy, VkBufferCopy>
{
    DeviceSize srcOffset{ 0 };
    DeviceSize dstOffset{ 0 };
    DeviceSize size{ 0 };

    DEFINE_CLASS_MEMBER(BufferCopy)

    constexpr BufferCopy(DeviceSize aSize) noexcept : size(aSize)
    {}

    constexpr BufferCopy(DeviceSize aSrcOffset, DeviceSize aDstOffset, DeviceSize aSize) noexcept : srcOffset(aSrcOffset), dstOffset(aDstOffset), size(aSize)
    {}

    BufferCopy& SetOffset(DeviceSize aSrcOffset, DeviceSize aDstOffset)
    {
        srcOffset = aSrcOffset;
        dstOffset = aDstOffset;

        return *this;
    }

    BufferCopy& SetSize(DeviceSize aSize)
    {
        size = aSize;

        return *this;
    }
};

ConsistencyCheck(BufferCopy, srcOffset, dstOffset, size)



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

StaticSizeCheck(Buffer)



enum class BufferViewCreateFlagBits
{};

VKPP_ENUM_BIT_MASK_FLAGS(BufferViewCreate)



class BufferViewCreateInfo : public internal::VkTrait<BufferViewCreateInfo, VkBufferViewCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eBufferView;

public:
    const void*             pNext{ nullptr };
    BufferViewCreateFlags   flags;
    Buffer                  buffer;
    Format                  format{ Format::eUndefined };
    DeviceSize              offset{ 0 };
    DeviceSize              range{ 0 };

    DEFINE_CLASS_MEMBER(BufferViewCreateInfo)

    BufferViewCreateInfo(const Buffer& aBuffer, Format aFormat, DeviceSize aOffset, DeviceSize aRange, const BufferViewCreateFlags& aFlags = DefaultFlags) noexcept
        : flags(aFlags), buffer(aBuffer), format(aFormat), offset(aOffset), range(aRange)
    {}

    BufferViewCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    BufferViewCreateInfo& SetFlags(const BufferViewCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    BufferViewCreateInfo& SetBuffer(const Buffer& aBuffer, Format aFormat, DeviceSize aOffset, DeviceSize aRange)
    {
        buffer  = aBuffer;
        format  = aFormat;
        offset  = aOffset;
        range   = aRange;

        return *this;
    }
};

ConsistencyCheck(BufferViewCreateInfo, pNext, flags, buffer, format, offset, range)



class BufferView : public internal::VkTrait<BufferView, VkBufferView>
{
private:
    VkBufferView mBufferView{ VK_NULL_HANDLE };

public:
    BufferView(void) = default;

    BufferView(std::nullptr_t)
    {}

    explicit BufferView(VkBufferView aBufferView) : mBufferView(aBufferView)
    {}
};

StaticSizeCheck(BufferView)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_BUFFER_H__
