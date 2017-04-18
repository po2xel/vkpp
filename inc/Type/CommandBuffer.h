#ifndef __VKPP_TYPE_COMMAND_BUFFER_H__
#define __VKPP_TYPE_COMMAND_BUFFER_H__



#include <cassert>

#include <Info/Common.h>
#include <Info/CommandBufferInfo.h>
#include <Info/PipelineStage.h>
#include <Info/MemoryBarrier.h>
#include <Info/RenderPassBeginInfo.h>

#include <Type/RenderPass.h>
#include <Type/GraphicsPipeline.h>



namespace vkpp
{



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
};



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_COMMAND_BUFFER_H__
