#ifndef __VKPP_TYPE_FRAME_BUFFER_H__
#define __VKPP_TYPE_FRAME_BUFFER_H__



#include <Info/Common.h>
#include <Type/RenderPass.h>



namespace vkpp
{



enum class FrameBufferCreateFlagBits
{};

using FrameBufferCreateFlags = internal::Flags<FrameBufferCreateFlagBits, VkFramebufferCreateFlags>;



class FrameBufferCreateInfo : public internal::VkTrait<FrameBufferCreateInfo, VkFramebufferCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eFrameBuffer;

public:
    const void*             pNext{ nullptr };
    FrameBufferCreateFlags  flags;
    RenderPass              renderPass;
    uint32_t                attachmentCount{ 0 };
    const ImageView*        pAttachments{ nullptr };
    uint32_t                width{ 1 };
    uint32_t                height{ 1 };
    uint32_t                layers{ 1 };

    DEFINE_CLASS_MEMBER(FrameBufferCreateInfo)

    FrameBufferCreateInfo(const RenderPass& aRenderPass, uint32_t aAttachmentCount, const ImageView* apAttachments,
        uint32_t aWidth =1 , uint32_t aHeight = 1, uint32_t aLayers = 1, const FrameBufferCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), renderPass(aRenderPass), attachmentCount(aAttachmentCount), pAttachments(apAttachments),
          width(aWidth), height(aHeight), layers(aLayers)
    {}

    FrameBufferCreateInfo(const RenderPass& aRenderPass, const std::vector<ImageView>& aAttachments,
        uint32_t aWidth = 1, uint32_t aHeight = 1, uint32_t aLayers = 1, const FrameBufferCreateFlags& aFlags = DefaultFlags)
        : FrameBufferCreateInfo(aRenderPass, static_cast<uint32_t>(aAttachments.size()), aAttachments.data(), aWidth, aHeight, aLayers, aFlags)
    {}

    template <std::size_t A>
    FrameBufferCreateInfo(const RenderPass& aRenderPass, const std::array<ImageView, A>& aAttachments,
        uint32_t aWidth = 1, uint32_t aHeight = 1, uint32_t aLayers = 1, const FrameBufferCreateFlags& aFlags = DefaultFlags)
        : FrameBufferCreateInfo(aRenderPass, static_cast<uint32_t>(aAttachments.size()), aAttachments.data(), aWidth, aHeight, aLayers, aFlags)
    {}

    FrameBufferCreateInfo& SetFlags(const FrameBufferCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    FrameBufferCreateInfo& SetRenderPass(const RenderPass& aRenderPass)
    {
        renderPass  = aRenderPass;

        return *this;
    }

    FrameBufferCreateInfo& SetAttachments(uint32_t aAttachmentCount, const ImageView* apAttachments)
    {
        attachmentCount = aAttachmentCount;
        pAttachments    = apAttachments;

        return *this;
    }

    FrameBufferCreateInfo& SetAttachments(const std::vector<ImageView>& aAttachments)
    {
        return SetAttachments(static_cast<uint32_t>(aAttachments.size()), aAttachments.data());
    }

    template <std::size_t A>
    FrameBufferCreateInfo& SetAttachments(const std::array<ImageView, A>& aAttachments)
    {
        return SetAttachments(static_cast<uint32_t>(aAttachments.size()), aAttachments.data());
    }

    FrameBufferCreateInfo& SetDimension(uint32_t aWidth, uint32_t aHeight = 1, uint32_t aLayers = 1)
    {
        width   = aWidth;
        height  = aHeight;
        layers  = aLayers;

        return *this;
    }
};

StaticSizeCheck(FrameBufferCreateInfo);



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
