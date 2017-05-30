#ifndef __VKPP_TYPE_FENCE_H__
#define __VKPP_TYPE_FENCE_H__



#include <Info/Common.h>
#include <Info/Flags.h>



namespace vkpp
{



enum class FenceCreateFlagBits
{
    eSignaled       = VK_FENCE_CREATE_SIGNALED_BIT
};

VKPP_ENUM_BIT_MASK_FLAGS(FenceCreate)



class FenceCreateInfo : public internal::VkTrait<FenceCreateInfo, VkFenceCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eFence;

public:
    const void*             pNext{ nullptr };
    FenceCreateFlags        flags;

    DEFINE_CLASS_MEMBER(FenceCreateInfo)

    explicit constexpr FenceCreateInfo(const FenceCreateFlags& aFlags) noexcept : flags(aFlags)
    {}

    FenceCreateInfo& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    FenceCreateInfo& SetFlags(const FenceCreateFlags& aFlags) noexcept
    {
        flags = aFlags;

        return *this;
    }
};

ConsistencyCheck(FenceCreateInfo, pNext, flags)



class Fence : public internal::VkTrait<Fence, VkFence>
{
private:
    VkFence mFence{ VK_NULL_HANDLE };

public:
    Fence(void) noexcept = default;

    Fence(std::nullptr_t) noexcept
    {}

    explicit Fence(VkFence aFence) noexcept : mFence(aFence)
    {}
};

StaticSizeCheck(Fence)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_FENCE_H__
