#ifndef __VKPP_SAMPLE_TEXTURED_PLATE_H__
#define __VKPP_SAMPLE_TEXTURED_PLATE_H__



#include <Base/ExampleBase.h>
#include <Window/WindowEvent.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gli/gli.hpp>



namespace vkpp::sample
{



struct VertexData
{
    glm::vec3 inPos;
    glm::vec3 inUV;
    glm::vec3 inNormal;

    constexpr static decltype(auto) GetAttributeDescriptions(void)
    {
        constexpr std::array<vkpp::VertexInputAttributeDescription, 3> lInputAttributeDescriptions
        { {
            // Location 0: Position.
            {
                0,                          // location
                0,                          // binding
                vkpp::Format::eRGB32sFloat, // format
                offsetof(VertexData, inPos) // offset
            },
            // Location 1: UV
            {
                1,                          // location
                0,                          // binding
                vkpp::Format::eRGB32sFloat, // format,
                offsetof(VertexData, inUV)  // offset
            },
            // Location 2: Normal
            {
                2,                              // Location
                0,                              // binding
                vkpp::Format::eRGB32sFloat,     // format
                offsetof(VertexData, inNormal)  // offset
            }
        } };

        return lInputAttributeDescriptions;
    }

    constexpr static vkpp::VertexInputBindingDescription GetBindingDescription(void)
    {
        return {
            0,                                  // binding
            sizeof(VertexData),                 // stride
            vkpp::VertexInputRate::eVertex      // input rate
        };
    }
};


class TexturedPlate : public ExampleBase, private CWindowEvent
{
private:
    struct Texture
    {
        vkpp::Sampler sampler;
        vkpp::ImageLayout layout;

        uint32_t width{ 0 }, height{ 0 }, mipLevels{ 0 };
    };

    vkpp::CommandPool mCmdPool;
    std::vector<vkpp::CommandBuffer> mDrawCmdBuffers;

    vkpp::RenderPass mRenderPass;
    ImageResource mDepthResource;
    std::vector<vkpp::Framebuffer> mFramebuffers;

    vkpp::DescriptorSetLayout mSetLayout;
    vkpp::PipelineLayout mPipelineLayout;
    vkpp::Pipeline mGraphicsPipeline;

    vkpp::DescriptorPool mDescriptorPool;
    vkpp::DescriptorSet mDescriptorSet;

    Texture mTexture;
    ImageResource mTextureResource;

    void CreateCmdPool(void);
    void AllocateCmdBuffers(void);

    void CreateRenderPass(void);
    void CreateDepthResource(void);
    void CreateFramebuffer(void);

    void CreateSetLayout(void);
    void CreatePipelineLayout(void);
    void CreateGraphicsPipeline(void);

    void CreateDescriptorPool(void);
    void AllocateDescriptorSet(void);
    void UpdateImageDescriptorSet(void);

    void LoadTexture(const std::string& aFilename, vkpp::Format mTexFormat);

    vkpp::CommandBuffer BeginOneTimeCmdBuffer(void) const;
    void EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const;

public:
    TexturedPlate(CWindow& aWindow, const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~TexturedPlate(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_TEXTURED_PLATE_H__
