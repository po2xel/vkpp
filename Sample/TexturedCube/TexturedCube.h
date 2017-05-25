#ifndef __VKPP_SAMPLE_TEXTURED_CUBE_H__
#define __VKPP_SAMPLE_TEXTURED_CUBE_H__



#include <Base/ExampleBase.h>
#include <Window/WindowEvent.h>
#include <Window/MouseEvent.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
    glm::vec3 inNormal;
    glm::vec2 inUV;

    constexpr static decltype(auto) GetAttributeDescriptions(void)
    {
        constexpr std::array<vkpp::VertexInputAttributeDescription, 3> lInputAttributeDescriptions
        { {
            // Location 0: Position
            {
                0,
                0,
                vkpp::Format::eRGB32sFloat,
                offsetof(VertexData, inPos)
            },
            // Location 1: Normal
            {
                1,
                0,
                vkpp::Format::eRGB32sFloat,
                offsetof(VertexData, inNormal)
            },
            // Location 2: UV
            {
                2,
                0,
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
    }
};



struct UniformBufferObject
{
    glm::mat4 projection;
    glm::mat4 model;
    float loadBias{ 0.0f };
};


class TexturedCube;


struct Model
{
    constexpr static auto DefaultImporterFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;

    const TexturedCube& texturedCubeSample;
    const vkpp::LogicalDevice& device;
    const vkpp::PhysicalDeviceMemoryProperties& memProperties;

    BufferResource vertices;
    BufferResource indices;

    // Stores vertex and index bases and counts for each part of a model.
    struct ModelPart
    {
        uint32_t vertexBase{ 0 };
        uint32_t vertexCount{ 0 };
        uint32_t indexBase{ 0 };
        uint32_t indexCount{ 0 };

        ModelPart(uint32_t aVertexBase, uint32_t aVertexCount, uint32_t aIndexBase, uint32_t aIndexCount)
            : vertexBase(aVertexBase), vertexCount(aVertexCount), indexBase(aIndexBase), indexCount(aIndexCount)
        {}
    };

    struct Dimension
    {
        glm::vec3 min{ FLT_MAX };
        glm::vec3 max{ FLT_MAX };
        glm::vec3 size;
    } dim;

    std::vector<ModelPart> modelParts;
    uint32_t vertexCount{ 0 }, indexCount{ 0 };

    Model(const TexturedCube& aTexturedCubeSample, const vkpp::LogicalDevice& aDevice, const vkpp::PhysicalDeviceMemoryProperties& aPhysicalDeviceMemProperties);

    void LoadMode(const std::string& aFilename, const glm::vec3& aScale,  unsigned int aImporterFlags = DefaultImporterFlags);
};



class TexturedCube : public ExampleBase, private CWindowEvent, private CMouseWheelEvent, private CMouseMotionEvent
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

    vkpp::Pipeline mSkyboxPipeline;
    vkpp::Pipeline mReflectPipeline;

    vkpp::DescriptorPool mDescriptorPool;

    struct
    {
        vkpp::DescriptorSet object;
        vkpp::DescriptorSet skybox;
    } mDescriptorSets;

    struct
    {
        vkpp::Sampler sampler;
        vkpp::ImageLayout layout{ vkpp::ImageLayout::eShaderReadOnlyOptimal };

        uint32_t width{ 0 }, height{ 0 }, mipLevels{ 0 };
    } mCubeMap;

    ImageResource mTextureResource;
    vkpp::Sampler mTextureSampler;

    Model mSkyModel;
    Model mSphereModel;
    Model mTeapotModel;
    Model mTorusKnotModel;

    UniformBufferObject mMVPMatrix;
    void* mpObjectUBOMapped{ nullptr };
    void* mpSkyboxUBOMapped{ nullptr };

    BufferResource mObjectUBORes;
    BufferResource mSkyboxUBORes;

    vkpp::Semaphore mPresentCompleteSemaphore;
    vkpp::Semaphore mRenderingCompleteSemaphore;
    std::vector<vkpp::Fence> mWaitFences;

    float mCurrentZoomLevel{ -4.0f };
    glm::vec3 mCurrentRotation{ -7.25f, -120.0f, 0.0f };

    void CreateCommandPool(void);
    void AllocateCommandBuffers(void);

    void CreateDepthResource(void);
    void CreateRenderPass(void);
    void CreateFramebuffers(void);

    void CreateSetLayout(void);
    void CreatePipelineLayout(void);

    void CreateGraphicsPipeline(void);

    void CreateDescriptorPool(void);
    void AllocateDescriptorSets(void);

    void LoadTexture(const std::string& aFilename, vkpp::Format aFormat);
    void CreateSampler(void);

    void LoadMeshes(void);

    void CreateUniformBuffers(void);
    void UpdateUniformBuffers(void);
    void MapUniformBuffers(void);
    void UnmapUniformBuffers(void) const;
    void UpdateDescriptorSets(void);

    void BuildCommandBuffers(void);

    void CreateSemaphores(void);
    void CreateFences(void);

    void Update(void);

    vkpp::CommandBuffer BeginOneTimeCommandBuffer(void) const;
    void EndOneTimeCommandBuffer(const vkpp::CommandBuffer& aCmdBuffer) const;

    template <vkpp::ImageLayout OldLayout, vkpp::ImageLayout NewLayout>
    static void TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange,
        const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask);

public:
    TexturedCube(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~TexturedCube(void);

    void CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const;
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_TEXTURED_CUBE_H__
