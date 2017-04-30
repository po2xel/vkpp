#ifndef __VKPP_INFO_PIPELINE_STAGE_H__
#define __VKPP_INFO_PIPELINE_STAGE_H__



#include <array>

#include <Info/Common.h>
#include <Info/Flags.h>
#include <Info/Format.h>

#include <Type/ShaderModule.h>
#include <Type/DescriptorSet.h>



namespace vkpp
{



enum class PipelineStageFlagBits
{
    eTopOfPipe                      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    eDrawIndirect                   = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
    eVertexInput                    = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
    eVertexShader                   = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
    eTessellationControlShader      = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT,
    eTessellationEvaluationShader   = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT,
    eGeometryShader                 = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT,
    eFragmentShader                 = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
    eEarlyFragmentTests             = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
    eLateFragementTests             = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
    eColorAttachmentOutput          = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    eComputeShader                  = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    eTransfer                       = VK_PIPELINE_STAGE_TRANSFER_BIT,
    eBottomOfPipe                   = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    eHost                           = VK_PIPELINE_STAGE_HOST_BIT,
    eAllGraphics                    = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
    eAllCommands                    = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
    eCommandProcessNVX              = VK_PIPELINE_STAGE_COMMAND_PROCESS_BIT_NVX
};

using PipelineStageFlags = internal::Flags<PipelineStageFlagBits, VkPipelineStageFlags>;



enum class PipelineShaderStageCreateFlagBits
{};

using PipelineShaderStageCreateFlags = internal::Flags<PipelineShaderStageCreateFlagBits, VkPipelineShaderStageCreateFlags>;



constexpr const char* DefaultShaderEntryName = "main";



class PipelineShaderStageCreateInfo : public internal::VkTrait<PipelineShaderStageCreateInfo, VkPipelineShaderStageCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::ePipelineShaderStage;

public:
    const void*                     pNext{ nullptr };
    PipelineShaderStageCreateFlags  flags;
    ShaderStageFlagBits             stage;
    ShaderModule                    module;
    const char*                     pName{ DefaultShaderEntryName };
    const SpecializationInfo*       pSpecializationInfo{ nullptr };

    DEFINE_CLASS_MEMBER(PipelineShaderStageCreateInfo)

    PipelineShaderStageCreateInfo(ShaderStageFlagBits aStage, const ShaderModule& aModule, const char* apName = DefaultShaderEntryName,
        const SpecializationInfo* apSpecializationInfo = nullptr, const PipelineShaderStageCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), stage(aStage), module(aModule), pName(apName), pSpecializationInfo(apSpecializationInfo)
    {}

    PipelineShaderStageCreateInfo& SetFlags(const PipelineShaderStageCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    PipelineShaderStageCreateInfo& SetShaderStage(ShaderStageFlagBits aStage, const ShaderModule& aModule, const char* apName = DefaultShaderEntryName)
    {
        stage   = aStage;
        module  = aModule;
        pName   = apName;

        return *this;
    }

    PipelineShaderStageCreateInfo& SetSpecializationInfo(const SpecializationInfo* apSpecializationInfo)
    {
        pSpecializationInfo = apSpecializationInfo;

        return *this;
    }

    PipelineShaderStageCreateInfo& SetSpecializationInfo(const SpecializationInfo& aSpecializationInfo)
    {
        return SetSpecializationInfo(aSpecializationInfo.AddressOf());
    }
};



enum class PipelineVertexInputStateCreateFlagBits
{};

using PipelineVertexInputStateCreateFlags = internal::Flags<PipelineVertexInputStateCreateFlagBits, VkPipelineVertexInputStateCreateFlags>;



enum class VertexInputRate
{
    eVertex     = VK_VERTEX_INPUT_RATE_VERTEX,
    eInstance   = VK_VERTEX_INPUT_RATE_INSTANCE
};



struct VertexInputBindingDescription : public internal::VkTrait<VertexInputBindingDescription, VkVertexInputBindingDescription>
{
    uint32_t            binding{ 0 };
    uint32_t            stride{ 0 };
    VertexInputRate     inputRate{ VertexInputRate::eVertex };

    DEFINE_CLASS_MEMBER(VertexInputBindingDescription)

    VertexInputBindingDescription(uint32_t aBinding, uint32_t aStride, VertexInputRate aInputRate)
        : binding(aBinding), stride(aStride), inputRate(aInputRate)
    {}

    VertexInputBindingDescription& SetBinding(uint32_t aBinding)
    {
        binding = aBinding;

        return *this;
    }

    VertexInputBindingDescription& SetStride(uint32_t aStride)
    {
        stride = aStride;

        return *this;
    }

    VertexInputBindingDescription& SetInputRate(VertexInputRate aInputRate)
    {
        inputRate = aInputRate;

        return *this;
    }
};

ConsistencyCheck(VertexInputBindingDescription, binding, stride, inputRate)



struct VertexInputAttributeDescription : public internal::VkTrait<VertexInputAttributeDescription, VkVertexInputAttributeDescription>
{
    uint32_t        location{ 0 };
    uint32_t        binding{ 0 };
    Format          format{ Format::eUndefined };
    uint32_t        offset{ 0 };

    DEFINE_CLASS_MEMBER(VertexInputAttributeDescription)

    VertexInputAttributeDescription(uint32_t aLocation, uint32_t aBinding, Format aFormat, uint32_t aOffset)
        : location(aLocation), binding(aBinding), format(aFormat), offset(aOffset)
    {}

    VertexInputAttributeDescription& SetLocation(uint32_t aLocation)
    {
        location = aLocation;

        return *this;
    }

    VertexInputAttributeDescription& SetBinding(uint32_t aBinding)
    {
        binding = aBinding;

        return *this;
    }

    VertexInputAttributeDescription& SetFormat(Format aFormat)
    {
        format = aFormat;

        return *this;
    }

    VertexInputAttributeDescription& SetOffset(uint32_t aOffset)
    {
        offset = aOffset;

        return *this;
    }
};

ConsistencyCheck(VertexInputAttributeDescription, location, binding, format, offset)



class PipelineVertexInputStateCreateInfo : public internal::VkTrait<PipelineVertexInputStateCreateInfo, VkPipelineVertexInputStateCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::ePipelineVertexInputState;

public:
    const void*                             pNext{ nullptr };
    PipelineVertexInputStateCreateFlags     flags;
    uint32_t                                vertexBindingDescriptionCount{ 0 };
    const VertexInputBindingDescription*    pVertexBindingDescriptions{ nullptr };
    uint32_t                                vertexAttributeDescriptionCount{ 0 };
    const VertexInputAttributeDescription*  pVertexAttributeDescriptions{ nullptr };

    DEFINE_CLASS_MEMBER(PipelineVertexInputStateCreateInfo)

    PipelineVertexInputStateCreateInfo(uint32_t aVertexBindingDescriptionCount, const VertexInputBindingDescription* apVertexBindingDescriptions,
        uint32_t aVertexAttributeDescriptionCount, const VertexInputAttributeDescription* apVertexAttributeDescriptions, const PipelineVertexInputStateCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), vertexBindingDescriptionCount(aVertexBindingDescriptionCount), pVertexBindingDescriptions(apVertexBindingDescriptions),
          vertexAttributeDescriptionCount(aVertexAttributeDescriptionCount), pVertexAttributeDescriptions(apVertexAttributeDescriptions)
    {}

    PipelineVertexInputStateCreateInfo(const std::vector<VertexInputBindingDescription>& aVertexBindingDescriptions,
        const std::vector<VertexInputAttributeDescription>& aVertexAttributeDescriptions, const PipelineVertexInputStateCreateFlags& aFlags = DefaultFlags)
        : PipelineVertexInputStateCreateInfo(static_cast<uint32_t>(aVertexBindingDescriptions.size()), aVertexBindingDescriptions.data(),
          static_cast<uint32_t>(aVertexAttributeDescriptions.size()), aVertexAttributeDescriptions.data(), aFlags)
    {}

    template <std::size_t B, std::size_t A>
    PipelineVertexInputStateCreateInfo(const std::array<VertexInputBindingDescription, B>& aVertexBindingDescriptions,
        const std::array<VertexInputAttributeDescription, A>& aVertexAttributeDescriptions, const PipelineVertexInputStateCreateFlags& aFlags = DefaultFlags)
        : PipelineVertexInputStateCreateInfo(static_cast<uint32_t>(aVertexBindingDescriptions.size()), aVertexBindingDescriptions.data(),
          static_cast<uint32_t>(aVertexAttributeDescriptions.size()), aVertexAttributeDescriptions.data(), aFlags)
    {}

    PipelineVertexInputStateCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    PipelineVertexInputStateCreateInfo& SetFlags(const PipelineVertexInputStateCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    PipelineVertexInputStateCreateInfo& SetVertexBindingDescriptions(uint32_t aVertexBindingDescriptionCount, const VertexInputBindingDescription* apVertexBindingDescriptions)
    {
        vertexBindingDescriptionCount   = aVertexBindingDescriptionCount;
        pVertexBindingDescriptions      = apVertexBindingDescriptions;

        return *this;
    }

    PipelineVertexInputStateCreateInfo& SetVertexBindingDescriptions(const std::vector<VertexInputBindingDescription>& aVertexBindingDescriptions)
    {
        return SetVertexBindingDescriptions(static_cast<uint32_t>(aVertexBindingDescriptions.size()), aVertexBindingDescriptions.data());
    }

    template <std::size_t B>
    PipelineVertexInputStateCreateInfo& SetVertexBindingDescriptions(const std::array<VertexInputBindingDescription, B>& aVertexBindingDescriptions)
    {
        return SetVertexBindingDescriptions(static_cast<uint32_t>(aVertexBindingDescriptions.size()), aVertexBindingDescriptions.data());
    }

    PipelineVertexInputStateCreateInfo& SetVertexAttributeDescriptions(uint32_t aVertexAttributeDescriptionCount, const VertexInputAttributeDescription* apVertexAttributeDescriptions)
    {
        vertexAttributeDescriptionCount = aVertexAttributeDescriptionCount;
        pVertexAttributeDescriptions    = apVertexAttributeDescriptions;

        return *this;
    }

    PipelineVertexInputStateCreateInfo& SetVertexAttributeDescriptions(const std::vector<VertexInputAttributeDescription>& aVertexAttributeDescriptions)
    {
        return SetVertexAttributeDescriptions(static_cast<uint32_t>(aVertexAttributeDescriptions.size()), aVertexAttributeDescriptions.data());
    }

    template <std::size_t A>
    PipelineVertexInputStateCreateInfo& SetVertexAttributeDescriptions(const std::array<VertexInputAttributeDescription, A>& aVertexAttributeDescriptions)
    {
        return SetVertexAttributeDescriptions(static_cast<uint32_t>(aVertexAttributeDescriptions.size()), aVertexAttributeDescriptions.data());
    }
};

ConsistencyCheck(PipelineVertexInputStateCreateInfo, pNext, flags, vertexBindingDescriptionCount, pVertexBindingDescriptions, vertexAttributeDescriptionCount, pVertexAttributeDescriptions)



enum class PipelineInputAssemblyStateCreateFlagBits
{};

using PipelineInputAssemblyStateCreateFlags = internal::Flags<PipelineInputAssemblyStateCreateFlagBits, VkPipelineInputAssemblyStateCreateFlags>;



enum class PrimitiveTopology
{
    ePointList                  = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    eLineList                   = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    eLineStrip                  = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    eTriangleList               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    eTriangleStrip              = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
    eTriangleFan                = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
    eLineListWithAdjacency      = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
    eLineStripWithAdjacency     = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
    eTriangleListWithAdjacency  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
    eTriangleStripWithAdjacency = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
    ePatchList                  = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST
};



class PipelineInputAssemblyStateCreateInfo : public internal::VkTrait<PipelineInputAssemblyStateCreateInfo, VkPipelineInputAssemblyStateCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::ePipelineInputAssemblyState;

public:
    const void*                             pNext{ nullptr };
    PipelineInputAssemblyStateCreateFlags   flags;
    PrimitiveTopology                       topology{ PrimitiveTopology::ePointList };
    Bool32                                  primitiveRestartEnable{ VK_FALSE };

    DEFINE_CLASS_MEMBER(PipelineInputAssemblyStateCreateInfo)

    explicit PipelineInputAssemblyStateCreateInfo(PrimitiveTopology aTopology, Bool32 aPrimitiveRestartEnable = VK_FALSE, const PipelineInputAssemblyStateCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), topology(aTopology), primitiveRestartEnable(aPrimitiveRestartEnable)
    {}

    PipelineInputAssemblyStateCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    PipelineInputAssemblyStateCreateInfo& SetTopology(PrimitiveTopology aTopology, Bool32 aPrimitiveRestartEnable = VK_FALSE)
    {
        topology                = aTopology;
        primitiveRestartEnable  = aPrimitiveRestartEnable;

        return *this;
    }
};

ConsistencyCheck(PipelineInputAssemblyStateCreateInfo, pNext, flags, topology, primitiveRestartEnable)



enum class PipelineTessellationStateCreateFlagBits
{};

using PipelineTessellationStateCreateFlags = internal::Flags<PipelineTessellationStateCreateFlagBits, VkPipelineTessellationStateCreateFlags>;



class PipelineTessellationStateCreateInfo : public internal::VkTrait<PipelineTessellationStateCreateInfo, VkPipelineTessellationStateCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::ePipelineTessellationState;

public:
    const void*                             pNext{ nullptr };
    PipelineTessellationStateCreateFlags    flags;
    uint32_t                                patchControlPoints{ 0 };

    DEFINE_CLASS_MEMBER(PipelineTessellationStateCreateInfo)

    PipelineTessellationStateCreateInfo(uint32_t aPatchControlPoints, const PipelineTessellationStateCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), patchControlPoints(aPatchControlPoints)
    {}

    PipelineTessellationStateCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    PipelineTessellationStateCreateInfo& SetFlags(const PipelineTessellationStateCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    PipelineTessellationStateCreateInfo& SetPatchControlPoints(uint32_t aPatchControlPoints)
    {
        patchControlPoints = aPatchControlPoints;

        return *this;
    }
};

ConsistencyCheck(PipelineTessellationStateCreateInfo, pNext, flags, patchControlPoints)



enum class PipelineViewportStateCreateFlagBits
{};

using PipelineViewportStateCreateFlags = internal::Flags<PipelineViewportStateCreateFlagBits, VkPipelineViewportStateCreateFlags>;



class PipelineViewportStateCreateInfo : public internal::VkTrait<PipelineViewportStateCreateInfo, VkPipelineViewportStateCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::ePipelineViewportState;

public:
    const void*                         pNext{ nullptr };
    PipelineViewportStateCreateFlags    flags;
    uint32_t                            viewportCount{ 0 };
    const Viewport*                     pViewports{ nullptr };
    uint32_t                            scissorCount{ 0 };
    const Rect2D*                       pScissors{ nullptr };

    DEFINE_CLASS_MEMBER(PipelineViewportStateCreateInfo)

    PipelineViewportStateCreateInfo(uint32_t aViewportCount, const Viewport* apViewports, uint32_t aScissorCount = 0, const Rect2D* apScissors = nullptr,
        const PipelineViewportStateCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), viewportCount(aViewportCount), pViewports(apViewports), scissorCount(aScissorCount), pScissors(apScissors)
    {}

    PipelineViewportStateCreateInfo(const std::vector<Viewport>& aViewports, const std::vector<Rect2D>& aScissors, const PipelineViewportStateCreateFlags& aFlags = DefaultFlags)
        : PipelineViewportStateCreateInfo(static_cast<uint32_t>(aViewports.size()), aViewports.data(), static_cast<uint32_t>(aScissors.size()), aScissors.data(), aFlags)
    {}

    template <std::size_t V, std::size_t S>
    PipelineViewportStateCreateInfo(const std::array<Viewport, V>& aViewports, const std::array<Rect2D, S>& aScissors, const PipelineViewportStateCreateFlags& aFlags = DefaultFlags)
        : PipelineViewportStateCreateInfo(static_cast<uint32_t>(aViewports.size()), aViewports.data(), static_cast<uint32_t>(aScissors.size()), aScissors.data(), aFlags)
    {}

    PipelineViewportStateCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    PipelineViewportStateCreateInfo& SetFlags(const PipelineViewportStateCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    PipelineViewportStateCreateInfo& SetViewports(uint32_t aViewportCount, const Viewport* apViewports)
    {
        viewportCount   = aViewportCount;
        pViewports      = apViewports;

        return *this;
    }

    PipelineViewportStateCreateInfo& SetViewports(const std::vector<Viewport>& aViewports)
    {
        return SetViewports(static_cast<uint32_t>(aViewports.size()), aViewports.data());
    }

    template <std::size_t V>
    PipelineViewportStateCreateInfo& SetViewports(const std::array<Viewport, V>& aViewports)
    {
        return SetViewports(static_cast<uint32_t>(aViewports.size()), aViewports.data());
    }

    PipelineViewportStateCreateInfo& SetScissors(uint32_t aScissorCount, const Rect2D* apScissors)
    {
        scissorCount    = aScissorCount;
        pScissors       = apScissors;

        return *this;
    }

    PipelineViewportStateCreateInfo& SetScissors(const std::vector<Rect2D>& aScissors)
    {
        return SetScissors(static_cast<uint32_t>(aScissors.size()), aScissors.data());
    }

    template <std::size_t S>
    PipelineViewportStateCreateInfo& SetScissors(const std::array<Rect2D, S>& aScissors)
    {
        return SetScissors(static_cast<uint32_t>(aScissors.size()), aScissors.data());
    }
};

ConsistencyCheck(PipelineViewportStateCreateInfo, pNext, flags, viewportCount, pViewports, scissorCount, pScissors)



enum class PipelineRasterizationStateCreateFlagBits
{};

using PipelineRasterizationStateCreateFlags = internal::Flags<PipelineRasterizationStateCreateFlagBits, VkPipelineRasterizationStateCreateFlags>;



enum class PolygonMode
{
    eFill       = VK_POLYGON_MODE_FILL,
    eLine       = VK_POLYGON_MODE_LINE,
    ePoint      = VK_POLYGON_MODE_POINT
};



enum class CullModeFlagBits
{
    eNone           = VK_CULL_MODE_NONE,
    eFront          = VK_CULL_MODE_FRONT_BIT,
    eBack           = VK_CULL_MODE_BACK_BIT,
    eFrontAndBack   = VK_CULL_MODE_FRONT_AND_BACK
};

using CullModeFlags = internal::Flags<CullModeFlagBits, VkCullModeFlags>;



enum class FrontFace
{
    eCounterClockwise   = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    eClockwise          = VK_FRONT_FACE_CLOCKWISE
};



class PipelineRasterizationStateCreateInfo : public internal::VkTrait<PipelineRasterizationStateCreateInfo, VkPipelineRasterizationStateCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::ePipelineRasterizationState;

public:
    const void*                             pNext{ nullptr };
    PipelineRasterizationStateCreateFlags   flags;
    Bool32                                  depthClampEnable{ VK_FALSE };
    Bool32                                  rasterizerDiscardEnable{ VK_FALSE };
    PolygonMode                             polygonMode{ PolygonMode::eFill };
    CullModeFlags                           cullMode;
    FrontFace                               frontFace{ FrontFace::eCounterClockwise };
    Bool32                                  depthBiasEnable{ VK_FALSE };
    float                                   depthBiasConstantFactor{ 0 };
    float                                   depthBiasClamp{ 0 };
    float                                   depthBiasSlopeFactor{ 0 };
    float                                   lineWidth{ 0 };

    DEFINE_CLASS_MEMBER(PipelineRasterizationStateCreateInfo)

    PipelineRasterizationStateCreateInfo(Bool32 aDepthClampEnable, Bool32 aRasterizerDiscardEnable, PolygonMode aPolygonMode, const CullModeFlags& aCullMode, FrontFace aFrontFace,
        Bool32 aDepthBiasEnable, float aDepthBiasConstantFactor, float aDepthBiasClamp, float aDepthBiasSlopeFactor, float aLineWidth, const PipelineRasterizationStateCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), depthClampEnable(aDepthClampEnable), rasterizerDiscardEnable(aRasterizerDiscardEnable), polygonMode(aPolygonMode), cullMode(aCullMode), frontFace(aFrontFace),
        depthBiasEnable(aDepthBiasEnable), depthBiasConstantFactor(aDepthBiasConstantFactor), depthBiasClamp(aDepthBiasClamp), depthBiasSlopeFactor(aDepthBiasSlopeFactor), lineWidth(aLineWidth)
    {}

    PipelineRasterizationStateCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    PipelineRasterizationStateCreateInfo& SetFlags(const PipelineRasterizationStateCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    // TODO: Setters
};

ConsistencyCheck(PipelineRasterizationStateCreateInfo, pNext, flags, depthClampEnable, rasterizerDiscardEnable, polygonMode, cullMode, frontFace,
    depthBiasEnable, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor, lineWidth)



enum class PipelineMultisampleStateCreateFlagBits
{};

using PipelineMultisampleStateCreateFlags = internal::Flags<PipelineMultisampleStateCreateFlagBits, VkPipelineMultisampleStateCreateFlags>;



using SampleMask = uint32_t;



class PipelineMultisampleStateCreateInfo : public internal::VkTrait<PipelineMultisampleStateCreateInfo, VkPipelineMultisampleStateCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::ePipelineMultisampleState;

public:
    const void*                         pNext{ nullptr };
    PipelineMultisampleStateCreateFlags flags;
    SampleCountFlagBits                 rasterizationSamples{ SampleCountFlagBits::e1 };
    Bool32                              sampleShadingEnable{ VK_FALSE };
    float                               minSampleShading{ 0 };
    const SampleMask*                   pSampleMask{ nullptr };
    Bool32                              alphaToCoverageEnable{ VK_FALSE };
    Bool32                              alphaToOneEnable{ VK_FALSE };

    DEFINE_CLASS_MEMBER(PipelineMultisampleStateCreateInfo)

    PipelineMultisampleStateCreateInfo(SampleCountFlagBits aRasterizationSamples, Bool32 aSampleShadingEnable, float aMinSampleShading, const SampleMask* apSampleMask,
        Bool32 aAlphaToCoverageEnable, Bool32 aAlphaToOneEnable, const PipelineMultisampleStateCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), rasterizationSamples(aRasterizationSamples), sampleShadingEnable(aSampleShadingEnable), minSampleShading(aMinSampleShading), pSampleMask(apSampleMask),
          alphaToCoverageEnable(aAlphaToCoverageEnable), alphaToOneEnable(aAlphaToOneEnable)
    {}

    PipelineMultisampleStateCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    PipelineMultisampleStateCreateInfo& SetFlags(const PipelineMultisampleStateCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    // TODO: Setters.
};

ConsistencyCheck(PipelineMultisampleStateCreateInfo, pNext, flags, rasterizationSamples, sampleShadingEnable, minSampleShading, pSampleMask, alphaToCoverageEnable, alphaToOneEnable)



enum class PipelineDepthStencilStateCreateFlagBits
{};

using PipelineDepthStencilStateCreateFlags = internal::Flags<PipelineDepthStencilStateCreateFlagBits, VkPipelineDepthStencilStateCreateFlags>;



enum class StencilOp
{
    eKeep               = VK_STENCIL_OP_KEEP,
    eZero               = VK_STENCIL_OP_ZERO,
    eReplace            = VK_STENCIL_OP_REPLACE,
    eIncrementAndClamp  = VK_STENCIL_OP_INCREMENT_AND_CLAMP,
    eDecrementAndClamp  = VK_STENCIL_OP_DECREMENT_AND_CLAMP,
    eInvert             = VK_STENCIL_OP_INVERT,
    eIncrementAndWrap   = VK_STENCIL_OP_INCREMENT_AND_WRAP,
    eDecrementAndWrap   = VK_STENCIL_OP_DECREMENT_AND_WRAP
};



struct StencilOpState : public internal::VkTrait<StencilOpState, VkStencilOpState>
{
    StencilOp   failOp{ StencilOp::eKeep };
    StencilOp   passOp{ StencilOp::eKeep };
    StencilOp   depthFailOp{ StencilOp::eKeep };
    CompareOp   compareOp{ CompareOp::eNever };
    uint32_t    compareMask{ 0 };
    uint32_t    writeMask{ 0 };
    uint32_t    reference{ 0 };

    DEFINE_CLASS_MEMBER(StencilOpState)

    StencilOpState(StencilOp aFailOp, StencilOp aPassOp, StencilOp aDepthFailOp, CompareOp aCompareOp, uint32_t aCompareMask, uint32_t aWriteMask, uint32_t aReference)
        : failOp(aFailOp), passOp(aPassOp), depthFailOp(aDepthFailOp), compareOp(aCompareOp), compareMask(aCompareMask), writeMask(aWriteMask), reference(aReference)
    {}
};

ConsistencyCheck(StencilOpState, failOp, passOp, depthFailOp, compareOp, compareMask, writeMask, reference)



class PipelineDepthStencilStateCreateInfo : public internal::VkTrait<PipelineDepthStencilStateCreateInfo, VkPipelineDepthStencilStateCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::ePipelineDepthStencilState;

public:
    const void*                             pNext{ nullptr };
    PipelineDepthStencilStateCreateFlags    flags;
    Bool32                                  depthTestEnable{ VK_FALSE };
    Bool32                                  depthWriteEnable{ VK_FALSE };
    CompareOp                               depthCompareOp{ CompareOp::eNever };
    Bool32                                  depthBoundsTestEnable{ VK_FALSE };
    Bool32                                  stencilTestEnable{ VK_FALSE };
    StencilOpState                          front;
    StencilOpState                          back;
    float                                   minDepthBounds{ 0 };
    float                                   maxDepthBounds{ 0 };

    DEFINE_CLASS_MEMBER(PipelineDepthStencilStateCreateInfo)
};

ConsistencyCheck(PipelineDepthStencilStateCreateInfo, pNext, flags, depthTestEnable, depthWriteEnable, depthCompareOp, depthBoundsTestEnable, stencilTestEnable,
    front, back, minDepthBounds, maxDepthBounds)



enum class BlendFactor
{
    eZero                   = VK_BLEND_FACTOR_ZERO,
    eOne                    = VK_BLEND_FACTOR_ONE,
    eSrcColor               = VK_BLEND_FACTOR_SRC_COLOR,
    eOneMinusSrcColor       = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
    eDstColor               = VK_BLEND_FACTOR_DST_COLOR,
    eOneMinusDstColor       = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
    eSrcAlpha               = VK_BLEND_FACTOR_SRC_ALPHA,
    eOneMinusSrcAlpha       = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    eDstAlpha               = VK_BLEND_FACTOR_DST_ALPHA,
    eOneMinusDstAlpha       = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
    eConstantColor          = VK_BLEND_FACTOR_CONSTANT_COLOR,
    eOneMinusConstantColor  = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
    eConstantAlpha          = VK_BLEND_FACTOR_CONSTANT_ALPHA,
    eOneMinusConstantAlpha  = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
    eSrcAlphaSaturate       = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
    eSrc1Color              = VK_BLEND_FACTOR_SRC1_COLOR,
    eOneMinusSrc1Color      = VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
    eSrc1Alpha              = VK_BLEND_FACTOR_SRC1_ALPHA,
    eOneMinusSrc1Alpha      = VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA
};



enum class BlendOp
{
    eAdd                = VK_BLEND_OP_ADD,
    eSubtract           = VK_BLEND_OP_SUBTRACT,
    eReverseSubtract    = VK_BLEND_OP_REVERSE_SUBTRACT,
    eMin                = VK_BLEND_OP_MIN,
    eMax                = VK_BLEND_OP_MAX
};



enum class ColorComponentFlagBits
{
    eR      = VK_COLOR_COMPONENT_R_BIT,
    eG      = VK_COLOR_COMPONENT_G_BIT,
    eB      = VK_COLOR_COMPONENT_B_BIT,
    eA      = VK_COLOR_COMPONENT_A_BIT,
};

using ColorComponentFlags = internal::Flags<ColorComponentFlagBits, VkColorComponentFlags>;



struct PipelineColorBlendAttachmentState : public internal::VkTrait<PipelineColorBlendAttachmentState, VkPipelineColorBlendAttachmentState>
{
    Bool32              blendEnable{ VK_FALSE };
    BlendFactor         srcColorBlendFactor;
    BlendFactor         dstColorBlendFactor;
    BlendOp             colorBlendOp;
    BlendFactor         srcAlphaBlendFactor;
    BlendFactor         dstAlphaBlendFactor;
    BlendOp             alphaBlendOp;
    ColorComponentFlags colorWriteMask;

    DEFINE_CLASS_MEMBER(PipelineColorBlendAttachmentState)

    PipelineColorBlendAttachmentState(Bool32 aBlendEnable, BlendFactor aSrcColorBlendFactor, BlendFactor aDstColorBlendFactor, BlendOp aColorBlendOp,
        BlendFactor aSrcAlphaBlendFactor, BlendFactor aDstAlphaBlendFactor, BlendOp aAlphaBlendOp, const ColorComponentFlags& aColorWriteMask)
        : blendEnable(aBlendEnable), srcColorBlendFactor(aSrcColorBlendFactor), dstColorBlendFactor(aDstColorBlendFactor), colorBlendOp(aColorBlendOp),
          srcAlphaBlendFactor(aSrcAlphaBlendFactor), dstAlphaBlendFactor(aDstAlphaBlendFactor), alphaBlendOp(aAlphaBlendOp), colorWriteMask(aColorWriteMask)
    {}
};

ConsistencyCheck(PipelineColorBlendAttachmentState, blendEnable, srcColorBlendFactor, dstColorBlendFactor, colorBlendOp,
    srcAlphaBlendFactor, dstAlphaBlendFactor, alphaBlendOp, colorWriteMask)



enum class PipelineColorBlendStateCreateFlagBits
{};

using PipelineColorBlendStateCreateFlags = internal::Flags<PipelineColorBlendStateCreateFlagBits, VkPipelineColorBlendStateCreateFlags>;



enum class LogicalOp
{
    eClear          = VK_LOGIC_OP_CLEAR,
    eAnd            = VK_LOGIC_OP_AND,
    eAndReverse     = VK_LOGIC_OP_AND_REVERSE,
    eCopy           = VK_LOGIC_OP_COPY,
    eAndInverted    = VK_LOGIC_OP_AND_INVERTED,
    eNoOp           = VK_LOGIC_OP_NO_OP,
    eXor            = VK_LOGIC_OP_XOR,
    eOr             = VK_LOGIC_OP_OR,
    eNor            = VK_LOGIC_OP_NOR,
    eEquivalent     = VK_LOGIC_OP_EQUIVALENT,
    eInvert         = VK_LOGIC_OP_INVERT,
    eOrReverse      = VK_LOGIC_OP_OR_REVERSE,
    eCopyInverted   = VK_LOGIC_OP_COPY_INVERTED,
    eOrInverted     = VK_LOGIC_OP_OR_INVERTED,
    eNand           = VK_LOGIC_OP_NAND,
    eSet            = VK_LOGIC_OP_SET
};



class PipelineColorBlendStateCreateInfo : public internal::VkTrait<PipelineColorBlendStateCreateInfo, VkPipelineColorBlendStateCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::ePipelineColorBlendState;

public:
    const void*                                 pNext{ nullptr };
    PipelineColorBlendStateCreateFlags          flags;
    Bool32                                      logicOpEnable{ VK_FALSE };
    LogicalOp                                   logicOp;
    uint32_t                                    attachmentCount{ 0 };
    const PipelineColorBlendAttachmentState*    pAttachments{ nullptr };
    float                                       blendConstants[4]{};

    DEFINE_CLASS_MEMBER(PipelineColorBlendStateCreateInfo)

    PipelineColorBlendStateCreateInfo(Bool32 aLogicOpEnable, LogicalOp aLogicOp, uint32_t aAttachmentCount, const PipelineColorBlendAttachmentState* apAttachments,
        const std::array<float, 4>& aBlendConstants, const PipelineColorBlendStateCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), logicOpEnable(aLogicOpEnable), logicOp(aLogicOp), attachmentCount(aAttachmentCount),
          pAttachments(apAttachments), blendConstants{aBlendConstants[0], aBlendConstants[1], aBlendConstants[2], aBlendConstants[3]}
    {}

    // TODO
};

ConsistencyCheck(PipelineColorBlendStateCreateInfo, pNext, flags, logicOpEnable, logicOp, attachmentCount, pAttachments, blendConstants)



enum class PipelineDynamicStateCreateFlagBits
{};

using PipelineDynamicStateCreateFlags = internal::Flags<PipelineDynamicStateCreateFlagBits, VkPipelineDynamicStateCreateFlags>;



enum class DynamicState
{
    eViewport               = VK_DYNAMIC_STATE_VIEWPORT,
    eScissor                = VK_DYNAMIC_STATE_SCISSOR,
    eLineWidth              = VK_DYNAMIC_STATE_LINE_WIDTH,
    eDepthBias              = VK_DYNAMIC_STATE_DEPTH_BIAS,
    eBlendConstants         = VK_DYNAMIC_STATE_BLEND_CONSTANTS,
    eDepthBounds            = VK_DYNAMIC_STATE_DEPTH_BOUNDS,
    eStencilCompareMask     = VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
    eStencilWriteMask       = VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
    eStencilReference       = VK_DYNAMIC_STATE_STENCIL_REFERENCE,
    eViewportWScalingNV     = VK_DYNAMIC_STATE_VIEWPORT_W_SCALING_NV,
    eDiscardRectangleEXT    = VK_DYNAMIC_STATE_DISCARD_RECTANGLE_EXT
};



class PipelineDynamicStateCreateInfo : public internal::VkTrait<PipelineDynamicStateCreateInfo, VkPipelineDynamicStateCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::ePipelineDynamicState;

public:
    const void*                         pNext{ nullptr };
    PipelineDynamicStateCreateFlags     flags;
    uint32_t                            dynamicStateCount{ 0 };
    const DynamicState*                 pDynamicStates{ nullptr };

    DEFINE_CLASS_MEMBER(PipelineDynamicStateCreateInfo)

    PipelineDynamicStateCreateInfo(uint32_t aDynamicStateCount, const DynamicState* apDynamicStates, const PipelineDynamicStateCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), dynamicStateCount(aDynamicStateCount), pDynamicStates(apDynamicStates)
    {}

    explicit PipelineDynamicStateCreateInfo(const std::vector<DynamicState>& aDynamicStates, const PipelineDynamicStateCreateFlags& aFlags = DefaultFlags)
        : PipelineDynamicStateCreateInfo(static_cast<uint32_t>(aDynamicStates.size()), aDynamicStates.data(), aFlags)
    {}

    template <std::size_t D>
    explicit PipelineDynamicStateCreateInfo(const std::array<DynamicState, D>& aDynamicStates, const PipelineDynamicStateCreateFlags& aFlags = DefaultFlags)
        : PipelineDynamicStateCreateInfo(static_cast<uint32_t>(aDynamicStates.size()), aDynamicStates.data(), aFlags)
    {}

    PipelineDynamicStateCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    PipelineDynamicStateCreateInfo& SetFlags(const PipelineDynamicStateCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    PipelineDynamicStateCreateInfo& SetDynamicStates(uint32_t aDynamicStateCount, const DynamicState* apDynamicStates)
    {
        dynamicStateCount   = aDynamicStateCount;
        pDynamicStates      = apDynamicStates;

        return *this;
    }

    PipelineDynamicStateCreateInfo& SetDynamicStates(const std::vector<DynamicState>& aDynamicStates)
    {
        return SetDynamicStates(static_cast<uint32_t>(aDynamicStates.size()), aDynamicStates.data());
    }

    template <std::size_t D>
    PipelineDynamicStateCreateInfo& SetDynamicStates(const std::array<DynamicState, D>& aDynamicStates)
    {
        return SetDynamicStates(static_cast<uint32_t>(aDynamicStates.size()), aDynamicStates.data());
    }
};

ConsistencyCheck(PipelineDynamicStateCreateInfo, pNext, flags, dynamicStateCount, pDynamicStates)



enum class PipelineLayoutCreateFlagBits
{};

using PipelineLayoutCreateFlags = internal::Flags<PipelineLayoutCreateFlagBits, VkPipelineLayoutCreateFlags>;



struct PushConstantRange : internal::VkTrait<PushConstantRange, VkPushConstantRange>
{
    ShaderStageFlags    stageFlags;
    uint32_t            offset{ 0 };
    uint32_t            size{ 0 };

    DEFINE_CLASS_MEMBER(PushConstantRange)

    PushConstantRange(const ShaderStageFlags& aStageFlags, uint32_t aOffset, uint32_t aSize) : stageFlags(aStageFlags), offset(aOffset), size(aSize)
    {}

    PushConstantRange& SetFlags(const ShaderStageFlags& aFlags)
    {
        stageFlags = aFlags;

        return *this;
    }

    PushConstantRange& SetSize(uint32_t aOffset, uint32_t aSize)
    {
        offset  = aOffset;
        size    = aSize;

        return *this;
    }
};

ConsistencyCheck(PushConstantRange, stageFlags, offset, size)



class PipelineLayoutCreateInfo : public internal::VkTrait<PipelineLayoutCreateInfo, VkPipelineLayoutCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::ePipelineLayout;

public:
    const void*                 pNext{ nullptr };
    PipelineLayoutCreateFlags   flags;
    uint32_t                    setLayoutCount{ 0 };
    const DescriptorSetLayout*  pSetLayouts{ nullptr };
    uint32_t                    pushConstantRangeCount{ 0 };
    const PushConstantRange*    pPushConstantRanges{ nullptr };

    DEFINE_CLASS_MEMBER(PipelineLayoutCreateInfo)

    PipelineLayoutCreateInfo(uint32_t aSetLayoutCount, const DescriptorSetLayout* apSetLayouts, uint32_t aPushConstantRangeCount, const PushConstantRange* apPushConstantRanges,
        const PipelineLayoutCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), setLayoutCount(aSetLayoutCount), pSetLayouts(apSetLayouts), pushConstantRangeCount(aPushConstantRangeCount), pPushConstantRanges(apPushConstantRanges)
    {}

    PipelineLayoutCreateInfo(const std::vector<DescriptorSetLayout>& aSetLayouts, const std::vector<PushConstantRange>& aPushConstantRanges,
        const PipelineLayoutCreateFlags& aFlags = DefaultFlags)
      : PipelineLayoutCreateInfo(static_cast<uint32_t>(aSetLayouts.size()), aSetLayouts.data(), static_cast<uint32_t>(aPushConstantRanges.size()), aPushConstantRanges.data(), aFlags)
    {}

    template <std::size_t D, std::size_t P>
    PipelineLayoutCreateInfo(const std::array<DescriptorSetLayout, D>& aSetLayouts, const std::array<PushConstantRange, P>& aPushConstantRanges,
        const PipelineLayoutCreateFlags& aFlags = DefaultFlags)
        : PipelineLayoutCreateInfo(static_cast<uint32_t>(aSetLayouts.size()), aSetLayouts.data(), static_cast<uint32_t>(aPushConstantRanges.size()), aPushConstantRanges.data(), aFlags)
    {}

    PipelineLayoutCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    PipelineLayoutCreateInfo& SetFlags(const PipelineLayoutCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    PipelineLayoutCreateInfo& SetLayouts(uint32_t aSetLayoutCount, const DescriptorSetLayout* apSetLayouts)
    {
        setLayoutCount  = aSetLayoutCount;
        pSetLayouts     = apSetLayouts;

        return *this;
    }

    PipelineLayoutCreateInfo& SetLayouts(const std::vector<DescriptorSetLayout>& aSetLayouts)
    {
        return SetLayouts(static_cast<uint32_t>(aSetLayouts.size()), aSetLayouts.data());
    }

    template <std::size_t D>
    PipelineLayoutCreateInfo& SetLayouts(const std::array<DescriptorSetLayout, D>& aSetLayouts)
    {
        return SetLayouts(static_cast<uint32_t>(aSetLayouts.size()), aSetLayouts.data());
    }

    PipelineLayoutCreateInfo& SetPushConstantRanges(uint32_t aPushConstantRangeCount, const PushConstantRange* apPushConstantRanges)
    {
        pushConstantRangeCount  = aPushConstantRangeCount;
        pPushConstantRanges     = apPushConstantRanges;

        return *this;
    }

    PipelineLayoutCreateInfo& SetPushConstantRanges(const std::vector<PushConstantRange>& aPushConstantRanges)
    {
        return SetPushConstantRanges(static_cast<uint32_t>(aPushConstantRanges.size()), aPushConstantRanges.data());
    }

    template <std::size_t P>
    PipelineLayoutCreateInfo& SetPushConstantRanges(const std::array<PushConstantRange, P>& aPushConstantRanges)
    {
        return SetPushConstantRanges(static_cast<uint32_t>(aPushConstantRanges.size()), aPushConstantRanges.data());
    }
};

ConsistencyCheck(PipelineLayoutCreateInfo, pNext, flags, setLayoutCount, pSetLayouts, pushConstantRangeCount, pPushConstantRanges)



class PipelineLayout : public internal::VkTrait<PipelineLayout, VkPipelineLayout>
{
private:
    VkPipelineLayout mPipelineLayout{ VK_NULL_HANDLE };

public:
    PipelineLayout(void) = default;

    PipelineLayout(std::nullptr_t)
    {}

    explicit PipelineLayout(VkPipelineLayout aPipelineLayout) : mPipelineLayout(aPipelineLayout)
    {}
};

StaticSizeCheck(PipelineLayout)



enum class PipelineCreateFlagBits
{
    eDisableOptimization            = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT,
    eAllowDerivatives               = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT,
    eDerivative                     = VK_PIPELINE_CREATE_DERIVATIVE_BIT,
    eViewIndexFromDeviceIndexKHX    = VK_PIPELINE_CREATE_VIEW_INDEX_FROM_DEVICE_INDEX_BIT_KHX,
    eDispatchBaseKHX                = VK_PIPELINE_CREATE_DISPATCH_BASE_KHX
};

using PipelineCreateFlags = internal::Flags<PipelineCreateFlagBits, VkPipelineCreateFlags>;



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_PIPELINE_STAGE_H__
