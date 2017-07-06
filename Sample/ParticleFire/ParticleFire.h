#ifndef __VKPP_SAMPLE_PARTICLE_FIRE_H__
#define __VKPP_SAMPLE_PARTICLE_FIRE_H__



#include <Base/ExampleBase.h>
#include <Window/WindowEvent.h>
#include <window/MouseEvent.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <gli/gli.hpp>



namespace vkpp::sample
{



struct ParticleVtxData
{
    glm::vec4 inPos;
    glm::vec4 inColor;
    float inAlpha;
    float inSize;
    float inRotation;
    int32_t inType;

    // Attributes not used in shader
    glm::vec4 vel;
    float rotationSpeed{ 0.f };

    constexpr static vkpp::VertexInputBindingDescription GetBindingDescription(void)
    {
        return {
            0, sizeof(ParticleVtxData), vkpp::VertexInputRate::eVertex
        };
    }

    constexpr static decltype(auto) GetAttributeDescriptions(void)
    {
        constexpr std::array<vkpp::VertexInputAttributeDescription, 6> lInputAttributeDescriptions
        { {
            // Location 0: inPos
            {
                0, 0,
                vkpp::Format::eRGBA32sFloat,
                offsetof(ParticleVtxData, inPos)
            },
            // Location 1: inColor
            {
                1, 0,
                vkpp::Format::eRGBA32sFloat,
                offsetof(ParticleVtxData, inColor)
            },
            // Location 2: inAlpha
            {
                2, 0,
                vkpp::Format::eR32sFloat,
                offsetof(ParticleVtxData, inAlpha)
            },
            // Location 3: inSize
            {
                3, 0,
                vkpp::Format::eR32sFloat,
                offsetof(ParticleVtxData, inSize)
            },
            // Location 4: inRotation
            {
                4, 0,
                vkpp::Format::eR32sFloat,
                offsetof(ParticleVtxData, inRotation)
            },
            // Location 5: inType
            {
                5, 0,
                vkpp::Format::eR32sInt,
                offsetof(ParticleVtxData, inType)
            }
        } };

        return lInputAttributeDescriptions;
    }
};



struct NormalMapVtxData
{
    glm::vec3 inPos;
    glm::vec2 inUV;
    glm::vec3 inNormal;
    glm::vec3 inTangent;
    glm::vec3 inBiTangent;

    constexpr static vkpp::VertexInputBindingDescription GetBindingDescription(void)
    {
        return {
            0, sizeof(NormalMapVtxData), vkpp::VertexInputRate::eVertex
        };
    }

    constexpr static decltype(auto) GetAttributeDescriptions(void)
    {
        constexpr std::array<vkpp::VertexInputAttributeDescription, 5> lInputAttributeDescriptions
        { {
            // Binding 0: inPos
            {
                0, 0,
                vkpp::Format::eRGB32sFloat,
                offsetof(NormalMapVtxData, inPos)
            },
            // Binding 1: inUV
            {
                1, 0,
                vkpp::Format::eRG32sFloat,
                offsetof(NormalMapVtxData, inUV)
            },
            // Binding 2: inNormal
            {
                2, 0,
                vkpp::Format::eRGB32sFloat,
                offsetof(NormalMapVtxData, inNormal)
            },
            // Binding 3: inTangent
            {
                3, 0,
                vkpp::Format::eRGB32sFloat,
                offsetof(NormalMapVtxData, inTangent)
            },
            // Binding 4: inBiTangent
            {
                4, 0,
                vkpp::Format::eRGB32sFloat,
                offsetof(NormalMapVtxData, inBiTangent)
            }
        } };

        return lInputAttributeDescriptions;
    }
};



struct UboEnv
{
    glm::mat4 proj;
    glm::mat4 model;
    glm::mat4 normal;
    glm::vec4 lightPos;
    glm::vec4 cameraPos;
};



struct UboVS
{
    glm::mat4 proj;
    glm::mat4 model;
    glm::vec2 viewportDim;
    float pointSize = 10.0f;
};



class ParticleFire : public ExampleBase, private CWindowEvent, private CMouseMotionEvent, private CMouseWheelEvent
{
private:
    constexpr static float MINIMUM_ZOOM_LEVEL = 0.05f;
    constexpr static auto PARTICLE_COUNT = 512;
    constexpr static auto PARTICLE_SIZE = 10.f;
    constexpr static auto FLAME_RADIUS = 8.f;
    constexpr static int32_t PARTICLE_TYPE_FLAME = 0;
    constexpr static int32_t PARTICLE_TYPE_SMOKE = 1;

    vkpp::CommandPool mCmdPool;
    std::vector<vkpp::CommandBuffer> mDrawCmdBuffers;
    vkpp::RenderPass mRenderPass;

    ImageResource mDepthRes;
    std::vector<vkpp::Framebuffer> mFramebuffers;
    vkpp::DescriptorSetLayout mSetLayout;
    vkpp::PipelineLayout mPipelineLayout;

    struct 
    {
        vkpp::Pipeline particle;
        vkpp::Pipeline normalmap;
    } mPipelines;

    vkpp::DescriptorPool mDescriptorPool;

    struct
    {
        vkpp::DescriptorSet particle;
        vkpp::DescriptorSet normalmap;
    } mDescriptorSets;

    struct Texture
    {
        vkpp::ImageLayout layout{ vkpp::ImageLayout::eShaderReadOnlyOptimal };

        uint32_t width{ 0 }, height{ 0 }, mipLevels{ 0 };
    };

    Texture mTexSmoke, mTexFire;
    Texture mTexColorMap, mTexNormalMap;

    ImageResource mSmokeTexRes, mFireTexRes;
    ImageResource mColorMapTexRes, mNormapMapTexRes;

    vkpp::Sampler mTextureSampler;
    BufferResource mUboEnv;
    BufferResource mUboVS;
    UboEnv mEnvMatrix;
    UboVS mVSMatrix;

    void* mpUboEnvMapped{ nullptr };
    void* mpUboVSMapped{ nullptr };

    BufferResource mVtxBufferRes;
    BufferResource mIdxBufferRes;
    uint32_t mIndexCount{ 0 };

    const glm::vec3 mEmitterPos{ 0.f, -FLAME_RADIUS + 2.f, 0.f };
    const glm::vec3 mMinVel{ -3.f, 0.5f, -3.f };
    const glm::vec3 mMaxVel{ 3.f, 7.f, 3.f };

    BufferResource mParticleBufferRes;
    void* mpParticleMapped{ nullptr };
    std::array<ParticleVtxData, PARTICLE_COUNT> mParticleBuffer;
    constexpr static std::size_t PARTICLE_BUFFER_SIZE = PARTICLE_COUNT * sizeof(ParticleVtxData);

    float mCurrentZoomLevel{ -100.f };
    glm::vec3 mCurrentRotation{ -15.f, 45.f, 0.0f };

    vkpp::Semaphore mRenderingCompleteSemaphore;
    vkpp::Semaphore mPresentCompleteSemaphore;
    std::vector<vkpp::Fence> mWaitFences;

    void CreateCmdPool(void);
    void AllocateCmdBuffers(void);
    void CreateRenderPass(void);
    void CreateDepthRes(void);
    void CreateFramebuffers(void);
    void CreateSetLayouts(void);
    void CreatePipelineLayout(void);
    void CreatePipelines(void);

    void CreateDescriptorPool(void);
    void AllocateDescriptorSets(void);

    void LoadTexture(const std::string& aFilename, vkpp::Format aTexFormat, Texture& aTexture, ImageResource& aTexRes) const;

    void CreateSampler(void);
    void CreateUniformBuffers(void);
    void UpdateUniformBuffers(void);
    void UpdateDescriptorSets(void);

    void BuildCmdBuffers(void);

    void CreateSemaphores(void);
    void CreateFences(void);

    void Update(void);

    void LoadModel(const std::string& aFilename);
    void CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const;

    void InitParticle(ParticleVtxData& aParticle, const glm::vec3& aEmitterPos) const;
    void CreateParticles(void);
    void TransitionParticle(ParticleVtxData& aParticle) const;
    void UpdateParticles(void);

    template <vkpp::ImageLayout OldLayout, vkpp::ImageLayout NewLayout>
    static void TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange,
        const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask);

    vkpp::CommandBuffer BeginOneTimeCmdBuffer(void) const;
    void EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const;

public:
    ParticleFire(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~ParticleFire(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_PARTICLE_FIRE_H__
