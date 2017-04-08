#ifndef __VKPP_TYPE_COMMAND_POOL_H__
#define __VKPP_TYPE_COMMAND_POOL_H__



#include <Info/Common.h>



namespace vkpp
{



class CommandPool : public internal::VkTrait<CommandPool, VkCommandPool>
{
private:
    VkCommandPool mCommandPool{ VK_NULL_HANDLE };

public:
    CommandPool(void) = default;

    CommandPool(std::nullptr_t)
    {}

    explicit CommandPool(VkCommandPool aCommandPool) : mCommandPool(aCommandPool)
    {}
};

StaticSizeCheck(CommandPool);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_COMMAND_POOL_H__
