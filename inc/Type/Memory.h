#ifndef __VKPP_TYPE_MEMORY_H__
#define __VKPP_TYPE_MEMORY_H__



#include <Info/Common.h>
#include <Info/Flags.h>



namespace vkpp
{



class MemoryAllocateInfo : public internal::VkTrait<MemoryAllocateInfo, VkMemoryAllocateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eMemoryAllocate;

public:
    const void*         pNext{ nullptr };
    DeviceSize          allocationSize{ 0 };
    uint32_t            memoryTypeIndex{ 0 };

    DEFINE_CLASS_MEMBER(MemoryAllocateInfo)

    constexpr MemoryAllocateInfo(DeviceSize aAllocationSize, uint32_t aMemoryTypeIndex) noexcept
        : allocationSize(aAllocationSize), memoryTypeIndex(aMemoryTypeIndex)
    {}

    MemoryAllocateInfo& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    MemoryAllocateInfo& SetSize(DeviceSize aAllocationSize, uint32_t aMemoryTypeIndex) noexcept
    {
        allocationSize  = aAllocationSize;
        memoryTypeIndex = aMemoryTypeIndex;

        return *this;
    }
};

ConsistencyCheck(MemoryAllocateInfo, pNext, allocationSize, memoryTypeIndex)



class DeviceMemory : public internal::VkTrait<DeviceMemory, VkDeviceMemory>
{
private:
    VkDeviceMemory mDeviceMemory{ VK_NULL_HANDLE };

public:
    DeviceMemory(void) noexcept = default;

    DeviceMemory(std::nullptr_t) noexcept
    {}

    explicit DeviceMemory(VkDeviceMemory aDeviceMemory) noexcept : mDeviceMemory(aDeviceMemory)
    {}
};

StaticSizeCheck(DeviceMemory)



enum class MemoryMapFlagBits
{};

VKPP_ENUM_BIT_MASK_FLAGS(MemoryMap)



class MappedMemoryRange : public internal::VkTrait<MappedMemoryRange, VkMappedMemoryRange>
{
private:
    const internal::Structure sType = internal::Structure::eMappedMemoryRange;

public:
    const void*         pNext{ nullptr };
    DeviceMemory        memory;
    DeviceSize          offset{ 0 };
    DeviceSize          size{ VK_WHOLE_SIZE };

    DEFINE_CLASS_MEMBER(MappedMemoryRange)

    MappedMemoryRange(const DeviceMemory& aDeviceMemory, DeviceSize aOffset = 0, DeviceSize aSize = VK_WHOLE_SIZE) noexcept
        : memory(aDeviceMemory), offset(aOffset), size(aSize)
    {}

    MappedMemoryRange& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    MappedMemoryRange& SetMemory(const DeviceMemory& aDeviceMemory) noexcept
    {
        memory = aDeviceMemory;

        return *this;
    }

    MappedMemoryRange& SetRange(DeviceSize aOffset, DeviceSize aSize = VK_WHOLE_SIZE) noexcept
    {
        offset  = aOffset;
        size    = aSize;

        return *this;
    }
};

ConsistencyCheck(MappedMemoryRange, pNext, memory, offset, size)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_MEMORY_H__
