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

    AttachementDescription& SetFlags(const AttachmentDescriptionFlags& aFlags) noexcept
    {
        flags = aFlags;

        return *this;
    }

    AttachementDescription& SetFormat(Format aFormat) noexcept
    {
        format = aFormat;

        return *this;
    }

    AttachementDescription& SetSamples(SampleCountFlagBits aSamples) noexcept
    {
        samples = aSamples;

        return *this;
    }

    AttachementDescription& SetOps(AttachmentLoadOp aLoadOp, AttachmentStoreOp aStoreOp) noexcept
    {
        loadOp  = aLoadOp;
        storeOp = aStoreOp;

        return *this;
    }

    AttachementDescription& SetStencilOps(AttachmentLoadOp aLoadOp, AttachmentStoreOp aStoreOp) noexcept
    {
        stencilLoadOp   = aLoadOp;
        stencilStoreOp  = aStoreOp;

        return *this;
    }

    AttachementDescription& SetLayouts(ImageLayout aInitialLayout, ImageLayout aFinalLayout) noexcept
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

    AttachmentReference& SetAttachment(uint32_t aAttachment, ImageLayout aLayout) noexcept
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



class SubpassDescription : public internal::VkTrait<SubpassDescription, VkSubpassDescription>
{
private:
    constexpr SubpassDescription(PipelineBindPoint, AttachmentReference&&, AttachmentReference&&, const SubpassDescriptionFlags& = DefaultFlags) noexcept = delete;

public:
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

    constexpr SubpassDescription(PipelineBindPoint aPipelineBindPoint, const AttachmentReference& aColorAttachment,
        const AttachmentReference& aDepthStencilAttachment, const SubpassDescriptionFlags& aFlags = DefaultFlags) noexcept
        : SubpassDescription(aPipelineBindPoint, 0, nullptr, 1, aColorAttachment.AddressOf(), nullptr, aDepthStencilAttachment.AddressOf(), 0, nullptr, aFlags)
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
        : SubpassDescription(aPipelineBindPoint, SizeOf<uint32_t>(aInputAttachments), DataOf(aInputAttachments),
            SizeOf<uint32_t>(aColorAttachments), DataOf(aColorAttachments), aResolveAttachments.data(), apDepthStencilAttachment,
            SizeOf<uint32_t>(aPreserveAttachments), DataOf(aPreserveAttachments), aFlags)
    {
        assert(aColorAttachments.size() == aResolveAttachments.size());
    }

    // Color and Depth attachments.
    template <typename A, typename = EnableIfValueType<ValueType<A>, AttachmentReference>>
    constexpr SubpassDescription(PipelineBindPoint aPipelineBindPoint, A&& aColorAttachments, const AttachmentReference* apDepthStencilAttachment, const SubpassDescriptionFlags& aFlags = DefaultFlags) noexcept
        : SubpassDescription(aPipelineBindPoint, 0, nullptr, SizeOf<uint32_t>(aColorAttachments), DataOf(aColorAttachments), nullptr, apDepthStencilAttachment, 0, nullptr, aFlags)
    {}

    // Color, Resolve and Depth attachments.
    template <typename A, typename = EnableIfValueType<ValueType<A>, AttachmentReference>>
    constexpr SubpassDescription(PipelineBindPoint aPipelineBindPoint, A&& aColorAttachments, A&& aResolveAttachments,
            const AttachmentReference* apDepthStencilAttachment, const SubpassDescriptionFlags& aFlags = DefaultFlags) noexcept
        : SubpassDescription(aPipelineBindPoint, 0, nullptr,
            SizeOf<uint32_t>(aColorAttachments), DataOf(aColorAttachments), DataOf(aResolveAttachments), apDepthStencilAttachment,
            0, nullptr, aFlags)
    {
        assert(aColorAttachments.size() == aResolveAttachments.size());
    }

    SubpassDescription& SetFlags(const SubpassDescriptionFlags& aFlags) noexcept
    {
        flags = aFlags;

        return *this;
    }

    SubpassDescription& SetPipelineBindPoint(PipelineBindPoint aPipelineBindPoint) noexcept
    {
        pipelineBindPoint = aPipelineBindPoint;

        return *this;
    }

    SubpassDescription& SetInputAttachments(uint32_t aInputAttachmentCount, const AttachmentReference* apInputAttachments) noexcept
    {
        inputAttachmentCount    = aInputAttachmentCount;
        pInputAttachments       = apInputAttachments;

        return *this;
    }

    template <typename I, typename = EnableIfValueType<ValueType<I>, AttachmentReference>>
    SubpassDescription& SetInputAttachments(I&& aInputAttachments) noexcept
    {
        StaticLValueRefAssert(I, aInputAttachments);

        return SetInputAttachments(SizeOf<uint32_t>(aInputAttachments), DataOf(aInputAttachments));
    }

    SubpassDescription& SetColorAttachments(uint32_t aColorAttachmentCount, const AttachmentReference* apColorAttachments, const AttachmentReference* apResolveAttachments = nullptr) noexcept
    {
        assert(aColorAttachmentCount != 0);

        colorAttachmentCount    = aColorAttachmentCount;
        pColorAttachments       = apColorAttachments;
        pResolveAttachments     = apResolveAttachments;

        return *this;
    }

    template <typename C, typename = EnableIfValueType<ValueType<C>, AttachmentReference>>
    SubpassDescription& SetColorAttachments(C&& aColorAttachments) noexcept
    {
        StaticLValueRefAssert(C, aColorAttachments);

        return SetColorAttachments(SizeOf<uint32_t>(aColorAttachments), DataOf(aColorAttachments));
    }

    template <typename C, typename = EnableIfValueType<ValueType<C>, AttachmentReference>>
    SubpassDescription& SetColorAttachments(C&& aColorAttachments, C&& aResolveAttachments) noexcept
    {
        StaticLValueRefAssert(C, aColorAttachments);
        StaticLValueRefAssert(C, aResolveAttachments);

        assert(aColorAttachments.size() == aResolveAttachments.size());

        return SetColorAttachments(SizeOf<uint32_t>(aColorAttachments), DataOf(aColorAttachments), DataOf(aResolveAttachments));
    }

    SubpassDescription& SetDepthStencilAttachment(const AttachmentReference* apDepthStencilAttachment) noexcept
    {
        pDepthStencilAttachment = apDepthStencilAttachment;

        return *this;
    }

    SubpassDescription& SetDepthStencilAttachment(const AttachmentReference& aDepthStencilAttachment) noexcept
    {
        return SetDepthStencilAttachment(aDepthStencilAttachment.AddressOf());
    }

    SubpassDescription& SetPreserveAttachments(uint32_t aPreserveAttachmentCount, const uint32_t* apPreserveAttachments) noexcept
    {
        preserveAttachmentCount = aPreserveAttachmentCount;
        pPreserveAttachments    = apPreserveAttachments;

        return *this;
    }
    
    template <typename P, typename = EnableIfValueType<ValueType<P>, uint32_t>>
    SubpassDescription& SetPreserveAttachments(P&& aPreserveAttachments) noexcept
    {
        StaticLValueRefAssert(P, aPreserveAttachments);

        return SetPreserveAttachments(SizeOf<uint32_t>(aPreserveAttachments), DataOf(aPreserveAttachments));
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

    SubpassDependency& SetSubpass(uint32_t aSrcSubpass, uint32_t aDstSubpass) noexcept
    {
        srcSubpass  = aSrcSubpass;
        dstSubpass  = aDstSubpass;

        return *this;
    }

    SubpassDependency& SetPipelineStage(const PipelineStageFlags& aSrcStageMask, const PipelineStageFlags& aDstStageMask) noexcept
    {
        srcStageMask    = aSrcStageMask;
        dstStageMask    = aDstStageMask;

        return *this;
    }

    SubpassDependency& SetAccessFlags(const AccessFlags& aSrcAccessMask, const AccessFlags& aDstAccessMask) noexcept
    {
        srcAccessMask   = aSrcAccessMask;
        dstAccessMask   = aDstAccessMask;

        return *this;
    }

    SubpassDependency& SetDependencyFlags(const DependencyFlags& aDependencyFlags) noexcept
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
        : RenderPassCreateInfo(SizeOf<uint32_t>(aAttachments), DataOf(aAttachments), SizeOf<uint32_t>(aSubpasses), DataOf(aSubpasses), 0, nullptr, aFlags)
    {
        StaticLValueRefAssert(A, aAttachments);
        StaticLValueRefAssert(S, aSubpasses);
    }

    template <typename A, typename S, typename D, typename = EnableIfValueType<ValueType<A>, AttachementDescription, ValueType<S>, SubpassDescription, ValueType<D>, SubpassDependency>>
    constexpr RenderPassCreateInfo(A&& aAttachments, S&& aSubpasses, D&& aDependencies, const RenderPassCreateFlags& aFlags = DefaultFlags) noexcept
        : RenderPassCreateInfo(SizeOf<uint32_t>(aAttachments), DataOf(aAttachments), SizeOf<uint32_t>(aSubpasses), DataOf(aSubpasses),
            SizeOf<uint32_t>(aDependencies), DataOf(aDependencies), aFlags)
    {
        StaticLValueRefAssert(A, aAttachments);
        StaticLValueRefAssert(S, aSubpasses);
        StaticLValueRefAssert(D, aDependencies);
    }

    RenderPassCreateInfo& SetNext(const void* apNext) noexcept
    {
        pNext   = apNext;

        return *this;
    }

    RenderPassCreateInfo& SetAttachments(uint32_t aAttachmentCount, const AttachementDescription* apAttachments) noexcept
    {
        attachmentCount = aAttachmentCount;
        pAttachments    = apAttachments;

        return *this;
    }

    template <typename A, typename = EnableIfValueType<ValueType<A>, AttachementDescription>>
    RenderPassCreateInfo& SetAttachments(A&& aAttachments) noexcept
    {
        StaticLValueRefAssert(A, aAttachments);

        return SetAttachments(SizeOf<uint32_t>(aAttachments), DataOf(aAttachments));
    }

    RenderPassCreateInfo& SetSubpasses(uint32_t aSubpassCount, const SubpassDescription* apSubpasses) noexcept
    {
        subpassCount    = aSubpassCount;
        pSubpasses      = apSubpasses;

        return *this;
    }

    template <typename S, typename = EnableIfValueType<ValueType<S>, SubpassDescription>>
    RenderPassCreateInfo& SetSubpasses(S&& aSubpasses) noexcept
    {
        StaticLValueRefAssert(S, aSubpasses);

        return SetSubpasses(SizeOf<uint32_t>(aSubpasses), DataOf(aSubpasses));
    }

    RenderPassCreateInfo& SetDependencies(uint32_t aDependencyCount, const SubpassDependency* apDependencies) noexcept
    {
        dependencyCount = aDependencyCount;
        pDependencies   = apDependencies;

        return *this;
    }

    template <typename D, typename = EnableIfValueType<ValueType<D>, SubpassDependency>>
    RenderPassCreateInfo& SetDependencies(D&& aDependencies) noexcept
    {
        StaticLValueRefAssert(D, aDependencies);

        return SetDependencies(SizeOf<uint32_t>(aDependencies), DataOf(aDependencies));
    }
};

ConsistencyCheck(RenderPassCreateInfo, pNext, flags, attachmentCount, pAttachments, subpassCount, pSubpasses, dependencyCount, pDependencies)



class RenderPass : public internal::VkTrait<RenderPass, VkRenderPass>
{
private:
    VkRenderPass mRenderPass{ VK_NULL_HANDLE };

public:
    RenderPass(void) noexcept = default;

    RenderPass(std::nullptr_t) noexcept
    {}

    explicit RenderPass(VkRenderPass aRenderPass) noexcept : mRenderPass(aRenderPass)
    {}
};

StaticSizeCheck(RenderPass)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_RENDER_PASS_H__
