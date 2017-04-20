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

using CommandBufferUsageFlags = internal::Flags<CommandBufferUsageFlagBits, VkCommandBufferUsageFlags>;



enum class QueryControlFlagBits
{
    ePrecise        = VK_QUERY_CONTROL_PRECISE_BIT
};

using QueryControlFlags = internal::Flags<QueryControlFlagBits, VkQueryControlFlags>;



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

using QueryPipelineStatisticFlags = internal::Flags<QueryPipelineStatisticFlagBits, VkQueryPipelineStatisticFlags>;



class CommandBufferInheritanceInfo : public internal::VkTrait<CommandBufferInheritanceInfo, VkCommandBufferInheritanceInfo>
{
private:
    const internal::Structure sType = internal::Structure::eCommandBufferInheritance;

public:
    const void*                 pNext{ nullptr };
    RenderPass                  renderPass;
    uint32_t                    subpass{ 0 };
    FrameBuffer                 frameBuffer;
    Bool32                      occlusionQueryEnable{ VK_FALSE };
    QueryControlFlags           queryFlags;
    QueryPipelineStatisticFlags pipelineStatistics;

    DEFINE_CLASS_MEMBER(CommandBufferInheritanceInfo)

    CommandBufferInheritanceInfo(const RenderPass& aRenderPass, uint32_t aSubpass, const FrameBuffer& aFrameBuffer, Bool32 aOcclusionQueryEnable,
        QueryControlFlags aQueryFlags = QueryControlFlags(), QueryPipelineStatisticFlags aPipelineStatistics = QueryPipelineStatisticFlags())
        : renderPass(aRenderPass), subpass(aSubpass), frameBuffer(aFrameBuffer), occlusionQueryEnable(aOcclusionQueryEnable),
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

    CommandBufferInheritanceInfo& SetFrameBuffer(const FrameBuffer& aFrameBuffer)
    {
        frameBuffer = aFrameBuffer;

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

StaticSizeCheck(CommandBufferInheritanceInfo);



class CommandBufferBeginInfo : public internal::VkTrait<CommandBufferBeginInfo, VkCommandBufferBeginInfo>
{
private:
    const internal::Structure sType = internal::Structure::eCommandBufferBegin;

public:
    const void*                         pNext{ nullptr };
    CommandBufferUsageFlags             flags;
    const CommandBufferInheritanceInfo* pInheritanceInfo{ nullptr };

    DEFINE_CLASS_MEMBER(CommandBufferBeginInfo)

    explicit CommandBufferBeginInfo(const CommandBufferUsageFlags& aFlags)
    : flags(aFlags)
    {}

    CommandBufferBeginInfo(const CommandBufferUsageFlags& aFlags, const CommandBufferInheritanceInfo& aInheritanceInfo)
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

StaticSizeCheck(CommandBufferBeginInfo);



struct CommandPipelineBarrier
{
    PipelineStageFlags          srcStageMask;
    PipelineStageFlags          dstStageMask;
    DependencyFlags             dependencyFlags;
    uint32_t                    memoryBarrierCount{ 0 };
    const MemoryBarrier*        pMemoryBarriers{ nullptr };
    uint32_t                    bufferMemoryBarrierCount{ 0 };
    const BufferMemoryBarrier*  pBufferMemoryBuffers{ nullptr };
    uint32_t                    imageMemoryBarrierCount{ 0 };
    const ImageMemoryBarrier*   pImageMemoryBarriers{ nullptr };
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

    void BeginRenderPass(const RenderPassBeginInfo& aRenderPassBeginInfo, SubpassContents aSubpassContents) const
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

    void Draw(uint32_t aVertexCount, uint32_t aInstanceCount, uint32_t aFirstVertex = 0, uint32_t aFirstInstance = 0) const
    {
        vkCmdDraw(mCommandBuffer, aVertexCount, aInstanceCount, aFirstVertex, aFirstInstance);
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
             aCmdPipelineBarrier.bufferMemoryBarrierCount, reinterpret_cast<const VkBufferMemoryBarrier*>(aCmdPipelineBarrier.pBufferMemoryBuffers),
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
        BindVertexBuffers(aFirstBinding, static_cast<uint32_t>(aBuffers.size()), aBuffers.data(), aOffsets.data());
    }
};



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_COMMAND_BUFFER_H__
