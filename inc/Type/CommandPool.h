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

VKPP_ENUM_BIT_MASK_FLAGS(CommandPoolCreate)



enum class CommandPoolResetFlagBits
{
    eReleaseResources       = VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
};

VKPP_ENUM_BIT_MASK_FLAGS(CommandPoolReset)



class CommandPoolCreateInfo : public internal::VkTrait<CommandPoolCreateInfo, VkCommandPoolCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eCommandPool;

public:
    const void*             pNext{ nullptr };
    CommandPoolCreateFlags  flags;
    uint32_t                queueFamilyIndex{ UINT32_MAX };

    DEFINE_CLASS_MEMBER(CommandPoolCreateInfo)

    constexpr CommandPoolCreateInfo(uint32_t aQueueFamilyIndex, const CommandPoolCreateFlags& aFlags) noexcept
    : flags(aFlags), queueFamilyIndex(aQueueFamilyIndex)
    {}

    CommandPoolCreateInfo& SetFlags(const CommandPoolCreateFlags& aFlags) noexcept
    {
        flags = aFlags;

        return *this;
    }

    CommandPoolCreateInfo& SetQueueFamilyIndex(uint32_t aQueueFamilyIndex) noexcept
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
    CommandPool(void) noexcept = default;

    constexpr CommandPool(std::nullptr_t) noexcept
    {}

    explicit constexpr CommandPool(VkCommandPool aCommandPool) noexcept : mCommandPool(aCommandPool)
    {}
};

StaticSizeCheck(CommandPool)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_COMMAND_POOL_H__
