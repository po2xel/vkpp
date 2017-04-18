#ifndef __VKPP_INFO_COMMON_H__
#define __VKPP_INFO_COMMON_H__



#include <cstring>
#include <cstdint>
#include <cassert>

#include <Type/Structure.h>
#include <Type/VkTrait.h>
#include <vector>
#include <Info/Flags.h>



#define DEFAULT_VK_API_VERSION VK_API_VERSION_1_0



#define DEFINE_CLASS_MEMBER(Class) \
Class(void) = default; \
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



#define StaticSizeCheck(Class) static_assert(sizeof(Class) == sizeof(Class::VkType))



namespace vkpp
{



using Bool32            = VkBool32;
using DeviceSize        = VkDeviceSize;
using Result            = VkResult;
using ClearColorValue   = VkClearColorValue;
using ClearValue        = VkClearValue;


template <typename T>
using Array = std::vector<T>;



inline void ThrowIfFailed(VkResult aResult)
{
    if (aResult != VK_SUCCESS)
        throw;
}



enum class SharingMode
{
    eExclusive = VK_SHARING_MODE_EXCLUSIVE,
    eConcurrent = VK_SHARING_MODE_CONCURRENT
};



enum class DependencyFlagBits
{
    eByRegion = VK_DEPENDENCY_BY_REGION_BIT,
    eViewLocalKHX = VK_DEPENDENCY_VIEW_LOCAL_BIT_KHX,
    eDeviceGroupKHX = VK_DEPENDENCY_DEVICE_GROUP_BIT_KHX
};

using DependencyFlags = internal::Flags<DependencyFlagBits, VkDependencyFlags>;



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


struct SpecializationMapEntry : public internal::VkTrait<SpecializationMapEntry, VkSpecializationMapEntry>
{
    uint32_t        constantID{ 0 };
    uint32_t        offset{ 0 };
    std::size_t     size{ 0 };

    DEFINE_CLASS_MEMBER(SpecializationMapEntry)

    SpecializationMapEntry(uint32_t aConstantID, uint32_t aOffset, std::size_t aSize)
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

StaticSizeCheck(SpecializationMapEntry);



struct SpecializationInfo : public internal::VkTrait<SpecializationInfo, VkSpecializationInfo>
{
    uint32_t                        mapEntryCount{ 0 };
    const SpecializationMapEntry*   pMapEntries{ nullptr };
    std::size_t                     dataSize{ 0 };
    const void*                     pData{ nullptr };

    DEFINE_CLASS_MEMBER(SpecializationInfo)

    SpecializationInfo(uint32_t aMapEntryCount, const SpecializationMapEntry* apMapEntries, std::size_t aDataSize, const void* apData)
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

StaticSizeCheck(SpecializationInfo);



struct Viewport : public internal::VkTrait<Viewport, VkViewport>
{
    float   x{ 0 };
    float   y{ 0 };
    float   width{ 0 };
    float   height{ 0 };
    float   minDepth{ 0 };
    float   maxDepth{ 0 };

    Viewport(float aOriginX, float aOriginY, float aWidth, float aHeight, float aMinDepth, float aMaxDepth)
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

StaticSizeCheck(Viewport);



struct Offset2D : public internal::VkTrait<Offset2D, VkOffset2D>
{
    int32_t     x{ 0 };
    int32_t     y{ 0 };

    DEFINE_CLASS_MEMBER(Offset2D)

    Offset2D(int32_t aX, int32_t aY) : x(aX), y(aY)
    {}

    Offset2D& SetOffset(int32_t aX, int32_t aY)
    {
        x = aX;
        y = aY;

        return *this;
    }
};

StaticSizeCheck(Offset2D);



struct Extent2D : public internal::VkTrait<Extent2D, VkExtent2D>
{
    uint32_t width{ 0 };
    uint32_t height{ 0 };

    DEFINE_CLASS_MEMBER(Extent2D)

    Extent2D(uint32_t aWidth, uint32_t aHeight) : width(aWidth), height(aHeight)
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

    bool operator==(const Extent2D& aRhs) const
    {
        return width == aRhs.width &&
            height == aRhs.height;
    }

    bool operator!=(const Extent2D& aRhs) const
    {
        return !(*this == aRhs);
    }
};

StaticSizeCheck(Extent2D);



struct Extent3D : public internal::VkTrait<Extent3D, VkExtent3D>
{
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    uint32_t depth{ 0 };

    DEFINE_CLASS_MEMBER(Extent3D)

    Extent3D(uint32_t aWidth, uint32_t aHeight, uint32_t aDepth) : width(aWidth), height(aHeight), depth(aDepth)
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

    bool operator==(const Extent3D& aRhs) const
    {
        return width == aRhs.width &&
               height == aRhs.height &&
               depth == aRhs.depth;
    }

    bool operator!=(const Extent3D& aRhs) const
    {
        return !(*this == aRhs);
    }
};

StaticSizeCheck(Extent3D);



struct Rect2D : public internal::VkTrait<Rect2D, VkRect2D>
{
    Offset2D    offset;
    Extent2D    extent;

    DEFINE_CLASS_MEMBER(Rect2D)

    Rect2D(const Offset2D& aOffset, const Extent2D& aExtent) : offset(aOffset), extent(aExtent)
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

StaticSizeCheck(Rect2D);



}                    // End of namespace vkpp.



#endif             // __VKPP_INFO_COMMON_H__
