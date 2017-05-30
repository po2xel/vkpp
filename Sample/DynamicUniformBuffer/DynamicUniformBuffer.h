#ifndef __VKPP_SAMPLE_DYNAMIC_UNIFORM_BUFFER_H__
#define __VKPP_SAMPLE_DYNAMIC_UNIFORM_BUFFER_H__



#include <Base/ExampleBase.h>
#include <Window/WindowEvent.h>
#include <Window/MouseEvent.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



namespace vkpp::sample
{



struct VertexData
{
    glm::vec3 inPos;
    glm::vec3 inColor;

    constexpr static decltype(auto) GetAttributeDescriptions(void)
    {
        constexpr std::array<vkpp::VertexInputAttributeDescription, 2> lInputAttributeDescriptions
        { {
            // Location 0: inPos
            {
                0, 0,
                vkpp::Format::eRGBA32sFloat,
                offsetof(VertexData, inPos)
            },
            // Location 1: inColor
            {
                1, 0,
                vkpp::Format::eRGBA32sFloat,
                offsetof(VertexData, inColor)
            }
        } };

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
    glm::mat4 view;
};



class DynamicUniformBuffer : public ExampleBase, private CWindowEvent, private CMouseMotionEvent, private CMouseWheelEvent
{
private:
    constexpr static float MINIMUM_ZOOM_LEVEL = 0.05f;
    constexpr static auto OBJECT_INSTANCES = 125;

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

    uint32_t mIndexCount{ 0 };
    BufferResource mVtxBufferRes;
    BufferResource mIdxBufferRes;
    BufferResource mUBORes;
    BufferResource mDynamicRes;

    vkpp::Semaphore mRenderingCompleteSemaphore;
    vkpp::Semaphore mPresentCompleteSemaphore;
    std::vector<vkpp::Fence> mWaitFences;

    // Store random per-object rotations.
    std::array<glm::vec3, OBJECT_INSTANCES> mRotations;
    std::array<glm::vec3, OBJECT_INSTANCES> mRotationSpeeds;

    // One big uniform buffer that contains all matrices.
    // Note that we need to manually allocate the data to copy for GPU-specific uniform buffer offset alignments.
    glm::mat4* mpUBODataDynamic{ nullptr };

    std::size_t mDynamicAlignment{ 0 };
    float mCurrentZoomLevel{ -30.0f };
    glm::vec3 mCurrentRotation{};

    UniformBufferObject mUniformBufferObject;
    std::size_t mDynamicBufferSize{ 0 };
    void* mpMatrixUBOMapped{ nullptr };
    void* mpDynamicUBOMapped{ nullptr };

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

    void CreateVertexBuffer(void);
    void CreateIndexBuffer(void);
    void CreateUniformBuffer(void);
    void UpdateUniformBuffer(void);
    void UpdateDynamicUniformBuffer(void);

    void UpdateDescriptorSets(void) const;

    void BuildCmdBuffers(void);

    void CreateSemaphores(void);
    void CreateFences(void);

    void Update(void);

public:
    DynamicUniformBuffer(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~DynamicUniformBuffer(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_DYNAMIC_UNIFORM_BUFFER_H__
