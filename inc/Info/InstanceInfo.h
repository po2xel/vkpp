#ifndef __VKPP_INFO_INSTANCE_INFO_H__
#define __VKPP_INFO_INSTANCE_INFO_H__



#include <array>
#include <vector>

#include <Info/Common.h>
#include <Info/ApplicationInfo.h>



namespace vkpp
{



class InstanceInfo : public internal::VkTrait<InstanceInfo, VkInstanceCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eInstance;

public:
    const void*                 pNext{ nullptr };
    VkInstanceCreateFlags       flags{ 0 };
    const ApplicationInfo*      pApplicationInfo{ nullptr };
    uint32_t                    enabledLayerCount{ 0 };
    const char* const*          ppEnalbedLayerNames{ nullptr };
    uint32_t                    enabledExtensionCount{ 0 };
    const char* const*          ppEnabledExtensionNames{ nullptr };

    DEFINE_CLASS_MEMBER(InstanceInfo)

    explicit InstanceInfo(const ApplicationInfo& aAppInfo, uint32_t aEnabledLayerCount = 0, const char* const* appEnabledLayerNames = nullptr,
        uint32_t aEnabledExtensionCount = 0, const char* const* appEnabledExtensionNames = nullptr)
        : pApplicationInfo(aAppInfo.AddressOf()), enabledLayerCount(aEnabledLayerCount), ppEnalbedLayerNames(appEnabledLayerNames),
        enabledExtensionCount(aEnabledExtensionCount), ppEnabledExtensionNames(appEnabledExtensionNames)
    {}

    InstanceInfo(const ApplicationInfo& aAppInfo, const std::vector<const char*>& aEnabledLayers, const std::vector<const char*>& aEnabledExtensions)
        : InstanceInfo(aAppInfo, static_cast<uint32_t>(aEnabledLayers.size()), aEnabledLayers.data(), static_cast<uint32_t>(aEnabledExtensions.size()), aEnabledExtensions.data())
    {}

    template <std::size_t L, std::size_t E>
    InstanceInfo(const ApplicationInfo& aAppInfo, const std::array<const char*, L>& aEnabledLayers, const std::array<const char*, E>& aEnabledExtensions)
        : InstanceInfo(aAppInfo, static_cast<uint32_t>(aEnabledLayers.size()), aEnabledLayers.data(), static_cast<uint32_t>(aEnabledExtensions.size()), aEnabledExtensions.data())
    {}

    InstanceInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    InstanceInfo& SetAppInfo(const ApplicationInfo& aAppInfo)
    {
        pApplicationInfo = aAppInfo.AddressOf();

        return *this;
    }

    InstanceInfo& SetEnabledLayers(uint32_t aEnabledLayerCount, const char* const* appEnabledLayerNames)
    {
        enabledLayerCount = aEnabledLayerCount;
        ppEnalbedLayerNames = appEnabledLayerNames;

        return *this;
    }

    InstanceInfo& SetEnabledLayers(const std::vector<const char*>& aEnabledLayers)
    {
        SetEnabledLayers(static_cast<uint32_t>(aEnabledLayers.size()), aEnabledLayers.data());
    }

    template <std::size_t L>
    InstanceInfo& SetEnabledLayers(const std::array<const char*, L>& aEnabledLayers)
    {
        SetEnabledLayers(static_cast<uint32_t>(L), aEnabledLayers.data());
    }

    InstanceInfo& SetEnabledExtensions(uint32_t aEnabledExtensionCount, const char* const* appEnalbedExtensionNames)
    {
        enabledExtensionCount = aEnabledExtensionCount;
        ppEnabledExtensionNames = appEnalbedExtensionNames;

        return *this;
    }

    InstanceInfo& SetEnalbedExtension(const std::vector<const char*>& aEnabledExtensions)
    {
        SetEnabledExtensions(static_cast<uint32_t>(aEnabledExtensions.size()), aEnabledExtensions.data());
    }

    template <std::size_t E>
    InstanceInfo& SetEnabledExtensions(const std::array<const char*, E>& aEnabledExtensions)
    {
        SetEnabledExtensions(static_cast<uint32_t>(E), aEnabledExtensions.data());
    }
};

StaticSizeCheck(InstanceInfo);



}                    // End of namespace vkpp



#endif             // __VKPP_INFO_INSTANCE_INFO_H__
