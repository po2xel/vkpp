#ifndef __VKPP_TYPE_INSTANCE_H__
#define __VKPP_TYPE_INSTANCE_H__



#include <vector>

#include <Info/Common.h>
#include <Info/InstanceInfo.h>
#include <Type/VkDeleter.h>
#include <Type/AllocationCallbacks.h>
#include <Type/PhysicalDevice.h>



namespace vkpp
{



struct ExtensionProperty : public internal::VkTrait<ExtensionProperty, VkExtensionProperties>
{
    char        extensionName[VK_MAX_EXTENSION_NAME_SIZE]{};
    uint32_t    specVersion{ 0 };

    DEFINE_CLASS_MEMBER(ExtensionProperty)
};

StaticSizeCheck(ExtensionProperty);



struct LayerProperty : public internal::VkTrait<LayerProperty, VkLayerProperties>
{
    char        layerName[VK_MAX_EXTENSION_NAME_SIZE]{};
    uint32_t    specVersion{ 0 };
    uint32_t    implementationVersion{ 0 };
    char        description[VK_MAX_DESCRIPTION_SIZE]{};

    DEFINE_CLASS_MEMBER(LayerProperty)
};

StaticSizeCheck(LayerProperty);



class Instance
{
private:
    internal::VkDeleter<VkInstance>       mInstance{ vkDestroyInstance };

public:
    Instance(void) = default;

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

    void Reset(const InstanceInfo& aInstanceInfo)
    {
        ThrowIfFailed(vkCreateInstance(&aInstanceInfo, nullptr, mInstance.Replace()));
    }

    void Reset(const InstanceInfo& aInstanceInfo, const AllocationCallbacks& aAllocator)
    {
        ThrowIfFailed(vkCreateInstance(&aInstanceInfo, &aAllocator, mInstance.Replace()));
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

    static std::vector<LayerProperty> GetLayers(void)
    {
        uint32_t lPropertyCount{ 0 };
        ThrowIfFailed(vkEnumerateInstanceLayerProperties(&lPropertyCount, nullptr));

        std::vector<LayerProperty> lProperties(lPropertyCount);
        ThrowIfFailed(vkEnumerateInstanceLayerProperties(&lPropertyCount, &lProperties[0]));

        return lProperties;
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
