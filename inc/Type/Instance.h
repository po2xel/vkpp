#ifndef __VKPP_TYPE_INSTANCE_H__
#define __VKPP_TYPE_INSTANCE_H__



#include <vector>

#include <Info/Common.h>
#include <Info/InstanceInfo.h>
#include <Info/SurfaceCreateInfo.h>
#include <Info/Layers.h>
#include <Info/Extensions.h>

#include <Type/VkDeleter.h>
#include <Type/AllocationCallbacks.h>
#include <Type/PhysicalDevice.h>
#include <Type/Surface.h>



namespace vkpp
{



class Instance : public internal::VkTrait<Instance, VkInstance>
{
private:
    VkInstance mInstance;

public:
    DEFINE_CLASS_MEMBER(Instance)

    Instance(std::nullptr_t)
    {}

    explicit Instance(const InstanceInfo& aInstanceInfo)
    {
        ThrowIfFailed(vkCreateInstance(&aInstanceInfo, nullptr, &mInstance));
    }

    Instance(const InstanceInfo& aInstanceInfo, const AllocationCallbacks& aAllocator)
    {
        ThrowIfFailed(vkCreateInstance(&aInstanceInfo, &aAllocator, &mInstance));
    }

    ~Instance(void)
    {
        vkDestroyInstance(mInstance, nullptr);          // TODO: Support allocator.
    }

    void Reset(const InstanceInfo& aInstanceInfo)
    {
        ThrowIfFailed(vkCreateInstance(&aInstanceInfo, nullptr, &mInstance));
    }

    void Reset(const InstanceInfo& aInstanceInfo, const AllocationCallbacks& aAllocator)
    {
        ThrowIfFailed(vkCreateInstance(&aInstanceInfo, &aAllocator, &mInstance));
    }

    Instance& operator=(VkInstance aInstance)
    {
        mInstance = aInstance;

        return *this;
    }

    Instance& operator=(std::nullptr_t)
    {
        mInstance = VK_NULL_HANDLE;

        return *this;
    }

    bool operator==(const Instance& aRhs) const
    {
        return mInstance == aRhs.mInstance;
    }

    bool operator!=(const Instance& aRhs) const
    {
        return !(*this == aRhs);
    }

    std::vector<PhysicalDevice> GetPhysicalDevices(void) const
    {
        uint32_t lPhysicalDeviceCount{ 0 };
        ThrowIfFailed(vkEnumeratePhysicalDevices(mInstance, &lPhysicalDeviceCount, nullptr));

        std::vector<PhysicalDevice> lPhysicalDevices(lPhysicalDeviceCount);
        ThrowIfFailed(vkEnumeratePhysicalDevices(mInstance, &lPhysicalDeviceCount, &lPhysicalDevices[0]));

        return lPhysicalDevices;
    }

    khr::Surface CreateSurface(const khr::SurfaceCreateInfo& aCreateInfo)
    {
        khr::Surface lSurface;
        ThrowIfFailed(vkCreateSurfaceKHR(mInstance, &aCreateInfo, nullptr, &lSurface));

        return lSurface;
    }

    khr::Surface CreateSurface(const khr::SurfaceCreateInfo& aCreateInfo, const AllocationCallbacks& aAllocator)
    {
        khr::Surface lSurface;
        ThrowIfFailed(vkCreateSurfaceKHR(mInstance, &aCreateInfo, &aAllocator, &lSurface));

        return lSurface;
    }

    void DestroySurface(khr::Surface aSurface)
    {
        vkDestroySurfaceKHR(mInstance, aSurface, nullptr);
    }

    void DestroySurface(khr::Surface aSurface, const AllocationCallbacks& aAllocator)
    {
        vkDestroySurfaceKHR(mInstance, aSurface, &aAllocator);
    }

    static std::vector<LayerProperty> GetLayers(void)
    {
        uint32_t lLayerCount{ 0 };
        ThrowIfFailed(vkEnumerateInstanceLayerProperties(&lLayerCount, nullptr));

        std::vector<LayerProperty> lLayers(lLayerCount);
        ThrowIfFailed(vkEnumerateInstanceLayerProperties(&lLayerCount, &lLayers[0]));

        return lLayers;
    }

    static std::vector<ExtensionProperty> GetExtensions(const char* apLayerName = nullptr)
    {
        uint32_t lExtensionCount{ 0 };
        vkEnumerateInstanceExtensionProperties(apLayerName, &lExtensionCount, nullptr);

        std::vector<ExtensionProperty> lExtensions(lExtensionCount);
        ThrowIfFailed(vkEnumerateInstanceExtensionProperties(apLayerName, &lExtensionCount, &lExtensions[0]));

        return lExtensions;
    }
};



}                    // End of namespace vkpp.



#endif               // __VKPP_TYPE_INSTANCE_H__
