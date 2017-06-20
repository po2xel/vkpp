#ifndef __VKPP__SAMPLE_RADIAL_BLUR_H__
#define __VKPP__SAMPLE_RADIAL_BLUR_H__



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

#include <gli/gli.hpp>



namespace vkpp::sample
{



struct VertexData
{
    glm::vec3 inPos;
    glm::vec3 inColor;
    glm::vec3 inNormal;

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
            // Location 0: Position
            {
                0, 0,
                vkpp::Format::eRGB32sFloat,
                offsetof(VertexData, inPos)
            },
            // Location 1: inColor
            {
                1, 0,
                vkpp::Format::eRGB32sFloat,
                offsetof(VertexData, inColor)
            },
            // Location 2: inNormal
            {
                2, 0,
                vkpp::Format::eRGB32sFloat,
                offsetof(VertexData, inNormal)
            }
        } };

        return lInputAttributeDescriptions;
    }
};



struct UBOScene
{
    glm::mat4 proj;
    glm::mat4 model;
    float gradientPos = 0.0f;
};



struct UBOBlurParams
{
    float radialBlurScale = 0.35f;
    float radialBlurStrength = 0.75f;
    glm::vec2 radialOrigin{ 0.5f, 0.5f };
};



// Setup the offscreen framebuffer for rendering the mirrored scene.
// The color attachment of this framebuffer will then be used to sample from in the fragment shader of the final pass.
struct VFrame
{
    const vkpp::LogicalDevice& mDevice;

    ImageResource color, depth;
    vkpp::Sampler sampler;

    uint32_t width{ 0 }, height{ 0 };
    vkpp::Framebuffer framebuffer;
    vkpp::RenderPass renderpass;
    vkpp::CommandBuffer cmdbuffer;

    // Semaphore used to synchronize between offscreen and final render pass.
    vkpp::Semaphore semaphore;

    VFrame(const vkpp::LogicalDevice& aDevice, const vkpp::PhysicalDeviceMemoryProperties& aPhysicalDeviceMemoryProperties,
        uint32_t aWidth, uint32_t aHeight, vkpp::Format aColorFormat, vkpp::Format aDepthFormat)
        : mDevice(aDevice), color(aDevice, aPhysicalDeviceMemoryProperties), depth(aDevice, aPhysicalDeviceMemoryProperties), width(aWidth), height(aHeight)
    {
        // Color attachment
        vkpp::ImageCreateInfo lImageCreateInfo
        {
            vkpp::ImageType::e2D,
            aColorFormat,
            { width, height, 1 },
            vkpp::ImageUsageFlagBits::eColorAttachment | vkpp::ImageUsageFlagBits::eSampled         // We will sample directly from the color attachment
        };

        vkpp::ImageViewCreateInfo lImageViewCreateInfo
        {
            vkpp::ImageViewType::e2D,
            aColorFormat,
            { vkpp::ImageAspectFlagBits::eColor, 0, 1 }
        };

        color.Reset(lImageCreateInfo, lImageViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

        // Create sampler to sample from the attachment in the fragment shader.
        CreateSampler();

        // Depth attachment
        lImageCreateInfo.format = aDepthFormat;
        lImageCreateInfo.usage = vkpp::ImageUsageFlagBits::eDepthStencilAttachment;

        lImageViewCreateInfo.format = aDepthFormat;
        lImageViewCreateInfo.subresourceRange.aspectMask = vkpp::ImageAspectFlagBits::eDepth;

        depth.Reset(lImageCreateInfo, lImageViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

        // Create a separate render pass for the offscreen rendering as it may differ from the one used for the one used for scene rendering.
        CreateRenderPass(aColorFormat, aDepthFormat);

        CreateFramebuffer();
    }

    ~VFrame(void)
    {
        mDevice.Wait();

        mDevice.DestroyFramebuffer(framebuffer);

        mDevice.DestroyRenderPass(renderpass);
        depth.Reset();
        color.Reset();

        mDevice.DestroySampler(sampler);
    }

    void CreateSampler(void)
    {
        vkpp::SamplerCreateInfo lSamplerCreateInfo
        {
            vkpp::Filter::eLinear, vkpp::Filter::eLinear,
            vkpp::SamplerMipmapMode::eLinear,
            vkpp::SamplerAddressMode::eClampToEdge, vkpp::SamplerAddressMode::eClampToEdge, vkpp::SamplerAddressMode::eClampToEdge,
            0.0f,
            vkpp::Anisotropy::Disable, 0.0f,
            vkpp::Compare::Disable, vkpp::CompareOp::eLess,
            0.0f, 1.0f,
            vkpp::BorderColor::eFloatOpaqueWhite
        };

        sampler = mDevice.CreateSampler(lSamplerCreateInfo);
    }

    void CreateRenderPass(vkpp::Format aColorFormat, vkpp::Format aDepthFormat)
    {
        const std::vector<vkpp::AttachementDescription> lAttachementDescriptions
        {
            // Depth attachment
            {
                aDepthFormat,
                vkpp::SampleCountFlagBits::e1,
                vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eDontCare,
                vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
            },
            // Color attachment
            {
                aColorFormat,
                vkpp::SampleCountFlagBits::e1,
                vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eStore,
                vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eShaderReadOnlyOptimal
            }
        };

        constexpr vkpp::AttachmentReference lColorRef{ 1, vkpp::ImageLayout::eColorAttachmentOptimal };
        constexpr vkpp::AttachmentReference lDepthRef{ 0, vkpp::ImageLayout::eDepthStencilAttachmentOptimal };

        const std::vector<vkpp::SubpassDescription> lSubpassDescriptions
        {
            { vkpp::PipelineBindPoint::eGraphics, lColorRef, lDepthRef }
        };

        // Use subpass dependencies for layout transitions
        constexpr std::array<vkpp::SubpassDependency, 2> lSubpassDependencies
        { {
            {
                vkpp::subpass::External, 0,
                vkpp::PipelineStageFlagBits::eBottomOfPipe, vkpp::PipelineStageFlagBits::eColorAttachmentOutput,
                vkpp::AccessFlagBits::eMemoryRead, vkpp::AccessFlagBits::eColorAttachmentWrite | vkpp::AccessFlagBits::eColorAttachmentRead,
                vkpp::DependencyFlagBits::eByRegion
            },
            {
                0, vkpp::subpass::External,
                vkpp::PipelineStageFlagBits::eColorAttachmentOutput, vkpp::PipelineStageFlagBits::eBottomOfPipe,
                vkpp::AccessFlagBits::eColorAttachmentWrite | vkpp::AccessFlagBits::eColorAttachmentRead, vkpp::AccessFlagBits::eMemoryRead,
                vkpp::DependencyFlagBits::eByRegion
            }
            } };

        const vkpp::RenderPassCreateInfo lRenderPassCreateInfo
        {
            lAttachementDescriptions, lSubpassDescriptions, lSubpassDependencies
        };

        renderpass = mDevice.CreateRenderPass(lRenderPassCreateInfo);
    }

    void CreateFramebuffer(void)
    {
        const std::array<vkpp::ImageView, 2> lAttachments{ depth.view, color.view };

        const vkpp::FramebufferCreateInfo lFramebufferCreateInfo
        {
            renderpass,
            lAttachments,
            width, height
        };

        framebuffer = mDevice.CreateFramebuffer(lFramebufferCreateInfo);
    }

    vkpp::CommandBuffer& AllocateCmdBuffer(const vkpp::CommandPool& aCmdPool)
    {
        cmdbuffer = mDevice.AllocateCommandBuffer({ aCmdPool });

        return cmdbuffer;
    }
};



class RadialBlur;

struct Model
{
    const vkpp::LogicalDevice& mLogicalDevice;
    const vkpp::PhysicalDeviceMemoryProperties& mPhysicalDeviceMemoryProperties;
    const RadialBlur& mRenderer;

    BufferResource mVtxBufferRes, mIdxBufferRes;
    uint32_t mIndexCount{ 0 };

    Model(const vkpp::LogicalDevice& aDeivce, const RadialBlur& aRenderer, const vkpp::PhysicalDeviceMemoryProperties& aPhysicalDeviceMemoryProperties)
        : mLogicalDevice(aDeivce), mPhysicalDeviceMemoryProperties(aPhysicalDeviceMemoryProperties), mRenderer(aRenderer),
        mVtxBufferRes(aDeivce, aPhysicalDeviceMemoryProperties), mIdxBufferRes(aDeivce, aPhysicalDeviceMemoryProperties)
    {}

    ~Model(void)
    {
        mLogicalDevice.Wait();

        mIdxBufferRes.Reset();
        mVtxBufferRes.Reset();
    }

    void LoadModel(const std::string& aFilename, float aScale);
};



class RadialBlur : public ExampleBase, private CWindowEvent, private CMouseMotionEvent, private CMouseWheelEvent
{
private:
    constexpr static float MINIMUM_ZOOM_LEVEL = 0.05f;

    vkpp::CommandPool mCmdPool;
    std::vector<vkpp::CommandBuffer> mDrawCmdBuffers;
    vkpp::RenderPass mRenderPass;
    ImageResource mDepthRes;
    std::vector<vkpp::Framebuffer> mFramebuffers;

    VFrame mOffscreenFrame;

    struct
    {
        vkpp::DescriptorSetLayout radialBlur;
        vkpp::DescriptorSetLayout scene;
    } mSetLayouts;

    struct
    {
        vkpp::PipelineLayout radialBlur;
        vkpp::PipelineLayout scene;
    } mPipelineLayouts;

    vkpp::DescriptorPool mDescriptorPool;

    struct 
    {
        vkpp::Pipeline radialBlur;
        vkpp::Pipeline colorPass;
        vkpp::Pipeline phongPass;
        vkpp::Pipeline offscreenDisplay;
    } mPipelines;

    struct
    {
        vkpp::DescriptorSet radialBlur;
        vkpp::DescriptorSet scene;
    } mDescriptorSets;

    struct Texture
    {
        vkpp::ImageLayout layout{ vkpp::ImageLayout::eShaderReadOnlyOptimal };

        uint32_t width{ 0 }, height{ 0 }, mipLevels{ 0 };
    };

    BufferResource mUboScene;
    BufferResource mUboBlurParams;
    void* mpUboSceneMapped{ nullptr };
    void* mpUboBlurParams{ nullptr };

    UBOScene mSceneMatrix;
    UBOBlurParams mBlurParams;

    Model mExample;
    Texture mTexture;
    ImageResource mTextureRes;
    vkpp::Sampler mTextureSampler;

    vkpp::Semaphore mRenderingCompleteSemaphore;
    vkpp::Semaphore mPresentCompleteSemaphore;
    vkpp::Semaphore mOffScreenCompleteSemaphore;
    std::vector<vkpp::Fence> mWaitFences;

    float mCurrentZoomLevel{ -10.0f };
    glm::vec3 mCurrentRotation{ -16.25f, -28.75f, 0.0f };

    void CreateCmdPool(void);
    void AllocateCmdBuffers(void);
    void CreateRenderPass(void);
    void CreateDepthResource(void);
    void CreateFramebuffers(void);

    void CreateSetLayouts(void);
    void CreatePipelineLayouts(void);
    void CreateGraphicsPipeline(void);
    void CreateDescriptorPool(void);
    void AllocateDescriptorSets(void);

    void CreateUniformBuffers(void);
    void UpdateUniformBuffers(void);
    void UpdateDescriptorSets(void);

    void LoadTexture(const std::string& aFilename, vkpp::Format aTexFormat);
    void CreateSampler(void);

    void BuildCmdBuffers(void);
    void BuildOffscreenCmdBuffer(void);

    void CreateSemaphores(void);
    void CreateFences(void);

    void Update(void);

    template <vkpp::ImageLayout OldLayout, vkpp::ImageLayout NewLayout>
    static void TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange,
        const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask);

    vkpp::CommandBuffer BeginOneTimeCmdBuffer(void) const;
    void EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const;

public:
    RadialBlur(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~RadialBlur(void);

    void CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const;
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP__SAMPLE_RADIAL_BLUR_H__
