#ifndef __VKPP_TYPE_RENDER_PASS_H__
#define __VKPP_TYPE_RENDER_PASS_H__



#include <Info/Common.h>
#include <Info/Flags.h>
#include <Info/Format.h>
#include <Info/PipelineStage.h>

#include <Type/MemoryBarrier.h>



namespace vkpp
{



enum class AttachmentDescriptionFlagBits
{
    eMayAlias   = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT
};

VKPP_ENUM_BIT_MASK_FLAGS(AttachmentDescription)



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
    Format                      format{ Format::eUndefined };
    SampleCountFlagBits         samples{ SampleCountFlagBits::e1 };
    AttachmentLoadOp            loadOp{ AttachmentLoadOp::eDontCare };
    AttachmentStoreOp           storeOp{ AttachmentStoreOp::eDontCare };
    AttachmentLoadOp            stencilLoadOp{ AttachmentLoadOp::eDontCare };
    AttachmentStoreOp           stencilStoreOp{ AttachmentStoreOp::eDontCare };
    ImageLayout                 initialLayout{ ImageLayout::eUndefined };
    ImageLayout                 finalLayout{ ImageLayout::eUndefined };

    DEFINE_CLASS_MEMBER(AttachementDescription)

    constexpr AttachementDescription(Format aFormat, SampleCountFlagBits aSamples, AttachmentLoadOp aLoadOp, AttachmentStoreOp aStoreOp,
        AttachmentLoadOp aStencilLoadOp, AttachmentStoreOp aStencilStoreOp, ImageLayout aInitialLayOut, ImageLayout aFinalLayout,
        const AttachmentDescriptionFlags& aFlags = DefaultFlags) noexcept
        : flags(aFlags), format(aFormat), samples(aSamples), loadOp(aLoadOp), storeOp(aStoreOp), stencilLoadOp(aStencilLoadOp), stencilStoreOp(aStencilStoreOp),
        initialLayout(aInitialLayOut), finalLayout(aFinalLayout)
    {}

    constexpr AttachementDescription(Format aFormat, SampleCountFlagBits aSamples, AttachmentLoadOp aLoadOp, AttachmentStoreOp aStoreOp,
        ImageLayout aInitialLayOut, ImageLayout aFinalLayout, const AttachmentDescriptionFlags& aFlags = DefaultFlags) noexcept
        : AttachementDescription(aFormat, aSamples, aLoadOp, aStoreOp, AttachmentLoadOp::eDontCare, AttachmentStoreOp::eDontCare, aInitialLayOut, aFinalLayout, aFlags)
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

ConsistencyCheck(AttachementDescription, flags, format, samples, loadOp, storeOp, stencilLoadOp, stencilStoreOp, initialLayout, finalLayout)



/**
 * \class AttachmentReference
 * \ingroup vkpp
 * 
 * \brief Structure specifying an attachment reference.
 */
struct AttachmentReference : public internal::VkTrait<AttachmentReference, VkAttachmentReference>
{
    /**
     * \var     attachment
     * \brief   attachment is the index of the attachment of the render pass, and corresponds to the index of the corresponding element
     *          in the pAttachments array of the RenderPassCreateInfo structure.
     */
    uint32_t    attachment{ UINT32_MAX };

    /**
     * \var layout
     * \brief layout is a ImageLayout value specifying the layout the attachment uses during the subpass.
     */
    ImageLayout layout;

    DEFINE_CLASS_MEMBER(AttachmentReference)

    constexpr AttachmentReference(uint32_t aAttachment, ImageLayout aLayout) noexcept : attachment(aAttachment), layout(aLayout)
    {}

    AttachmentReference& SetAttachment(uint32_t aAttachment, ImageLayout aLayout)
    {
        attachment  = aAttachment;
        layout      = aLayout;

        return *this;
    }
};

ConsistencyCheck(AttachmentReference, attachment, layout)



enum class SubpassDescriptionFlagBits
{
    ePerViewAttributesNVX       = VK_SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX,
    ePerViewPositionXOnlyNVX    = VK_SUBPASS_DESCRIPTION_PER_VIEW_POSITION_X_ONLY_BIT_NVX
};

VKPP_ENUM_BIT_MASK_FLAGS(SubpassDescription)



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

    DEFINE_CLASS_MEMBER(SubpassDescription)

    // Input, Color, Resolve, Depth and Preserve attachments.
    constexpr SubpassDescription(PipelineBindPoint aPipelineBindPoint, uint32_t aInputAttachmentCount, const AttachmentReference* apInputAttachments,
        uint32_t aColorAttachmentCount, const AttachmentReference* apColorAttachments, const AttachmentReference* apResolveAttachments,
        const AttachmentReference* apDepthStencilAttachment,
        uint32_t aPreserveAttachmentCount = 0, const uint32_t* apPreserveAttachments = nullptr, const SubpassDescriptionFlags& aFlags = DefaultFlags) noexcept
        : flags(aFlags), pipelineBindPoint(aPipelineBindPoint), inputAttachmentCount(aInputAttachmentCount), pInputAttachments(apInputAttachments),
          colorAttachmentCount(aColorAttachmentCount), pColorAttachments(apColorAttachments), pResolveAttachments(apResolveAttachments),
          pDepthStencilAttachment(apDepthStencilAttachment),
          preserveAttachmentCount(aPreserveAttachmentCount), pPreserveAttachments(apPreserveAttachments)
    {}

    // Color and Depth attachments.
    constexpr SubpassDescription(PipelineBindPoint aPipelineBindPoint, uint32_t aColorAttachmentCount, const AttachmentReference* apColorAttachments,
        const AttachmentReference* apDepthStencilAttachment, const SubpassDescriptionFlags& aFlags = DefaultFlags) noexcept
        : SubpassDescription(aPipelineBindPoint, 0, nullptr, aColorAttachmentCount, apColorAttachments, nullptr, apDepthStencilAttachment, 0, nullptr, aFlags)
    {}

    // Color, Resolve and Depth attachments.
    constexpr SubpassDescription(PipelineBindPoint aPipelineBindPoint, uint32_t aColorAttachmentCount, const AttachmentReference* apColorAttachments,
        const AttachmentReference* apResolveAttachments, const AttachmentReference* apDepthStencilAttachment, const SubpassDescriptionFlags& aFlags = DefaultFlags) noexcept
        : SubpassDescription(aPipelineBindPoint, 0, nullptr, aColorAttachmentCount, apColorAttachments, apResolveAttachments, apDepthStencilAttachment, 0, nullptr, aFlags)
    {}

    // Input, Color, Resolve, Preserve and Depth attachments.
    template <typename A, typename = EnableIfValueType<ValueType<A>, AttachmentReference>>
    constexpr SubpassDescription(PipelineBindPoint aPipelineBindPoint, A&& aInputAttachments, A&& aColorAttachments, A&& aResolveAttachments,
        A&& aPreserveAttachments, const AttachmentReference* apDepthStencilAttachment, const SubpassDescriptionFlags& aFlags = DefaultFlags) noexcept
        : SubpassDescription(aPipelineBindPoint, static_cast<uint32_t>(aInputAttachments.size()), aInputAttachments.data(),
            static_cast<uint32_t>(aColorAttachments.size()), aColorAttachments.data(), aResolveAttachments.data(), apDepthStencilAttachment,
            static_cast<uint32_t>(aPreserveAttachments.size()), aPreserveAttachments.data(), aFlags)
    {
        assert(aColorAttachments.size() == aResolveAttachments.size());
    }

    // Color and Depth attachments.
    template <typename A, typename = EnableIfValueType<ValueType<A>, AttachmentReference>>
    constexpr SubpassDescription(PipelineBindPoint aPipelineBindPoint, A&& aColorAttachments, const AttachmentReference* apDepthStencilAttachment, const SubpassDescriptionFlags& aFlags = DefaultFlags) noexcept
        : SubpassDescription(aPipelineBindPoint, 0, nullptr, static_cast<uint32_t>(aColorAttachments.size()), aColorAttachments.data(), nullptr, apDepthStencilAttachment, 0, nullptr, aFlags)
    {}

    // Color, Resolve and Depth attachments.
    template <typename A, typename = EnableIfValueType<ValueType<A>, AttachmentReference>>
    constexpr SubpassDescription(PipelineBindPoint aPipelineBindPoint, A&& aColorAttachments, A&& aResolveAttachments,
            const AttachmentReference* apDepthStencilAttachment, const SubpassDescriptionFlags& aFlags = DefaultFlags) noexcept
        : SubpassDescription(aPipelineBindPoint, 0, nullptr,
            static_cast<uint32_t>(aColorAttachments.size()), aColorAttachments.data(), aResolveAttachments.data(), apDepthStencilAttachment,
            0, nullptr, aFlags)
    {
        assert(aColorAttachments.size() == aResolveAttachments.size());
    }

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

    template <typename I, typename = EnableIfValueType<ValueType<I>, AttachmentReference>>
    SubpassDescription& SetInputAttachments(I&& aInputAttachments)
    {
        StaticLValueRefAssert(I, aInputAttachments);

        return SetInputAttachments(static_cast<uint32_t>(aInputAttachments.size()), aInputAttachments.data());
    }

    SubpassDescription& SetColorAttachments(uint32_t aColorAttachmentCount, const AttachmentReference* apColorAttachments, const AttachmentReference* apResolveAttachments = nullptr)
    {
        colorAttachmentCount    = aColorAttachmentCount;
        pColorAttachments       = apColorAttachments;
        pResolveAttachments     = apResolveAttachments;

        return *this;
    }

    template <typename C, typename = EnableIfValueType<ValueType<C>, AttachmentReference>>
    SubpassDescription& SetColorAttachments(C&& aColorAttachments)
    {
        StaticLValueRefAssert(C, aColorAttachments);

        return SetColorAttachments(static_cast<uint32_t>(aColorAttachments.size()), aColorAttachments.data());
    }

    template <typename C, typename = EnableIfValueType<ValueType<C>, AttachmentReference>>
    SubpassDescription& SetColorAttachments(C&& aColorAttachments, C&& aResolveAttachments)
    {
        StaticLValueRefAssert(C, aColorAttachments);
        StaticLValueRefAssert(C, aResolveAttachments);

        assert(aColorAttachments.size() == aResolveAttachments.size());

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
    
    template <typename P, typename = EnableIfValueType<ValueType<P>, uint32_t>>
    SubpassDescription& SetPreserveAttachments(P&& aPreserveAttachments)
    {
        StaticLValueRefAssert(P, aPreserveAttachments);

        return SetPreserveAttachments(static_cast<uint32_t>(aPreserveAttachments.size()), aPreserveAttachments.data());
    }
};

ConsistencyCheck(SubpassDescription, flags, pipelineBindPoint, inputAttachmentCount, pInputAttachments, colorAttachmentCount, pColorAttachments, pResolveAttachments,
    pDepthStencilAttachment, preserveAttachmentCount, pPreserveAttachments)



enum class RenderPassCreateFlagBits
{};

VKPP_ENUM_BIT_MASK_FLAGS(RenderPassCreate)



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
    DependencyFlags     dependencyFlags;

    DEFINE_CLASS_MEMBER(SubpassDependency)

    constexpr SubpassDependency(uint32_t aSrcSubpass, uint32_t aDstSubpass, const PipelineStageFlags& aSrcStageMask = DefaultFlags, const PipelineStageFlags& aDstStageMask = DefaultFlags,
        const AccessFlags& aSrcAccessMask = DefaultFlags, const AccessFlags& aDstAccessMask = DefaultFlags, const DependencyFlags& aDependencyFlags = DefaultFlags) noexcept
        : srcSubpass(aSrcSubpass), dstSubpass(aDstSubpass), srcStageMask(aSrcStageMask), dstStageMask(aDstStageMask),
        srcAccessMask(aSrcAccessMask), dstAccessMask(aDstAccessMask), dependencyFlags(aDependencyFlags)
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
        dependencyFlags   = aDependencyFlags;

        return *this;
    }
};

ConsistencyCheck(SubpassDependency, srcSubpass, dstSubpass, srcStageMask, dstStageMask, srcAccessMask, dstAccessMask, dependencyFlags)



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

    constexpr RenderPassCreateInfo(uint32_t aAttachmentCount, const AttachementDescription* apAttachments, uint32_t aSubpassCount, const SubpassDescription* apSubpasses,
        uint32_t aDependencyCount = 0, const SubpassDependency* apDependencies = nullptr, const RenderPassCreateFlags& aFlags = DefaultFlags) noexcept
        : flags(aFlags), attachmentCount(aAttachmentCount), pAttachments(apAttachments), subpassCount(aSubpassCount), pSubpasses(apSubpasses),
          dependencyCount(aDependencyCount), pDependencies(apDependencies)
    {}

    template <typename A, typename S, typename = EnableIfValueType<ValueType<A>, AttachementDescription, ValueType<S>, SubpassDescription>>
    constexpr RenderPassCreateInfo(A&& aAttachments, S&& aSubpasses, const RenderPassCreateFlags& aFlags = DefaultFlags) noexcept
        : RenderPassCreateInfo(static_cast<uint32_t>(aAttachments.size()), aAttachments.data(), static_cast<uint32_t>(aSubpasses.size()), aSubpasses.data(), 0, nullptr, aFlags)
    {
        StaticLValueRefAssert(A, aAttachments);
        StaticLValueRefAssert(S, aSubpasses);
    }

    template <typename A, typename S, typename D, typename = EnableIfValueType<ValueType<A>, AttachementDescription, ValueType<S>, SubpassDescription, ValueType<D>, SubpassDependency>>
    constexpr RenderPassCreateInfo(A&& aAttachments, S&& aSubpasses, D&& aDependencies, const RenderPassCreateFlags& aFlags = DefaultFlags) noexcept
        : RenderPassCreateInfo(static_cast<uint32_t>(aAttachments.size()), aAttachments.data(), static_cast<uint32_t>(aSubpasses.size()), aSubpasses.data(),
          static_cast<uint32_t>(aDependencies.size()), aDependencies.data(), aFlags)
    {
        StaticLValueRefAssert(A, aAttachments);
        StaticLValueRefAssert(S, aSubpasses);
        StaticLValueRefAssert(D, aDependencies);
    }

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

    template <typename A, typename = EnableIfValueType<ValueType<A>, AttachementDescription>>
    RenderPassCreateInfo& SetAttachments(A&& aAttachments)
    {
        StaticLValueRefAssert(A, aAttachments);

        return SetAttachments(static_cast<uint32_t>(aAttachments.size()), aAttachments.data());
    }

    RenderPassCreateInfo& SetSubpasses(uint32_t aSubpassCount, const SubpassDescription* apSubpasses)
    {
        subpassCount    = aSubpassCount;
        pSubpasses      = apSubpasses;

        return *this;
    }

    template <typename S, typename = EnableIfValueType<ValueType<S>, SubpassDescription>>
    RenderPassCreateInfo& SetSubpasses(S&& aSubpasses)
    {
        StaticLValueRefAssert(S, aSubpasses);

        return SetSubpasses(static_cast<uint32_t>(aSubpasses.size()), aSubpasses.data());
    }

    RenderPassCreateInfo& SetDependencies(uint32_t aDependencyCount, const SubpassDependency* apDependencies)
    {
        dependencyCount = aDependencyCount;
        pDependencies   = apDependencies;

        return *this;
    }

    template <typename D, typename = EnableIfValueType<ValueType<D>, SubpassDependency>>
    RenderPassCreateInfo& SetDependencies(D&& aDependencies)
    {
        StaticLValueRefAssert(D, aDependencies);

        return SetDependencies(static_cast<uint32_t>(aDependencies.size()), aDependencies.data());
    }
};

ConsistencyCheck(RenderPassCreateInfo, pNext, flags, attachmentCount, pAttachments, subpassCount, pSubpasses, dependencyCount, pDependencies)



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

StaticSizeCheck(RenderPass)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_RENDER_PASS_H__
