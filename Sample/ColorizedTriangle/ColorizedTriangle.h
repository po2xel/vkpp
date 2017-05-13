#ifndef __VKPP_SAMPLE_COLORIZED_TRIANGLE_H__
#define __VKPP_SAMPLE_COLORIZED_TRIANGLE_H__



#include <Info/PipelineStage.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm//glm.hpp>

#include "Base/ExampleBase.h"
#include "Window/WindowEvent.h"



namespace vkpp::sample
{



struct VertexData
{
    glm::vec3 inPosition;
    glm::vec3 inColor;

    constexpr static vkpp::VertexInputBindingDescription GetBindingDescription(void)
    {
        constexpr vkpp::VertexInputBindingDescription lInputBindingDescription
        {
            0,                                  // binding
            sizeof(VertexData),                 // stride
            vkpp::VertexInputRate::eVertex      // inputRate
        };

        return lInputBindingDescription;
    }

    constexpr static decltype(auto) GetAttributeDescriptions(void)
    {
        constexpr std::array<vkpp::VertexInputAttributeDescription, 2> lInputAttributeDescriptions
        { {
            {
                0,                                  // location
                0,                                  // binding
                vkpp::Format::eRGB32sFloat,         // format
                offsetof(VertexData, inPosition)    // offset
            },
            {
                1,                                  // location
                0,                                  // binding
                vkpp::Format::eRGB32sFloat,          // format
                offsetof(VertexData, inColor)       // offset
            }
        } };

        return lInputAttributeDescriptions;
    }

    constexpr static vkpp::PipelineVertexInputStateCreateInfo GetInputStateCreateInfo(void)
    {
        decltype(auto) lInputBindingDescription = GetBindingDescription();
        decltype(auto) lAttributeDescription = GetAttributeDescriptions();

        return { 1, lInputBindingDescription.AddressOf(), 2, lAttributeDescription.data() };
    }
};



struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};



class ColorizedTriangle : public ExampleBase, private CWindowEvent
{
private:
    vkpp::CommandPool mCommandPool;
    std::vector<vkpp::CommandBuffer> mDrawCmdBuffers;
    ImageResource mDepthResources;
    vkpp::Semaphore mPresentCompleteSemaphore;
    vkpp::Semaphore mRenderCompleteSemaphore;
    std::vector<vkpp::Fence> mWaitFences;
    vkpp::RenderPass mRenderPass;
    vkpp::PipelineCache mPipelineCache;
    std::vector<vkpp::Framebuffer> mFramebuffers;
    vkpp::DescriptorSetLayout mSetLayout;
    vkpp::PipelineLayout mPipelineLayout;
    vkpp::Pipeline mGraphicsPipeline;

    BufferResource mVertexBufferResource;
    BufferResource mIndexBufferResource;
    BufferResource mUniformBufferResource;

    vkpp::DescriptorPool mDescriptorPool;
    vkpp::DescriptorSet mDescriptorSet;

    UniformBufferObject mMVPMatrix;

    void CreateCommandPool(void);
    void AllocateDrawCmdBuffers(void);
    void CreateDepthResources(void);

    void CreateSemaphores(void);
    void CreateFences(void);
    void CreateRenderPass(void);
    void CreatePipelineCache(void);
    void CreateFramebuffers(void);
    void CreateDescriptorSetLayout(void);
    void CreatePipelineLayout(void);

    void CreateGraphicsPipeline(void);
    void CreateShaderResources(void);

    void CreateDescriptorPool(void);
    void AllocateDescriptorSet(void);
    void UpdateDescriptorSet(void) const;

    void BuildCommandBuffers(void);

    void CreateVertexBuffer(void);
    void CreateIndexBuffer(void);
    void CreateUniformBuffers(void);
    void CopyBuffer(DeviceSize aSize, const vkpp::Buffer& aSrcBuffer, vkpp::Buffer& aDstBuffer) const;

    void UpdateUniformBuffer(void) const;
    vkpp::CommandBuffer BeginOneTimeCommandBuffer(void) const;
    void EndOneTimeCommandBuffer(const vkpp::CommandBuffer& aCmdBuffer) const;

    void Update(void);

public:
    ColorizedTriangle(CWindow& aWindow, const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~ColorizedTriangle(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_COLORIZED_TRIANGLE_H__
