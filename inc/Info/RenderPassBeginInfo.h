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

    RenderPassBeginInfo(const RenderPass& aRenderPass, const Framebuffer& aFramebuffer, const Offset2D& aOffset, const Extent2D& aExtent,
        uint32_t aClearValueCount, const ClearValue* apClearValues) noexcept
        : renderPass(aRenderPass), framebuffer(aFramebuffer), renderArea(aOffset, aExtent), clearValueCount(aClearValueCount), pClearValues(apClearValues)
    {}

    template <typename C, typename = EnableIfValueType<ValueType<C>, ClearValue>>
    RenderPassBeginInfo(const RenderPass& aRenderPass, const Framebuffer& aFramebuffer, const Rect2D& aRenderArea, C&& aClearValues) noexcept
        : RenderPassBeginInfo(aRenderPass, aFramebuffer, aRenderArea, SizeOf<uint32_t>(aClearValues), DataOf(aClearValues))
    {
        StaticLValueRefAssert(C, aClearValues);
    }

    template <typename C, typename = EnableIfValueType<ValueType<C>, ClearValue>>
    RenderPassBeginInfo(const RenderPass& aRenderPass, const Framebuffer& aFramebuffer, const Offset2D& aOffset, const Extent2D& aExtent, C&& aClearValues) noexcept
        : RenderPassBeginInfo(aRenderPass, aFramebuffer, aOffset, aExtent, SizeOf<uint32_t>(aClearValues), DataOf(aClearValues))
    {
        StaticLValueRefAssert(C, aClearValues);
    }

    RenderPassBeginInfo& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    RenderPassBeginInfo& SetRenderPass(const RenderPass& aRenderPass) noexcept
    {
        renderPass = aRenderPass;

        return *this;
    }

    RenderPassBeginInfo& SetFramebuffer(const Framebuffer& aFramebuffer) noexcept
    {
        framebuffer = aFramebuffer;

        return *this;
    }

    RenderPassBeginInfo& SetRenderArea(const Rect2D& aRenderArea) noexcept
    {
        renderArea = aRenderArea;

        return *this;
    }

    RenderPassBeginInfo& SetRenderArea(const Offset2D& aOffset, const Extent2D& aExtent) noexcept
    {
        renderArea
            .SetOffset(aOffset)
            .SetExtent(aExtent);

        return *this;
    }

    RenderPassBeginInfo& SetClearValue(uint32_t aClearValueCount, const ClearValue* apClearValues) noexcept
    {
        assert((aClearValueCount != 0 && apClearValues != nullptr) || (0 == aClearValueCount && nullptr == apClearValues));

        clearValueCount = aClearValueCount;
        pClearValues    = apClearValues;

        return *this;
    }

    template <typename C, typename = EnableIfValueType<ValueType<C>, ClearValue>>
    RenderPassBeginInfo& SetClearValue(C&& aClearValues) noexcept
    {
        StaticLValueRefAssert(C, aClearValues);

        return SetClearValue(SizeOf<uint32_t>(aClearValues), DataOf(aClearValues));
    }
};

ConsistencyCheck(RenderPassBeginInfo, pNext, renderPass, framebuffer, renderArea, clearValueCount, pClearValues)



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_RENDER_PASS_BEGIN_INFO_H__
