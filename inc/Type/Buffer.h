#ifndef __VKPP_TYPE_BUFFER_H__
#define __VKPP_TYPE_BUFFER_H__



#include <Info/Common.h>



namespace vkpp
{



class Buffer : public internal::VkTrait<Buffer, VkBuffer>
{
private:
    VkBuffer mBuffer{ VK_NULL_HANDLE };

public:
    Buffer(void) = default;

    Buffer(std::nullptr_t)
    {}

    explicit Buffer(VkBuffer aBuffer) : mBuffer(aBuffer)
    {}
};

StaticSizeCheck(Buffer);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_BUFFER_H__
