#ifndef __VKPP_SAMPLE_STAGING_TRIANGLE_H__
#define __VKPP_SAMPLE_STAGING_TRIANGLE_H__



#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

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



struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};



class UniformTriangle : public Application
{
private:
    constexpr static std::size_t DefaultResourceSize = 3;

    vkpp::CommandPool mCommandPool;
    std::vector<RenderingResource> mRenderingResources;
    vkpp::RenderPass mRenderPass;
    vkpp::DescriptorSetLayout mDescriptorSetLayout;
    vkpp::PipelineLayout mPipelineLayout;
    vkpp::Pipeline mGraphicsPipeline;
    vkpp::Buffer mVertexBuffer;
    vkpp::DeviceMemory mVertexBufferMemory;
    vkpp::Buffer mIndexBuffer;
    vkpp::DeviceMemory mIndexBufferMemory;
    vkpp::Buffer mUniformBuffer;
    vkpp::DeviceMemory mUniformBufferMemory;
    vkpp::Buffer mUniformStagingBuffer;
    vkpp::DeviceMemory mUniformStagingBufferMemory;

    vkpp::DescriptorPool mDescriptorPool;
    vkpp::DescriptorSet mDescriptorSet;

    void CreateCommandPool(void);
    void CreateRenderingResources(void);
    void AllocateCommandBuffers(vkpp::CommandBuffer& aCommandBuffer) const;
    void CreateSemaphore(vkpp::Semaphore& aSemaphore) const;
    void CreateFence(vkpp::Fence& aFence) const;

    void CreateRenderPass(void);
    vkpp::DescriptorSetLayout CreateDescriptorSetLayout(void) const;
    void CreateGraphicsPipeline(void);
    void CreateVertexBuffer(void);
    void CreateIndexBuffer(void);
    void CreateUniformBuffer(void);
    void CreateDescriptorPool(void);
    void CreateDescriptorSet(void);
    void CopyBuffer(vkpp::Buffer& aDstBuffer, const vkpp::Buffer& aSrcBuffer, vkpp::DeviceSize aSize) const;
    vkpp::DeviceMemory AllocateBufferMemory(const vkpp::Buffer& aBuffer, const vkpp::MemoryPropertyFlags& aMemoryProperties) const;
    vkpp::Framebuffer CreateFramebuffer(const vkpp::ImageView& aImageView) const;
    void PrepareFrame(vkpp::Framebuffer& aFramebuffer, const vkpp::CommandBuffer& aCommandBuffer, const vkpp::ImageView& aImageView) const;

    void UpdateUniformBuffer(void);

    virtual void DrawFrame(void) override;

public:
    UniformTriangle(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~UniformTriangle(void);
};



}                   // End of namespace sample.



#endif              // __VKPP_SAMPLE_STAGING_TRIANGLE_H__
