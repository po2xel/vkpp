#ifndef __VKPP_INFO_COMMAND_BUFFER_ALLOCATE_INFO_H__
#define __VKPP_INFO_COMMAND_BUFFER_ALLOCATE_INFO_H__



#include <Info/Common.h>
#include <Type/CommandPool.h>



namespace vkpp
{



enum class CommandBufferLevel
{
    ePrimary        = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    eSecondary      = VK_COMMAND_BUFFER_LEVEL_SECONDARY
};


class CommandBufferAllocateInfo : public internal::VkTrait<CommandBufferAllocateInfo, VkCommandBufferAllocateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eCommandBufferAllocate;

public:
    const void*         pNext{ nullptr };
    CommandPool         commandPool;
    CommandBufferLevel  level{ CommandBufferLevel::ePrimary };
    uint32_t            commandBufferCount{ 0 };

    DEFINE_CLASS_MEMBER(CommandBufferAllocateInfo)

    CommandBufferAllocateInfo(const CommandPool& aCommandPool, uint32_t aCommandBufferCount, CommandBufferLevel aLevel = CommandBufferLevel::ePrimary) noexcept
        : commandPool(aCommandPool), level(aLevel), commandBufferCount(aCommandBufferCount)
    {}

    CommandBufferAllocateInfo(const CommandPool& aCommandPool, CommandBufferLevel aLevel = CommandBufferLevel::ePrimary) noexcept
        : CommandBufferAllocateInfo(aCommandPool, 1, aLevel)
    {}

    CommandBufferAllocateInfo& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    CommandBufferAllocateInfo& SetCommandPool(const CommandPool& aCommandPool) noexcept
    {
        commandPool = aCommandPool;

        return *this;
    }

    CommandBufferAllocateInfo& SetCommandBufferInfo(CommandBufferLevel aLevel, uint32_t aCommandBufferCount) noexcept
    {
        level               = aLevel;
        commandBufferCount  = aCommandBufferCount;

        return *this;
    }
};

ConsistencyCheck(CommandBufferAllocateInfo, pNext, commandPool, level, commandBufferCount)



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_COMMAND_BUFFER_ALLOCATE_INFO_H__
