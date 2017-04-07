#ifndef __VKPP_TYPE_PHYSICAL_DEVICE_H__
#define __VKPP_TYPE_PHYSICAL_DEVICE_H__



#include <vector>

#include <Info/Common.h>
#include <Info/PhysicalDeviceProperties.h>
#include <Info/Layers.h>
#include <Info/Extensions.h>
#include <Info/SurfaceCapabilities.h>
#include <Info/SwapchainCreateInfo.h>

#include <Type/LogicalDevice.h>
#include <Type/Surface.h>



namespace vkpp
{



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

    LogicalDevice CreateLogicalDevice(const LogicalDeviceCreateInfo& aCreateInfo)
    {
        VkDevice lLogicalDevice;
        ThrowIfFailed(vkCreateDevice(mPhysicalDevice, &aCreateInfo, nullptr, &lLogicalDevice));

        return lLogicalDevice;
    }

    LogicalDevice CreateLogicalDevice(const LogicalDeviceCreateInfo& aCreateInfo, const AllocationCallbacks& aAllocator)
    {
        VkDevice lLogicalDevice;
        ThrowIfFailed(vkCreateDevice(mPhysicalDevice, &aCreateInfo, &aAllocator, &lLogicalDevice));

        return lLogicalDevice;
    }
};



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_PHYSICAL_DEVICE_H__
