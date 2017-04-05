#ifndef __VKPP_TYPE_STRUCTURE_H__
#define __VKPP_TYPE_STRUCTURE_H__



#include <vulkan/vulkan.h>



namespace vkpp::internal
{



enum class Structure
{
    eApplication    = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    eInstance       = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    eQueue          = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    eDevice         = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO
};



}                    // End of namespace vkpp::type



#endif             // __VKPP_TYPE_STRUCTURE_H__
