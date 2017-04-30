#ifndef __VKPP_TYPE_SEMAPHORE_H__
#define __VKPP_TYPE_SEMAPHORE_H__



#include <Info/Common.h>
#include <Info/Flags.h>



namespace vkpp
{



enum class SemaphoreCreateFlagBits
{};


using SemaphoreCreateFlags = internal::Flags<SemaphoreCreateFlagBits, VkSemaphoreCreateFlags>;



class SemaphoreCreateInfo : public internal::VkTrait<SemaphoreCreateInfo, VkSemaphoreCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eSemaphore;

public:
    const void*             pNext{ nullptr };
    SemaphoreCreateFlags    flags;

    DEFINE_CLASS_MEMBER(SemaphoreCreateInfo)

    explicit SemaphoreCreateInfo(const SemaphoreCreateFlags& aFlags) noexcept : flags(aFlags)
    {}

    SemaphoreCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    SemaphoreCreateInfo& SetFlags(const SemaphoreCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }
};

ConsistencyCheck(SemaphoreCreateInfo, pNext, flags)



class Semaphore : public internal::VkTrait<Semaphore, VkSemaphore>
{
private:
    VkSemaphore mSemaphore{ VK_NULL_HANDLE };

public:
    Semaphore(void) = default;

    Semaphore(std::nullptr_t)
    {}

    explicit Semaphore(VkSemaphore aSemaphore) : mSemaphore(aSemaphore)
    {}
};

StaticSizeCheck(Semaphore)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_SEMAPHORE_H__
