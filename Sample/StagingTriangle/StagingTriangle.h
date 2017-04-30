#ifndef __VKPP_SAMPLE_STAGING_TRIANGLE_H__
#define __VKPP_SAMPLE_STAGING_TRIANGLE_H__



#include <glm/glm.hpp>

#include "Application/Application.h"



namespace sample
{



struct RenderingResource
{
    vkpp::Framebuffer mFramebuffer;
    vkpp::CommandBuffer mCommandBuffer;
    vkpp::Semaphore mImageAvailSemaphore;
    vkpp::Semaphore mFinishedRenderingSemaphore;
    vkpp::Fence mFence;
};



struct VertexData
{
    glm::vec2 inPosition;
    glm::vec3 inColor;
};



class StagingTriangle : public Application
{
private:
    constexpr static std::size_t DefaultResourceSize = 3;

    vkpp::CommandPool mCommandPool;
    std::vector<RenderingResource> mRenderingResources;
    vkpp::RenderPass mRenderPass;
    vkpp::Pipeline mGraphicsPipeline;
    vkpp::Buffer mVertexBuffer;
    vkpp::DeviceMemory mVertexBufferMemory;

    void CreateCommandPool(void);
    void CreateRenderingResources(void);
    void AllocateCommandBuffers(vkpp::CommandBuffer& aCommandBuffer) const;
    void CreateSemaphore(vkpp::Semaphore& aSemaphore) const;
    void CreateFence(vkpp::Fence& aFence) const;

    void CreateRenderPass(void);
    void CreatePipeline(void);
    void CreateVertexBuffer(void);
    void CopyBuffer(vkpp::Buffer& aDstBuffer, const vkpp::Buffer& aSrcBuffer, vkpp::DeviceSize aSize) const;
    vkpp::DeviceMemory AllocateBufferMemory(const vkpp::Buffer& aBuffer, const vkpp::MemoryPropertyFlags& aMemoryProperties) const;
    vkpp::Framebuffer CreateFramebuffer(const vkpp::ImageView& aImageView) const;
    void PrepareFrame(vkpp::Framebuffer& aFramebuffer, const vkpp::CommandBuffer& aCommandBuffer, const vkpp::ImageView& aImageView) const;

    virtual void DrawFrame(void) override;

public:
    StagingTriangle(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~StagingTriangle(void);
};



}                   // End of namespace sample.



#endif              // __VKPP_SAMPLE_STAGING_TRIANGLE_H__
