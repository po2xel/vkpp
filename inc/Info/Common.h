#ifndef __VKPP_INFO_COMMON_H__
#define __VKPP_INFO_COMMON_H__



#include <vector>
#include <cassert>
#include <type_traits>

#include <Type/Structure.h>
#include <Type/VkTrait.h>
#include <Info/Flags.h>
#include <Info/TypeConsistency.h>



#define DEFAULT_VK_API_VERSION VK_API_VERSION_1_0



#define DEFINE_CLASS_MEMBER(Class) \
constexpr Class(void) noexcept = default; \
\
explicit Class(const Class::VkType& aRhs) \
{ \
    std::memcpy(this, &aRhs, sizeof(Class)); \
} \
\
Class& operator=(const Class::VkType& aRhs) \
{ \
    std::memcpy(this, &aRhs, sizeof(Class)); \
    return *this; \
}


#define StaticLValueRefAssert(T, ArgName) static_assert(std::is_lvalue_reference_v<T&&>, "Argument \"" #ArgName "\" shouldn't be rvalue reference.");



namespace vkpp
{



template <typename T>
using ValueType = typename std::decay_t<T>::value_type;



template <typename... Ts>
struct EnableIfValueTypeImpl;



template <>
struct EnableIfValueTypeImpl<> : std::true_type
{};



template <typename L, typename R>
struct EnableIfValueTypeImpl<L, R> : std::is_same<L, R>
{};



template <typename L, typename R, typename... Ts>
struct EnableIfValueTypeImpl<L, R, Ts...> : std::conditional_t<EnableIfValueTypeImpl<L, R>::value, EnableIfValueTypeImpl<Ts...>, std::false_type>
{};



template <typename L, typename R, typename... Ts>
using EnableIfValueType = std::enable_if_t<EnableIfValueTypeImpl<L, R, Ts...>::value>;



using Bool32            = VkBool32;
using DeviceSize        = VkDeviceSize;
using Result            = VkResult;
using ClearColorValue   = VkClearColorValue;
using ClearValue        = VkClearValue;



constexpr auto WholeSize = VK_WHOLE_SIZE;



inline void ThrowIfFailed(VkResult aResult)
{
    if (aResult != VK_SUCCESS)
        throw;
}



enum class SharingMode
{
    eExclusive      = VK_SHARING_MODE_EXCLUSIVE,
    eConcurrent     = VK_SHARING_MODE_CONCURRENT
};



enum class DependencyFlagBits
{
    eByRegion = VK_DEPENDENCY_BY_REGION_BIT,
    eViewLocalKHX = VK_DEPENDENCY_VIEW_LOCAL_BIT_KHX,
    eDeviceGroupKHX = VK_DEPENDENCY_DEVICE_GROUP_BIT_KHX
};

VKPP_ENUM_BIT_MASK_FLAGS(Dependency)



enum class SampleCountFlagBits
{
    e1      = VK_SAMPLE_COUNT_1_BIT,
    e2      = VK_SAMPLE_COUNT_2_BIT,
    e4      = VK_SAMPLE_COUNT_4_BIT,
    e8      = VK_SAMPLE_COUNT_8_BIT,
    e16     = VK_SAMPLE_COUNT_16_BIT,
    e32     = VK_SAMPLE_COUNT_32_BIT,
    e64     = VK_SAMPLE_COUNT_64_BIT
};



enum class CompareOp
{
    eNever          = VK_COMPARE_OP_NEVER,
    eLess           = VK_COMPARE_OP_LESS,
    eEqual          = VK_COMPARE_OP_EQUAL,
    eLessOrEqual    = VK_COMPARE_OP_LESS_OR_EQUAL,
    eGreater        = VK_COMPARE_OP_GREATER,
    eNotEqual       = VK_COMPARE_OP_NOT_EQUAL,
    eGreaterOrEqual = VK_COMPARE_OP_GREATER_OR_EQUAL,
    eAlways         = VK_COMPARE_OP_ALWAYS
};



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

VKPP_ENUM_BIT_MASK_FLAGS(ImageUsage)



struct SpecializationMapEntry : public internal::VkTrait<SpecializationMapEntry, VkSpecializationMapEntry>
{
    uint32_t        constantID{ 0 };
    uint32_t        offset{ 0 };
    std::size_t     size{ 0 };

    DEFINE_CLASS_MEMBER(SpecializationMapEntry)

    constexpr SpecializationMapEntry(uint32_t aConstantID, uint32_t aOffset, std::size_t aSize) noexcept
        : constantID(aConstantID), offset(aOffset), size(aSize)
    {}

    SpecializationMapEntry& SetConstantID(uint32_t aConstantID)
    {
        constantID = aConstantID;

        return *this;
    }

    SpecializationMapEntry& SetSize(uint32_t aOffset, std::size_t aSize)
    {
        offset  = aOffset;
        size    = aSize;

        return *this;
    }
};

ConsistencyCheck(SpecializationMapEntry, constantID, offset, size)



struct SpecializationInfo : public internal::VkTrait<SpecializationInfo, VkSpecializationInfo>
{
    uint32_t                        mapEntryCount{ 0 };
    const SpecializationMapEntry*   pMapEntries{ nullptr };
    std::size_t                     dataSize{ 0 };
    const void*                     pData{ nullptr };

    DEFINE_CLASS_MEMBER(SpecializationInfo)

    constexpr SpecializationInfo(uint32_t aMapEntryCount, const SpecializationMapEntry* apMapEntries, std::size_t aDataSize, const void* apData) noexcept
        : mapEntryCount(aMapEntryCount), pMapEntries(apMapEntries), dataSize(aDataSize), pData(apData)
    {}

    SpecializationInfo& SetMapEntries(uint32_t aMapEntryCount, const SpecializationMapEntry* apMapEntries)
    {
        mapEntryCount   = aMapEntryCount;
        pMapEntries     = apMapEntries;

        return *this;
    }

    SpecializationInfo& SetData(const std::size_t aDataSize, const void* apData)
    {
        dataSize    = aDataSize;
        pData       = apData;

        return *this;
    }
};

ConsistencyCheck(SpecializationInfo, mapEntryCount, pMapEntries, dataSize, pData)



struct Viewport : public internal::VkTrait<Viewport, VkViewport>
{
    float   x{ 0 };
    float   y{ 0 };
    float   width{ 0 };
    float   height{ 0 };
    float   minDepth{ 0 };
    float   maxDepth{ 0 };

    constexpr Viewport(float aOriginX, float aOriginY, float aWidth, float aHeight, float aMinDepth = 0.0f, float aMaxDepth = 1.0f) noexcept
        : x(aOriginX), y(aOriginY), width(aWidth), height(aHeight), minDepth(aMinDepth), maxDepth(aMaxDepth)
    {}

    Viewport& SetOrigin(float aOriginX, float aOriginY)
    {
        x = aOriginX;
        y = aOriginY;

        return *this;
    }

    Viewport& SetDimension(float aWidth, float aHeight)
    {
        width   = aWidth;
        height  = aHeight;

        return *this;
    }

    Viewport& SetDepth(float aMinDepth, float aMaxDepth)
    {
        minDepth = aMinDepth;
        maxDepth = aMaxDepth;

        return *this;
    }
};

ConsistencyCheck(Viewport, x, y, width, height, minDepth, maxDepth)



struct Offset2D : public internal::VkTrait<Offset2D, VkOffset2D>
{
    int32_t     x{ 0 };
    int32_t     y{ 0 };

    DEFINE_CLASS_MEMBER(Offset2D)

    constexpr Offset2D(int32_t aX, int32_t aY) noexcept : x(aX), y(aY)
    {}

    Offset2D& SetOffset(int32_t aX, int32_t aY)
    {
        x = aX;
        y = aY;

        return *this;
    }
};

ConsistencyCheck(Offset2D, x, y)



struct Offset3D : public internal::VkTrait<Offset3D, VkOffset3D>
{
    int32_t     x{ 0 };
    int32_t     y{ 0 };
    int32_t     z{ 0 };

    DEFINE_CLASS_MEMBER(Offset3D)

    constexpr Offset3D(int32_t aX, int32_t aY, int32_t aZ) noexcept : x(aX), y(aY), z(aZ)
    {}

    Offset3D& SetOffset(int32_t aX, int32_t aY, int32_t aZ)
    {
        x = aX;
        y = aY;
        z = aZ;

        return *this;
    }
};

ConsistencyCheck(Offset3D, x, y, z)



struct Extent2D : public internal::VkTrait<Extent2D, VkExtent2D>
{
    uint32_t width{ 0 };
    uint32_t height{ 0 };

    DEFINE_CLASS_MEMBER(Extent2D)

    constexpr Extent2D(uint32_t aWidth, uint32_t aHeight) noexcept : width(aWidth), height(aHeight)
    {}

    Extent2D& SetWidth(uint32_t aWidth)
    {
        width = aWidth;

        return *this;
    }

    Extent2D& SetHeight(uint32_t aHeight)
    {
        height = aHeight;

        return *this;
    }

    constexpr bool operator==(const Extent2D& aRhs) const
    {
        return width == aRhs.width && height == aRhs.height;
    }

    constexpr bool operator!=(const Extent2D& aRhs) const
    {
        return !(*this == aRhs);
    }
};

ConsistencyCheck(Extent2D, width, height)



struct Extent3D : public internal::VkTrait<Extent3D, VkExtent3D>
{
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    uint32_t depth{ 0 };

    DEFINE_CLASS_MEMBER(Extent3D)

    constexpr Extent3D(uint32_t aWidth, uint32_t aHeight, uint32_t aDepth) noexcept : width(aWidth), height(aHeight), depth(aDepth)
    {}

    constexpr Extent3D(const Extent2D& aExtent2D) noexcept : width(aExtent2D.width), height(aExtent2D.height), depth(1)
    {}

    Extent3D& SetWidth(uint32_t aWidth)
    {
        width = aWidth;

        return *this;
    }

    Extent3D& SetHeight(uint32_t aHeight)
    {
        height = aHeight;

        return *this;
    }

    Extent3D& SetDepth(uint32_t aDepth)
    {
        depth = aDepth;

        return *this;
    }

    constexpr bool operator==(const Extent3D& aRhs) const
    {
        return width == aRhs.width &&
               height == aRhs.height &&
               depth == aRhs.depth;
    }

    constexpr bool operator!=(const Extent3D& aRhs) const
    {
        return !(*this == aRhs);
    }

    constexpr operator Extent2D(void) const
    {
        return Extent2D{ width, height };
    }
};

ConsistencyCheck(Extent3D, width, height, depth)



struct Rect2D : public internal::VkTrait<Rect2D, VkRect2D>
{
    Offset2D    offset;
    Extent2D    extent;

    DEFINE_CLASS_MEMBER(Rect2D)

    constexpr Rect2D(const Offset2D& aOffset, const Extent2D& aExtent) noexcept : offset(aOffset), extent(aExtent)
    {}

    Rect2D& SetOffset(const Offset2D& aOffset)
    {
        offset = aOffset;

        return *this;
    }

    Rect2D& SetExtent(const Extent2D& aExtent)
    {
        extent = aExtent;

        return *this;
    }
};

ConsistencyCheck(Rect2D, offset, extent)



}                    // End of namespace vkpp.



#endif             // __VKPP_INFO_COMMON_H__
