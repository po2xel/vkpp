#ifndef __VKPP_TYPE_FRAME_BUFFER_H__
#define __VKPP_TYPE_FRAME_BUFFER_H__



#include <Info/Common.h>



namespace vkpp
{



class FrameBuffer : public internal::VkTrait<FrameBuffer, VkFramebuffer>
{
private:
    VkFramebuffer mFrameBuffer{ VK_NULL_HANDLE };

public:
    FrameBuffer(void) = default;

    FrameBuffer(std::nullptr_t)
    {}

    explicit FrameBuffer(VkFramebuffer aFrameBuffer) : mFrameBuffer(aFrameBuffer)
    {}
};

StaticSizeCheck(FrameBuffer);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_FRAME_BUFFER_H__
