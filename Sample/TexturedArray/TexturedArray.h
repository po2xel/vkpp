#ifndef __VKPP_SAMPLE_TEXTURED_ARRAY_H__
#define __VKPP_SAMPLE_TEXTURED_ARRAY_H__



#include <Base/ExampleBase.h>
#include <Window/WindowEvent.h>
#include <Window/MouseEvent.h>

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
    glm::vec2 inUV;

    constexpr static decltype(auto) GetAttributeDescriptions(void)
    {
        constexpr std::array<vkpp::VertexInputAttributeDescription, 2> lInputAttributeDescriptions
        { {
            // Location 0: inPos
            {
                0, 0,
                vkpp::Format::eRGB32sFloat,
                offsetof(VertexData, inPos)
            },
            // Location 1: inUV
            {
                1, 0,
                vkpp::Format::eRG32sFloat,
                offsetof(VertexData, inUV)
            }
        } };

        return lInputAttributeDescriptions;
    }

    constexpr static vkpp::VertexInputBindingDescription GetInputBindingDescription(void)
    {
        return {
            0,
            sizeof(VertexData),
            vkpp::VertexInputRate::eVertex
        };
    };
};



struct UboInstanceData
{
    // Model Matrix
    glm::mat4 model;

    // Texture array index. vec4 due to padding.
    glm::vec4 arrayIndex;

    UboInstanceData(const glm::mat4& aModel, uint32_t aArrayIndex) : model(aModel), arrayIndex(static_cast<float>(aArrayIndex))
    {}
};



struct UniformBufferObject
{
    // Global matrices
    struct
    {
        glm::mat4 projection;
        glm::mat4 view;
    } matrices;

    // Separate data for each instance.
    std::vector<UboInstanceData> instances;
};


class TexturedArray : public ExampleBase, private CWindowEvent, private CMouseMotionEvent, private CMouseWheelEvent
{
private:
    constexpr static float MINIMUM_ZOOM_LEVEL = 0.05f;

    vkpp::CommandPool mCmdPool;
    std::vector<vkpp::CommandBuffer> mDrawCmdBuffers;

    ImageResource mDepthRes;
    vkpp::RenderPass mRenderPass;
    std::vector<vkpp::Framebuffer> mFramebuffers;

    vkpp::DescriptorSetLayout mSetLayout;
    vkpp::PipelineLayout mPipelineLayout;
    vkpp::Pipeline mPipeline;

    vkpp::DescriptorPool mDescriptorPool;
    vkpp::DescriptorSet mDescriptorSet;

    struct
    {
        const vkpp::ImageLayout layout{ vkpp::ImageLayout::eShaderReadOnlyOptimal };

        uint32_t width{ 0 }, height{ 0 }, layerCount{ 0 };
    } mTexture;

    ImageResource mTextureRes;
    vkpp::Sampler mTextureSampler;

    BufferResource mVertexBufferRes;
    BufferResource mIndexBufferRes;

    UniformBufferObject mUniformBufferObject;
    void* mpMatrixUBOMapped{ nullptr };
    BufferResource mUBORes;

    vkpp::Semaphore mPresentCompleteSemaphore;
    vkpp::Semaphore mRenderingCompleteSemaphore;
    std::vector<vkpp::Fence> mWaitFences;

    float mCurrentZoomLevel{ -15.0f };
    glm::vec3 mCurrentRotation{ -15.0f, 35.0f, 0.0f };

    void CreateCmdPool(void);
    void AllocateCmdBuffers(void);

    void CreateDepthResource(void);
    void CreateRenderPass(void);
    void CreateFramebuffers(void);

    void CreateSetLayout(void);
    void CreatePipelineLayout(void);
    void CreateGraphicsPipeline(void);

    void CreateDescriptorPool(void);
    void AllocateDescriptorSet(void);

    void LoadTexture(const std::string& aFilename, vkpp::Format aFormat);
    void CreateSampler(void);

    void CreateVertexBuffer(void);
    void CreateIndexBuffer(void);

    void CreateUniformBuffer(void);
    void UpdateUniformBuffers(void);

    void UpdateDescriptorSets(void) const;

    void BuildCmdBuffers(void);

    void CreateSemaphores(void);
    void CreateFences(void);

    void Update(void);

    vkpp::CommandBuffer BeginOneTimeCmdBuffer(void) const;
    void EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const;

    template <vkpp::ImageLayout OldLayour, vkpp::ImageLayout NewLayout>
    static void TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange,
        const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask);

public:
    TexturedArray(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngingName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~TexturedArray(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_TEXTURED_ARRAY_H__
