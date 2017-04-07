#ifndef __VKPP_TYPE_SEMAPHORE_H__
#define __VKPP_TYPE_SEMAPHORE_H__



#include <Info/Common.h>



namespace vkpp
{



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

StaticSizeCheck(Semaphore);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_SEMAPHORE_H__
