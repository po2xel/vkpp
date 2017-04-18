#ifndef __VKPP_INFO_RENDER_PASS_BEGIN_INFO_H__
#define __VKPP_INFO_RENDER_PASS_BEGIN_INFO_H__



#include <Info/Common.h>
#include <Type/RenderPass.h>
#include <Type/FrameBuffer.h>



namespace vkpp
{



class RenderPassBeginInfo : public internal::VkTrait<RenderPassBeginInfo, VkRenderPassBeginInfo>
{
private:
    const internal::Structure sType = internal::Structure::eRenderPassBegin;

public:
    const void*         pNext{ nullptr };
    RenderPass          renderPass;
    FrameBuffer         framebuffer;
    Rect2D              renderArea;
    uint32_t            clearValueCount{ 0 };
    const ClearValue*   pClearValues{ nullptr };

    DEFINE_CLASS_MEMBER(RenderPassBeginInfo)

    RenderPassBeginInfo(const RenderPass& aRenderPass, const FrameBuffer& aFrameBuffer, const Rect2D& aRenderArea,
        uint32_t aClearValueCount, const ClearValue* apClearValues)
        : renderPass(aRenderPass), framebuffer(aFrameBuffer), renderArea(aRenderArea), clearValueCount(aClearValueCount), pClearValues(apClearValues)
    {}

    RenderPassBeginInfo(const RenderPass& aRenderPass, const FrameBuffer& aFrameBuffer, const Rect2D& aRenderArea,
        const std::vector<ClearValue>& aClearValues)
        : RenderPassBeginInfo(aRenderPass, aFrameBuffer, aRenderArea, static_cast<uint32_t>(aClearValues.size()), aClearValues.data())
    {}

    template <std::size_t C>
    RenderPassBeginInfo(const RenderPass& aRenderPass, const FrameBuffer& aFrameBuffer, const Rect2D& aRenderArea,
        const std::array<ClearValue, C>& aClearValues)
        : RenderPassBeginInfo(aRenderPass, aFrameBuffer, aRenderArea, static_cast<uint32_t>(aClearValues.size()), aClearValues.data())
    {}

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

    RenderPassBeginInfo& SetFrameBuffer(const FrameBuffer& aFrameBuffer)
    {
        framebuffer = aFrameBuffer;

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

    RenderPassBeginInfo& SetClearValue(const std::vector<ClearValue>& aClearValues)
    {
        return SetClearValue(static_cast<uint32_t>(aClearValues.size()), aClearValues.data());
    }

    template <std::size_t C>
    RenderPassBeginInfo& SetClearValue(const std::array<ClearValue, C>& aClearValues)
    {
        return SetClearValue(static_cast<uint32_t>(aClearValues.size()), aClearValues.data());
    }
};

StaticSizeCheck(RenderPassBeginInfo);



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_RENDER_PASS_BEGIN_INFO_H__
