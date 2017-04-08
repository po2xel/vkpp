#ifndef __VKPP_TYPE_RENDER_PASS_H__
#define __VKPP_TYPE_RENDER_PASS_H__



#include <Info/Common.h>



namespace vkpp
{



class RenderPass : public internal::VkTrait<RenderPass, VkRenderPass>
{
private:
    VkRenderPass mRenderPass{ VK_NULL_HANDLE };

public:
    RenderPass(void) = default;

    RenderPass(std::nullptr_t)
    {}

    explicit RenderPass(VkRenderPass aRenderPass) : mRenderPass(aRenderPass)
    {}
};

StaticSizeCheck(RenderPass);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_RENDER_PASS_H__
