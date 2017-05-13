#ifndef __VKPP_TYPE_COMMAND_BUFFER_H__
#define __VKPP_TYPE_COMMAND_BUFFER_H__



#include <cassert>

#include <Info/Common.h>
#include <Info/PipelineStage.h>
#include <Info/RenderPassBeginInfo.h>

#include <Type/RenderPass.h>
#include <Type/GraphicsPipeline.h>
#include <Type/MemoryBarrier.h>



namespace vkpp
{



enum class CommandBufferUsageFlagBits
{
    eOneTimeSubmit          = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    eRenderPassContinue     = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
    eSimultaneousUse        = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
};

VKPP_ENUM_BIT_MASK_FLAGS(CommandBufferUsage)



enum class QueryControlFlagBits
{
    ePrecise        = VK_QUERY_CONTROL_PRECISE_BIT
};

VKPP_ENUM_BIT_MASK_FLAGS(QueryControl)



enum class QueryPipelineStatisticFlagBits
{
    eInputAssemblyVertices                      = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT,
    eInputAssemblyPrimitives                    = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT,
    eVertexShaderInvocations                    = VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT,
    eGeometryShaderInvocations                  = VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT,
    eGeometryShaderPrimitives                   = VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT,
    eClippingInvocations                        = VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT,
    eClippingPrimitives                         = VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT,
    eFragmentShaderInvocations                  = VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT,
    eTessellationControlShaderPatches           = VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT,
    eTessellationEvaluationShaderInvocations    = VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT,
    eComputeShaderInvocations                   = VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT
};

VKPP_ENUM_BIT_MASK_FLAGS(QueryPipelineStatistic)



class CommandBufferInheritanceInfo : public internal::VkTrait<CommandBufferInheritanceInfo, VkCommandBufferInheritanceInfo>
{
private:
    const internal::Structure sType = internal::Structure::eCommandBufferInheritance;

public:
    const void*                 pNext{ nullptr };
    RenderPass                  renderPass;
    uint32_t                    subpass{ 0 };
    Framebuffer                 framebuffer;
    Bool32                      occlusionQueryEnable{ VK_FALSE };
    QueryControlFlags           queryFlags;
    QueryPipelineStatisticFlags pipelineStatistics;

    DEFINE_CLASS_MEMBER(CommandBufferInheritanceInfo)

    CommandBufferInheritanceInfo(const RenderPass& aRenderPass, uint32_t aSubpass, const Framebuffer& aFramebuffer, Bool32 aOcclusionQueryEnable,
        QueryControlFlags aQueryFlags = DefaultFlags, QueryPipelineStatisticFlags aPipelineStatistics = DefaultFlags)
        : renderPass(aRenderPass), subpass(aSubpass), framebuffer(aFramebuffer), occlusionQueryEnable(aOcclusionQueryEnable),
          queryFlags(aQueryFlags), pipelineStatistics(aPipelineStatistics)
    {}

    CommandBufferInheritanceInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    CommandBufferInheritanceInfo& SetRenderPass(const RenderPass& aRenderPass, uint32_t aSubpass)
    {
        renderPass  = aRenderPass;
        subpass     = aSubpass;

        return *this;
    }

    CommandBufferInheritanceInfo& SetFramebuffer(const Framebuffer& aFramebuffer)
    {
        framebuffer = aFramebuffer;

        return *this;
    }

    CommandBufferInheritanceInfo& SetOcclusionQuery(Bool32 aOcclusionQueryEnable = VK_TRUE)
    {
        occlusionQueryEnable = aOcclusionQueryEnable;

        return *this;
    }

    CommandBufferInheritanceInfo& SetQueryFlags(const QueryControlFlags& aQueryFlags)
    {
        queryFlags = aQueryFlags;

        return *this;
    }

    CommandBufferInheritanceInfo& SetPipelineStatistics(const QueryPipelineStatisticFlags& aPipelineStatistics)
    {
        pipelineStatistics = aPipelineStatistics;

        return *this;
    }
};

ConsistencyCheck(CommandBufferInheritanceInfo, pNext, renderPass, subpass, framebuffer, occlusionQueryEnable, queryFlags, pipelineStatistics)



class CommandBufferBeginInfo : public internal::VkTrait<CommandBufferBeginInfo, VkCommandBufferBeginInfo>
{
private:
    const internal::Structure sType = internal::Structure::eCommandBufferBegin;

public:
    const void*                         pNext{ nullptr };
    CommandBufferUsageFlags             flags;
    const CommandBufferInheritanceInfo* pInheritanceInfo{ nullptr };

    DEFINE_CLASS_MEMBER(CommandBufferBeginInfo)

    explicit constexpr CommandBufferBeginInfo(const CommandBufferUsageFlags& aFlags) noexcept
    : flags(aFlags)
    {}

    constexpr CommandBufferBeginInfo(const CommandBufferUsageFlags& aFlags, const CommandBufferInheritanceInfo& aInheritanceInfo) noexcept
        : flags(aFlags), pInheritanceInfo(aInheritanceInfo.AddressOf())
    {}

    CommandBufferBeginInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    CommandBufferBeginInfo& SetInheritanceInfo(const CommandBufferInheritanceInfo* apInheritanceInfo)
    {
        pInheritanceInfo = apInheritanceInfo;

        return *this;
    }
};

ConsistencyCheck(CommandBufferBeginInfo, pNext, flags, pInheritanceInfo)



// TODO
struct CommandPipelineBarrier
{
    PipelineStageFlags          srcStageMask;
    PipelineStageFlags          dstStageMask;
    DependencyFlags             dependencyFlags;
    uint32_t                    memoryBarrierCount{ 0 };
    const MemoryBarrier*        pMemoryBarriers{ nullptr };
    uint32_t                    bufferMemoryBarrierCount{ 0 };
    const BufferMemoryBarrier*  pBufferMemoryBarriers{ nullptr };
    uint32_t                    imageMemoryBarrierCount{ 0 };
    const ImageMemoryBarrier*   pImageMemoryBarriers{ nullptr };

    CommandPipelineBarrier& SetStageMask(const PipelineStageFlags& aSrcStageMask, const PipelineStageFlags& aDstStageMask)
    {
        srcStageMask = aSrcStageMask;
        dstStageMask = aDstStageMask;

        return *this;
    }

    CommandPipelineBarrier& SetDependencyFlags(const DependencyFlags& aDependencyFlags)
    {
        dependencyFlags = aDependencyFlags;

        return *this;
    }

    CommandPipelineBarrier& SetMemoryBarriers(uint32_t aMemoryBarrierCount, const MemoryBarrier* apMemoryBarriers)
    {
        memoryBarrierCount  = aMemoryBarrierCount;
        pMemoryBarriers     = apMemoryBarriers;

        return *this;
    }

    template <typename Array>
    CommandPipelineBarrier& SetMemoryBarriers(const Array& aMemoryBarriers)
    {
        return SetMemoryBarriers(static_cast<uint32_t>(aMemoryBarriers.size()), aMemoryBarriers.data());
    }

    CommandPipelineBarrier& SetBufferMemoryBarriers(uint32_t aBufferMemoryBarrierCount, const BufferMemoryBarrier* apBufferMemoryBarriers)
    {
        bufferMemoryBarrierCount    = aBufferMemoryBarrierCount;
        pBufferMemoryBarriers        = apBufferMemoryBarriers;

        return *this;
    }

    template <typename Array>
    CommandPipelineBarrier& SetBufferMemoryBarriers(const Array& aBufferMemoryBarriers)
    {
        return SetBufferMemoryBarriers(static_cast<uint32_t>(aBufferMemoryBarriers.size()), aBufferMemoryBarriers.data());
    }

    CommandPipelineBarrier& SetImageMemoryBarriers(uint32_t aImageMemoryBarrierCount, const ImageMemoryBarrier* apImageMemoryBarriers)
    {
        imageMemoryBarrierCount = aImageMemoryBarrierCount;
        pImageMemoryBarriers    = apImageMemoryBarriers;

        return *this;
    }

    template <typename Array>
    CommandPipelineBarrier& SetImageMemoryBarriers(const Array& aImageMemoryBarriers)
    {
        return SetImageMemoryBarriers(static_cast<uint32_t>(aImageMemoryBarriers.size()), aImageMemoryBarriers.data());
    }
};


//struct CommandPipelineBarrier
//{
//    PipelineStageFlags          srcStageMask;
//    PipelineStageFlags          dstStageMask;
//    DependencyFlags             dependencyFlags;
//    const std::vector<MemoryBarrier>        MemoryBarriers;
//    const std::vector<BufferMemoryBarrier>  BufferMemoryBuffers;
//    const std::vector<ImageMemoryBarrier>   ImageMemoryBarriers;
//};



struct BufferImageCopy : public internal::VkTrait<BufferImageCopy, VkBufferImageCopy>
{
    DeviceSize              bufferOffset{ 0 };
    uint32_t                bufferRowLength{ 0 };
    uint32_t                bufferImageHeight{ 0 };
    ImageSubresourceLayers  imageSubresource;
    Offset3D                imageOffset;
    Extent3D                imageExtent;

    DEFINE_CLASS_MEMBER(BufferImageCopy)

    BufferImageCopy(DeviceSize aBufferOffset, uint32_t aBufferRowLength, uint32_t aBufferImageHeight, const ImageSubresourceLayers& aImageSubresource, const Offset3D& aImageOffset, const Extent3D& aImageExtent)
        : bufferOffset(aBufferOffset), bufferRowLength(aBufferRowLength), bufferImageHeight(aBufferImageHeight), imageSubresource(aImageSubresource), imageOffset(aImageOffset), imageExtent(aImageExtent)
    {}

    BufferImageCopy& SetBufferExtent(DeviceSize aBufferOffset, uint32_t aBufferRowLength, uint32_t aBufferImageHeight)
    {
        bufferOffset        = aBufferOffset;
        bufferRowLength     = aBufferRowLength;
        bufferImageHeight   = aBufferImageHeight;

        return *this;
    }

    BufferImageCopy& SetImageExtent(const ImageSubresourceLayers& aImageSubresource, const Offset3D& aImageOffset, const Extent3D& aImageExtent)
    {
        imageSubresource    = aImageSubresource;
        imageOffset         = aImageOffset;
        imageExtent         = aImageExtent;

        return *this;
    }
};

ConsistencyCheck(BufferImageCopy, bufferOffset, bufferRowLength, bufferImageHeight, imageSubresource, imageOffset, imageExtent)



class CommandBuffer : public internal::VkTrait<CommandBuffer, VkCommandBuffer>
{
private:
    VkCommandBuffer mCommandBuffer{ VK_NULL_HANDLE };

public:
    CommandBuffer(void) = default;

    CommandBuffer(std::nullptr_t)
    {}

    explicit CommandBuffer(VkCommandBuffer aCommandBuffer) : mCommandBuffer(aCommandBuffer)
    {}

    void Begin(const CommandBufferBeginInfo& aCommandBufferBeginInfo) const
    {
        ThrowIfFailed(vkBeginCommandBuffer(mCommandBuffer, &aCommandBufferBeginInfo));
    }

    void End(void) const
    {
        ThrowIfFailed(vkEndCommandBuffer(mCommandBuffer));
    }

    void Execute(const CommandBuffer& aCommandBuffer) const
    {
        vkCmdExecuteCommands(mCommandBuffer, 1, &aCommandBuffer);
    }

    void Execute(const std::vector<CommandBuffer>& aCommandBuffers) const
    {
        assert(!aCommandBuffers.empty());

        vkCmdExecuteCommands(mCommandBuffer, static_cast<uint32_t>(aCommandBuffers.size()), &aCommandBuffers[0]);
    }

    template <std::size_t C>
    void Execute(const std::array<CommandBuffer, C>& aCommandBuffers) const
    {
        static_assert(!aCommandBuffers.empty());

        vkCmdExecuteCommands(mCommandBuffer, static_cast<uint32_t>(aCommandBuffers.size()), &aCommandBuffers[0]);
    }

    void BeginRenderPass(const RenderPassBeginInfo& aRenderPassBeginInfo, SubpassContents aSubpassContents = SubpassContents::eInline) const
    {
        vkCmdBeginRenderPass(mCommandBuffer, &aRenderPassBeginInfo, static_cast<VkSubpassContents>(aSubpassContents));
    }

    void EndRenderPass(void) const
    {
        vkCmdEndRenderPass(mCommandBuffer);
    }

    void BindGraphicsPipeline(const Pipeline& aPipeline) const
    {
        vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipeline); 
    }

    void BindComputePipeline(const Pipeline& aPipeline) const
    {
        vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, aPipeline);
    }

    void BindGraphicsDescriptorSet(const PipelineLayout& aPipelineLayout, uint32_t aFirstSet, const DescriptorSet& aDescriptorSet,
        uint32_t aDynamicOffsetCount = 0, const uint32_t* apDynamicOffsets = nullptr) const
    {
        vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipelineLayout, aFirstSet, 1, &aDescriptorSet,
            aDynamicOffsetCount, &apDynamicOffsets[0]);
    }

    void BindComputeDescriptorSet(const PipelineLayout& aPipelineLayout, uint32_t aFirstSet, const DescriptorSet& aDescriptorSet,
        uint32_t aDynamicOffsetCount = 0, const uint32_t* apDynamicOffsets = nullptr) const
    {
        vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, aPipelineLayout, aFirstSet, 1, &aDescriptorSet,
            aDynamicOffsetCount, &apDynamicOffsets[0]);
    }

    void BindGraphicsDescriptorSets(const PipelineLayout& aPipelineLayout, uint32_t aFirstSet, uint32_t aDescriptorSetCount, const DescriptorSet* apDescriptorSets,
        uint32_t aDynamicOffsetCount = 0, const uint32_t* apDynamicOffsets = nullptr) const
    {
        vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipelineLayout, aFirstSet, aDescriptorSetCount, &apDescriptorSets[0],
            aDynamicOffsetCount, &apDynamicOffsets[0]);
    }

    void BindComputeDescriptorSets(const PipelineLayout& aPipelineLayout, uint32_t aFirstSet, uint32_t aDescriptorSetCount, const DescriptorSet* apDescriptorSets,
        uint32_t aDynamicOffsetCount = 0, const uint32_t* apDynamicOffsets = nullptr) const
    {
        vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, aPipelineLayout, aFirstSet, aDescriptorSetCount, &apDescriptorSets[0],
            aDynamicOffsetCount, &apDynamicOffsets[0]);
    }

    void PushConstants(const PipelineLayout& aPipelineLayout, const ShaderStageFlags& aShaderStageFlags, uint32_t aOffset, uint32_t aSize, const void* apValues) const
    {
        assert(aSize > 0 && apValues != nullptr);

        vkCmdPushConstants(mCommandBuffer, aPipelineLayout, aShaderStageFlags, aOffset, aSize, apValues);
    }

    void Draw(uint32_t aVertexCount, uint32_t aInstanceCount = 1, uint32_t aFirstVertex = 0, uint32_t aFirstInstance = 0) const
    {
        vkCmdDraw(mCommandBuffer, aVertexCount, aInstanceCount, aFirstVertex, aFirstInstance);
    }

    void DrawIndexed(uint32_t aIndexCount, uint32_t aInstanceCount = 1, uint32_t aFirstIndex = 0, uint32_t aVertexOffset = 0, uint32_t aFirstInstance = 0) const
    {
        vkCmdDrawIndexed(mCommandBuffer, aIndexCount, aInstanceCount, aFirstIndex, aVertexOffset, aFirstInstance);
    }

    void PipelineBarrier(const PipelineStageFlags& aSrcStageMask, const PipelineStageFlags& aDstStageMask, const DependencyFlags& aDependencyFlags,
        const std::vector<MemoryBarrier>& aMemoryBarriers, const std::vector<BufferMemoryBarrier>& aBufferMemoryBarriers, const std::vector<ImageMemoryBarrier>& aImageMemoryBarriers) const
    {
        assert(!(aMemoryBarriers.empty() || aBufferMemoryBarriers.empty() || aImageMemoryBarriers.empty()));

        vkCmdPipelineBarrier(mCommandBuffer, aSrcStageMask, aDstStageMask, aDependencyFlags,
            static_cast<uint32_t>(aMemoryBarriers.size()), &aMemoryBarriers[0],
            static_cast<uint32_t>(aBufferMemoryBarriers.size()), &aBufferMemoryBarriers[0],
            static_cast<uint32_t>(aImageMemoryBarriers.size()), &aImageMemoryBarriers[0]);
    }

     void PipelineBarrier(const CommandPipelineBarrier& aCmdPipelineBarrier) const
     {
         vkCmdPipelineBarrier(mCommandBuffer, aCmdPipelineBarrier.srcStageMask, aCmdPipelineBarrier.dstStageMask, aCmdPipelineBarrier.dependencyFlags,
             aCmdPipelineBarrier.memoryBarrierCount, reinterpret_cast<const VkMemoryBarrier*>(aCmdPipelineBarrier.pMemoryBarriers),
             aCmdPipelineBarrier.bufferMemoryBarrierCount, reinterpret_cast<const VkBufferMemoryBarrier*>(aCmdPipelineBarrier.pBufferMemoryBarriers),
             aCmdPipelineBarrier.imageMemoryBarrierCount, reinterpret_cast<const VkImageMemoryBarrier*>(aCmdPipelineBarrier.pImageMemoryBarriers));
     }

    void ClearColorImage(const Image& aImage, ImageLayout aImageLayout, const ClearColorValue& aClearColor, const std::vector<ImageSubresourceRange>& aRanges) const
    {
        assert(!aRanges.empty());

        vkCmdClearColorImage(mCommandBuffer, aImage, static_cast<VkImageLayout>(aImageLayout), &aClearColor, static_cast<uint32_t>(aRanges.size()), &aRanges[0]);
    }

    void SetViewport(const Viewport& aViewport, uint32_t aFirstViewport = 0) const
    {
        vkCmdSetViewport(mCommandBuffer, aFirstViewport, 1, &aViewport);
    }

    void SetViewports(uint32_t aFirstViewport, uint32_t aViewportCount, const Viewport* apViewports) const
    {
        assert(aViewportCount != 0 && apViewports != nullptr);

        vkCmdSetViewport(mCommandBuffer, aFirstViewport, aViewportCount, &apViewports[0]);
    }

    void SetViewports(uint32_t aFirstViewport, const std::vector<Viewport>& aViewports) const
    {
        return SetViewports(aFirstViewport, static_cast<uint32_t>(aViewports.size()), aViewports.data());
    }

    template <std::size_t V>
    void SetViewports(uint32_t aFirstViewport, const std::array<Viewport, V>& aViewports) const
    {
        static_assert(!aViewports.empty());

        return SetViewports(aFirstViewport, static_cast<uint32_t>(aViewports.size()), aViewports.data());
    }

    void SetScissor(const Rect2D& aScissor, uint32_t aFirstScissor = 0) const
    {
        vkCmdSetScissor(mCommandBuffer, aFirstScissor, 1, &aScissor);
    }

    void SetScissors(uint32_t aFirstScissor, uint32_t aScissorCount, const Rect2D* apScissors) const
    {
        assert(aScissorCount != 0 && apScissors != nullptr);

        vkCmdSetScissor(mCommandBuffer, aFirstScissor, aScissorCount, &apScissors[0]);
    }

    void SetScissors(uint32_t aFirstScissor, const std::vector<Rect2D>& aScissors) const
    {
        return SetScissors(aFirstScissor, static_cast<uint32_t>(aScissors.size()), aScissors.data());
    }

    template <std::size_t S>
    void SetScissors(uint32_t aFirstScissor, const std::array<Rect2D, S>& aScissors) const
    {
        static_assert(!aScissors.empty());

        return SetScissors(aFirstScissor, static_cast<uint32_t>(aScissors.size()), aScissors.data());
    }

    void BindVertexBuffer(const Buffer& aBuffer, const DeviceSize& aOffset) const
    {
        vkCmdBindVertexBuffers(mCommandBuffer, 0, 1, &aBuffer, &aOffset);
    }

    void BindVertexBuffers(uint32_t aFirstBinding, uint32_t aBindingCount, const Buffer* apBuffers, const DeviceSize* apOffsets) const
    {
        assert(aBindingCount != 0 && apBuffers != nullptr && apOffsets != nullptr);

        vkCmdBindVertexBuffers(mCommandBuffer, aFirstBinding, aBindingCount, &apBuffers[0], &apOffsets[0]);
    }

    void BindVertexBuffers(uint32_t aFirstBinding, const std::vector<Buffer>& aBuffers, const std::vector<DeviceSize>& aOffsets) const
    {
        assert(aBuffers.size() == aOffsets.size());

        BindVertexBuffers(aFirstBinding, static_cast<uint32_t>(aBuffers.size()), aBuffers.data(), aOffsets.data());
    }

    template <std::size_t B>
    void BindVertexBuffers(uint32_t aFirstBinding, const std::array<Buffer, B>& aBuffers, const std::array<DeviceSize, B>& aOffsets) const
    {
        static_assert(!aBuffers.empty());

        BindVertexBuffers(aFirstBinding, static_cast<uint32_t>(aBuffers.size()), aBuffers.data(), aOffsets.data());
    }

    void BindIndexBuffer(const Buffer& aBuffer, DeviceSize aOffset, IndexType aIndexType) const
    {
        vkCmdBindIndexBuffer(mCommandBuffer, aBuffer, aOffset, static_cast<VkIndexType>(aIndexType));
    }

    // Copy Data Between Buffers
    void Copy(Buffer& aDstBuffer, const Buffer& aSrcBuffer, const BufferCopy& aRegion) const
    {
        vkCmdCopyBuffer(mCommandBuffer, aSrcBuffer, aDstBuffer, 1, &aRegion);
    }

    void Copy(Buffer& aDstBuffer, const Buffer& aSrcBuffer, const std::vector<BufferCopy>& aRegions) const
    {
        assert(!aRegions.empty());

        vkCmdCopyBuffer(mCommandBuffer, aSrcBuffer, aDstBuffer, static_cast<uint32_t>(aRegions.size()), &aRegions[0]);
    }

    template <std::size_t R>
    void Copy(Buffer& aDstBuffer, const Buffer& aSrcBuffer, const std::array<BufferCopy, R>& aRegions) const
    {
        static_assert(!aRegions.empty());

        vkCmdCopyBuffer(mCommandBuffer, aSrcBuffer, aDstBuffer, static_cast<uint32_t>(aRegions.size()), &aRegions[0]);
    }

    // Copy Data Between Images
    void Copy(Image& aDstImage, ImageLayout aDstImageLayout, const Image& aSrcImage, ImageLayout aSrcImageLayout, const ImageCopy& aRegion) const
    {
        assert(vkpp::ImageLayout::eTransferDstOptimal == aDstImageLayout || vkpp::ImageLayout::eGeneral == aDstImageLayout);
        assert(vkpp::ImageLayout::eTransferSrcOptimal == aSrcImageLayout || vkpp::ImageLayout::eGeneral == aSrcImageLayout);

        vkCmdCopyImage(mCommandBuffer, aSrcImage, static_cast<VkImageLayout>(aSrcImageLayout), aDstImage, static_cast<VkImageLayout>(aDstImageLayout), 1, &aRegion);
    }

    void Copy(Image& aDstImage, ImageLayout aDstImageLayout, const Image& aSrcImage, ImageLayout aSrcImageLayout, const std::vector<ImageCopy>& aRegions) const
    {
        assert(!aRegions.empty());
        assert(vkpp::ImageLayout::eTransferDstOptimal == aDstImageLayout || vkpp::ImageLayout::eGeneral == aDstImageLayout);
        assert(vkpp::ImageLayout::eTransferSrcOptimal == aSrcImageLayout || vkpp::ImageLayout::eGeneral == aSrcImageLayout);

        vkCmdCopyImage(mCommandBuffer, aSrcImage, static_cast<VkImageLayout>(aSrcImageLayout), aDstImage, static_cast<VkImageLayout>(aDstImageLayout), static_cast<uint32_t>(aRegions.size()), &aRegions[0]);
    }

    template <std::size_t R>
    void Copy(Image& aDstImage, ImageLayout aDstImageLayout, const Image& aSrcImage, ImageLayout aSrcImageLayout, const std::array<ImageCopy, R>& aRegions) const
    {
        static_assert(!aRegions.empty());
        assert(vkpp::ImageLayout::eTransferDstOptimal == aDstImageLayout || vkpp::ImageLayout::eGeneral == aDstImageLayout);
        assert(vkpp::ImageLayout::eTransferSrcOptimal == aSrcImageLayout || vkpp::ImageLayout::eGeneral == aSrcImageLayout);

        vkCmdCopyImage(mCommandBuffer, aSrcImage, static_cast<VkImageLayout>(aSrcImageLayout), aDstImage, static_cast<VkImageLayout>(aDstImageLayout), static_cast<uint32_t>(aRegions.size()), &aRegions[0]);
    }

    // Copy Data From Buffers to Images
    void Copy(Image& aDstImage, ImageLayout aDstImageLayout, const Buffer& aSrcBuffer, const BufferImageCopy& aRegion) const
    {
        assert(vkpp::ImageLayout::eTransferDstOptimal == aDstImageLayout || vkpp::ImageLayout::eGeneral == aDstImageLayout);

        vkCmdCopyBufferToImage(mCommandBuffer, aSrcBuffer, aDstImage, static_cast<VkImageLayout>(aDstImageLayout), 1, &aRegion);
    }

    void Copy(Image& aDstImage, ImageLayout aDstImageLayout, const Buffer& aSrcBuffer, const std::vector<BufferImageCopy>& aRegions) const
    {
        assert(vkpp::ImageLayout::eTransferDstOptimal == aDstImageLayout || vkpp::ImageLayout::eGeneral == aDstImageLayout);
        assert(!aRegions.empty());

        vkCmdCopyBufferToImage(mCommandBuffer, aSrcBuffer, aDstImage, static_cast<VkImageLayout>(aDstImageLayout), static_cast<uint32_t>(aRegions.size()), &aRegions[0]);
    }

    template <std::size_t R>
    void Copy(Image& aDstImage, ImageLayout aDstImageLayout, const Buffer& aSrcBuffer, const std::array<BufferImageCopy, R>& aRegions) const
    {
        assert(vkpp::ImageLayout::eTransferDstOptimal == aDstImageLayout || vkpp::ImageLayout::eGeneral == aDstImageLayout);
        static_assert(!aRegions.empty());

        vkCmdCopyBufferToImage(mCommandBuffer, aSrcBuffer, aDstImage, static_cast<VkImageLayout>(aDstImageLayout), static_cast<uint32_t>(aRegions.size()), &aRegions[0]);
    }

    // Copy Data From Images to Buffers
    void Copy(Buffer& aDstBuffer, const Image& aSrcImage, ImageLayout aSrcImageLayout, const BufferImageCopy& aRegion) const
    {
        assert(vkpp::ImageLayout::eTransferSrcOptimal == aSrcImageLayout || vkpp::ImageLayout::eGeneral == aSrcImageLayout);

        vkCmdCopyImageToBuffer(mCommandBuffer, aSrcImage, static_cast<VkImageLayout>(aSrcImageLayout), aDstBuffer, 1, &aRegion);
    }

    void Copy(Buffer& aDstBuffer, const Image& aSrcImage, ImageLayout aSrcImageLayout, const std::vector<BufferImageCopy>& aRegions) const
    {
        assert(vkpp::ImageLayout::eTransferSrcOptimal == aSrcImageLayout || vkpp::ImageLayout::eGeneral == aSrcImageLayout);
        assert(!aRegions.empty());

        vkCmdCopyImageToBuffer(mCommandBuffer, aSrcImage, static_cast<VkImageLayout>(aSrcImageLayout), aDstBuffer, static_cast<uint32_t>(aRegions.size()), &aRegions[0]);
    }

    template <std::size_t R>
    void Copy(Buffer& aDstBuffer, const Image& aSrcImage, ImageLayout aSrcImageLayout, const std::array<BufferImageCopy, R>& aRegions) const
    {
        assert(vkpp::ImageLayout::eTransferSrcOptimal == aSrcImageLayout || vkpp::ImageLayout::eGeneral == aSrcImageLayout);
        static_assert(!aRegions.empty());

        vkCmdCopyImageToBuffer(mCommandBuffer, aSrcImage, static_cast<VkImageLayout>(aSrcImageLayout), aDstBuffer, static_cast<uint32_t>(aRegions.size()), &aRegions[0]);
    }

    // Image Copies with Scaling
    void Blit(Image& aDstImage, ImageLayout aDstImageLayout, const Image& aSrcImage, ImageLayout aSrcImageLayout, const ImageBlit& aRegion, Filter aFilter) const
    {
        assert(vkpp::ImageLayout::eTransferDstOptimal == aDstImageLayout || vkpp::ImageLayout::eGeneral == aDstImageLayout);
        assert(vkpp::ImageLayout::eTransferSrcOptimal == aSrcImageLayout || vkpp::ImageLayout::eGeneral == aSrcImageLayout);

        vkCmdBlitImage(mCommandBuffer, aSrcImage, static_cast<VkImageLayout>(aSrcImageLayout), aDstImage, static_cast<VkImageLayout>(aDstImageLayout), 1, &aRegion, static_cast<VkFilter>(aFilter));
    }

    void Blit(Image& aDstImage, ImageLayout aDstImageLayout, const Image& aSrcImage, ImageLayout aSrcImageLayout, const std::vector<ImageBlit>& aRegions, Filter aFilter) const
    {
        assert(!aRegions.empty());
        assert(vkpp::ImageLayout::eTransferDstOptimal == aDstImageLayout || vkpp::ImageLayout::eGeneral == aDstImageLayout);
        assert(vkpp::ImageLayout::eTransferSrcOptimal == aSrcImageLayout || vkpp::ImageLayout::eGeneral == aSrcImageLayout);

        vkCmdBlitImage(mCommandBuffer, aSrcImage, static_cast<VkImageLayout>(aSrcImageLayout), aDstImage, static_cast<VkImageLayout>(aDstImageLayout), static_cast<uint32_t>(aRegions.size()), &aRegions[0], static_cast<VkFilter>(aFilter));
    }

    template <std::size_t R>
    void Blit(Image& aDstImage, ImageLayout aDstImageLayout, const Image& aSrcImage, ImageLayout aSrcImageLayout, const std::array<ImageBlit, R>& aRegions, Filter aFilter) const
    {
        static_assert(!aRegions.empty());
        assert(vkpp::ImageLayout::eTransferDstOptimal == aDstImageLayout || vkpp::ImageLayout::eGeneral == aDstImageLayout);
        assert(vkpp::ImageLayout::eTransferSrcOptimal == aSrcImageLayout || vkpp::ImageLayout::eGeneral == aSrcImageLayout);

        vkCmdBlitImage(mCommandBuffer, aSrcImage, static_cast<VkImageLayout>(aSrcImageLayout), aDstImage, static_cast<VkImageLayout>(aDstImageLayout), static_cast<uint32_t>(aRegions.size()), &aRegions[0], static_cast<VkFilter>(aFilter));
    }
};

StaticSizeCheck(CommandBuffer)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_COMMAND_BUFFER_H__
