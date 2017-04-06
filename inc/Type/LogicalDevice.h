#ifndef __VKPP_TYPE_LOGICAL_DEVICE_H__
#define __VKPP_TYPE_LOGICAL_DEVICE_H__



#include <utility>

#include <Info/Common.h>
#include <Info/LogicalDeviceInfo.h>
#include <Info/BufferCreateInfo.h>
#include <Type/VkDeleter.h>
#include <Type/Queue.h>



namespace vkpp
{



class LogicalDevice
{
private:
    internal::VkDeleter<VkDevice> mpDevice{ vkDestroyDevice };

public:
    LogicalDevice(void) = default;

    LogicalDevice(std::nullptr_t)
    {}

    LogicalDevice(VkDevice aLogicalDevice)
    {
        mpDevice = aLogicalDevice;
    }

    LogicalDevice(LogicalDevice&& aLogicalDevice) : mpDevice(std::move(aLogicalDevice.mpDevice))
    {}

    LogicalDevice& operator=(LogicalDevice&& aLogicalDevice)
    {
        mpDevice = std::move(aLogicalDevice.mpDevice);

        return *this;
    }

    Queue GetQueue(uint32_t aQueueFamilyIndex, uint32_t aQueueIndex) const
    {
        Queue lQueue;
        vkGetDeviceQueue(mpDevice, aQueueFamilyIndex, aQueueIndex, &lQueue);

        return lQueue;
    }

    VkResult Wait(void) const
    {
        return vkDeviceWaitIdle(mpDevice);
    }
};



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_LOGICAL_DEVICE_H__
