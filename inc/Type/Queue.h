#ifndef __VKPP_TYPE_QUEUE_H__
#define __VKPP_TYPE_QUEUE_H__



#include <Info/Common.h>



namespace vkpp
{



class Queue : public internal::VkTrait<Queue, VkQueue>
{
private:
    VkQueue mQueue{ VK_NULL_HANDLE };

public:
    Queue(std::nullptr_t)
    {}

    explicit Queue(VkQueue aQueue) : mQueue(aQueue)
    {}

    VkResult Wait(void) const
    {
        return vkQueueWaitIdle(mQueue);
    }

    DEFINE_CLASS_MEMBER(Queue)
};

StaticSizeCheck(Queue);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_QUEUE_H__
