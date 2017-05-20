#ifndef __VKPP_TYPE_FRAME_BUFFER_H__
#define __VKPP_TYPE_FRAME_BUFFER_H__



#include <Info/Common.h>
#include <Type/RenderPass.h>



namespace vkpp
{



enum class FramebufferCreateFlagBits
{};

VKPP_ENUM_BIT_MASK_FLAGS(FramebufferCreate)



class FramebufferCreateInfo : public internal::VkTrait<FramebufferCreateInfo, VkFramebufferCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eFramebuffer;

public:
    const void*             pNext{ nullptr };
    FramebufferCreateFlags  flags;
    RenderPass              renderPass;
    uint32_t                attachmentCount{ 0 };
    const ImageView*        pAttachments{ nullptr };
    uint32_t                width{ 1 };
    uint32_t                height{ 1 };
    uint32_t                layers{ 1 };

    DEFINE_CLASS_MEMBER(FramebufferCreateInfo)

    FramebufferCreateInfo(const RenderPass& aRenderPass, uint32_t aAttachmentCount, const ImageView* apAttachments,
        uint32_t aWidth = 1, uint32_t aHeight = 1, uint32_t aLayers = 1, const FramebufferCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), renderPass(aRenderPass), attachmentCount(aAttachmentCount), pAttachments(apAttachments),
        width(aWidth), height(aHeight), layers(aLayers)
    {}

    template <typename A, typename = EnableIfValueType<ValueType<A>, ImageView>>
    FramebufferCreateInfo(const RenderPass& aRenderPass, A&& aAttachments,
        uint32_t aWidth = 1, uint32_t aHeight = 1, uint32_t aLayers = 1, const FramebufferCreateFlags& aFlags = DefaultFlags)
        : FramebufferCreateInfo(aRenderPass, static_cast<uint32_t>(aAttachments.size()), aAttachments.data(), aWidth, aHeight, aLayers, aFlags)
    {
        StaticLValueRefAssert(A, aAttachments);
    }

    template <typename A, typename = EnableIfValueType<ValueType<A>, ImageView>>
    FramebufferCreateInfo(const RenderPass& aRenderPass, A&& aAttachments, const Extent2D& aExtent, uint32_t aLayers = 1, const FramebufferCreateFlags& aFlags = DefaultFlags)
        : FramebufferCreateInfo(aRenderPass, static_cast<uint32_t>(aAttachments.size()), aAttachments.data(), aExtent.width, aExtent.height, aLayers, aFlags)
    {
        StaticLValueRefAssert(A, aAttachments);
    }

    FramebufferCreateInfo& SetFlags(const FramebufferCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    FramebufferCreateInfo& SetRenderPass(const RenderPass& aRenderPass)
    {
        renderPass  = aRenderPass;

        return *this;
    }

    FramebufferCreateInfo& SetAttachments(uint32_t aAttachmentCount, const ImageView* apAttachments)
    {
        attachmentCount = aAttachmentCount;
        pAttachments    = apAttachments;

        return *this;
    }

    template <typename A, typename = EnableIfValueType<ValueType<A>, ImageView>>
    FramebufferCreateInfo& SetAttachments(A&& aAttachments)
    {
        StaticLValueRefAssert(A, aAttachments);

        return SetAttachments(static_cast<uint32_t>(aAttachments.size()), aAttachments.data());
    }

    FramebufferCreateInfo& SetDimension(uint32_t aWidth, uint32_t aHeight = 1, uint32_t aLayers = 1)
    {
        width   = aWidth;
        height  = aHeight;
        layers  = aLayers;

        return *this;
    }
};

StaticSizeCheck(FramebufferCreateInfo);



class Framebuffer : public internal::VkTrait<Framebuffer, VkFramebuffer>
{
private:
    VkFramebuffer mFramebuffer{ VK_NULL_HANDLE };

public:
    Framebuffer(void) = default;

    Framebuffer(std::nullptr_t)
    {}

    explicit Framebuffer(VkFramebuffer aFramebuffer) : mFramebuffer(aFramebuffer)
    {}

    operator bool() const
    {
        return mFramebuffer != VK_NULL_HANDLE;
    }
};

StaticSizeCheck(Framebuffer);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_FRAME_BUFFER_H__
