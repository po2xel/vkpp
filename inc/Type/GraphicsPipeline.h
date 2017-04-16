#ifndef __VKPP_TYPE_GRAPHICS_PIPELINE_H__
#define __VKPP_TYPE_GRAPHICS_PIPELINE_H__



#include <Info/Common.h>
#include <Info/PipelineStage.h>
#include <Type/RenderPass.h>



namespace vkpp
{



class Pipeline : public internal::VkTrait<Pipeline, VkPipeline>
{
private:
    VkPipeline mPipeline{ VK_NULL_HANDLE };

public:
    Pipeline(void) = default;

    Pipeline(std::nullptr_t)
    {}

    explicit Pipeline(VkPipeline aPipeline) : mPipeline(aPipeline)
    {}
};

StaticSizeCheck(Pipeline);



class PipelineCache : public internal::VkTrait<PipelineCache, VkPipelineCache>
{
private:
    VkPipelineCache mPipelineCache{ VK_NULL_HANDLE };

public:
    PipelineCache(void) = default;

    PipelineCache(std::nullptr_t)
    {}

    explicit PipelineCache(VkPipelineCache aPipelineCache) : mPipelineCache(aPipelineCache)
    {}
};

StaticSizeCheck(Pipeline);



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
    int32_t                                     basePipelineIndex{ 0 };
};

StaticSizeCheck(GraphicsPipelineCreateInfo);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_GRAPHICS_PIPELINE_H__
