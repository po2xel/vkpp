#ifndef __VKPP_SAMPLE_MULTIPIPELINE_H__
#define __VKPP_SAMPLE_MULTIPIPELINE_H__



#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <Base/ExampleBase.h>
#include <Window/Window.h>
#include <Window/WindowEvent.h>



namespace vkpp::sample
{



struct VertexData
{
    glm::vec3 inPosition;
    glm::vec3 inColor;
    glm::vec2 texCoord;
    glm::vec3 normal;

    constexpr static vkpp::VertexInputBindingDescription GetBindingDescription(void)
    {
        return {
            0,                                  // binding
            sizeof(VertexData),                 // stride
            vkpp::VertexInputRate::eVertex      // input rate
        };
    }

    constexpr static decltype(auto) GetAttributeDescriptions(void)
    {
        constexpr std::array<vkpp::VertexInputAttributeDescription, 4> lInputAttributeDescriptions
        {{
            // Location 0: Position
            {
                0,                                  // location
                0,                                  // binding
                vkpp::Format::eRGBA32sFloat,        // format
                offsetof(VertexData, inPosition)    // offset
            },
            // Location 1: Color
            {
                1,                                  // location
                0,                                  // binding
                vkpp::Format::eRGBA32sFloat,        // format
                offsetof(VertexData, inColor)       // offset
            },
            // Location 2: Texture Coordinates
            {
                2,                                  // location
                0,                                  // binding
                vkpp::Format::eRG32sFloat,          // format
                offsetof(VertexData, texCoord)      // offset
            },
            // Location 3: Normal
            {
                3,                                  // location
                0,                                  // binding
                vkpp::Format::eRGB32sFloat,         // format
                offsetof(VertexData, normal)        // offset
            }
        } };

        return lInputAttributeDescriptions;
    }
};



class MultiPipelines : public ExampleBase, public CWindowEvent
{
private:
    vkpp::CommandPool mCmdPool;
    std::vector<vkpp::CommandBuffer> mCmdDrawBuffers;

    vkpp::RenderPass mRenderPass;
    ImageResource mDepthResource;
    std::vector<vkpp::Framebuffer> mFramebuffers;
    vkpp::DescriptorSetLayout mSetLayout;
    vkpp::PipelineLayout mPipelineLayout;

    struct
    {
        vkpp::Pipeline phong;
        vkpp::Pipeline wireframe;
        vkpp::Pipeline toon;
    } mPipelines;

    vkpp::DescriptorPool mDescriptorPool;
    vkpp::DescriptorSet mDescriptorSet;

    void CreateCommandPool(void);
    void AllocateDrawCmdBuffers(void);

    void CreateRenderPass(void);
    void CreateDepthResource(void);
    void CreateFramebuffer(void);
    void CreateUniformBuffer(void);
    void CreateSetLayout(void);
    void CreatePipelineLayout(void);

    void CreateGraphicsPipelines(void);

    void CreateDescriptorPool(void);
    void AllocateDescriptorSet(void);

public:
    MultiPipelines(CWindow& aWindow, const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~MultiPipelines(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_MULTIPIPELINE_H__
