#include "TextRendering.h"
#include <fstream>
#include <sstream>
#include <iomanip>



namespace vkpp::sample
{



TextRenderer::TextRenderer(const DeviceQueue& aGraphicsQueue, const LogicalDevice& aLogicalDevice, const vkpp::PhysicalDeviceMemoryProperties& aPhysicalDeviceMemoryProperties, const std::vector<vkpp::Framebuffer>& aFramebuffers,
        const vkpp::Format aColorFormat, const uint32_t aFramebufferWidth, const uint32_t aFramebufferHeight)
    : mGraphicsQueue(aGraphicsQueue), mLogicalDevice(aLogicalDevice), mPhysicalDeviceMemoryProperties(aPhysicalDeviceMemoryProperties), mFramebuffers(aFramebuffers), mColorFormat(aColorFormat),
      mFramebufferWidth(aFramebufferWidth), mFramebufferHeight(aFramebufferHeight),
      mBufferRes(aLogicalDevice, aPhysicalDeviceMemoryProperties),
      mImgRes(aLogicalDevice, aPhysicalDeviceMemoryProperties)
{
    CreateCmdPool();
    AllocateCmdBuffers();
    CreateRenderPass();

    CreateSetLayouts();
    CreatePipelineLayouts();
    CreateGraphicsPipeline();

    CreateDescriptorPool();
    AllocateDescriptorSet();

    CreateVertexBuffer();
    CreateFontTexture();
    CreateSampler();
    UpdateDescriptorSets();

    CreateSemaphores();
    CreateFences();
}


TextRenderer::~TextRenderer(void)
{
    mLogicalDevice.Wait();

    mLogicalDevice.DestroySemaphore(mRenderingCompleteSemaphore);
    mLogicalDevice.DestroySemaphore(mPresentCompleteSemaphore);

    mLogicalDevice.DestroyFences(mWaitFences);

    mLogicalDevice.DestroySampler(mSampler);

    mImgRes.Reset();
    mBufferRes.Reset();

    mLogicalDevice.DestroyDescriptorPool(mDescriptorPool);

    mLogicalDevice.DestroyPipeline(mTextRenderingPipeline);
    mLogicalDevice.DestroyPipelineLayout(mTextPipelineLayout);
    mLogicalDevice.DestroyDescriptorSetLayout(mTextSetLayout);

    mLogicalDevice.DestroyFramebuffers(mFramebuffers);
    mLogicalDevice.DestroyRenderPass(mRenderPass);
    mLogicalDevice.FreeCommandBuffers(mCmdPool, mDrawCmdBuffers);
    mLogicalDevice.DestroyCommandPool(mCmdPool);
}


void TextRenderer::CreateCmdPool(void)
{
    mCmdPool = mLogicalDevice.CreateCommandPool({ mGraphicsQueue.familyIndex, vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer });
}


void TextRenderer::AllocateCmdBuffers(void)
{
    const vkpp::CommandBufferAllocateInfo lCmdBufferAllocateInfo
    {
        mCmdPool,
        static_cast<uint32_t>(mFramebuffers.size())
    };

    mDrawCmdBuffers = mLogicalDevice.AllocateCommandBuffers(lCmdBufferAllocateInfo);
}


// Create a render pass for rendering the text as an overlay.
void TextRenderer::CreateRenderPass(void)
{
    const std::vector<vkpp::AttachementDescription> lAttachementDescriptions
    {
        // Color attachment
        {
            mColorFormat,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eLoad,          // Don't clear the framebuffer
            vkpp::AttachmentStoreOp::eStore,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::ePresentSrcKHR
        },
        // Depth attachment
        {
            vkpp::Format::eD32sFloat,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eDontCare,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
        }
    };

    const vkpp::AttachmentReference lColorRef{ 0, vkpp::ImageLayout::eColorAttachmentOptimal };
    const vkpp::AttachmentReference lDepthRef{ 1, vkpp::ImageLayout::eDepthStencilAttachmentOptimal };

    constexpr std::array<vkpp::SubpassDescription, 1> lSubpassDescriptions
    { {
        { vkpp::PipelineBindPoint::eGraphics, 0, nullptr, 1, lColorRef.AddressOf(), nullptr, nullptr /*lDepthRef*/ }
    } };

    const std::vector<vkpp::SubpassDependency> lSubpassDependencies
    {
        // Transition from final to initial (External refers to all commands executed outside of the actual renderpass).
        {
            vkpp::subpass::External, 0,
            vkpp::PipelineStageFlagBits::eBottomOfPipe, vkpp::PipelineStageFlagBits::eColorAttachmentOutput,
            vkpp::AccessFlagBits::eMemoryRead, vkpp::AccessFlagBits::eColorAttachmentWrite | vkpp::AccessFlagBits::eColorAttachmentRead,
            vkpp::DependencyFlagBits::eByRegion
        },
        // Transition from initial to final.
        {
            0, vkpp::subpass::External,
            vkpp::PipelineStageFlagBits::eColorAttachmentOutput, vkpp::PipelineStageFlagBits::eBottomOfPipe,
            vkpp::AccessFlagBits::eColorAttachmentWrite | vkpp::AccessFlagBits::eColorAttachmentRead, vkpp::AccessFlagBits::eMemoryRead,
            vkpp::DependencyFlagBits::eByRegion
        }
    };

    const vkpp::RenderPassCreateInfo lRenderPassCreateInfo
    {
        lAttachementDescriptions, lSubpassDescriptions, lSubpassDependencies
    };

    mRenderPass = mLogicalDevice.CreateRenderPass(lRenderPassCreateInfo);
}


void TextRenderer::CreateSetLayouts(void)
{
    const std::vector<vkpp::DescriptorSetLayoutBinding> lSetLayoutBindings
    {
        // Binding 0: Fragment shader image sampler.
        {
            0,
            vkpp::DescriptorType::eCombinedImageSampler,
            vkpp::ShaderStageFlagBits::eFragment
        }
    };

    mTextSetLayout = mLogicalDevice.CreateDescriptorSetLayout(lSetLayoutBindings);
}


void TextRenderer::CreatePipelineLayouts(void)
{
    mTextPipelineLayout = mLogicalDevice.CreatePipelineLayout({ mTextSetLayout });
}


void TextRenderer::CreateGraphicsPipeline(void)
{
    const auto& lVertexShaderModule = CreateShaderModule("Shaders/SPV/text.vert.spv");
    const auto& lFragmentShaderModule = CreateShaderModule("Shaders/SPV/text.frag.spv");

    const std::vector<vkpp::PipelineShaderStageCreateInfo> lShaderStageCreateInfos
    {
        { vkpp::ShaderStageFlagBits::eVertex, lVertexShaderModule },
        { vkpp::ShaderStageFlagBits::eFragment, lFragmentShaderModule }
    };

    constexpr auto lVertexInputBinding = TextVertexData::GetBindingDescription();
    constexpr auto lVertexAttributes = TextVertexData::GetAttributeDescriptions();

    const vkpp::PipelineVertexInputStateCreateInfo lVertexInputStateCreateInfo{ lVertexAttributes, lVertexInputBinding };
    constexpr vkpp::PipelineInputAssemblyStateCreateInfo lInputAssemblyStateCreateInfo{ vkpp::PrimitiveTopology::eTriangleStrip };
    constexpr vkpp::PipelineViewportStateCreateInfo lViewportStateCreateInfo{ 1, 1 };

    constexpr vkpp::PipelineRasterizationStateCreateInfo lRasterizationStateCreateInfo
    {
        vkpp::PolygonMode::eFill,
        vkpp::CullModeFlagBits::eBack,
        vkpp::FrontFace::eClockwise
    };

    constexpr vkpp::PipelineMultisampleStateCreateInfo lMultisampleStateCreateInfo;

    constexpr vkpp::PipelineDepthStencilStateCreateInfo lDepthStencilStateCreateInfo
    {
        vkpp::DepthTest::Disable, vkpp::DepthWrite::Disable,
        vkpp::CompareOp::eLessOrEqual
    };

    // Enable blending
    constexpr vkpp::PipelineColorBlendAttachmentState lColorBlendAttachmentState
    { 
        vkpp::Blend::Enable,
        vkpp::BlendFactor::eOne, vkpp::BlendFactor::eOne, vkpp::BlendOp::eAdd,
        vkpp::BlendFactor::eOne, vkpp::BlendFactor::eOne, vkpp::BlendOp::eAdd,
        vkpp::IdentityColorComponents
    };

    constexpr vkpp::PipelineColorBlendStateCreateInfo lColorBlendStateCreateInfo{ lColorBlendAttachmentState };

    constexpr std::array<vkpp::DynamicState, 2> lDynamicStates
    {
        vkpp::DynamicState::eViewport, vkpp::DynamicState::eScissor
    };

    const vkpp::PipelineDynamicStateCreateInfo lDynamicStateCreateInfo{ lDynamicStates };

    const vkpp::GraphicsPipelineCreateInfo lGraphicsPipelineCreateInfo
    {
        2, lShaderStageCreateInfos.data(),
        lVertexInputStateCreateInfo.AddressOf(),
        lInputAssemblyStateCreateInfo.AddressOf(),
        nullptr,
        lViewportStateCreateInfo.AddressOf(),
        lRasterizationStateCreateInfo.AddressOf(),
        lMultisampleStateCreateInfo.AddressOf(),
        lDepthStencilStateCreateInfo.AddressOf(),
        lColorBlendStateCreateInfo.AddressOf(),
        lDynamicStateCreateInfo.AddressOf(),
        mTextPipelineLayout,
        mRenderPass,
        0
    };

    mTextRenderingPipeline = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);
}


void TextRenderer::CreateDescriptorPool(void)
{
    constexpr vkpp::DescriptorPoolSize lPoolSize
    {
        vkpp::DescriptorType::eCombinedImageSampler, 1
    };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool({ lPoolSize, 1 });
}


void TextRenderer::AllocateDescriptorSet(void)
{
    mDescriptorSet = mLogicalDevice.AllocateDescriptorSet({ mDescriptorPool, mTextSetLayout });
}


void TextRenderer::CreateVertexBuffer(void)
{
    constexpr vkpp::DeviceSize lBufferSize = TEXTOVERLAY_MAX_CHAR_COUNT * sizeof(TextVertexData);
    constexpr vkpp::BufferCreateInfo lBufferCreateInfo{ lBufferSize, vkpp::BufferUsageFlagBits::eVertexBuffer };

    mBufferRes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);
}


void TextRenderer::CreateFontTexture(void)
{
    constexpr vkpp::ImageCreateInfo lImageCreateInfo
    {
        vkpp::ImageType::e2D,
        vkpp::Format::eR8uNorm,
        { STB_FONT_WIDTH, STB_FONT_HEIGHT, 1 },
        vkpp::ImageUsageFlagBits::eSampled | vkpp::ImageUsageFlagBits::eTransferDst,
        vkpp::ImageLayout::eUndefined,
        vkpp::ImageTiling::eOptimal,
        vkpp::SampleCountFlagBits::e1,
        1, 1
    };

    vkpp::ImageViewCreateInfo lImageViewCreateInfo
    {
        vkpp::ImageViewType::e2D,
        vkpp::Format::eR8uNorm,
        {
            vkpp::ImageAspectFlagBits::eColor,
            0, 1
        }
    };

    mImgRes.Reset(lImageCreateInfo, lImageViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    // Staging
    BufferResource lStagingBuffer{ mLogicalDevice, mPhysicalDeviceMemoryProperties };

    const vkpp::BufferCreateInfo lBufferCreateInfo{ mImgRes.memSize, vkpp::BufferUsageFlagBits::eTransferSrc };
    lStagingBuffer.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lpMappedMem = lStagingBuffer.Map<uint8_t>();
    // Size of the font texture is WIDTH * HEIGHT * 1 byte (only one channel).
    byte lFont24Pixels[STB_FONT_HEIGHT][STB_FONT_WIDTH];
    STB_FONT_NAME(mSTBFontData, lFont24Pixels, STB_FONT_HEIGHT);

    std::memcpy(lpMappedMem, &lFont24Pixels[0][0], STB_FONT_WIDTH * STB_FONT_HEIGHT);
    lStagingBuffer.Unmap();

    // Copy to font texture
    const auto& lCopyCmd = BeginOneTimeCmdBuffer();

    constexpr vkpp::ImageSubresourceRange lImageSubRange
    {
        vkpp::ImageAspectFlagBits::eColor,
        0, 1,
        0, 1
    };

    TransitionImageLayout<vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eTransferDstOptimal>
    (
        lCopyCmd, mImgRes.image,
        lImageSubRange,
        vkpp::DefaultFlags,
        vkpp::AccessFlagBits::eTransferWrite
    );

    constexpr vkpp::BufferImageCopy lBufferCopyRegion
    {
        0,
        {
            vkpp::ImageAspectFlagBits::eColor,
            0
        },
        { 0, 0, 0 },
        {
            STB_FONT_WIDTH,
            STB_FONT_HEIGHT,
            1
        }
    };

    lCopyCmd.Copy(mImgRes.image, vkpp::ImageLayout::eTransferDstOptimal, lStagingBuffer.buffer, lBufferCopyRegion);

    EndOneTimeCmdBuffer(lCopyCmd);

    lStagingBuffer.Reset();
}


void TextRenderer::CreateSampler(void)
{
    const vkpp::SamplerCreateInfo lSamplerCreateInfo
    {
        vkpp::Filter::eLinear,
        vkpp::Filter::eLinear,
        vkpp::SamplerMipmapMode::eLinear,
        vkpp::SamplerAddressMode::eRepeat,
        vkpp::SamplerAddressMode::eRepeat,
        vkpp::SamplerAddressMode::eRepeat,
        0.0f,
        vkpp::Anisotropy::Disable,
        1.0f,
        vkpp::Compare::Disable,
        vkpp::CompareOp::eNever,
        0.0f, 1.0f,
        vkpp::BorderColor::eFloatOpaqueWhite
    };

    mSampler = mLogicalDevice.CreateSampler(lSamplerCreateInfo);
}


void TextRenderer::UpdateDescriptorSets(void) const
{
    const vkpp::DescriptorImageInfo lTextSampler{ mSampler, mImgRes.view, vkpp::ImageLayout::eShaderReadOnlyOptimal };

    const vkpp::WriteDescriptorSetInfo lWriteDescriptorSetInfos
    { 
        mDescriptorSet, 0, vkpp::DescriptorType::eCombinedImageSampler, lTextSampler
    };

    mLogicalDevice.UpdateDescriptorSet(lWriteDescriptorSetInfos);
}


template <vkpp::ImageLayout OldLayout, vkpp::ImageLayout NewLayout>
void TextRenderer::TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange, const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask)
{
    // Create an image barrier object.
    const vkpp::ImageMemoryBarrier lImageBarrier
    {
        aSrcAccessMask, aDstAccessMask,
        OldLayout, NewLayout,
        aImage,
        aImageSubRange
    };

    // Put barrier inside setup command buffer.
    // Put barrier on top of pipeline.
    aCmdBuffer.PipelineBarrier(vkpp::PipelineStageFlagBits::eTopOfPipe, vkpp::PipelineStageFlagBits::eTopOfPipe, vkpp::DependencyFlagBits::eByRegion, { lImageBarrier });
}


vkpp::CommandBuffer TextRenderer::BeginOneTimeCmdBuffer(void) const
{
    const vkpp::CommandBufferAllocateInfo lCmdBufferAllocateInfo
    {
        mCmdPool, 1
    };

    auto lCmdBuffer = mLogicalDevice.AllocateCommandBuffer(lCmdBufferAllocateInfo);

    constexpr vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo
    {
        vkpp::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    lCmdBuffer.Begin(lCmdBufferBeginInfo);

    return lCmdBuffer;
}


void TextRenderer::EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const
{
    aCmdBuffer.End();

    constexpr vkpp::FenceCreateInfo lFenceCreateInfo;
    const auto& lFence = mLogicalDevice.CreateFence(lFenceCreateInfo);

    const vkpp::SubmitInfo lSubmitInfo{ aCmdBuffer };

    mGraphicsQueue.handle.Submit(lSubmitInfo, lFence);

    mLogicalDevice.WaitForFence(lFence);

    mLogicalDevice.DestroyFence(lFence);
    mLogicalDevice.FreeCommandBuffer(mCmdPool, aCmdBuffer);
}


void TextRenderer::BeginTextUpdate(void)
{
    mpMapped = mBufferRes.Map<TextVertexData>();
    mNumLetters = 0;
}


void TextRenderer::EndTextUpdate(void)
{
    mBufferRes.Unmap();
    mpMapped = nullptr;
    BuildCmdBuffers();
}


// Add text to the current buffer.
void TextRenderer::AddText(const std::string& aText, float aX, float aY, TextAlign aAlign)
{
    assert(mpMapped != nullptr);

    const auto lCharW = 1.5f / mFramebufferWidth;
    const auto lCharH = 1.5f / mFramebufferHeight;

    const auto lFbW = static_cast<float>(mFramebufferWidth);
    const auto lFbH = static_cast<float>(mFramebufferHeight);
    aX = (aX / lFbW * 2.0f) - 1.0f;
    aY = (aY / lFbH * 2.0f) - 1.0f;

    // Calculate text width
    float lTextWidth{ 0 };
    for (auto lChar : aText)
    {
        auto lCharData = mSTBFontData[static_cast<uint32_t>(lChar) - STB_FIRST_CHAR];
        lTextWidth += lCharData.advance * lCharW;
    }

    switch (aAlign)
    {
    case TextAlign::eRight:
        aX -= lTextWidth;
        break;

    case TextAlign::eCenter:
        aX -= lTextWidth / 2.0f;
        break;
    }

    // Generate a UV mapped quad per char in the new text
    for (auto lChar : aText)
    {
        auto lCharData = mSTBFontData[static_cast<uint32_t>(lChar) - STB_FIRST_CHAR];

        mpMapped->inPos.x = aX + lCharData.x0 * lCharW;
        mpMapped->inPos.y = aY + lCharData.y0 * lCharH;
        mpMapped->inUV.s = lCharData.s0;
        mpMapped->inUV.t = lCharData.t0;
        ++mpMapped;

        mpMapped->inPos.x = aX + lCharData.x1 * lCharW;
        mpMapped->inPos.y = aY + lCharData.y0 * lCharH;
        mpMapped->inUV.s = lCharData.s1;
        mpMapped->inUV.t = lCharData.t0;
        ++mpMapped;

        mpMapped->inPos.x = aX + lCharData.x0 * lCharW;
        mpMapped->inPos.y = aY + lCharData.y1 * lCharH;
        mpMapped->inUV.s = lCharData.s0;
        mpMapped->inUV.t = lCharData.t1;
        ++mpMapped;

        mpMapped->inPos.x = aX + lCharData.x1 * lCharW;
        mpMapped->inPos.y = aY + lCharData.y1 * lCharH;
        mpMapped->inUV.s = lCharData.s1;
        mpMapped->inUV.t = lCharData.t1;
        ++mpMapped;

        aX += lCharData.advance * lCharW;

        ++mNumLetters;
    };
}


void TextRenderer::BuildCmdBuffers(void)
{
    constexpr vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo;

    constexpr vkpp::ClearValue lClearValues[]
    {
        { 0.129411f, 0.156862f, 0.188235f, 1.0f },
        { 1.0f, 0.0f }
    };

    for (std::size_t lIndex=0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
    {
        const vkpp::RenderPassBeginInfo lRenderPassBeginInfo
        {
            mRenderPass,
            mFramebuffers[lIndex],
            {
                0, 0,
                mFramebufferWidth, mFramebufferHeight
            },
            2, lClearValues
        };

        const auto& lDrawCmdBuffer = mDrawCmdBuffers[lIndex];
        lDrawCmdBuffer.Begin(lCmdBufferBeginInfo);

        lDrawCmdBuffer.BeginRenderPass(lRenderPassBeginInfo); 

        const vkpp::Viewport lViewport
        {
            0.0f, 0.0f,
            static_cast<float>(mFramebufferWidth), static_cast<float>(mFramebufferHeight)
        };

        lDrawCmdBuffer.SetViewport(lViewport);

        const vkpp::Scissor lScissor
        {
            0, 0,
            mFramebufferWidth, mFramebufferHeight
        };

        lDrawCmdBuffer.SetScissor(lScissor);

        lDrawCmdBuffer.BindGraphicsPipeline(mTextRenderingPipeline);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mTextPipelineLayout, 0, mDescriptorSet);
        lDrawCmdBuffer.BindVertexBuffer(mBufferRes.buffer);

        for (uint32_t lChar = 0; lChar < mNumLetters; ++lChar)
        {
            lDrawCmdBuffer.Draw(4, 1, lChar * 4);
        }

        lDrawCmdBuffer.EndRenderPass();

        lDrawCmdBuffer.End();
    }
}


void TextRenderer::CreateSemaphores(void)
{
    constexpr vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    mPresentCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mRenderingCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void TextRenderer::CreateFences(void)
{
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    for (std::size_t lIndex = 0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
        mWaitFences.emplace_back(mLogicalDevice.CreateFence(lFenceCreateInfo));
}


vkpp::ShaderModule TextRenderer::CreateShaderModule(const std::string& aFilename) const
{
    std::ifstream lFin(aFilename, std::ios::binary);
    assert(lFin);

    std::vector<char> lShaderContent((std::istreambuf_iterator<char>(lFin)), std::istreambuf_iterator<char>());
    assert(!lShaderContent.empty());

    return mLogicalDevice.CreateShaderModule(lShaderContent);
}


void TextRenderer::Submit(uint32_t aBufferIndex)
{
    const vkpp::SubmitInfo lSubmitInfo{ mDrawCmdBuffers[aBufferIndex] };

    mGraphicsQueue.handle.Submit(lSubmitInfo);
    mGraphicsQueue.handle.Wait();
}


SceneRenderer::SceneRenderer(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName, uint32_t aEngineVersion)
    : ExampleBase(aWindow, apAppName, aAppVersion, apEngineName, aEngineVersion),
      mTile(apAppName),
      CWindowEvent{aWindow}, CMouseMotionEvent{aWindow}, CMouseWheelEvent{aWindow},
      mDepthRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mUboScene(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mBkTexRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mCubeTexRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mVtxBufferRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mIdxBufferRes(mLogicalDevice, mPhysicalDeviceMemoryProperties)
{
    theApp.RegisterUpdateEvent([this](void)
    {
        Update();
    });

    mMouseWheelFunc = [this](Sint32 /*aPosX*/, Sint32 aPosY)
    {
        mCurrentZoomLevel *= (1.0f + aPosY * MINIMUM_ZOOM_LEVEL);
        UpdateUniformBuffers();
    };

    mMouseMotionFunc = [this](Uint32 aMouseState, Sint32 /*aPosX*/, Sint32 /*aPosY*/, Sint32 aRelativeX, Sint32 aRelativeY)
    {
        if (aMouseState & CMouseEvent::ButtonMask::eLMask)
        {
            mCurrentRotation.x += aRelativeY;
            mCurrentRotation.y += aRelativeX;

            UpdateUniformBuffers();
        }
    };

    CreateCmdPool();
    AllocateCmdBuffers();
    CreateRenderPass();
    CreateDepthRes();
    CreateFramebuffers();

    CreateSetLayouts();
    CreatePipelineLayout();
    CreateGraphicsPipelines();

    CreateDescriptorPool();
    AllocateDescriptorSets();

    CreateUniformBuffers();
    UpdateUniformBuffers();

    LoadTexture("Texture/skysphere_bc3_unorm.ktx", vkpp::Format::eBC3_uNormBlock, mBackground, mBkTexRes);
    LoadTexture("Texture/round_window_bc3_unorm.ktx", vkpp::Format::eBC3_uNormBlock, mCube, mCubeTexRes);
    CreateSamplers();
    UpdateDescriptorSets();

    LoadModel("Model/cube.dae");

    BuildCmdBuffers();

    CreateSemaphores();
    CreateFences();

    mTextRenderer = std::make_unique<TextRenderer>(mGraphicsQueue, mLogicalDevice, mPhysicalDeviceMemoryProperties, mFramebuffers,
        mSwapchain.surfaceFormat.format, mSwapchain.extent.width, mSwapchain.extent.height);
}


SceneRenderer::~SceneRenderer(void)
{
    mLogicalDevice.Wait();

    mLogicalDevice.DestroySemaphore(mRenderingCompleteSemaphore);
    mLogicalDevice.DestroySemaphore(mPresentCompleteSemaphore);

    mLogicalDevice.DestroyFences(mWaitFences);

    mLogicalDevice.DestroySampler(mTextureSampler);

    mIdxBufferRes.Reset();
    mVtxBufferRes.Reset();

    mCubeTexRes.Reset();
    mBkTexRes.Reset();

    mUboScene.Unmap();
    mUboScene.Reset();

    mLogicalDevice.DestroyDescriptorPool(mDescriptorPool);

    mLogicalDevice.DestroyPipeline(mPipelines.cube);
    mLogicalDevice.DestroyPipeline(mPipelines.background);

    mLogicalDevice.DestroyPipelineLayout(mPipelineLayouts.background);
    mLogicalDevice.DestroyPipelineLayout(mPipelineLayouts.cube);

    mLogicalDevice.DestroyDescriptorSetLayout(mSetLayouts.background);
    mLogicalDevice.DestroyDescriptorSetLayout(mSetLayouts.cube);

    mLogicalDevice.DestroyFramebuffers(mFramebuffers);
    mDepthRes.Reset();
    mLogicalDevice.DestroyRenderPass(mRenderPass);
    mLogicalDevice.FreeCommandBuffers(mCmdPool, mDrawCmdBuffers);
    mLogicalDevice.DestroyCommandPool(mCmdPool);
}


void SceneRenderer::CreateCmdPool(void)
{
    mCmdPool = mLogicalDevice.CreateCommandPool({ mPresentQueue.familyIndex, vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer });
}


void SceneRenderer::AllocateCmdBuffers(void)
{
    const vkpp::CommandBufferAllocateInfo lCmdBufferAllocateInfo
    {
        mCmdPool,
        static_cast<uint32_t>(mSwapchain.buffers.size())
    };

    mDrawCmdBuffers = mLogicalDevice.AllocateCommandBuffers(lCmdBufferAllocateInfo);
}


void SceneRenderer::CreateRenderPass(void)
{
    const std::vector<vkpp::AttachementDescription> lAttachementDescriptions
    {
        // Color attachment
        {
            mSwapchain.surfaceFormat.format,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eStore,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::ePresentSrcKHR
        },
        // Depth attachment
        {
            vkpp::Format::eD32sFloat,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eDontCare,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
        }
    };

    constexpr vkpp::AttachmentReference lColorRef
    {
        0, vkpp::ImageLayout::eColorAttachmentOptimal
    };

    constexpr vkpp::AttachmentReference lDepthRef
    {
        1, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
    };

    constexpr std::array<vkpp::SubpassDescription, 1> lSubpassDescriptions
    { {
        { vkpp::PipelineBindPoint::eGraphics, lColorRef, lDepthRef }
        } };

    const std::vector<vkpp::SubpassDependency> lSubpassDependencies
    {
        {
            vkpp::subpass::External, 0,
            vkpp::PipelineStageFlagBits::eBottomOfPipe, vkpp::PipelineStageFlagBits::eColorAttachmentOutput,
            vkpp::AccessFlagBits::eMemoryRead, vkpp::AccessFlagBits::eColorAttachmentWrite,
            vkpp::DependencyFlagBits::eByRegion
        },
        {
            0, vkpp::subpass::External,
            vkpp::PipelineStageFlagBits::eColorAttachmentOutput, vkpp::PipelineStageFlagBits::eBottomOfPipe,
            vkpp::AccessFlagBits::eColorAttachmentWrite, vkpp::AccessFlagBits::eMemoryRead,
            vkpp::DependencyFlagBits::eByRegion
        }
    };

    const vkpp::RenderPassCreateInfo lRenderPassCreateInfo
    {
        lAttachementDescriptions, lSubpassDescriptions, lSubpassDependencies
    };

    mRenderPass = mLogicalDevice.CreateRenderPass(lRenderPassCreateInfo);
}


void SceneRenderer::CreateDepthRes(void)
{
    const vkpp::ImageCreateInfo lImageCreateInfo
    {
        vkpp::ImageType::e2D,
        vkpp::Format::eD32sFloat,
        mSwapchain.extent,
        vkpp::ImageUsageFlagBits::eDepthStencilAttachment
    };

    vkpp::ImageViewCreateInfo lImageViewCreateInfo
    {
        vkpp::ImageViewType::e2D,
        vkpp::Format::eD32sFloat,
        {
            vkpp::ImageAspectFlagBits::eDepth,
            0, 1,
            0, 1
        }
    };

    mDepthRes.Reset(lImageCreateInfo, lImageViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);
}


void SceneRenderer::CreateFramebuffers(void)
{
    std::array<vkpp::ImageView, 2> lAttachments{ nullptr, mDepthRes.view };

    const vkpp::FramebufferCreateInfo lFramebufferCreateInfo
    {
        mRenderPass,
        lAttachments,
        mSwapchain.extent
    };

    for (auto& lSwapchain : mSwapchain.buffers)
    {
        lAttachments[0] = lSwapchain.view;

        mFramebuffers.emplace_back(mLogicalDevice.CreateFramebuffer(lFramebufferCreateInfo));
    }
}


void SceneRenderer::CreateSetLayouts(void)
{
    const std::vector<vkpp::DescriptorSetLayoutBinding> lSetLayoutBindings
    {
        // Binding 0: Vertex shader uniform buffer
        {
            0,
            vkpp::DescriptorType::eUniformBuffer,
            vkpp::ShaderStageFlagBits::eVertex
        },
        // Binding 1: Fragment shader combined sampler
        {
            1,
            vkpp::DescriptorType::eCombinedImageSampler,
            vkpp::ShaderStageFlagBits::eFragment
        }
    };

    mSetLayouts.cube = mLogicalDevice.CreateDescriptorSetLayout(lSetLayoutBindings);

    auto& lBkSetLayoutBinding = lSetLayoutBindings[1];
    mSetLayouts.background = mLogicalDevice.CreateDescriptorSetLayout(lBkSetLayoutBinding);
}


void SceneRenderer::CreatePipelineLayout(void)
{
    mPipelineLayouts.cube = mLogicalDevice.CreatePipelineLayout(mSetLayouts.cube);
    mPipelineLayouts.background = mLogicalDevice.CreatePipelineLayout(mSetLayouts.background);
}


void SceneRenderer::CreateGraphicsPipelines(void)
{
    std::array<vkpp::PipelineShaderStageCreateInfo, 2> lShaderStageCreateInfos
    { {
        { vkpp::ShaderStageFlagBits::eVertex },
        { vkpp::ShaderStageFlagBits::eFragment }
    } };

    auto& lVertexShaderModule = lShaderStageCreateInfos[0].module;
    auto& lFragmentShaderModule = lShaderStageCreateInfos[1].module;

    vkpp::PipelineVertexInputStateCreateInfo lInputStateCreateInfo;

    constexpr vkpp::PipelineInputAssemblyStateCreateInfo lInputAssemblyStateCreateInfo{ vkpp::PrimitiveTopology::eTriangleList };
    constexpr vkpp::PipelineViewportStateCreateInfo lViewportStateCreateInfo{ 1, 1 };
    constexpr vkpp::PipelineRasterizationStateCreateInfo lRasterizationStateCreateInfo
    {
        vkpp::PolygonMode::eFill,
        vkpp::CullModeFlagBits::eBack,
        vkpp::FrontFace::eClockwise
    };

    constexpr vkpp::PipelineMultisampleStateCreateInfo lMultisampleStateCreateInfo;
    vkpp::PipelineDepthStencilStateCreateInfo lDepthStencilStateCreateInfo
    {
        vkpp::DepthTest::Disable, vkpp::DepthWrite::Disable,
        vkpp::CompareOp::eLess
    };

    constexpr vkpp::PipelineColorBlendAttachmentState lColorBlendAttachmentState;
    constexpr vkpp::PipelineColorBlendStateCreateInfo lColorBlendStateCreateInfo{ lColorBlendAttachmentState };

    constexpr std::array<vkpp::DynamicState, 2> lDynamicStates
    {
        vkpp::DynamicState::eViewport, vkpp::DynamicState::eScissor
    };

    const vkpp::PipelineDynamicStateCreateInfo lDynamicStateCreateInfo{ lDynamicStates };

    vkpp::GraphicsPipelineCreateInfo lGraphicsPipelineCreateInfo
    {
        2, lShaderStageCreateInfos.data(),
        lInputStateCreateInfo.AddressOf(),
        lInputAssemblyStateCreateInfo.AddressOf(),
        nullptr,
        lViewportStateCreateInfo.AddressOf(),
        lRasterizationStateCreateInfo.AddressOf(),
        lMultisampleStateCreateInfo.AddressOf(),
        lDepthStencilStateCreateInfo.AddressOf(),
        lColorBlendStateCreateInfo.AddressOf(),
        lDynamicStateCreateInfo.AddressOf(),
        mPipelineLayouts.background,
        mRenderPass,
        0
    };

    // Background rendering pipeline
    lVertexShaderModule = CreateShaderModule("Shaders/SPV/background.vert.spv");
    lFragmentShaderModule = CreateShaderModule("Shaders/SPV/background.frag.spv");
    mPipelines.background = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);

    // Cube rendering pipeline
    constexpr auto lVertexInputBindings = VertexData::GetBindingDescription();
    constexpr auto lVertexAttributes = VertexData::GetAttributeDescriptions();
    lInputStateCreateInfo
        .SetBindingDescription(lVertexInputBindings)
        .SetAttributeDescriptions(lVertexAttributes);

    lDepthStencilStateCreateInfo.EnableDepthTest();
    lGraphicsPipelineCreateInfo.SetLayout(mPipelineLayouts.cube);
    lVertexShaderModule = CreateShaderModule("Shaders/SPV/mesh.vert.spv");
    lFragmentShaderModule = CreateShaderModule("Shaders/SPV/mesh.frag.spv");
    mPipelines.cube = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);
}


void SceneRenderer::CreateDescriptorPool(void)
{
    // Example uses one UBO and two image sampler.
    constexpr std::array<vkpp::DescriptorPoolSize, 2> lPoolSizes
    { {
        { vkpp::DescriptorType::eUniformBuffer, 1 },
        { vkpp::DescriptorType::eCombinedImageSampler, 2 }
    } };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool({ lPoolSizes, 2 });
}


void SceneRenderer::AllocateDescriptorSets(void)
{
    // background
    vkpp::DescriptorSetAllocateInfo lSetAllocationInfo
    {
        mDescriptorPool, mSetLayouts.background
    };

    mDescriptorSets.background = mLogicalDevice.AllocateDescriptorSet(lSetAllocationInfo);

    // cube
    lSetAllocationInfo.SetLayout(mSetLayouts.cube);
    mDescriptorSets.cube = mLogicalDevice.AllocateDescriptorSet(lSetAllocationInfo);
}


void SceneRenderer::CreateUniformBuffers(void)
{
    vkpp::BufferCreateInfo lBufferCreateInfo{ sizeof(UboScene), vkpp::BufferUsageFlagBits::eUniformBuffer };

    mUboScene.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    mpUboSceneMapped = mUboScene.Map();
}


void SceneRenderer::UpdateUniformBuffers(void)
{
    auto lWidth = static_cast<float>(mSwapchain.extent.width);
    auto lHeight = static_cast<float>(mSwapchain.extent.height);

    const static glm::vec3 lCameraPos;

    mSceneMatrix.proj = glm::perspective(glm::radians(60.0f), lWidth / lHeight, 0.1f, 256.0f);

    const auto& lViewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, mCurrentZoomLevel));
    mSceneMatrix.model = lViewMatrix * glm::translate(glm::mat4(), lCameraPos);
    mSceneMatrix.model = glm::rotate(mSceneMatrix.model, glm::radians(mCurrentRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    mSceneMatrix.model = glm::rotate(mSceneMatrix.model, glm::radians(mCurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    mSceneMatrix.model = glm::rotate(mSceneMatrix.model, glm::radians(mCurrentRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));


    std::memcpy(mpUboSceneMapped, &mSceneMatrix, sizeof(UboScene));
}


void SceneRenderer::LoadTexture(const std::string& aFilename, vkpp::Format aTexFormat, Texture& aTexture, ImageResource& aTexRes) const
{
    const gli::texture2d lTex2D{ gli::load(aFilename) };
    assert(!lTex2D.empty());

    aTexture.width = static_cast<uint32_t>(lTex2D[0].extent().x);
    aTexture.height = static_cast<uint32_t>(lTex2D[0].extent().y);
    aTexture.mipLevels = static_cast<uint32_t>(lTex2D.levels());

    // Create a host-visible staging buffer that contains the raw image data.
    const vkpp::BufferCreateInfo lStagingBufferCreateInfo
    {
        lTex2D.size(),
        vkpp::BufferUsageFlagBits::eTransferSrc
    };

    BufferResource lStagingBuffer{ mLogicalDevice, mPhysicalDeviceMemoryProperties };
    lStagingBuffer.Reset(lStagingBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lpMappedMem = lStagingBuffer.Map();
    std::memcpy(lpMappedMem, lTex2D.data(), lTex2D.size());
    lStagingBuffer.Unmap();

    // Setup buffer copy regions for each mip-level.
    std::vector<vkpp::BufferImageCopy> lBufferCopyRegions;
    uint32_t lOffset{ 0 };

    for (uint32_t lIndex = 0; lIndex < aTexture.mipLevels; ++lIndex)
    {
        const vkpp::BufferImageCopy lBufferCopyRegion
        {
            lOffset,
            {
                vkpp::ImageAspectFlagBits::eColor,
                lIndex,
            },
            { 0, 0, 0 },
            {
                static_cast<uint32_t>(lTex2D[lIndex].extent().x),
                static_cast<uint32_t>(lTex2D[lIndex].extent().y),
                1
            }
        };

        lBufferCopyRegions.emplace_back(lBufferCopyRegion);
        lOffset += static_cast<uint32_t>(lTex2D[lIndex].size());
    }

    // Create optimal tiled target image.
    // Only use linear tiling if requested (and supported by the device).
    // Support for linear tiling is mostly limited, so prefer to use optimal tiling instead.
    // On most implementations, linear tiling will only support a very limited amount of formats and features (mip-maps, cube-maps, arrays, etc.).
    const vkpp::ImageCreateInfo lImageCreateInfo
    {
        vkpp::ImageType::e2D,
        aTexFormat,
        { aTexture.width, aTexture.height, 1 },
        vkpp::ImageUsageFlagBits::eSampled | vkpp::ImageUsageFlagBits::eTransferDst,
        vkpp::ImageLayout::eUndefined,
        vkpp::ImageTiling::eOptimal,
        vkpp::SampleCountFlagBits::e1,
        aTexture.mipLevels
    };

    vkpp::ImageViewCreateInfo lImageViewCreateInfo
    {
        vkpp::ImageViewType::e2D,
        aTexFormat,
        // The subresource range describes the set of mip-levels (and array layers) that can be accessed through this image view.
        // It is possible to create multiple image views for a single image referring to different (and/or overlapping) ranges of the image.
        {
            vkpp::ImageAspectFlagBits::eColor,
            0, aTexture.mipLevels,
            0, 1
        }
    };

    aTexRes.Reset(lImageCreateInfo, lImageViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    const auto& lCopyCmd = BeginOneTimeCmdBuffer();

    // Image barrier for optimal image.

    // The sub-resource range describes the regions of the image which will be transitioned.
    const vkpp::ImageSubresourceRange lImageSubRange
    {
        vkpp::ImageAspectFlagBits::eColor,              // aspectMask: Only contains color data.
        0,                                              // baseMipLevel: Start at first mip-level.
        aTexture.mipLevels,                             // levelCount: Transition on all mip-levels.
        0,                                              // baseArrayLayer: Start at first element in the array. (only one element in this example.)
        1                                               // layerCount: The 2D texture only has one layer.
    };

    // Optimal image will be used as the destination for the copy, so it must be transfered from the initial undefined image layout to the transfer destination layout.
    TransitionImageLayout<vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eTransferDstOptimal>
    (
        lCopyCmd, aTexRes.image,
        lImageSubRange,
        vkpp::DefaultFlags,                         // srcAccessMask = 0: Only valid as initial layout, memory contents are not preserved.
                                                    //                    Can be accessed directly, no source dependency required.
        vkpp::AccessFlagBits::eTransferWrite        // dstAccessMask: Transfer destination (copy, blit).
                                                    //                Make sure any write operation to the image has been finished.
    );

    // Copy all mip-levels from staging buffer.
    lCopyCmd.Copy(aTexRes.image, vkpp::ImageLayout::eTransferDstOptimal, lStagingBuffer.buffer, lBufferCopyRegions);

    // Transfer texture image layout to shader read after all mip-levels have been copied.
    TransitionImageLayout<vkpp::ImageLayout::eTransferDstOptimal, vkpp::ImageLayout::eShaderReadOnlyOptimal>
    (
        lCopyCmd, aTexRes.image,
        lImageSubRange,
        vkpp::AccessFlagBits::eTransferWrite,       // srcAccessMask: Old layout is transfer destination.
                                                    //                Make sure any write operation to the destination image has been finished.
        vkpp::AccessFlagBits::eShaderRead           // dstAccessMask: Shader read, like sampler, input attachment.
    );

    EndOneTimeCmdBuffer(lCopyCmd);

    lStagingBuffer.Reset();
}


void SceneRenderer::CreateSamplers(void)
{
    const vkpp::SamplerCreateInfo lSamplerCreateInfo
    {
        vkpp::Filter::eLinear,                                      // magFilter
        vkpp::Filter::eLinear,                                      // minFilter
        vkpp::SamplerMipmapMode::eLinear,                           // mipmapMode
        vkpp::SamplerAddressMode::eRepeat,                          // addressModeU
        vkpp::SamplerAddressMode::eRepeat,                          // addressModeV
        vkpp::SamplerAddressMode::eRepeat,                          // addressModeW
        0.0f,                                                       // mipLodBias
        Anisotropy::Enable,                                         // anisotropyEnable
        mPhysicalDeviceProperties.limits.maxSamplerAnisotropy,      // maxAnisotropy
        Compare::Enable,                                            // compareEnable,
        vkpp::CompareOp::eNever,                                    // compareOp
        0.0f,                                                       // minLoad
        static_cast<float>(mBackground.mipLevels),                  // maxLoad: Set max level-of-detail to mip-level count of the texture.
        vkpp::BorderColor::eFloatOpaqueWhite                        // borderColor
    };

    mTextureSampler = mLogicalDevice.CreateSampler(lSamplerCreateInfo);
}


void SceneRenderer::UpdateDescriptorSets(void)
{
    const vkpp::DescriptorBufferInfo lVSUbo{ mUboScene.buffer };
    const vkpp::DescriptorImageInfo lCubeTexSampler{ mTextureSampler, mCubeTexRes.view, mCube.layout };

    const std::array<vkpp::WriteDescriptorSetInfo, 2> lCubeWriteDescriptorSetInfos
    { {
        // Binding 0: Vertex shader uniform buffer
        {
            mDescriptorSets.cube, 0,
            vkpp::DescriptorType::eUniformBuffer,
            lVSUbo
        },
        // Binding 1: Color gradient sampler
        {
            mDescriptorSets.cube, 1,
            vkpp::DescriptorType::eCombinedImageSampler,
            lCubeTexSampler
        }
    } };

    mLogicalDevice.UpdateDescriptorSets(lCubeWriteDescriptorSetInfos);

    const vkpp::DescriptorImageInfo lBkTexSampler{ mTextureSampler, mBkTexRes.view, mBackground.layout };
    const vkpp::WriteDescriptorSetInfo lBkWriteDescriptorSetInfo
    {
        mDescriptorSets.background, 1,
        vkpp::DescriptorType::eCombinedImageSampler,
        lBkTexSampler
    };

    mLogicalDevice.UpdateDescriptorSet(lBkWriteDescriptorSetInfo);
}


void SceneRenderer::LoadModel(const std::string& aFilename)
{
    // Flags for loading the mesh
    constexpr static auto lAssimpFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices;
    constexpr static auto lScale = 1.0f;

    Assimp::Importer lImporter;

    auto lpScene = lImporter.ReadFile(aFilename, lAssimpFlags);
    assert(lpScene != nullptr);

    std::vector<float> lVertexBuffer;
    std::vector<uint32_t> lIndexBuffer;

    // Iterate through all meshes in the file and extract the vertex components.
    for (unsigned int lMeshIdx = 0; lMeshIdx < lpScene->mNumMeshes; ++lMeshIdx)
    {
        const auto lpMesh = lpScene->mMeshes[lMeshIdx];
        mIndexCount += lpScene->mMeshes[lMeshIdx]->mNumFaces * 3;

        for (unsigned int lVtxIdx = 0; lVtxIdx < lpMesh->mNumVertices; ++lVtxIdx)
        {
            // Vertex positions
            const auto lPos = lpMesh->mVertices[lVtxIdx];
            lVertexBuffer.emplace_back(lPos.x * lScale);
            lVertexBuffer.emplace_back(-lPos.y * lScale);                // Vulkan use a right handed NDC.
            lVertexBuffer.emplace_back(lPos.z * lScale);

            // Vertex normal
            const auto lNormal = lpMesh->mNormals[lVtxIdx];
            lVertexBuffer.emplace_back(lNormal.x);
            lVertexBuffer.emplace_back(lNormal.y);
            lVertexBuffer.emplace_back(lNormal.z);

            // Vertex texture coordinates
            const auto lTexCoord = lpMesh->mTextureCoords[0][lVtxIdx];
            lVertexBuffer.emplace_back(lTexCoord.x);
            lVertexBuffer.emplace_back(lTexCoord.y);

            // Vertex color
            /*const auto lColor = lpMesh->HasVertexColors(0) ? lpMesh->mColors[0][lVtxIdx] : aiColor4D(1.0f);
            lVertexBuffer.emplace_back(lColor.r);
            lVertexBuffer.emplace_back(lColor.g);
            lVertexBuffer.emplace_back(lColor.b);*/
        }

        auto lIndexOffset = static_cast<uint32_t>(lIndexBuffer.size());
        for (unsigned int lIdxIndex = 0; lIdxIndex < lpMesh->mNumFaces; ++lIdxIndex)
        {
            const auto& lFace = lpMesh->mFaces[lIdxIndex];

            if (lFace.mNumIndices != 3)
                continue;

            // Assume that all faces are triangulated
            lIndexBuffer.emplace_back(lIndexOffset + lFace.mIndices[0]);
            lIndexBuffer.emplace_back(lIndexOffset + lFace.mIndices[1]);
            lIndexBuffer.emplace_back(lIndexOffset + lFace.mIndices[2]);
        }
    }

    // Static mesh should always be device local.
    // Use staging buffer to move vertex and index buffers to device local memory.

    // Vertex buffer
    const auto lVtxBufferSize = static_cast<uint32_t>(lVertexBuffer.size()) * sizeof(float);
    const vkpp::BufferCreateInfo lVtxStagingBufferCreateInfo
    {
        lVtxBufferSize, vkpp::BufferUsageFlagBits::eTransferSrc
    };

    BufferResource lVtxStagingBuffer{ mLogicalDevice, mPhysicalDeviceMemoryProperties };
    lVtxStagingBuffer.Reset(lVtxStagingBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lMappedMem = mLogicalDevice.MapMemory(lVtxStagingBuffer.memory, 0, lVtxBufferSize);
    std::memcpy(lMappedMem, lVertexBuffer.data(), lVtxBufferSize);
    mLogicalDevice.UnmapMemory(lVtxStagingBuffer.memory);

    // Index buffer
    const auto lIdxBufferSize = static_cast<uint32_t>(lIndexBuffer.size()) * sizeof(uint32_t);
    const vkpp::BufferCreateInfo lIdxStagingBufferCreateInfo
    {
        lIdxBufferSize, vkpp::BufferUsageFlagBits::eTransferSrc
    };

    BufferResource lIdxStagingBuffer{ mLogicalDevice, mPhysicalDeviceMemoryProperties };
    lIdxStagingBuffer.Reset(lIdxStagingBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    lMappedMem = mLogicalDevice.MapMemory(lIdxStagingBuffer.memory, 0, lIdxBufferSize);
    std::memcpy(lMappedMem, lIndexBuffer.data(), lIdxBufferSize);
    mLogicalDevice.UnmapMemory(lIdxStagingBuffer.memory);

    // Create device local target buffers.
    // Vertex buffer
    const vkpp::BufferCreateInfo lVtxBufferCreateInfo
    {
        lVtxBufferSize, vkpp::BufferUsageFlagBits::eVertexBuffer | vkpp::BufferUsageFlagBits::eTransferDst
    };

    mVtxBufferRes.Reset(lVtxBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    // Index buffer
    const vkpp::BufferCreateInfo lIdxBufferCreateInfo
    {
        lIdxBufferSize, vkpp::BufferUsageFlagBits::eIndexBuffer | vkpp::BufferUsageFlagBits::eTransferDst
    };

    mIdxBufferRes.Reset(lIdxBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    CopyBuffer(mVtxBufferRes.buffer, lVtxStagingBuffer.buffer, lVtxBufferSize);
    CopyBuffer(mIdxBufferRes.buffer, lIdxStagingBuffer.buffer, lIdxBufferSize);
}


void SceneRenderer::CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const
{
    const auto& lCopyCmd = BeginOneTimeCmdBuffer();

    const vkpp::BufferCopy lBufferCopy{ aSize };

    lCopyCmd.Copy(aDstBuffer, aSrcBuffer, lBufferCopy);

    EndOneTimeCmdBuffer(lCopyCmd);
}


void SceneRenderer::BuildCmdBuffers(void)
{
    constexpr vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo;

    constexpr vkpp::ClearValue lClearValues[]
    {
        { 0.129411f, 0.156862f, 0.188235f, 1.0f },
        { 1.0f, 0.0f }
    };

    for (std::size_t lIndex = 0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
    {
        const vkpp::RenderPassBeginInfo lRenderPassBeginInfo
        {
            mRenderPass,
            mFramebuffers[lIndex],
            {
                { 0, 0 },
                mSwapchain.extent
            },
            2, lClearValues
        };

        const auto& lDrawCmdBuffer = mDrawCmdBuffers[lIndex];

        lDrawCmdBuffer.Begin(lCmdBufferBeginInfo);

        lDrawCmdBuffer.BeginRenderPass(lRenderPassBeginInfo);

        const vkpp::Viewport lViewport
        {
            0.0f, 0.0f,
            static_cast<float>(mSwapchain.extent.width), static_cast<float>(mSwapchain.extent.height)
        };

        lDrawCmdBuffer.SetViewport(lViewport);

        const vkpp::Rect2D lScissor
        {
            0, 0,
            mSwapchain.extent
        };

        lDrawCmdBuffer.SetScissor(lScissor);

        // Background
        lDrawCmdBuffer.BindGraphicsPipeline(mPipelines.background);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayouts.background, 0, mDescriptorSets.background);
        lDrawCmdBuffer.Draw(4);

        // Cube
        lDrawCmdBuffer.BindVertexBuffer(mVtxBufferRes.buffer);
        lDrawCmdBuffer.BindIndexBuffer(mIdxBufferRes.buffer);
        lDrawCmdBuffer.BindGraphicsPipeline(mPipelines.cube);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayouts.cube, 0, mDescriptorSets.cube);
        lDrawCmdBuffer.DrawIndexed(mIndexCount);

        lDrawCmdBuffer.EndRenderPass();

        lDrawCmdBuffer.End();
    }
}


template <vkpp::ImageLayout OldLayout, vkpp::ImageLayout NewLayout>
void SceneRenderer::TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange, const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask)
{
    // Create an image barrier object.
    const vkpp::ImageMemoryBarrier lImageBarrier
    {
        aSrcAccessMask, aDstAccessMask,
        OldLayout, NewLayout,
        aImage,
        aImageSubRange
    };

    // Put barrier inside setup command buffer.
    // Put barrier on top of pipeline.
    aCmdBuffer.PipelineBarrier(vkpp::PipelineStageFlagBits::eTopOfPipe, vkpp::PipelineStageFlagBits::eTopOfPipe, vkpp::DependencyFlagBits::eByRegion, { lImageBarrier });
}


vkpp::CommandBuffer SceneRenderer::BeginOneTimeCmdBuffer(void) const
{
    const vkpp::CommandBufferAllocateInfo lCmdBufferAllocateInfo
    {
        mCmdPool, 1
    };

    auto lCmdBuffer = mLogicalDevice.AllocateCommandBuffer(lCmdBufferAllocateInfo);

    constexpr vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo
    {
        vkpp::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    lCmdBuffer.Begin(lCmdBufferBeginInfo);

    return lCmdBuffer;
}


void SceneRenderer::EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const
{
    aCmdBuffer.End();

    constexpr vkpp::FenceCreateInfo lFenceCreateInfo;
    const auto& lFence = mLogicalDevice.CreateFence(lFenceCreateInfo);

    const vkpp::SubmitInfo lSubmitInfo{ aCmdBuffer };

    mGraphicsQueue.handle.Submit(lSubmitInfo, lFence);

    mLogicalDevice.WaitForFence(lFence);

    mLogicalDevice.DestroyFence(lFence);
    mLogicalDevice.FreeCommandBuffer(mCmdPool, aCmdBuffer);
}


void SceneRenderer::CreateSemaphores(void)
{
    constexpr vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    mPresentCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mRenderingCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void SceneRenderer::CreateFences(void)
{
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    for (std::size_t lIndex = 0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
        mWaitFences.emplace_back(mLogicalDevice.CreateFence(lFenceCreateInfo));
}


void SceneRenderer::UpdateTextOverlay(void)
{
    mTextRenderer->BeginTextUpdate();
    mTextRenderer->AddText(mTile, 5.0f, 5.0f, TextRenderer::TextAlign::eLeft);
    mTextRenderer->AddText(mPhysicalDeviceProperties.deviceName, 5.0f, 25.0f, TextRenderer::TextAlign::eLeft);

    auto lWidth = static_cast<float>(mSwapchain.extent.width);
    auto lHeight = static_cast<float>(mSwapchain.extent.height);

    // Display projected cube vertices
    for (auto x = -1; x <= 1; x += 2)
    {
        for (auto y = -1; y <= 1; y += 2)
        {
            for (auto z = -1; z <= 1; z += 2)
            {
                std::stringstream vpos;
                vpos << std::showpos << x << "/" << y << "/" << z;
                auto projected = glm::project(glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)), mSceneMatrix.model, mSceneMatrix.proj,
                    glm::vec4(0, 0, static_cast<float>(lWidth), static_cast<float>(lHeight)));
                mTextRenderer->AddText(vpos.str(), projected.x, projected.y + (y > -1 ? 5.0f : -20.0f), TextRenderer::TextAlign::eCenter);
            }
        }
    }

    // Display current model view matrix
    mTextRenderer->AddText("Model View Matrix", static_cast<float>(lWidth), 5.0f, TextRenderer::TextAlign::eRight);

    std::stringstream ss;
    for (uint32_t i = 0; i < 4; i++)
    {
        ss.str("");
        ss << std::fixed << std::setprecision(2) << std::showpos;
        ss << mSceneMatrix.model[0][i] << " " << mSceneMatrix.model[1][i] << " " << mSceneMatrix.model[2][i] << " " << mSceneMatrix.model[3][i];
        mTextRenderer->AddText(ss.str(), static_cast<float>(lWidth), 25.0f + static_cast<float>(i) * 20.0f, TextRenderer::TextAlign::eRight);
    }

    mTextRenderer->EndTextUpdate();
}


void SceneRenderer::Update(void)
{
    UpdateTextOverlay();

    auto lIndex = mLogicalDevice.AcquireNextImage(mSwapchain.handle, mPresentCompleteSemaphore);

    mLogicalDevice.WaitForFence(mWaitFences[lIndex]);
    mLogicalDevice.ResetFence(mWaitFences[lIndex]);

    constexpr vkpp::PipelineStageFlags lWaitDstStageMask{ vkpp::PipelineStageFlagBits::eColorAttachmentOutput };

    const vkpp::SubmitInfo lSubmitInfo
    {
        1, mPresentCompleteSemaphore.AddressOf(),
        &lWaitDstStageMask,
        1, mDrawCmdBuffers[lIndex].AddressOf(),
        1, mRenderingCompleteSemaphore.AddressOf()
    };

    mPresentQueue.handle.Submit(lSubmitInfo, mWaitFences[lIndex]);

    // Submit text overlay to queue
    mTextRenderer->Submit(lIndex);

    const vkpp::khr::PresentInfo lPresentInfo
    {
        1, mRenderingCompleteSemaphore.AddressOf(),
        1, mSwapchain.handle.AddressOf(),
        &lIndex
    };

    mPresentQueue.handle.Present(lPresentInfo);
}


}                   // End of namespace vkpp::sample.