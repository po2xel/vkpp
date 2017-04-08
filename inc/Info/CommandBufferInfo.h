#ifndef __VKPP_INFO_COMMAND_BUFFER_INFO_H__
#define __VKPP_INFO_COMMAND_BUFFER_INFO_H__



#include <Info/Common.h>
#include <Info/Flags.h>
#include <Type/RenderPass.h>
#include <Type/FrameBuffer.h>



namespace vkpp
{



enum class CommandBufferUsageFlagBits
{
    eOneTimeSubmit      = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    eRenderPassContinue = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
    eSimultaneousUse    = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
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

    CommandBufferBeginInfo(const CommandBufferUsageFlags& aFlags)
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



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_COMMAND_BUFFER_INFO_H__
