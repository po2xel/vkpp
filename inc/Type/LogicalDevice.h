#ifndef __VKPP_TYPE_LOGICAL_DEVICE_H__
#define __VKPP_TYPE_LOGICAL_DEVICE_H__



#include <Info/Common.h>
#include <Info/LogicalDeviceInfo.h>
#include <Type/VkDeleter.h>



namespace vkpp
{



class LogicalDevice : public internal::VkTrait<LogicalDevice, VkDevice>
{
private:
    internal::VkDeleter<VkDevice> mpDevice{ vkDestroyDevice };

public:
    LogicalDevice(void) = default;

    LogicalDevice(std::nullptr_t)
    {}
};



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_LOGICAL_DEVICE_H__
