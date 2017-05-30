#ifndef __VKPP_SAMPLE_PUSH_CONSTANTS_H__
#define __VKPP_SAMPLE_PUSH_CONSTANTS_H__



#include <Base/ExampleBase.h>
#include <Window/WindowEvent.h>
#include <window/MouseEvent.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



namespace vkpp::sample
{



struct VertexData
{
    glm::vec3 inPos;
    glm::vec3 inNormal;
    glm::vec3 inColor;

    constexpr static decltype(auto) GetAttributeDescriptions(void)
    {
        constexpr std::array<vkpp::VertexInputAttributeDescription, 3> lInputAttributeDescriptions
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
            // Location 2: inColor
            {
                2, 0,
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



struct UBOVS
{
    glm::mat4 proj;
    glm::mat4 model;
    glm::vec4 lightPos{ 0.0f, 0.0f, -2.0f, 1.0f };
};



class PushConstants : public ExampleBase, private CWindowEvent, private CMouseMotionEvent, private CMouseWheelEvent
{
private:
    constexpr static float MINIMUM_ZOOM_LEVEL = 0.05f;

    // This array holds the light positions and will be updated via a push constant.
    std::array<glm::vec4, 6> mPushConstants;

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

    BufferResource mUBORes;
    void* mpMatrixUBOMapped;
    UBOVS mMVPMatrix;
    float mCurrentZoomLevel{ -30.0f };
    glm::vec3 mCurrentRotation{ -32.5f, 45.0f, 0.0f };

    BufferResource mVtxBufferRes;
    BufferResource mIdxBufferRes;
    uint32_t mIndexCount{ 0 };

    vkpp::Semaphore mRenderingCompleteSemaphore;
    vkpp::Semaphore mPresentCompleteSemaphore;
    std::vector<vkpp::Fence> mWaitFences;

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

    void CreateUniformBuffer(void);
    void UpdateUniformBuffer(void);
    void UpdateDescriptorSet(void) const;

    void LoadModel(const std::string& aFilename);

    void BuildCmdBuffers(void);

    void CreateSemaphores(void);
    void CreateFences(void);

    void Update(void);

    void CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const;

    vkpp::CommandBuffer BeginOneTimeCmdBuffer(void) const;
    void EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const;

    void UpdateLightPositions(void);

public:
    PushConstants(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~PushConstants(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_PUSH_CONSTANTS_H__
