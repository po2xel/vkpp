#ifndef __VKPP_INFO_SEMAPHORE_CREATE_INFO_H__
#define __VKPP_INFO_SEMAPHORE_CREATE_INFO_H__



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

    SemaphoreCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    SemaphoreCreateInfo& SetFlags(SemaphoreCreateFlags aFlags)
    {
        flags = aFlags;

        return *this;
    }
};

StaticSizeCheck(SemaphoreCreateInfo);



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_SEMAPHORE_CREATE_INFO_H__
