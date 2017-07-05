#include "RadialBlur.h"



namespace vkpp::sample
{



RadialBlur::RadialBlur(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName, uint32_t aEngineVersion)
    : ExampleBase(aWindow, apAppName, aAppVersion, apEngineName, aEngineVersion),
      CWindowEvent(aWindow), CMouseMotionEvent(aWindow), CMouseWheelEvent(aWindow),
      mDepthRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mOffscreenFrame(mLogicalDevice, mPhysicalDeviceMemoryProperties, 512, 512, mSwapchain.surfaceFormat.format, vkpp::Format::eD32sFloat),
      mUboScene(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mUboBlurParams(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mExample(mLogicalDevice, *this, mPhysicalDeviceMemoryProperties),
      mTextureRes(mLogicalDevice, mPhysicalDeviceMemoryProperties)
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
    CreateDepthResource();
    CreateFramebuffers();

    CreateSetLayouts();
    CreatePipelineLayouts();
    CreateGraphicsPipeline();
    CreateDescriptorPool();
    AllocateDescriptorSets();

    CreateUniformBuffers();
    UpdateUniformBuffers();

    mExample.LoadModel("Model/glowsphere.dae", 0.05f);
    LoadTexture("Texture/particle_gradient_rgba.ktx", vkpp::Format::eRGBA8uNorm);
    CreateSampler();
    UpdateDescriptorSets();

    BuildCmdBuffers();
    BuildOffscreenCmdBuffer();

    CreateSemaphores();
    CreateFences();
}


RadialBlur::~RadialBlur(void)
{
    mLogicalDevice.Wait();

    mLogicalDevice.DestroySemaphore(mOffScreenCompleteSemaphore);
    mLogicalDevice.DestroySemaphore(mRenderingCompleteSemaphore);
    mLogicalDevice.DestroySemaphore(mPresentCompleteSemaphore);

    mLogicalDevice.DestroyFences(mWaitFences);

    mLogicalDevice.DestroySampler(mTextureSampler);

    mLogicalDevice.UnmapMemory(mUboBlurParams.memory);
    mLogicalDevice.UnmapMemory(mUboScene.memory);

    mUboBlurParams.Reset();
    mUboScene.Reset();

    mLogicalDevice.DestroyDescriptorPool(mDescriptorPool);

    mLogicalDevice.DestroyPipeline(mPipelines.offscreenDisplay);
    mLogicalDevice.DestroyPipeline(mPipelines.radialBlur);
    mLogicalDevice.DestroyPipeline(mPipelines.phongPass);
    mLogicalDevice.DestroyPipeline(mPipelines.colorPass);

    mLogicalDevice.DestroyPipelineLayout(mPipelineLayouts.scene);
    mLogicalDevice.DestroyPipelineLayout(mPipelineLayouts.radialBlur);

    mLogicalDevice.DestroyDescriptorSetLayout(mSetLayouts.scene);
    mLogicalDevice.DestroyDescriptorSetLayout(mSetLayouts.radialBlur);

    mLogicalDevice.DestroyFramebuffers(mFramebuffers);

    mDepthRes.Reset();

    mLogicalDevice.DestroyRenderPass(mRenderPass);
    mLogicalDevice.FreeCommandBuffers(mCmdPool, mDrawCmdBuffers);
    mLogicalDevice.DestroyCommandPool(mCmdPool);
}


void RadialBlur::CreateCmdPool(void)
{
    const vkpp::CommandPoolCreateInfo lCmdCreateInfo
    {
        mGraphicsQueue.familyIndex,
        vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer
    };

    mCmdPool = mLogicalDevice.CreateCommandPool(lCmdCreateInfo);
}


void RadialBlur::AllocateCmdBuffers(void)
{
    const vkpp::CommandBufferAllocateInfo lCmdBufferAllocateInfo
    {
        mCmdPool,
        static_cast<uint32_t>(mSwapchain.buffers.size())
    };

    mDrawCmdBuffers = mLogicalDevice.AllocateCommandBuffers(lCmdBufferAllocateInfo);
}


void RadialBlur::CreateRenderPass(void)
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


void RadialBlur::CreateDepthResource(void)
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


void RadialBlur::CreateFramebuffers(void)
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


void RadialBlur::CreateSetLayouts(void)
{
    const std::vector<vkpp::DescriptorSetLayoutBinding> lSetLayoutBindings
    {
        // Binding 0: Vertex shader uniform buffer
        {
            0,
            vkpp::DescriptorType::eUniformBuffer,
            vkpp::ShaderStageFlagBits::eVertex
        },
        // Binding 1: Fragment shader image sampler
        {
            1,
            vkpp::DescriptorType::eCombinedImageSampler,
            vkpp::ShaderStageFlagBits::eFragment
        },
        // Binding 2: Fragment shader uniform buffer
        {
            2,
            vkpp::DescriptorType::eUniformBuffer,
            vkpp::ShaderStageFlagBits::eFragment
        }
    };

    // Scene rendering (use first and second layout binding)
    auto lSceneSetLayoutBindings = { lSetLayoutBindings[0], lSetLayoutBindings[1] };
    mSetLayouts.scene = mLogicalDevice.CreateDescriptorSetLayout(lSceneSetLayoutBindings);

    // Fullscreen radial blur (use second and third layout binding)
    lSceneSetLayoutBindings = { lSetLayoutBindings[1], lSetLayoutBindings[2] };
    mSetLayouts.radialBlur = mLogicalDevice.CreateDescriptorSetLayout(lSceneSetLayoutBindings);
}


void RadialBlur::CreatePipelineLayouts(void)
{
    mPipelineLayouts.radialBlur = mLogicalDevice.CreatePipelineLayout({ mSetLayouts.radialBlur });
    mPipelineLayouts.scene = mLogicalDevice.CreatePipelineLayout({ mSetLayouts.scene });
}


void RadialBlur::CreateGraphicsPipeline(void)
{
    constexpr auto lVertexInputBinding = VertexData::GetBindingDescription();
    constexpr auto lVertexAttributes = VertexData::GetAttributeDescriptions();
    vkpp::PipelineVertexInputStateCreateInfo lInputStateCreateInfo;

    constexpr vkpp::PipelineInputAssemblyStateCreateInfo lInputAssemblyStateCreateInfo{ vkpp::PrimitiveTopology::eTriangleList };
    constexpr vkpp::PipelineViewportStateCreateInfo lViewportStateCreateInfo{ 1, 1 };
    constexpr vkpp::PipelineRasterizationStateCreateInfo lRasterizationStateCreateInfo
    {
        vkpp::PolygonMode::eFill,
        vkpp::CullModeFlagBits::eNone,
        vkpp::FrontFace::eCounterClockwise
    };

    constexpr vkpp::PipelineMultisampleStateCreateInfo lMultisampleStateCreateInfo;
    constexpr vkpp::PipelineDepthStencilStateCreateInfo lDepthStencilStateCreateInfo
    {
        vkpp::DepthTest::Enable, vkpp::DepthWrite::Enable,
        vkpp::CompareOp::eLess
    };

    vkpp::PipelineColorBlendAttachmentState lColorBlendAttachmentState;
    vkpp::PipelineColorBlendStateCreateInfo lColorBlendStateCreateInfo{ lColorBlendAttachmentState };

    constexpr std::array<vkpp::DynamicState, 2> lDynamicStates
    {
        vkpp::DynamicState::eViewport, vkpp::DynamicState::eScissor
    };

    const vkpp::PipelineDynamicStateCreateInfo lDynamicStateCreateInfo{ lDynamicStates };

    // Radial blur pipeline
    std::array<vkpp::PipelineShaderStageCreateInfo, 2> lShaderStageCreateInfos
    { {
        { vkpp::ShaderStageFlagBits::eVertex },
        { vkpp::ShaderStageFlagBits::eFragment }
    } };

    auto& lVertexShaderModule = lShaderStageCreateInfos[0].module;
    auto& lFragmentShaderModule = lShaderStageCreateInfos[1].module;

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
        mPipelineLayouts.radialBlur,
        mRenderPass,
        0
    };

    lColorBlendAttachmentState
        .EnableBlendOp()
        .SetColorBlend(vkpp::BlendFactor::eOne, vkpp::BlendFactor::eOne, vkpp::BlendOp::eAdd)
        .SetAlphaBlend(vkpp::BlendFactor::eSrcAlpha, vkpp::BlendFactor::eDstAlpha, vkpp::BlendOp::eAdd);

    lVertexShaderModule = CreateShaderModule("Shader/SPV/radialblur.vert.spv");
    lFragmentShaderModule = CreateShaderModule("Shader/SPV/radialblur.frag.spv");

    mPipelines.radialBlur = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    // No blending for debug display
    lColorBlendAttachmentState.DisableBlendOp();
    mPipelines.offscreenDisplay = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);

    // Phong pass
    lInputStateCreateInfo
        .SetBindingDescription(lVertexInputBinding)
        .SetAttributeDescriptions(lVertexAttributes);

    lGraphicsPipelineCreateInfo.SetLayout(mPipelineLayouts.scene);
    lVertexShaderModule = CreateShaderModule("Shader/SPV/phongpass.vert.spv");
    lFragmentShaderModule = CreateShaderModule("Shader/SPV/phongpass.frag.spv");
    mPipelines.phongPass = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);

    // Color only pass (offscreen blur phase)
    lVertexShaderModule = CreateShaderModule("Shader/SPV/colorpass.vert.spv");
    lFragmentShaderModule = CreateShaderModule("Shader/SPV/colorpass.frag.spv");
    mPipelines.colorPass = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);
}


void RadialBlur::CreateDescriptorPool(void)
{
    // Example uses three ubos and one image sampler.
    constexpr std::array<vkpp::DescriptorPoolSize, 2> lPoolSizes
    { {
        { vkpp::DescriptorType::eUniformBuffer, 2 },
        { vkpp::DescriptorType::eCombinedImageSampler, 2 }
    } };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool({ lPoolSizes, 2 });
}


void RadialBlur::AllocateDescriptorSets(void)
{
    // Fullscreen radial blur
    vkpp::DescriptorSetAllocateInfo lSetAllocateInfo
    {
        mDescriptorPool, mSetLayouts.radialBlur
    };

    mDescriptorSets.radialBlur = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);

    // Scene rendering
    lSetAllocateInfo.SetLayout(mSetLayouts.scene);
    mDescriptorSets.scene = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);
}


void RadialBlur::CreateUniformBuffers(void)
{
    constexpr auto lMemFlags = vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent;

    vkpp::BufferCreateInfo lBufferCreateInfo{ sizeof(UBOScene), vkpp::BufferUsageFlagBits::eUniformBuffer };

    // Phong and color pass vertex shader uniform buffer
    mUboScene.Reset(lBufferCreateInfo, lMemFlags);

    // Fullscreen radial blur parameters
    lBufferCreateInfo.SetSize(sizeof(UBOBlurParams));
    mUboBlurParams.Reset(lBufferCreateInfo, lMemFlags);

    mpUboSceneMapped = mLogicalDevice.MapMemory(mUboScene.memory);

    mpUboBlurParams = mLogicalDevice.MapMemory(mUboBlurParams.memory);
    std::memcpy(mpUboBlurParams, &mBlurParams, sizeof(UBOBlurParams));
}


void RadialBlur::UpdateUniformBuffers(void)
{
    auto lWidth = static_cast<float>(mSwapchain.extent.width);
    auto lHeight = static_cast<float>(mSwapchain.extent.height);

    const static glm::vec3 lCameraPos;

    mSceneMatrix.proj = glm::perspective(glm::radians(45.0f), lWidth / lHeight, 1.0f, 256.0f);

    const auto& lViewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, mCurrentZoomLevel));
    mSceneMatrix.model = lViewMatrix * glm::translate(glm::mat4(), lCameraPos);
    mSceneMatrix.model = glm::rotate(mSceneMatrix.model, glm::radians(mCurrentRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    mSceneMatrix.model = glm::rotate(mSceneMatrix.model, glm::radians(mCurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    mSceneMatrix.model = glm::rotate(mSceneMatrix.model, glm::radians(mCurrentRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    mSceneMatrix.gradientPos += 0.1f;

    std::memcpy(mpUboSceneMapped, &mSceneMatrix, sizeof(UBOScene));
}


void RadialBlur::LoadTexture(const std::string& aFilename, vkpp::Format aTexFormat)
{
    const gli::texture2d lTex2D{ gli::load(aFilename) };
    assert(!lTex2D.empty());

    mTexture.width = static_cast<uint32_t>(lTex2D[0].extent().x);
    mTexture.height = static_cast<uint32_t>(lTex2D[0].extent().y);
    mTexture.mipLevels = static_cast<uint32_t>(lTex2D.levels());

    // Get device properties for the requested texture format.
    // const auto& lFormatProperties = mPhysicalDevice.GetFormatProperties(mTexFormat);

    // Create a host-visible staging buffer that contains the raw image data.
    const vkpp::BufferCreateInfo lStagingBufferCreateInfo
    {
        lTex2D.size(),
        vkpp::BufferUsageFlagBits::eTransferSrc             // This buffer is used as a transfer source for the buffer copy.
    };

    BufferResource lStagingBufferRes{ mLogicalDevice, mPhysicalDeviceMemoryProperties };
    lStagingBufferRes.Reset(lStagingBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lMappedMem = mLogicalDevice.MapMemory(lStagingBufferRes.memory, 0, lTex2D.size());
    std::memcpy(lMappedMem, lTex2D.data(), lTex2D.size());
    mLogicalDevice.UnmapMemory(lStagingBufferRes.memory);

    // Setup buffer copy regions for each mip-level.
    std::vector<vkpp::BufferImageCopy> lBufferCopyRegions;
    uint32_t lOffset{ 0 };

    for (uint32_t lIndex = 0; lIndex < mTexture.mipLevels; ++lIndex)
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
        { mTexture.width, mTexture.height, 1 },
        vkpp::ImageUsageFlagBits::eSampled | vkpp::ImageUsageFlagBits::eTransferDst,
        vkpp::ImageLayout::eUndefined,
        vkpp::ImageTiling::eOptimal,
        vkpp::SampleCountFlagBits::e1,
        mTexture.mipLevels
    };

    vkpp::ImageViewCreateInfo lImageViewCreateInfo
    {
        vkpp::ImageViewType::e2D,
        aTexFormat,
        // The subresource range describes the set of mip-levels (and array layers) that can be accessed through this image view.
        // It is possible to create multiple image views for a single image referring to different (and/or overlapping) ranges of the image.
        {
            vkpp::ImageAspectFlagBits::eColor,
            0, mTexture.mipLevels,
            0, 1
        }
    };

    mTextureRes.Reset(lImageCreateInfo, lImageViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    const auto& lCopyCmd = BeginOneTimeCmdBuffer();

    // Image barrier for optimal image.

    // The sub-resource range describes the regions of the image which will be transitioned.
    const vkpp::ImageSubresourceRange lImageSubRange
    {
        vkpp::ImageAspectFlagBits::eColor,              // aspectMask: Only contains color data.
        0,                                              // baseMipLevel: Start at first mip-level.
        mTexture.mipLevels,                             // levelCount: Transition on all mip-levels.
        0,                                              // baseArrayLayer: Start at first element in the array. (only one element in this example.)
        1                                               // layerCount: The 2D texture only has one layer.
    };

    // Optimal image will be used as the destination for the copy, so it must be transfered from the initial undefined image layout to the transfer destination layout.
    TransitionImageLayout<vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eTransferDstOptimal>
    (
        lCopyCmd, mTextureRes.image,
        lImageSubRange,
        vkpp::DefaultFlags,                         // srcAccessMask = 0: Only valid as initial layout, memory contents are not preserved.
                                                    //                    Can be accessed directly, no source dependency required.
        vkpp::AccessFlagBits::eTransferWrite        // dstAccessMask: Transfer destination (copy, blit).
                                                    //                Make sure any write operation to the image has been finished.
    );

    // Copy all mip-levels from staging buffer.
    lCopyCmd.Copy(mTextureRes.image, vkpp::ImageLayout::eTransferDstOptimal, lStagingBufferRes.buffer, lBufferCopyRegions);

    // Transfer texture image layout to shader read after all mip-levels have been copied.
    TransitionImageLayout<vkpp::ImageLayout::eTransferDstOptimal, vkpp::ImageLayout::eShaderReadOnlyOptimal>
    (
        lCopyCmd, mTextureRes.image,
        lImageSubRange,
        vkpp::AccessFlagBits::eTransferWrite,       // srcAccessMask: Old layout is transfer destination.
                                                    //                Make sure any write operation to the destination image has been finished.
        vkpp::AccessFlagBits::eShaderRead           // dstAccessMask: Shader read, like sampler, input attachment.
    );

    EndOneTimeCmdBuffer(lCopyCmd);

    lStagingBufferRes.Reset();
}


void RadialBlur::CreateSampler()
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
        static_cast<float>(mTexture.mipLevels),                     // maxLoad: Set max level-of-detail to mip-level count of the texture.
        vkpp::BorderColor::eFloatOpaqueWhite                        // borderColor
    };

    mTextureSampler = mLogicalDevice.CreateSampler(lSamplerCreateInfo);
}


void RadialBlur::BuildCmdBuffers(void)
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

        // 3D scene
        lDrawCmdBuffer.BindGraphicsPipeline(mPipelines.phongPass);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayouts.scene, 0, mDescriptorSets.scene);
        lDrawCmdBuffer.BindVertexBuffer(mExample.mVtxBufferRes.buffer);
        lDrawCmdBuffer.BindIndexBuffer(mExample.mIdxBufferRes.buffer);
        lDrawCmdBuffer.DrawIndexed(mExample.mIndexCount);

        // Fullscreen triangle (clipped to a quad) with radial blur
        lDrawCmdBuffer.BindGraphicsPipeline(mPipelines.radialBlur);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayouts.radialBlur, 0, mDescriptorSets.radialBlur);
        lDrawCmdBuffer.Draw(3);

        lDrawCmdBuffer.EndRenderPass();

        lDrawCmdBuffer.End();
    }
}


void RadialBlur::UpdateDescriptorSets(void)
{
    // Scene rendering
    const vkpp::DescriptorBufferInfo lVSUbo{ mUboScene.buffer };
    const vkpp::DescriptorBufferInfo lFSUbo{ mUboBlurParams.buffer };
    const vkpp::DescriptorImageInfo lTexSampler{ mTextureSampler, mTextureRes.view, mTexture.layout };

    std::array<vkpp::WriteDescriptorSetInfo, 2> lWriteDescriptorSetInfos
    { {
        // Binding 0: Vertex shader uniform buffer
        {
            mDescriptorSets.scene, 0,
            vkpp::DescriptorType::eUniformBuffer,
            lVSUbo
        },
        // Binding 1: Color gradient sampler
        {
            mDescriptorSets.scene, 1,
            vkpp::DescriptorType::eCombinedImageSampler,
            lTexSampler
        }
    } };

    mLogicalDevice.UpdateDescriptorSets(lWriteDescriptorSetInfos);

    // Fullscreen radial blur
    lWriteDescriptorSetInfos[0]
        .SetDescriptorSet(mDescriptorSets.radialBlur)
        .SetBinding(2)
        .SetBuffer(lFSUbo);

    const vkpp::DescriptorImageInfo lOffscreenTexSampler{ mOffscreenFrame.sampler, mOffscreenFrame.color.view, vkpp::ImageLayout::eShaderReadOnlyOptimal };

    lWriteDescriptorSetInfos[1]
        .SetDescriptorSet(mDescriptorSets.radialBlur)
        .SetImage(lOffscreenTexSampler);

    mLogicalDevice.UpdateDescriptorSets(lWriteDescriptorSetInfos);
}


void RadialBlur::CreateSemaphores(void)
{
    constexpr vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    mPresentCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mRenderingCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mOffScreenCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void RadialBlur::CreateFences(void)
{
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    for (std::size_t lIndex = 0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
        mWaitFences.emplace_back(mLogicalDevice.CreateFence(lFenceCreateInfo));
}


template <vkpp::ImageLayout OldLayout, vkpp::ImageLayout NewLayout>
void RadialBlur::TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange, const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask)
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


vkpp::CommandBuffer RadialBlur::BeginOneTimeCmdBuffer(void) const
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


void RadialBlur::EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const
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


void RadialBlur::CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const
{
    const auto& lCopyCmd = BeginOneTimeCmdBuffer();

    const vkpp::BufferCopy lBufferCopy{ aSize };

    lCopyCmd.Copy(aDstBuffer, aSrcBuffer, lBufferCopy);

    EndOneTimeCmdBuffer(lCopyCmd);
}


void RadialBlur::Update(void)
{
    auto lIndex = mLogicalDevice.AcquireNextImage(mSwapchain.handle, mPresentCompleteSemaphore);

    /*mLogicalDevice.WaitForFence(mWaitFences[lIndex]);
    mLogicalDevice.ResetFence(mWaitFences[lIndex]);*/

    constexpr vkpp::PipelineStageFlags lWaitDstStageMask{ vkpp::PipelineStageFlagBits::eColorAttachmentOutput };

    // Offscreen rendering
    vkpp::SubmitInfo lSubmitInfo
    {
        1, mPresentCompleteSemaphore.AddressOf(),
        &lWaitDstStageMask,
        1, mOffscreenFrame.cmdbuffer.AddressOf(),
        1, mOffScreenCompleteSemaphore.AddressOf()
    };

    mPresentQueue.handle.Submit(lSubmitInfo/*, mWaitFences[lIndex]*/);

    // Scene rendering
    lSubmitInfo
        .SetWaitSemaphore(mOffScreenCompleteSemaphore, lWaitDstStageMask)
        .SetCommandBuffer(mDrawCmdBuffers[lIndex])
        .SetSignalSemaphore(mRenderingCompleteSemaphore);

    mPresentQueue.handle.Submit(lSubmitInfo);

    const vkpp::khr::PresentInfo lPresentInfo
    {
        1, mRenderingCompleteSemaphore.AddressOf(),
        1, mSwapchain.handle.AddressOf(),
        &lIndex
    };

    mPresentQueue.handle.Present(lPresentInfo);

    mLogicalDevice.Wait();
}


// Setup the command buffer that renders the scene to the offscreen frame buffer.
void RadialBlur::BuildOffscreenCmdBuffer(void)
{
    auto& lCmdBuffer = mOffscreenFrame.AllocateCmdBuffer(mCmdPool);

    constexpr vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo;

    constexpr vkpp::ClearValue lClearValues[]
    {
        { 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f }
    };

    const vkpp::RenderPassBeginInfo lRenderPassBeginInfo
    {
        mOffscreenFrame.renderpass,
        mOffscreenFrame.framebuffer,
        {
            { 0, 0 },
            { mOffscreenFrame.width, mOffscreenFrame. height }
        },
        2, lClearValues
    };

    lCmdBuffer.Begin(lCmdBufferBeginInfo);

    lCmdBuffer.BeginRenderPass(lRenderPassBeginInfo);
    lCmdBuffer.BindVertexBuffer(mExample.mVtxBufferRes.buffer);
    lCmdBuffer.BindIndexBuffer(mExample.mIdxBufferRes.buffer);

    const vkpp::Viewport lViewport
    {
        0.0f, 0.0f,
        static_cast<float>(mOffscreenFrame.width), static_cast<float>(mOffscreenFrame.height)
    };

    lCmdBuffer.SetViewport(lViewport);

    const vkpp::Rect2D lScissor
    {
        0, 0 ,
        mOffscreenFrame.width, mOffscreenFrame.height
    };

    lCmdBuffer.SetScissor(lScissor);

    lCmdBuffer.BindGraphicsPipeline(mPipelines.colorPass);
    lCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayouts.scene, 0, mDescriptorSets.scene);

    lCmdBuffer.DrawIndexed(mExample.mIndexCount);

    lCmdBuffer.EndRenderPass();

    lCmdBuffer.End();
}


void Model::LoadModel(const std::string& aFilename, float aScale)
{
    // Flags for loading the mesh
    constexpr static auto lAssimpFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices;

    Assimp::Importer lImporter;

    auto lpScene = lImporter.ReadFile(aFilename, lAssimpFlags);
    assert(lpScene != nullptr);

    std::vector<float> lVertexBuffer;
    std::vector<uint32_t> lIndexBuffer;

    const aiVector3D lZero3D;

    // Iterate through all meshes in the file and extract the vertex components.
    for (unsigned int lMeshIdx = 0; lMeshIdx < lpScene->mNumMeshes; ++lMeshIdx)
    {
        const auto lpMesh = lpScene->mMeshes[lMeshIdx];
        mIndexCount += lpScene->mMeshes[lMeshIdx]->mNumFaces * 3;

        aiColor3D lColor;
        lpScene->mMaterials[lpMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, lColor);

        for (unsigned int lVtxIdx = 0; lVtxIdx < lpMesh->mNumVertices; ++lVtxIdx)
        {
            // Vertex positions
            const auto lPos = lpMesh->mVertices[lVtxIdx];
            lVertexBuffer.emplace_back(lPos.x * aScale);
            lVertexBuffer.emplace_back(-lPos.y * aScale);                // Vulkan use a right handed NDC.
            lVertexBuffer.emplace_back(lPos.z * aScale);

            // Vertex color
            lVertexBuffer.emplace_back(lColor.r);
            lVertexBuffer.emplace_back(lColor.g);
            lVertexBuffer.emplace_back(lColor.b);

            // Vertex normal
            const auto lNormal = lpMesh->mNormals[lVtxIdx];
            lVertexBuffer.emplace_back(lNormal.x);
            lVertexBuffer.emplace_back(lNormal.y);
            lVertexBuffer.emplace_back(lNormal.z);
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

    mRenderer.CopyBuffer(mVtxBufferRes.buffer, lVtxStagingBuffer.buffer, lVtxBufferSize);
    mRenderer.CopyBuffer(mIdxBufferRes.buffer, lIdxStagingBuffer.buffer, lIdxBufferSize);
}


}                   // End of namespace vkpp::sample.