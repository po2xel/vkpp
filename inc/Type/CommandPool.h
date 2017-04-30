#ifndef __VKPP_TYPE_COMMAND_POOL_H__
#define __VKPP_TYPE_COMMAND_POOL_H__



#include <Info/Common.h>



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

    CommandPoolCreateInfo(uint32_t aQueueFamilyIndex, const CommandPoolCreateFlags& aFlags = DefaultFlags)
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

ConsistencyCheck(CommandPoolCreateInfo, pNext, flags, queueFamilyIndex)



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

StaticSizeCheck(CommandPool)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_COMMAND_POOL_H__
