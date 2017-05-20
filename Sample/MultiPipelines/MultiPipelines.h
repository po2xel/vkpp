#ifndef __VKPP_SAMPLE_MULTIPIPELINE_H__
#define __VKPP_SAMPLE_MULTIPIPELINE_H__



#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Base/ExampleBase.h>
#include <Window/Window.h>
#include <Window/WindowEvent.h>



namespace vkpp::sample
{



struct VertexData
{
    glm::vec3 inPosition;
    glm::vec3 inNormal;
    glm::vec2 inTexCoord;
    glm::vec3 inColor;

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
                vkpp::Format::eRGB32sFloat,         // format
                offsetof(VertexData, inPosition)    // offset
            },
            // Location 1: Normal
            {
                1,                                  // location
                0,                                  // binding
                vkpp::Format::eRGB32sFloat,         // format
                offsetof(VertexData, inNormal)      // offset
            },
            // Location 2: Texture Coordinates
            {
                2,                                  // location
                0,                                  // binding
                vkpp::Format::eRG32sFloat,          // format
                offsetof(VertexData, inTexCoord)      // offset
            },
            // Location 3: Color
            {
                3,                                  // location
                0,                                  // binding
                vkpp::Format::eRGB32sFloat,         // format
                offsetof(VertexData, inColor)       // offset
            }
        } };

        return lInputAttributeDescriptions;
    }
};



struct UniformBufferObject
{
    glm::mat4 projection;
    glm::mat4 modelView;
    glm::vec4 lightPos{ 0.0f, 2.0f, 1.0f, 0.0f };
};



class MultiPipelines;


struct Model
{
    constexpr static auto DefaultImporterFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;

    const MultiPipelines& multiPipelineSample;
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

    Model(const MultiPipelines& aMultiPipelineSample, const vkpp::LogicalDevice& aDevice, const vkpp::PhysicalDeviceMemoryProperties& aPhysicalDeviceMemProperties);

    void LoadMode(const std::string& aFilename, unsigned int aImporterFlags = DefaultImporterFlags);
};



class MultiPipelines : public ExampleBase, public CWindowEvent
{
private:
    Model mModel;
    vkpp::CommandPool mCmdPool;
    std::vector<vkpp::CommandBuffer> mCmdDrawBuffers;

    vkpp::Semaphore mPresentCompleteSemaphore;
    vkpp::Semaphore mRenderCompleteSemaphore;
    std::vector<vkpp::Fence> mWaitFences;

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
    UniformBufferObject mMVPMatrix;
    BufferResource mUniformBufferResource;

    void CreateCommandPool(void);
    void AllocateDrawCmdBuffers(void);

    void CreateSemaphores(void);
    void CreateFences(void);
    void CreateRenderPass(void);
    void CreateDepthResource(void);
    void CreateFramebuffer(void);
    void CreateUniformBuffer(void);
    void UpdateUniformBuffer(void);
    void CreateSetLayout(void);
    void CreatePipelineLayout(void);

    void CreateGraphicsPipelines(void);

    void CreateDescriptorPool(void);
    void AllocateDescriptorSet(void);
    void UpdateDescriptorSet(void) const;

    void BuildCommandBuffers(void);

    vkpp::CommandBuffer BeginOneTimeCommandBuffer(void) const;
    void EndOneTimeCommandBuffer(const vkpp::CommandBuffer& aCmdBuffer) const;

    void Update();

public:
    MultiPipelines(CWindow& aWindow, const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~MultiPipelines(void);

    void CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const;
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_MULTIPIPELINE_H__
