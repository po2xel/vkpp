#ifndef __VKPP_TYPE_GRAPHICS_PIPELINE_H__
#define __VKPP_TYPE_GRAPHICS_PIPELINE_H__



#include <Info/Common.h>
#include <Info/PipelineStage.h>
#include <Type/RenderPass.h>



namespace vkpp
{



enum class PipelineCacheCreateFlagBits
{};

VKPP_ENUM_BIT_MASK_FLAGS(PipelineCacheCreate)



class PipelineCacheCreateInfo : public internal::VkTrait<PipelineCacheCreateInfo, VkPipelineCacheCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::ePipelineCache;

public:
    const void*                 pNext{ nullptr };
    PipelineCacheCreateFlags    flags;
    size_t                      initialDataSize{ 0 };
    const void*                 pInitialData{ nullptr };

    DEFINE_CLASS_MEMBER(PipelineCacheCreateInfo)

    constexpr PipelineCacheCreateInfo(size_t aInitialDataSize, const void* apInitialData, const PipelineCacheCreateFlags& aFlags = DefaultFlags) noexcept 
        : flags(aFlags), initialDataSize(aInitialDataSize), pInitialData(apInitialData)
    {}

    PipelineCacheCreateInfo& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    PipelineCacheCreateInfo& SetFlags(const PipelineCacheCreateFlags& aFlags) noexcept
    {
        flags = aFlags;

        return *this;
    }

    PipelineCacheCreateInfo& SetInitialData(size_t aInitialDataSize, const void* apInitialData) noexcept
    {
        initialDataSize = aInitialDataSize;
        pInitialData    = apInitialData;

        return *this;
    }
};

ConsistencyCheck(PipelineCacheCreateInfo, pNext, flags, initialDataSize, pInitialData)



class Pipeline : public internal::VkTrait<Pipeline, VkPipeline>
{
private:
    VkPipeline mPipeline{ VK_NULL_HANDLE };

public:
    Pipeline(void) noexcept = default;

    Pipeline(std::nullptr_t) noexcept
    {}

    explicit Pipeline(VkPipeline aPipeline) noexcept : mPipeline(aPipeline)
    {}
};

StaticSizeCheck(Pipeline)



class PipelineCache : public internal::VkTrait<PipelineCache, VkPipelineCache>
{
private:
    VkPipelineCache mPipelineCache{ VK_NULL_HANDLE };

public:
    PipelineCache(void) noexcept = default;

    PipelineCache(std::nullptr_t) noexcept
    {}

    explicit PipelineCache(VkPipelineCache aPipelineCache) noexcept : mPipelineCache(aPipelineCache)
    {}
};

StaticSizeCheck(PipelineCache)



class GraphicsPipelineCreateInfo : public internal::VkTrait<GraphicsPipelineCreateInfo, VkGraphicsPipelineCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::ePipelineGraphics;

public:
    const void*                                 pNext{ nullptr };
    PipelineCreateFlags                         flags;
    uint32_t                                    stageCount{ 0 };
    const PipelineShaderStageCreateInfo*        pStages{ nullptr };
    const PipelineVertexInputStateCreateInfo*   pVertexInputState{ nullptr };
    const PipelineInputAssemblyStateCreateInfo* pInputAssemblyState{ nullptr };
    const PipelineTessellationStateCreateInfo*  pTessellationState{ nullptr };
    const PipelineViewportStateCreateInfo*      pViewportState{ nullptr };
    const PipelineRasterizationStateCreateInfo* pRasterizationState{ nullptr };
    const PipelineMultisampleStateCreateInfo*   pMultisampleState{ nullptr };
    const PipelineDepthStencilStateCreateInfo*  pDepthStencilState{ nullptr };
    const PipelineColorBlendStateCreateInfo*    pColorBlendState{ nullptr };
    const PipelineDynamicStateCreateInfo*       pDynamicState{ nullptr };
    PipelineLayout                              layout;
    RenderPass                                  renderPass;
    uint32_t                                    subpass{ 0 };
    Pipeline                                    basePipelineHandle;
    int32_t                                     basePipelineIndex{ -1 };

    DEFINE_CLASS_MEMBER(GraphicsPipelineCreateInfo)

    GraphicsPipelineCreateInfo(uint32_t aStageCount, const PipelineShaderStageCreateInfo* apStages,
        const PipelineVertexInputStateCreateInfo* apVertexInputState, const PipelineInputAssemblyStateCreateInfo* apInputAssemblyState,
        const PipelineTessellationStateCreateInfo* apTessellationState, const PipelineViewportStateCreateInfo* apViewportState,
        const PipelineRasterizationStateCreateInfo* apRasterizationState, const PipelineMultisampleStateCreateInfo* apMultisampleState,
        const PipelineDepthStencilStateCreateInfo* apDepthStencilState, const PipelineColorBlendStateCreateInfo* apColorBlendState,
        const PipelineDynamicStateCreateInfo* apDynamicState,
        const PipelineLayout& aLayout, const RenderPass& aRenderPass, uint32_t aSubpass,
        const Pipeline& aBasePipelineHandle = nullptr, int32_t aBasePipelineIndex = -1,
        const PipelineCreateFlags& aFlags = DefaultFlags) noexcept
        : flags(aFlags), stageCount(aStageCount), pStages(apStages), pVertexInputState(apVertexInputState), pInputAssemblyState(apInputAssemblyState),
          pTessellationState(apTessellationState), pViewportState(apViewportState), pRasterizationState(apRasterizationState), pMultisampleState(apMultisampleState),
          pDepthStencilState(apDepthStencilState), pColorBlendState(apColorBlendState), pDynamicState(apDynamicState),
          layout(aLayout), renderPass(aRenderPass), subpass(aSubpass), basePipelineHandle(aBasePipelineHandle), basePipelineIndex(aBasePipelineIndex)
    {}
};

ConsistencyCheck(GraphicsPipelineCreateInfo, pNext, flags, stageCount, pStages, pVertexInputState, pInputAssemblyState, pTessellationState, pViewportState,
    pRasterizationState, pMultisampleState, pDepthStencilState, pColorBlendState, pDynamicState, layout, renderPass, subpass, basePipelineHandle, basePipelineIndex)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_GRAPHICS_PIPELINE_H__
