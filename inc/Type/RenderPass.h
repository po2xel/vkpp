#ifndef __VKPP_TYPE_RENDER_PASS_H__
#define __VKPP_TYPE_RENDER_PASS_H__



#include <Info/Common.h>
#include <Info/Flags.h>
#include <Info/Format.h>

#include <Type/MemoryBarrier.h>



namespace vkpp
{



enum class AttachmentDescriptionFlagBits
{
    eMayAlias   = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT
};

using AttachmentDescriptionFlags = internal::Flags<AttachmentDescriptionFlagBits, VkAttachmentDescriptionFlags>;



enum class AttachmentLoadOp
{
    eLoad       = VK_ATTACHMENT_LOAD_OP_LOAD,
    eClear      = VK_ATTACHMENT_LOAD_OP_CLEAR,
    eDontCare   = VK_ATTACHMENT_LOAD_OP_DONT_CARE
};



enum class AttachmentStoreOp
{
    eStore      = VK_ATTACHMENT_STORE_OP_STORE,
    eDontCare   = VK_ATTACHMENT_STORE_OP_DONT_CARE
};



struct AttachementDescription : public internal::VkTrait<AttachementDescription, VkAttachmentDescription>
{
    AttachmentDescriptionFlags  flags;
    Format                      format;
    SampleCountFlagBits         samples;
    AttachmentLoadOp            loadOp;
    AttachmentStoreOp           storeOp;
    AttachmentLoadOp            stencilLoadOp;
    AttachmentStoreOp           stencilStoreOp;
    ImageLayout                 initialLayout;
    ImageLayout                 finalLayout;

    DEFINE_CLASS_MEMBER(AttachementDescription)

    AttachementDescription(Format aFormat, SampleCountFlagBits aSamples, AttachmentLoadOp aLoadOp, AttachmentStoreOp aStoreOp,
        AttachmentLoadOp aStencilLoadOp, AttachmentStoreOp aStencilStoreOp, ImageLayout aInitialLayOut, ImageLayout aFinalLayout,
        const AttachmentDescriptionFlags& aFlags = DefaultFlags)
        : flags(aFlags), format(aFormat), samples(aSamples), loadOp(aLoadOp), storeOp(aStoreOp), stencilLoadOp(aStencilLoadOp), stencilStoreOp(aStencilStoreOp),
        initialLayout(aInitialLayOut), finalLayout(aFinalLayout)
    {}

    AttachementDescription& SetFlags(const AttachmentDescriptionFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    AttachementDescription& SetFormat(Format aFormat)
    {
        format = aFormat;

        return *this;
    }

    AttachementDescription& SetSamples(SampleCountFlagBits aSamples)
    {
        samples = aSamples;

        return *this;
    }

    AttachementDescription& SetOps(AttachmentLoadOp aLoadOp, AttachmentStoreOp aStoreOp)
    {
        loadOp  = aLoadOp;
        storeOp = aStoreOp;

        return *this;
    }

    AttachementDescription& SetStencilOps(AttachmentLoadOp aLoadOp, AttachmentStoreOp aStoreOp)
    {
        stencilLoadOp   = aLoadOp;
        stencilStoreOp  = aStoreOp;

        return *this;
    }

    AttachementDescription& SetLayouts(ImageLayout aInitialLayout, ImageLayout aFinalLayout)
    {
        initialLayout   = aInitialLayout;
        finalLayout     = aFinalLayout;

        return *this;
    }
};

StaticSizeCheck(AttachementDescription);



struct AttachmentReference : public internal::VkTrait<AttachmentReference, VkAttachmentReference>
{
    uint32_t    attachment{ UINT32_MAX };
    ImageLayout layout;

    DEFINE_CLASS_MEMBER(AttachmentReference)

    AttachmentReference(uint32_t aAttachment, ImageLayout aLayout) : attachment(aAttachment), layout(aLayout)
    {}

    AttachmentReference& SetAttachment(uint32_t aAttachment, ImageLayout aLayout)
    {
        attachment  = aAttachment;
        layout      = aLayout;

        return *this;
    }
};

StaticSizeCheck(AttachmentReference);



enum class SubpassDescriptionFlagBits
{
    ePerViewAttributesNVX       = VK_SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX,
    ePerViewPositionXOnlyNVX    = VK_SUBPASS_DESCRIPTION_PER_VIEW_POSITION_X_ONLY_BIT_NVX
};

using SubpassDescriptionFlags = internal::Flags<SubpassDescriptionFlagBits, VkSubpassDescriptionFlags>;



enum class PipelineBindPoint
{
    eGraphics   = VK_PIPELINE_BIND_POINT_GRAPHICS,
    eCompute    = VK_PIPELINE_BIND_POINT_COMPUTE
};



enum class SubpassContents
{
    eInline                     = VK_SUBPASS_CONTENTS_INLINE,
    eSecondaryCommandBuffers    = VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
};



struct SubpassDescription : public internal::VkTrait<SubpassDescription, VkSubpassDescription>
{
    SubpassDescriptionFlags     flags;
    PipelineBindPoint           pipelineBindPoint;
    uint32_t                    inputAttachmentCount{ 0 };
    const AttachmentReference*  pInputAttachments{ nullptr };
    uint32_t                    colorAttachmentCount{ 0 };
    const AttachmentReference*  pColorAttachments{ nullptr };
    const AttachmentReference*  pResolveAttachments{ nullptr };
    const AttachmentReference*  pDepthStencilAttachment{ nullptr };
    uint32_t                    preserveAttachmentCount{ 0 };
    const uint32_t*             pPreserveAttachments{ nullptr };

    SubpassDescription(PipelineBindPoint aPipelineBindPoint, uint32_t aInputAttachmentCount, const AttachmentReference* apInputAttachments,
        uint32_t aColorAttachmentCount, const AttachmentReference* apColorAttachments, const AttachmentReference* apResolveAttachments = nullptr,
        const AttachmentReference* apDepthStencilAttachment = nullptr,
        uint32_t apReserveAttachmentCount = 0, const uint32_t* apPreserveAttachments = nullptr, const SubpassDescriptionFlags& aFlags = DefaultFlags)
        : flags(aFlags), pipelineBindPoint(aPipelineBindPoint), inputAttachmentCount(aInputAttachmentCount), pInputAttachments(apInputAttachments),
          colorAttachmentCount(aColorAttachmentCount), pColorAttachments(apColorAttachments), pResolveAttachments(apResolveAttachments),
          pDepthStencilAttachment(apDepthStencilAttachment),
          preserveAttachmentCount(apReserveAttachmentCount), pPreserveAttachments(apPreserveAttachments)
    {}

    DEFINE_CLASS_MEMBER(SubpassDescription)

    SubpassDescription& SetFlags(const SubpassDescriptionFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    SubpassDescription& SetPipelineBindPoint(PipelineBindPoint aPipelineBindPoint)
    {
        pipelineBindPoint = aPipelineBindPoint;

        return *this;
    }

    SubpassDescription& SetInputAttachments(uint32_t aInputAttachmentCount, const AttachmentReference* apInputAttachments)
    {
        inputAttachmentCount    = aInputAttachmentCount;
        pInputAttachments       = apInputAttachments;

        return *this;
    }

    SubpassDescription& SetInputAttachments(const std::vector<AttachmentReference>& aInputAttachments)
    {
        return SetInputAttachments(static_cast<uint32_t>(aInputAttachments.size()), aInputAttachments.data());
    }

    template <std::size_t I>
    SubpassDescription& SetInputAttachments(const std::array<AttachmentReference, I>& aInputAttachments)
    {
        return SetInputAttachments(static_cast<uint32_t>(aInputAttachments.size()), aInputAttachments.data());
    }

    SubpassDescription& SetColorAttachments(uint32_t aColorAttachmentCount, const AttachmentReference* apColorAttachments, const AttachmentReference* apResolveAttachments)
    {
        colorAttachmentCount    = aColorAttachmentCount;
        pColorAttachments       = apColorAttachments;
        pResolveAttachments     = apResolveAttachments;

        return *this;
    }

    SubpassDescription& SetColorAttachments(const std::vector<AttachmentReference>& aColorAttachments, const std::vector<AttachmentReference>& aResolveAttachments)
    {
        assert(aColorAttachments.size() == aResolveAttachments.size());

        return SetColorAttachments(static_cast<uint32_t>(aColorAttachments.size()), aColorAttachments.data(), aResolveAttachments.data());
    }

    template <std::size_t C>
    SubpassDescription& SetColorAttachments(const std::array<AttachmentReference, C>& aColorAttachments, const std::array<AttachmentReference, C>& aResolveAttachments)
    {
        return SetColorAttachments(static_cast<uint32_t>(aColorAttachments.size()), aColorAttachments.data(), aResolveAttachments.data());
    }

    SubpassDescription& SetDepthStencilAttachment(const AttachmentReference* apDepthStencilAttachment)
    {
        pDepthStencilAttachment = apDepthStencilAttachment;

        return *this;
    }

    SubpassDescription& SetDepthStencilAttachment(const AttachmentReference& aDepthStencilAttachment)
    {
        return SetDepthStencilAttachment(aDepthStencilAttachment.AddressOf());
    }

    SubpassDescription& SetPreserveAttachments(uint32_t aPreserveAttachmentCount, const uint32_t* apPreserveAttachments)
    {
        preserveAttachmentCount = aPreserveAttachmentCount;
        pPreserveAttachments    = apPreserveAttachments;

        return *this;
    }

    SubpassDescription& SetPreserveAttachments(const std::vector<uint32_t>& aPreserveAttachments)
    {
        return SetPreserveAttachments(static_cast<uint32_t>(aPreserveAttachments.size()), aPreserveAttachments.data());
    }

    template <std::size_t P>
    SubpassDescription& SetPreserveAttachments(const std::array<uint32_t, P>& aPreserveAttachments)
    {
        return SetPreserveAttachments(static_cast<uint32_t>(aPreserveAttachments.size()), aPreserveAttachments.data());
    }
};

StaticSizeCheck(SubpassDescription);



enum RenderPassCreateFlagBits
{};

using RenderPassCreateFlags = internal::Flags<RenderPassCreateFlagBits, VkRenderPassCreateFlags>;



namespace subpass
{
    constexpr auto External = VK_SUBPASS_EXTERNAL;
}           // End of namespace subpass;



struct SubpassDependency : public internal::VkTrait<SubpassDependency, VkSubpassDependency>
{
    uint32_t            srcSubpass{ 0 };
    uint32_t            dstSubpass{ 0 };
    PipelineStageFlags  srcStageMask;
    PipelineStageFlags  dstStageMask;
    AccessFlags         srcAccessMask;
    AccessFlags         dstAccessMask;
    DependencyFlags     depdencyFlags;

    DEFINE_CLASS_MEMBER(SubpassDependency)

    SubpassDependency(uint32_t aSrcSubpass, uint32_t aDstSubpass, const PipelineStageFlags& aSrcStageMask = DefaultFlags, const PipelineStageFlags& aDstStageMask = DefaultFlags,
        const AccessFlags& aSrcAccessMask = DefaultFlags, const AccessFlags& aDstAccessMask = DefaultFlags, const DependencyFlags& aDepdencyFlags = DefaultFlags)
        : srcSubpass(aSrcSubpass), dstSubpass(aDstSubpass), srcStageMask(aSrcStageMask), dstStageMask(aDstStageMask),
        srcAccessMask(aSrcAccessMask), dstAccessMask(aDstAccessMask), depdencyFlags(aDepdencyFlags)
    {}

    SubpassDependency& SetSubpass(uint32_t aSrcSubpass, uint32_t aDstSubpass)
    {
        srcSubpass  = aSrcSubpass;
        dstSubpass  = aDstSubpass;

        return *this;
    }

    SubpassDependency& SetPipelineStage(const PipelineStageFlags& aSrcStageMask, const PipelineStageFlags& aDstStageMask)
    {
        srcStageMask    = aSrcStageMask;
        dstStageMask    = aDstStageMask;

        return *this;
    }

    SubpassDependency& SetAccessFlags(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessMask)
    {
        srcAccessMask   = aSrcAccessMask;
        dstAccessMask   = aDstAccessMask;

        return *this;
    }

    SubpassDependency& SetDependencyFlags(const DependencyFlags& aDependencyFlags)
    {
        depdencyFlags   = aDependencyFlags;

        return *this;
    }
};

StaticSizeCheck(SubpassDependency);



class RenderPassCreateInfo : public internal::VkTrait<RenderPassCreateInfo, VkRenderPassCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eRenderPass;

public:
    const void*                     pNext{ nullptr };
    RenderPassCreateFlags           flags;
    uint32_t                        attachmentCount{ 0 };
    const AttachementDescription*   pAttachments{ nullptr };
    uint32_t                        subpassCount{ 0 };
    const SubpassDescription*       pSubpasses{ nullptr };
    uint32_t                        dependencyCount{ 0 };
    const SubpassDependency*        pDependencies{ nullptr };

    DEFINE_CLASS_MEMBER(RenderPassCreateInfo)

    RenderPassCreateInfo(uint32_t aAttachmentCount, const AttachementDescription* apAttachments, uint32_t aSubpassCount, const SubpassDescription* apSubpasses,
        uint32_t aDependencyCount = 0, const SubpassDependency* apDependencies = nullptr, const RenderPassCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), attachmentCount(aAttachmentCount), pAttachments(apAttachments), subpassCount(aSubpassCount), pSubpasses(apSubpasses),
          dependencyCount(aDependencyCount), pDependencies(apDependencies)
    {}

    RenderPassCreateInfo(const std::vector<AttachementDescription>& aAttachments, const std::vector<SubpassDescription>& aSubpasses, const RenderPassCreateFlags& aFlags = DefaultFlags)
        : RenderPassCreateInfo(static_cast<uint32_t>(aAttachments.size()), aAttachments.data(), static_cast<uint32_t>(aSubpasses.size()), aSubpasses.data(), 0, nullptr, aFlags)
    {}

    RenderPassCreateInfo(const std::vector<AttachementDescription>& aAttachments, const std::vector<SubpassDescription>& aSubpasses, const std::vector<SubpassDependency>& aDependencies,
        const RenderPassCreateFlags& aFlags = DefaultFlags)
        : RenderPassCreateInfo(static_cast<uint32_t>(aAttachments.size()), aAttachments.data(), static_cast<uint32_t>(aSubpasses.size()), aSubpasses.data(),
          static_cast<uint32_t>(aDependencies.size()), aDependencies.data(), aFlags)
    {}

    template <std::size_t A, std::size_t S, std::size_t D>
    RenderPassCreateInfo(const std::array<AttachementDescription, A>& aAttachments, const std::array<SubpassDescription, S>& aSubpasses, const RenderPassCreateFlags& aFlags = DefaultFlags)
        : RenderPassCreateInfo(static_cast<uint32_t>(aAttachments.size()), aAttachments.data(), static_cast<uint32_t>(aSubpasses.size()), aSubpasses.data(),
          0, nullptr, aFlags)
    {}

    template <std::size_t A, std::size_t S, std::size_t D>
    RenderPassCreateInfo(const std::array<AttachementDescription, A>& aAttachments, const std::array<SubpassDescription, S>& aSubpasses,
        const std::array<SubpassDependency, D>& aDependencies, const RenderPassCreateFlags& aFlags = DefaultFlags)
        : RenderPassCreateInfo(static_cast<uint32_t>(aAttachments.size()), aAttachments.data(), static_cast<uint32_t>(aSubpasses.size()), aSubpasses.data(),
          static_cast<uint32_t>(aDependencies.size()), aDependencies.data(), aFlags)
    {}

    RenderPassCreateInfo& SetNext(const void* apNext)
    {
        pNext   = apNext;

        return *this;
    }

    RenderPassCreateInfo& SetAttachments(uint32_t aAttachmentCount, const AttachementDescription* apAttachments)
    {
        attachmentCount = aAttachmentCount;
        pAttachments    = apAttachments;

        return *this;
    }

    RenderPassCreateInfo& SetAttachments(const std::vector<AttachementDescription>& aAttachments)
    {
        return SetAttachments(static_cast<uint32_t>(aAttachments.size()), aAttachments.data());
    }

    template <std::size_t A>
    RenderPassCreateInfo& SetAttachments(const std::array<AttachementDescription, A>& aAttachments)
    {
        return SetAttachments(static_cast<uint32_t>(aAttachments.size()), aAttachments.data());
    }

    RenderPassCreateInfo& SetSubpasses(uint32_t aSubpassCount, const SubpassDescription* apSubpasses)
    {
        subpassCount    = aSubpassCount;
        pSubpasses      = apSubpasses;

        return *this;
    }

    RenderPassCreateInfo& SetSubpasses(const std::vector<SubpassDescription>& aSubpasses)
    {
        return SetSubpasses(static_cast<uint32_t>(aSubpasses.size()), aSubpasses.data());
    }

    template <std::size_t S>
    RenderPassCreateInfo& SetSubpasses(const std::array<SubpassDescription, S>& aSubpasses)
    {
        return SetSubpasses(static_cast<uint32_t>(aSubpasses.size()), aSubpasses.data());
    }

    RenderPassCreateInfo& SetDependencies(uint32_t aDependencyCount, const SubpassDependency* apDependencies)
    {
        dependencyCount = aDependencyCount;
        pDependencies   = apDependencies;

        return *this;
    }

    RenderPassCreateInfo& SetDependencies(const std::vector<SubpassDependency>& aDependencies)
    {
        return SetDependencies(static_cast<uint32_t>(aDependencies.size()), aDependencies.data());
    }

    template <std::size_t D>
    RenderPassCreateInfo& SetDependencies(const std::array<SubpassDependency, D>& aDependencies)
    {
        return SetDependencies(static_cast<uint32_t>(aDependencies.size()), aDependencies.data());
    }
};

StaticSizeCheck(RenderPassCreateInfo);



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
