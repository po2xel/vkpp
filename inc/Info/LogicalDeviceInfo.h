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



class LogicalDeviceCreateInfo : public internal::VkTrait<LogicalDeviceCreateInfo, VkDeviceCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eDevice;

public:
    const void*                             pNext{ nullptr };
    DeviceCreateFlags                       flags;
    uint32_t                                queueCreateInfoCount{ 0 };
    const QueueCreateInfo*                  pQueueCreateInfo{ nullptr };

private:
    [[deprecated]] uint32_t                 enabledLayerCount{ 0 };
    [[deprecated]] const char* const*       ppEnabledLayerNames{ nullptr };

public:
    uint32_t                                enabledExtensionCount{ 0 };
    const char* const*                      ppEnabledExtensionNames{ nullptr };
    const PhysicalDeviceFeatures*           pEnabledFeatures{ nullptr };

    DEFINE_CLASS_MEMBER(LogicalDeviceCreateInfo)

    LogicalDeviceCreateInfo(uint32_t aQueueCreateInfoCount, const QueueCreateInfo* apQueueCreateInfos,
        uint32_t aEnalbedExtensionCount, const char* const* appEnabledExtensionNames, const PhysicalDeviceFeatures* apEnabledFeatures)
        : queueCreateInfoCount(aQueueCreateInfoCount), pQueueCreateInfo(apQueueCreateInfos),
        enabledExtensionCount(aEnalbedExtensionCount), ppEnabledExtensionNames(appEnabledExtensionNames), pEnabledFeatures(apEnabledFeatures)
    {}

    LogicalDeviceCreateInfo(const std::vector<QueueCreateInfo>& aQueueCreateInfos, const std::vector<const char*>& aEnabledExtensions,
        const PhysicalDeviceFeatures& aEnabledFeatures)
        : LogicalDeviceCreateInfo(static_cast<uint32_t>(aQueueCreateInfos.size()), aQueueCreateInfos.data(),
            static_cast<uint32_t>(aEnabledExtensions.size()), aEnabledExtensions.data(),
            &aEnabledFeatures)
    {}

    template <std::size_t Q, std::size_t E>
    LogicalDeviceCreateInfo(const std::array<QueueCreateInfo, Q>& aQueueCreateInfos, const std::array<const char*, E>& aEnabledExtensions,
        const PhysicalDeviceFeatures& aEnabledFeatures)
        : LogicalDeviceCreateInfo(static_cast<uint32_t>(aQueueCreateInfos.size()), aQueueCreateInfos.data(),
            static_cast<uint32_t>(aEnabledExtensions.size()), aEnabledExtensions.data(),
            &aEnabledFeatures)
    {}

    LogicalDeviceCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    LogicalDeviceCreateInfo& SetFlags(DeviceCreateFlags aFlags)
    {
        flags = aFlags;

        return *this;
    }

    LogicalDeviceCreateInfo& SetQueueCreateInfo(const QueueCreateInfo& aQueueCreateInfo)
    {
        pQueueCreateInfo = aQueueCreateInfo.AddressOf();

        return *this;
    }

    LogicalDeviceCreateInfo& SetEnabledExtensions(uint32_t aEnabledExtensionCount, const char* const* appEnabledExtensionNames)
    {
        enabledExtensionCount = aEnabledExtensionCount;
        ppEnabledExtensionNames = appEnabledExtensionNames;

        return *this;
    }

    LogicalDeviceCreateInfo& SetEnabledExtensions(const std::vector<const char*>& aEnabledExtensionNames)
    {
        return SetEnabledExtensions(static_cast<uint32_t>(aEnabledExtensionNames.size()), aEnabledExtensionNames.data());
    }

    template <std::size_t E>
    LogicalDeviceCreateInfo& SetEnabledExtensions(const std::array<const char*, E>& aEnabledExtensionNames)
    {
        return SetEnabledExtensions(static_cast<uint32_t>(aEnabledExtensionNames.size()), aEnabledExtensionNames.data());
    }
};

StaticSizeCheck(LogicalDeviceCreateInfo);



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_LOGICAL_DEVICE_INFO_H__
