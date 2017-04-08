#ifndef __VKPP_INFO_COMMAND_POOL_CREATE_INFO_H__
#define __VKPP_INFO_COMMAND_POOL_CREATE_INFO_H__



#include <Info/Common.h>
#include <Info/Flags.h>



namespace vkpp
{



enum class CommandPoolCreateFlagBits
{
    eTransient          = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
    eResetCommandBuffer = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
};

using CommandPoolCreateFlags = internal::Flags<CommandPoolCreateFlagBits, VkCommandPoolCreateFlags>;



class CommandPoolCreateInfo : public internal::VkTrait<CommandPoolCreateInfo, VkCommandPoolCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eCommandPool;

public:
    const void*             pNext{ nullptr };
    CommandPoolCreateFlags  flags;
    uint32_t                queueFamilyIndex{ UINT32_MAX };

    DEFINE_CLASS_MEMBER(CommandPoolCreateInfo)

    CommandPoolCreateInfo(uint32_t aQueueFamilyIndex) : queueFamilyIndex(aQueueFamilyIndex)
    {}

    CommandPoolCreateInfo(const CommandPoolCreateFlags& aFlags, uint32_t aQueueFamilyIndex)
        : flags(aFlags), queueFamilyIndex(aQueueFamilyIndex)
    {}

    CommandPoolCreateInfo& SetFlags(const CommandPoolCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    CommandPoolCreateInfo& SetQueueFamilyIndex(uint32_t aQueueFamilyIndex)
    {
        queueFamilyIndex = aQueueFamilyIndex;

        return *this;
    }
};

StaticSizeCheck(CommandPoolCreateInfo);



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_COMMAND_POOL_CREATE_INFO_H__
