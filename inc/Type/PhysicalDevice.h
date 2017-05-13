#ifndef __VKPP_TYPE_PHYSICAL_DEVICE_H__
#define __VKPP_TYPE_PHYSICAL_DEVICE_H__



#include <vector>

#include <Info/Common.h>
#include <Info/Layers.h>
#include <Info/Extensions.h>
#include <Info/SurfaceCapabilities.h>
#include <Info/PhysicalDeviceFeatures.h>

#include <Type/Surface.h>
#include <Type/Swapchain.h>



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

VKPP_ENUM_BIT_MASK_FLAGS(MemoryProperty)



enum class MemoryHeapFlagBits
{
    eDeviceLocal        = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
    eMultiInstanceKHX   = VK_MEMORY_HEAP_MULTI_INSTANCE_BIT_KHX
};

VKPP_ENUM_BIT_MASK_FLAGS(MemoryHeap)



enum class QueueFlagBits
{
    eGraphics       = VK_QUEUE_GRAPHICS_BIT,
    eCompute        = VK_QUEUE_COMPUTE_BIT,
    eTransfer       = VK_QUEUE_TRANSFER_BIT,
    eSparseBinding  = VK_QUEUE_SPARSE_BINDING_BIT
};

VKPP_ENUM_BIT_MASK_FLAGS(Queue)



struct QueueFamilyProperties : public internal::VkTrait<QueueFamilyProperties, VkQueueFamilyProperties>
{
    QueueFlags  queueFlags;
    uint32_t    queueCount{ 0 };
    uint32_t    timestampValidBits{ 0 };
    Extent3D    minImageTransferGranularity;

    DEFINE_CLASS_MEMBER(QueueFamilyProperties)
};

ConsistencyCheck(QueueFamilyProperties, queueFlags, queueCount, timestampValidBits, minImageTransferGranularity)



struct PhysicalDeviceSparseProperties : public internal::VkTrait<PhysicalDeviceSparseProperties, VkPhysicalDeviceSparseProperties>
{
    Bool32 residencyStandard2DBlockShape{ VK_FALSE };
    Bool32 residencyStandard2DMultisampleBlockShape{ VK_FALSE };
    Bool32 residencyStandard3DBlockShape{ VK_FALSE };
    Bool32 residencyAlignedMipSize{ VK_FALSE };
    Bool32 residencyNonResidentStrict{ VK_FALSE };

    DEFINE_CLASS_MEMBER(PhysicalDeviceSparseProperties)
};

ConsistencyCheck(PhysicalDeviceSparseProperties, residencyStandard2DBlockShape, residencyStandard2DMultisampleBlockShape, residencyStandard3DBlockShape, residencyAlignedMipSize, residencyNonResidentStrict)



struct MemoryType : public internal::VkTrait<MemoryType, VkMemoryType>
{
    MemoryPropertyFlags propertyFlags;
    uint32_t            heapIndex{ 0 };

    DEFINE_CLASS_MEMBER(MemoryType)
};

ConsistencyCheck(MemoryType, propertyFlags, heapIndex)



struct MemoryHeap : public internal::VkTrait<MemoryHeap, VkMemoryHeap>
{
    DeviceSize      size{ 0 };
    MemoryHeapFlags flags;

    DEFINE_CLASS_MEMBER(MemoryHeap)
};

ConsistencyCheck(MemoryHeap, size, flags)



struct PhysicalDeviceMemoryProperties : public internal::VkTrait<PhysicalDeviceMemoryProperties, VkPhysicalDeviceMemoryProperties>
{
    uint32_t        memoryTypeCount{ 0 };
    MemoryType      memoryTypes[VK_MAX_MEMORY_TYPES];

    uint32_t        memoryHeapCount{ 0 };
    MemoryHeap      memoryHeaps[VK_MAX_MEMORY_HEAPS];

    DEFINE_CLASS_MEMBER(PhysicalDeviceMemoryProperties)
};

ConsistencyCheck(PhysicalDeviceMemoryProperties, memoryTypeCount, memoryTypes, memoryHeapCount, memoryHeaps)



struct PhysicalDeviceProperties : public internal::VkTrait<PhysicalDeviceProperties, VkPhysicalDeviceProperties>
{
    uint32_t            apiVersion{ 0 };
    uint32_t            driverVersion{ 0 };
    uint32_t            vendorID{ 0 };
    uint32_t            deviceID{ 0 };
    PhysicalDeviceType  deviceType;
    char                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE]{};
    uint8_t             pipelineCacheUUID[VK_UUID_SIZE]{};
    PhysicalDeviceLimits  limits{};               // TODO
    PhysicalDeviceSparseProperties  sparseProperties;

    DEFINE_CLASS_MEMBER(PhysicalDeviceProperties)
};

ConsistencyCheck(PhysicalDeviceProperties, apiVersion, driverVersion, vendorID, deviceID, deviceType, deviceName, pipelineCacheUUID, limits, sparseProperties)



class PhysicalDevice : public internal::VkTrait<PhysicalDevice, VkPhysicalDevice>
{
private:
    VkPhysicalDevice mPhysicalDevice{ VK_NULL_HANDLE };

public:
    PhysicalDevice(void) = default;

    PhysicalDevice(std::nullptr_t) : mPhysicalDevice(VK_NULL_HANDLE)
    {}

    explicit PhysicalDevice(VkPhysicalDevice aPhysicalDevice) : mPhysicalDevice(aPhysicalDevice)
    {}

    PhysicalDevice& operator=(VkPhysicalDevice aPhysicalDevice)
    {
        mPhysicalDevice = aPhysicalDevice;

        return *this;
    }

    PhysicalDevice& operator=(std::nullptr_t)
    {
        mPhysicalDevice = VK_NULL_HANDLE;

        return *this;
    }

    bool operator==(const PhysicalDevice& aRhs) const
    {
        return mPhysicalDevice == aRhs.mPhysicalDevice;
    }

    bool operator!=(const PhysicalDevice& aRhs) const
    {
        return !(*this == aRhs);
    }

    std::vector<LayerProperty> GetLayers(void) const
    {
        uint32_t lLayerCount{ 0 };
        ThrowIfFailed(vkEnumerateDeviceLayerProperties(mPhysicalDevice, &lLayerCount, nullptr));

        std::vector<LayerProperty> lLayers(lLayerCount);
        ThrowIfFailed(vkEnumerateDeviceLayerProperties(mPhysicalDevice, &lLayerCount, &lLayers[0]));

        return lLayers;
    }

    std::vector<ExtensionProperty> GetExtensions(const char* apLayerName = nullptr) const
    {
        uint32_t lExtensionCount{ 0 };
        ThrowIfFailed(vkEnumerateDeviceExtensionProperties(mPhysicalDevice, apLayerName, &lExtensionCount, nullptr));

        std::vector<ExtensionProperty> lExtensions(lExtensionCount);
        ThrowIfFailed(vkEnumerateDeviceExtensionProperties(mPhysicalDevice, apLayerName, &lExtensionCount, &lExtensions[0]));

        return lExtensions;
    }

    PhysicalDeviceProperties GetProperties(void) const
    {
        PhysicalDeviceProperties lProperties;
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &lProperties);

        return lProperties;
    }

    PhysicalDeviceFeatures GetFeatures(void) const
    {
        PhysicalDeviceFeatures lFeatures;
        vkGetPhysicalDeviceFeatures(mPhysicalDevice, &lFeatures);

        return lFeatures;
    }

    FormatProperties GetFormatProperties(Format aFormat) const
    {
        FormatProperties lFormatProperties;
        vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, static_cast<VkFormat>(aFormat), &lFormatProperties);

        return lFormatProperties;
    }

    PhysicalDeviceMemoryProperties GetMemoryProperties(void) const
    {
        PhysicalDeviceMemoryProperties lMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &lMemoryProperties);

        return lMemoryProperties;
    }

    std::vector<QueueFamilyProperties> GetQueueFamilyProperties(void) const
    {
        uint32_t lQueueFamilyPropertyCount{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &lQueueFamilyPropertyCount, nullptr);

        std::vector<QueueFamilyProperties> lQueueFamilyProperties(lQueueFamilyPropertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &lQueueFamilyPropertyCount, &lQueueFamilyProperties[0]);

        return lQueueFamilyProperties;
    }

    Bool32 IsSurfaceSupported(uint32_t aQueueFamilyIndex, khr::Surface aSurface) const
    {
        Bool32 lIsSupported{ VK_FALSE };
        ThrowIfFailed(vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, aQueueFamilyIndex, aSurface, &lIsSupported));

        return lIsSupported;
    }

    khr::SurfaceCapabilities GetSurfaceCapabilities(khr::Surface aSurface) const
    {
        khr::SurfaceCapabilities lSurfaceCapabilities;
        ThrowIfFailed(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, aSurface, &lSurfaceCapabilities));

        return lSurfaceCapabilities;
    }

    std::vector<khr::SurfaceFormat> GetSurfaceFormats(khr::Surface aSurface) const
    {
        uint32_t lSurfaceFormatCount{ 0 };
        ThrowIfFailed(vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, aSurface, &lSurfaceFormatCount, nullptr));

        std::vector<khr::SurfaceFormat> lSurfaceFormats(lSurfaceFormatCount);
        ThrowIfFailed(vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, aSurface, &lSurfaceFormatCount, &lSurfaceFormats[0]));

        return lSurfaceFormats;
    }

    std::vector<khr::PresentMode> GetSurfacePresentModes(khr::Surface aSurface) const
    {
        uint32_t lPresentModeCount{ 0 };
        ThrowIfFailed(vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, aSurface, &lPresentModeCount, nullptr));

        std::vector<khr::PresentMode> lPresentModes(lPresentModeCount);
        ThrowIfFailed(vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, aSurface, &lPresentModeCount, reinterpret_cast<VkPresentModeKHR*>(lPresentModes.data())));

        return lPresentModes;
    }
};



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_PHYSICAL_DEVICE_H__
