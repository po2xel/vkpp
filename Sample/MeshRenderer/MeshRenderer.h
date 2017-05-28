#ifndef __VKPP_SAMPLE_MESH_RENDERER_H__
#define __VKPP_SAMPLE_MESH_RENDERER_H__



#include <Base/ExampleBase.h>
#include <Window/WindowEvent.h>
#include <Window/MouseEvent.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <gli/gli.hpp>



namespace vkpp::sample
{



struct VertexData
{
    glm::vec3 inPos;
    glm::vec3 inNormal;
    glm::vec2 inUV;
    glm::vec3 inColor;

    constexpr static decltype(auto) GetAttributeDescriptions(void)
    {
        constexpr std::array<vkpp::VertexInputAttributeDescription, 4> lInputAttributeDescriptions
        {{
            // Location 0: inPos
            {
                0, 0,
                vkpp::Format::eRGB32sFloat,
                offsetof(VertexData, inPos)
            },
            // Location 1: inNormal
            {
                1, 0,
                vkpp::Format::eRGB32sFloat,
                offsetof(VertexData, inNormal)
            },
            // Location 2: inUV
            {
                2, 0,
                vkpp::Format::eRG32sFloat,
                offsetof(VertexData, inUV)
            },
            // Location 3: inColor
            {
                3, 0,
                vkpp::Format::eRGB32sFloat,
                offsetof(VertexData, inColor)
            }
        }};

        return lInputAttributeDescriptions;
    }

    constexpr static vkpp::VertexInputBindingDescription GetBindingDescription(void)
    {
        return {
            0,
            sizeof(VertexData),
            vkpp::VertexInputRate::eVertex
        };
    }
};



struct UniformBufferObject
{
    glm::mat4 proj;
    glm::mat4 model;
    glm::vec4 lightPos{ 25.0f, 5.0f, 5.0f, 1.0f };
};



class MeshRenderer : public ExampleBase, private CWindowEvent, private CMouseMotionEvent, private CMouseWheelEvent
{
private:
    constexpr static float MINIMUM_ZOOM_LEVEL = 0.05f;

    struct Texture
    {
        vkpp::Sampler sampler;
        const vkpp::ImageLayout layout{ vkpp::ImageLayout::eShaderReadOnlyOptimal };

        uint32_t width{ 0 }, height{ 0 }, mipLevels{ 0 };
    };

    vkpp::CommandPool mCmdPool;
    std::vector<vkpp::CommandBuffer> mDrawCmdBuffers;

    vkpp::RenderPass mRenderPass;
    ImageResource mDepthRes;
    std::vector<vkpp::Framebuffer> mFramebuffers;

    vkpp::DescriptorSetLayout mSetLayout;
    vkpp::PipelineLayout mPipelineLayout;
    vkpp::Pipeline mGraphicsPipeline;

    vkpp::DescriptorPool mDescriptorPool;
    vkpp::DescriptorSet mDescriptorSet;

    BufferResource mVertexBufferRes;
    BufferResource mIndexBufferRes;
    uint32_t mIndexCount{ 0 };

    Texture mTexture;
    ImageResource mTextureRes;
    vkpp::Sampler mTextureSampler;

    BufferResource mUBORes;
    UniformBufferObject mMVPMatrix;

    vkpp::Semaphore mRenderingCompleteSemaphore;
    vkpp::Semaphore mPresentCompleteSemaphore;
    std::vector<vkpp::Fence> mWaitFences;

    float mCurrentZoomLevel{ -5.5f };
    glm::vec3 mCurrentRotation{ -0.5f, -112.75f, 0.0f };

    void* mpMatrixUBOMapped{ nullptr };

    void CreateCmdPool(void);
    void AllocateCmdBuffers(void);

    void CreateRenderPass(void);
    void CreateDepthResource(void);
    void CreateFramebuffers(void);

    void CreateSetLayout(void);
    void CreatePipelineLayout(void);
    void CreateGraphicsPipeline(void);

    void CreateDescriptorPool(void);
    void AllocateDescriptorSet(void);

    void LoadModel(const std::string& aFilename);
    void LoadTexture(const std::string& aFilename, vkpp::Format aTexFormat);
    void CreateSampler(void);

    void CreateUniformBuffer(void);
    void UpdateUniformBuffer(void);
    void UpdateDescriptorSets(void) const;

    void BuildCmdBuffers(void);

    void CreateSemaphores(void);
    void CreateFences(void);

    void Update(void);

    void CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const;

    vkpp::CommandBuffer BeginOneTimeCmdBuffer(void) const;
    void EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const;

    template <vkpp::ImageLayout OldLayour, vkpp::ImageLayout NewLayout>
    static void TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange, const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask);

public:
    MeshRenderer(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~MeshRenderer(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_MESH_RENDERER_H__
