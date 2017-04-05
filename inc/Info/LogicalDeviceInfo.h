#ifndef __VKPP_INFO_LOGICAL_DEVICE_INFO_H__
#define __VKPP_INFO_LOGICAL_DEVICE_INFO_H__



#include <vector>

#include <Info/Common.h>



namespace vkpp
{



enum class QueueCreateFlagBits
{};

using QueueCreateFlags = internal::Flags<QueueCreateFlagBits, VkDeviceQueueCreateFlags>;



class QueueCreateInfo : public internal::VkTrait<QueueCreateInfo, VkDeviceQueueCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eQueue;

public:
    const void*         pNext{ nullptr };
    QueueCreateFlags    flags;
    uint32_t            queueFamilyIndex{ 0 };
    uint32_t            queueCount{ 0 };
    const float*        pQueuePriorities{ nullptr };

    DEFINE_CLASS_MEMBER(QueueCreateInfo)

    QueueCreateInfo(uint32_t aQueueFamilyIndex, uint32_t aQueueCount, const float* apQueuePriorities)
        : queueFamilyIndex(aQueueFamilyIndex), queueCount(aQueueCount), pQueuePriorities(apQueuePriorities)
    {}

    QueueCreateInfo(uint32_t aQueueFamilyIndex, const std::vector<float>& aQueuePriorities)
        : QueueCreateInfo(aQueueFamilyIndex, static_cast<uint32_t>(aQueuePriorities.size()), aQueuePriorities.data())
    {}

    template <std::size_t P>
    QueueCreateInfo(uint32_t aQueueFamilyIndex, const std::array<float, P>& aQueuePriorities)
        : QueueCreateInfo(aQueueFamilyIndex, static_cast<uint32_t>(aQueuePriorities.size()), aQueuePriorities.data())
    {}

    QueueCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    QueueCreateInfo& SetFamilyIndex(uint32_t aQueueFamilyIndex)
    {
        queueFamilyIndex = aQueueFamilyIndex;

        return *this;
    }

    QueueCreateInfo& SetQueuePriorities(uint32_t aQueueCount, const float* apQueuePriorities)
    {
        queueCount = aQueueCount;
        pQueuePriorities = apQueuePriorities;

        return *this;
    }

    QueueCreateInfo& SetQueuePriorities(const std::vector<float>& aQueuePriorities)
    {
        return SetQueuePriorities(static_cast<uint32_t>(aQueuePriorities.size()), aQueuePriorities.data());
    }

    template <std::size_t P>
    QueueCreateInfo& SetQueuePriorities(const std::array<float, P>& aQueuePriorities)
    {
        return SetQueuePriorities(static_cast<uint32_t>(aQueuePriorities.size()), aQueuePriorities.data())
    }
};

StaticSizeCheck(QueueCreateInfo);



enum class DeviceCreateFlagBits
{};

using DeviceCreateFlags = internal::Flags<DeviceCreateFlagBits, VkDeviceCreateFlags>;



class LogicalDeviceInfo : public internal::VkTrait<LogicalDeviceInfo, VkDeviceCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eDevice;

public:
    const void*             pNext{ nullptr };
    DeviceCreateFlags       flags;
    uint32_t                queueCreateInfoCount{ 0 };
    const QueueCreateInfo*  pQueueCreateInfo{ nullptr };
    uint32_t                enabledLayerCount{ 0 };
    const char* const*      ppEnabledLayerNames{ nullptr };
    uint32_t                enabledExtensionCount{ 0 };
    const char* const*      ppEnabledExtensionNames{ nullptr };
    const PhysicalDeviceFeatures* pEnabledFeatures{ nullptr };

    DEFINE_CLASS_MEMBER(LogicalDeviceInfo)

    LogicalDeviceInfo(uint32_t aQueueCreateInfoCount, const QueueCreateInfo* apQueueCreateInfos, uint32_t aEnabledLayerCount, const char* const* appEnabledLayerNames,
        uint32_t aEnalbedExtensionCount, const char* const* appEnabledExtensionNames, const PhysicalDeviceFeatures& aEnabledFeatures)
        : queueCreateInfoCount(aQueueCreateInfoCount), pQueueCreateInfo(apQueueCreateInfos), enabledLayerCount(aEnabledLayerCount), ppEnabledLayerNames(appEnabledLayerNames),
        enabledExtensionCount(aEnalbedExtensionCount), ppEnabledExtensionNames(appEnabledExtensionNames), pEnabledFeatures(&aEnabledFeatures)
    {}

    LogicalDeviceInfo(const std::vector<QueueCreateInfo>& aQueueCreateInfos, const std::vector<const char*>& aEnabledLayers, const std::vector<const char*>& aEnabledExtensions,
        const PhysicalDeviceFeatures& aEnabledFeatures)
        : LogicalDeviceInfo(static_cast<uint32_t>(aQueueCreateInfos.size()), aQueueCreateInfos.data(),
            static_cast<uint32_t>(aEnabledLayers.size()), aEnabledLayers.data(),
            static_cast<uint32_t>(aEnabledExtensions.size()), aEnabledExtensions.data(),
            aEnabledFeatures)
    {}

    template <std::size_t Q, std::size_t L, std::size_t E>
    LogicalDeviceInfo(const std::array<QueueCreateInfo, Q>& aQueueCreateInfos, const std::array<const char*, L>& aEnabledLayers, const std::array<const char*, E>& aEnabledExtensions,
        const PhysicalDeviceFeatures& aEnabledFeatures)
        : LogicalDeviceInfo(static_cast<uint32_t>(aQueueCreateInfos.size()), aQueueCreateInfos.data(),
            static_cast<uint32_t>(aEnabledLayers.size()), aEnabledLayers.data(),
            static_cast<uint32_t>(aEnabledExtensions.size()), aEnabledExtensions.data(),
            aEnabledFeatures)
    {}
};

StaticSizeCheck(LogicalDeviceInfo);



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_LOGICAL_DEVICE_INFO_H__
