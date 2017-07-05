#ifndef __VKPP_SAMPLE_TEXT_RENDERING_H__
#define __VKPP_SAMPLE_TEXT_RENDERING_H__



#include <memory>
#include <Base/ExampleBase.h>
#include <Window/WindowEvent.h>
#include <Window/MouseEvent.h>

#include <stb_font/stb_font_consolas_24_latin1.inl>

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



struct TextVertexData
{
    glm::vec2 inPos;
    glm::vec2 inUV;

    constexpr static vkpp::VertexInputBindingDescription GetBindingDescription(void)
    {
        return {
            0, sizeof(TextVertexData), vkpp::VertexInputRate::eVertex
        };
    }

    constexpr static decltype(auto) GetAttributeDescriptions(void)
    {
        constexpr std::array<vkpp::VertexInputAttributeDescription, 2> lInputAttributeDescriptions
        { {
            // Location 0: inPos
            {
                0, 0,
                vkpp::Format::eRG32sFloat,
                offsetof(TextVertexData, inPos)
            },
            // Location 1: inUV
            {
                1, 0,
                vkpp::Format::eRG32sFloat,
                offsetof(TextVertexData, inUV)
            }
        } };

        return lInputAttributeDescriptions;
    }
};


class TextRenderer
{
public:
    enum class TextAlign
    {
        eLeft, eCenter, eRight
    };

private:
    constexpr static auto STB_FONT_NAME = stb_font_consolas_24_latin1;
    constexpr static auto STB_FONT_WIDTH = static_cast<uint32_t>(STB_FONT_consolas_24_latin1_BITMAP_WIDTH);
    constexpr static auto STB_FONT_HEIGHT = static_cast<uint32_t>(STB_FONT_consolas_24_latin1_BITMAP_HEIGHT);
    constexpr static auto STB_FIRST_CHAR = STB_FONT_consolas_24_latin1_FIRST_CHAR;
    constexpr static auto STB_NUM_CHARS = STB_FONT_consolas_24_latin1_NUM_CHARS;

    // Maximum number of chars the text overlay buffer can hold.
    constexpr static auto TEXTOVERLAY_MAX_CHAR_COUNT = 2048;

    const DeviceQueue& mGraphicsQueue;
    const vkpp::LogicalDevice& mLogicalDevice;
    const vkpp::PhysicalDeviceMemoryProperties& mPhysicalDeviceMemoryProperties;
    const std::vector<vkpp::Framebuffer>& mFramebuffers;
    const vkpp::Format mColorFormat;
    const uint32_t mFramebufferWidth, mFramebufferHeight;

    vkpp::CommandPool mCmdPool;
    std::vector<vkpp::CommandBuffer> mDrawCmdBuffers;
    vkpp::RenderPass mRenderPass;

    vkpp::DescriptorSetLayout mTextSetLayout;
    vkpp::PipelineLayout mTextPipelineLayout;
    vkpp::Pipeline mTextRenderingPipeline;

    vkpp::DescriptorPool mDescriptorPool;
    vkpp::DescriptorSet mDescriptorSet;

    BufferResource mBufferRes;
    ImageResource mImgRes;
    vkpp::Sampler mSampler;

    // Pointer to mapped vertex buffer.
    TextVertexData* mpMapped{ nullptr };

    stb_fontchar mSTBFontData[STB_NUM_CHARS];
    uint32_t mNumLetters{ 0 };

    vkpp::Semaphore mRenderingCompleteSemaphore;
    vkpp::Semaphore mPresentCompleteSemaphore;
    std::vector<vkpp::Fence> mWaitFences;

    void CreateCmdPool(void);
    void AllocateCmdBuffers(void);
    void CreateRenderPass(void);

    void CreateSetLayouts(void);
    void CreatePipelineLayouts(void);
    void CreateGraphicsPipeline(void);

    void CreateDescriptorPool(void);
    void AllocateDescriptorSet(void);

    void CreateVertexBuffer(void);
    void CreateFontTexture(void);
    void CreateSampler(void);
    void UpdateDescriptorSets(void) const;

    void BuildCmdBuffers(void);

    void CreateSemaphores(void);
    void CreateFences(void);

    vkpp::ShaderModule CreateShaderModule(const std::string& aFilename) const;

    template <vkpp::ImageLayout OldLayout, vkpp::ImageLayout NewLayout>
    static void TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange,
        const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask);

    vkpp::CommandBuffer BeginOneTimeCmdBuffer(void) const;
    void EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const;

public:
    // TextRenderer(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    TextRenderer(const DeviceQueue& aGraphicsQueue, const LogicalDevice& aLogicalDevice, const vkpp::PhysicalDeviceMemoryProperties& aPhysicalDeviceMemoryProperties, const std::vector<vkpp::Framebuffer>& aFramebuffers,
        const vkpp::Format aColorFormat, const uint32_t aFramebufferWidth, const uint32_t aFramebufferHeight);
    virtual ~TextRenderer(void);

    void BeginTextUpdate(void);
    void EndTextUpdate(void);
    void AddText(const std::string& aText, float aX, float aY, TextAlign aAlign);
    void Submit(uint32_t aBufferIndex);
};



struct VertexData
{
    glm::vec3 inPos;
    glm::vec3 inNormal;
    glm::vec2 inUV;

    constexpr static vkpp::VertexInputBindingDescription GetBindingDescription(void)
    {
        return {
            0, sizeof(VertexData), vkpp::VertexInputRate::eVertex
        };
    }

    constexpr static decltype(auto) GetAttributeDescriptions(void)
    {
        constexpr std::array<vkpp::VertexInputAttributeDescription, 3> lInputAttributeDescriptions
        { {
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
            }
            } };

        return lInputAttributeDescriptions;
    }
};



struct UboScene
{
    glm::mat4 proj;
    glm::mat4 model;
    glm::vec4 lightPos{ 0.0f, 0.0f, -2.0f, 1.0f };
};


class SceneRenderer : public ExampleBase, private CWindowEvent, private CMouseMotionEvent, private CMouseWheelEvent
{
private:
    constexpr static float MINIMUM_ZOOM_LEVEL = 0.05f;

    const std::string mTile;

    std::unique_ptr<TextRenderer> mTextRenderer;

    vkpp::CommandPool mCmdPool;
    std::vector<vkpp::CommandBuffer> mDrawCmdBuffers;
    vkpp::RenderPass mRenderPass;
    ImageResource mDepthRes;
    std::vector<vkpp::Framebuffer> mFramebuffers;

    struct
    {
        vkpp::DescriptorSetLayout background;
        vkpp::DescriptorSetLayout cube;
    } mSetLayouts;

    struct
    {
        vkpp::PipelineLayout background;
        vkpp::PipelineLayout cube;
    } mPipelineLayouts;

    struct
    {
        vkpp::Pipeline background;
        vkpp::Pipeline cube;
    } mPipelines;

    vkpp::DescriptorPool mDescriptorPool;

    struct
    {
        vkpp::DescriptorSet background;
        vkpp::DescriptorSet cube;
    } mDescriptorSets;

    BufferResource mUboScene;
    void* mpUboSceneMapped{ nullptr };

    UboScene mSceneMatrix;
    float mCurrentZoomLevel{ -4.5f };
    glm::vec3 mCurrentRotation{ -25.0f, 0.0f, 0.0f };

    struct Texture
    {
        vkpp::ImageLayout layout{ vkpp::ImageLayout::eShaderReadOnlyOptimal };

        uint32_t width{ 0 }, height{ 0 }, mipLevels{ 0 };
    };

    Texture mBackground, mCube;

    ImageResource mBkTexRes;
    ImageResource mCubeTexRes;

    vkpp::Sampler mTextureSampler;

    BufferResource mVtxBufferRes;
    BufferResource mIdxBufferRes;
    uint32_t mIndexCount{ 0 };

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
    void CreateGraphicsPipelines(void);

    void CreateDescriptorPool(void);
    void AllocateDescriptorSets(void);

    void CreateUniformBuffers(void);
    void UpdateUniformBuffers(void);

    void LoadTexture(const std::string& aFilename, vkpp::Format aTexFormat, Texture& aTexture, ImageResource& aTexRes) const;
    void CreateSamplers(void);
    void UpdateDescriptorSets(void);

    void LoadModel(const std::string& aFilename);
    void CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const;

    void BuildCmdBuffers(void);

    void CreateSemaphores(void);
    void CreateFences(void);

    void UpdateTextOverlay(void);
    void Update(void);

    template <vkpp::ImageLayout OldLayout, vkpp::ImageLayout NewLayout>
    static void TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange,
        const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask);

    vkpp::CommandBuffer BeginOneTimeCmdBuffer(void) const;
    void EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const;

public:
    SceneRenderer(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~SceneRenderer(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_TEXT_RENDERING_H__
