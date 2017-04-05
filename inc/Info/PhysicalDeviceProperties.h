#ifndef __VKPP_INFO_PHYSICAL_DEVICE_PROPERTIES_H__
#define __VKPP_INFO_PHYSICAL_DEVICE_PROPERTIES_H__



#include <Info/Common.h>
#include <Info/Flags.h>
#include <Type/Extent3D.h>



namespace vkpp
{



enum class PhysicalDeviceType
{
    eOther              = VK_PHYSICAL_DEVICE_TYPE_OTHER,
    eIntegrateGPU       = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
    eDiscreteGPU        = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
    eVirtualGPU         = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,
    eCPU                = VK_PHYSICAL_DEVICE_TYPE_CPU
};



enum class MemoryPropertyFlagBits
{
    eDeviceLocal        = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    eHostVisible        = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    eHostCoherent       = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    eHostCached         = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
    eLazilyAllocated    = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT
};

using MemoryPropertyFlags = internal::Flags<MemoryPropertyFlagBits, VkMemoryPropertyFlags>;



enum class MemoryHeapFlagBits
{
    eDeviceLocal        = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
    eMultiInstanceKHX   = VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHX
};

using MemoryHeapFlags = internal::Flags<MemoryHeapFlagBits, VkMemoryHeapFlags>;



enum class QueueFlagBits
{
    eGraphics       = VK_QUEUE_GRAPHICS_BIT,
    eCompute        = VK_QUEUE_COMPUTE_BIT,
    eTransfer       = VK_QUEUE_TRANSFER_BIT,
    eSparseBinding  = VK_QUEUE_SPARSE_BINDING_BIT
};

using QueueFlags = internal::Flags<QueueFlagBits, VkQueueFlags>;



// TODO: PhysicalDeviceFeatures.
using PhysicalDeviceFeatures = VkPhysicalDeviceFeatures;
using PhysicalDeviceLimits = VkPhysicalDeviceLimits;



struct QueueFamilyProperties : public internal::VkTrait<QueueFamilyProperties, VkQueueFamilyProperties>
{
    QueueFlags  queueFlags;
    uint32_t    queueCount{ 0 };
    uint32_t    timestampValidBits{ 0 };
    Extent3D    minImageTransferGranularity;

    DEFINE_CLASS_MEMBER(QueueFamilyProperties)
};

StaticSizeCheck(QueueFamilyProperties);



struct PhysicalDeviceSparseProperties : public internal::VkTrait<PhysicalDeviceSparseProperties, VkPhysicalDeviceSparseProperties>
{
    Bool32 residencyStandard2DBlockShape{ VK_FALSE };
    Bool32 residencyStandard2DMultisampleBlockShape{ VK_FALSE };
    Bool32 residencyStandard3DBlockShape{ VK_FALSE };
    Bool32 residencyAlignedMipSize{ VK_FALSE };
    Bool32 residencyNonResidentStrict{ VK_FALSE };

    DEFINE_CLASS_MEMBER(PhysicalDeviceSparseProperties)
};

StaticSizeCheck(PhysicalDeviceSparseProperties);



struct MemoryType : public internal::VkTrait<MemoryType, VkMemoryType>
{
    MemoryPropertyFlags propertyFlags;
    uint32_t            heapIndex{ 0 };

    DEFINE_CLASS_MEMBER(MemoryType)
};

StaticSizeCheck(MemoryType);



struct MemoryHeap : public internal::VkTrait<MemoryHeap, VkMemoryHeap>
{
    DeviceSize      size{ 0 };
    MemoryHeapFlags flags;

    DEFINE_CLASS_MEMBER(MemoryHeap)
};

StaticSizeCheck(MemoryHeap);



struct PhysicalDeviceMemoryProperties : public internal::VkTrait<PhysicalDeviceMemoryProperties, VkPhysicalDeviceMemoryProperties>
{
    uint32_t        memoryTypeCount{ 0 };
    MemoryType      memoryTypes[VK_MAX_MEMORY_TYPES];

    uint32_t        memoryHeapCount{ 0 };
    MemoryHeap      memoryHeaps[VK_MAX_MEMORY_HEAPS];

    DEFINE_CLASS_MEMBER(PhysicalDeviceMemoryProperties)
};



struct PhysicalDeviceProperties : public internal::VkTrait<PhysicalDeviceProperties, VkPhysicalDeviceProperties>
{
    uint32_t            apiVersion{ 0 };
    uint32_t            apiDriverVersion{ 0 };
    uint32_t            vendorID{ 0 };
    uint32_t            deviceID{ 0 };
    PhysicalDeviceType  deviceType;
    char                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE]{};
    uint8_t             pipelineCacheUUID[VK_UUID_SIZE]{};
    PhysicalDeviceLimits  limits{};               // TODO
    PhysicalDeviceSparseProperties  sparseProperties;

    DEFINE_CLASS_MEMBER(PhysicalDeviceProperties)
};

StaticSizeCheck(PhysicalDeviceSparseProperties);



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_PHYSICAL_DEVICE_PROPERTIES_H__
