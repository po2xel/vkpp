#ifndef __VKPP_INFO_RENDER_PASS_BEGIN_INFO_H__
#define __VKPP_INFO_RENDER_PASS_BEGIN_INFO_H__



#include <Info/Common.h>
#include <Type/RenderPass.h>
#include <Type/Framebuffer.h>



namespace vkpp
{



class RenderPassBeginInfo : public internal::VkTrait<RenderPassBeginInfo, VkRenderPassBeginInfo>
{
private:
    const internal::Structure sType = internal::Structure::eRenderPassBegin;

public:
    const void*         pNext{ nullptr };
    RenderPass          renderPass;
    Framebuffer         framebuffer;
    Rect2D              renderArea;
    uint32_t            clearValueCount{ 0 };
    const ClearValue*   pClearValues{ nullptr };

    DEFINE_CLASS_MEMBER(RenderPassBeginInfo)

    RenderPassBeginInfo(const RenderPass& aRenderPass, const Framebuffer& aFramebuffer, const Rect2D& aRenderArea,
        uint32_t aClearValueCount, const ClearValue* apClearValues) noexcept
        : renderPass(aRenderPass), framebuffer(aFramebuffer), renderArea(aRenderArea), clearValueCount(aClearValueCount), pClearValues(apClearValues)
    {}

    template <typename C, typename = EnableIfValueType<ValueType<C>, ClearValue>>
    RenderPassBeginInfo(const RenderPass& aRenderPass, const Framebuffer& aFramebuffer, const Rect2D& aRenderArea, C&& aClearValues) noexcept
        : RenderPassBeginInfo(aRenderPass, aFramebuffer, aRenderArea, static_cast<uint32_t>(aClearValues.size()), aClearValues.data())
    {
        StaticLValueRefAssert(C, aClearValues);
    }

    RenderPassBeginInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    RenderPassBeginInfo& SetRenderPass(const RenderPass& aRenderPass)
    {
        renderPass = aRenderPass;

        return *this;
    }

    RenderPassBeginInfo& SetFramebuffer(const Framebuffer& aFramebuffer)
    {
        framebuffer = aFramebuffer;

        return *this;
    }

    RenderPassBeginInfo& SetRenderArea(const Rect2D& aRenderArea)
    {
        renderArea = aRenderArea;

        return *this;
    }

    RenderPassBeginInfo& SetClearValue(uint32_t aClearValueCount, const ClearValue* apClearValues)
    {
        clearValueCount = aClearValueCount;
        pClearValues    = apClearValues;

        return *this;
    }

    template <typename C, typename = EnableIfValueType<ValueType<C>, ClearValue>>
    RenderPassBeginInfo& SetClearValue(C&& aClearValues)
    {
        StaticLValueRefAssert(C, aClearValues);

        return SetClearValue(static_cast<uint32_t>(aClearValues.size()), aClearValues.data());
    }
};

ConsistencyCheck(RenderPassBeginInfo, pNext, renderPass, framebuffer, renderArea, clearValueCount, pClearValues)



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_RENDER_PASS_BEGIN_INFO_H__
