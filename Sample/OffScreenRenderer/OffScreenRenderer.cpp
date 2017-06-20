#include "OffScreenRenderer.h"



namespace vkpp::sample
{



OffScreenRenderer::OffScreenRenderer(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName, uint32_t aEngineVersion)
    : ExampleBase(aWindow, apAppName, aAppVersion, apEngineName, aEngineVersion),
      CWindowEvent(aWindow), CMouseMotionEvent(aWindow), CMouseWheelEvent(aWindow),
      mDepthRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mOffscreenFrame(mLogicalDevice, mPhysicalDeviceMemoryProperties, 512, 512, mSwapchain.surfaceFormat.format, vkpp::Format::eD32sFloat),
      mUBORes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mPlane(mLogicalDevice, *this, mPhysicalDeviceMemoryProperties),
      mMesh(mLogicalDevice, *this, mPhysicalDeviceMemoryProperties),
      mDebugQuadVtxRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mDebugQuadIdxRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
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
        UpdateUniformBufferOffScreen();
    };

    mMouseMotionFunc = [this](Uint32 aMouseState, Sint32 /*aPosX*/, Sint32 /*aPosY*/, Sint32 aRelativeX, Sint32 aRelativeY)
    {
        if (aMouseState & CMouseEvent::ButtonMask::eLMask)
        {
            mCurrentRotation.x += aRelativeY;
            mCurrentRotation.y += aRelativeX;

            UpdateUniformBuffers();
            UpdateUniformBufferOffScreen();
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
    UpdateUniformBufferOffScreen();

    GenerateDebugQuad();
    mPlane.LoadModel("Model/plane.obj", 0.5f);
    mMesh.LoadModel("Model/ChineseDragon.dae", 0.3f);
    LoadTexture("Texture/darkmetal_bc3_unorm.ktx", vkpp::Format::eBC3_uNormBlock);
    CreateSampler();
    UpdateDescriptorSets();

    BuildCmdBuffers();
    BuildOffScreenCmdBuffer();

    CreateSemaphores();
    CreateFences();
}


OffScreenRenderer::~OffScreenRenderer(void)
{
    mLogicalDevice.Wait();

    mLogicalDevice.DestroySemaphore(mOffScreenCompleteSemaphore);
    mLogicalDevice.DestroySemaphore(mRenderingCompleteSemaphore);
    mLogicalDevice.DestroySemaphore(mPresentCompleteSemaphore);

    mLogicalDevice.DestroyFences(mWaitFences);

    mLogicalDevice.DestroySampler(mTextureSampler);

    mTextureRes.Reset();

    mUBORes.Unmap();

    mLogicalDevice.DestroyDescriptorPool(mDescriptorPool);

    mLogicalDevice.DestroyPipeline(mPipelines.shadedOffscreen);
    mLogicalDevice.DestroyPipeline(mPipelines.shaded);
    mLogicalDevice.DestroyPipeline(mPipelines.mirror);
    mLogicalDevice.DestroyPipeline(mPipelines.debug);

    mLogicalDevice.DestroyPipelineLayout(mPipelineLayouts.shaded);
    mLogicalDevice.DestroyPipelineLayout(mPipelineLayouts.textured);

    mLogicalDevice.DestroyDescriptorSetLayout(mSetLayouts.shaded);
    mLogicalDevice.DestroyDescriptorSetLayout(mSetLayouts.textured);

    mLogicalDevice.DestroyFramebuffers(mFramebuffers);
    mDepthRes.Reset();
    mLogicalDevice.DestroyRenderPass(mRenderPass);

    // mLogicalDevice.FreeCommandBuffer(mCmdPool, mOffscreenFrame.cmdbuffer);
    mLogicalDevice.FreeCommandBuffers(mCmdPool, mDrawCmdBuffers);
    mLogicalDevice.DestroyCommandPool(mCmdPool);
}


void OffScreenRenderer::CreateCmdPool(void)
{
    mCmdPool = mLogicalDevice.CreateCommandPool({ mGraphicsQueue.familyIndex, vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer });
}


void OffScreenRenderer::AllocateCmdBuffers(void)
{
    mDrawCmdBuffers = mLogicalDevice.AllocateCommandBuffers({ mCmdPool, static_cast<uint32_t>(mSwapchain.buffers.size()) });
}


void OffScreenRenderer::CreateRenderPass(void)
{
    const std::vector<vkpp::AttachementDescription> lAttachementDescriptions
    {
        // Depth Attachment
        {
            vkpp::Format::eD32sFloat,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear,
            vkpp::AttachmentStoreOp::eDontCare,
            vkpp::ImageLayout::eUndefined,
            vkpp::ImageLayout::eDepthStencilAttachmentOptimal
        },
        // Color Attachment
        {
            mSwapchain.surfaceFormat.format,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear,
            vkpp::AttachmentStoreOp::eStore,
            vkpp::ImageLayout::eUndefined,
            vkpp::ImageLayout::ePresentSrcKHR
        }
    };

    constexpr vkpp::AttachmentReference lDepthRef
    {
        0, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
    };

    constexpr vkpp::AttachmentReference lColorRef
    {
        1, vkpp::ImageLayout::eColorAttachmentOptimal
    };

    const std::vector<vkpp::SubpassDescription> lSubpassDescription
    {
        { vkpp::PipelineBindPoint::eGraphics, lColorRef, lDepthRef }
    };

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
        lAttachementDescriptions, lSubpassDescription, lSubpassDependencies
    };

    mRenderPass = mLogicalDevice.CreateRenderPass(lRenderPassCreateInfo);
}


void OffScreenRenderer::CreateDepthResource(void)
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


void OffScreenRenderer::CreateFramebuffers(void)
{
    // Depth/stencil attachment is the same for all frame buffers.
    std::array<vkpp::ImageView, 2> lAttachments{ mDepthRes.view };

    const vkpp::FramebufferCreateInfo lFramebufferCreateInfo
    {
        mRenderPass,
        lAttachments,
        mSwapchain.extent
    };

    // Create frame buffers for every swap chain image.
    for (auto& lSwapchain : mSwapchain.buffers)
    {
        lAttachments[1] = lSwapchain.view;

        mFramebuffers.emplace_back(mLogicalDevice.CreateFramebuffer(lFramebufferCreateInfo));
    }
}


void OffScreenRenderer::CreateSetLayouts(void)
{
    constexpr std::array<vkpp::DescriptorSetLayoutBinding, 3> lSetLayoutBindings
    { {
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
        // Binding 2: Fragment shader image sampler
        {
            2,
            vkpp::DescriptorType::eCombinedImageSampler,
            vkpp::ShaderStageFlagBits::eFragment
        }
    } };

    // Shaded layouts (only use first layout binding)
    mSetLayouts.shaded = mLogicalDevice.CreateDescriptorSetLayout({ lSetLayoutBindings[0] });

    // Textured layouts (use all layout bindings)
    mSetLayouts.textured = mLogicalDevice.CreateDescriptorSetLayout({ lSetLayoutBindings });
}


void OffScreenRenderer::CreatePipelineLayouts(void)
{
    mPipelineLayouts.shaded = mLogicalDevice.CreatePipelineLayout({ mSetLayouts.shaded });
    mPipelineLayouts.textured = mLogicalDevice.CreatePipelineLayout({ mSetLayouts.textured });
}


void OffScreenRenderer::CreateGraphicsPipeline(void)
{
    constexpr auto lVertexInputBinding = VertexData::GetBindingDescription();
    constexpr auto lVertexAttributes = VertexData::GetAttributeDescriptions();
    const vkpp::PipelineVertexInputStateCreateInfo lVertexInputStateCreateInfo
    {
        lVertexAttributes,
        1, lVertexInputBinding.AddressOf()
    };

    constexpr vkpp::PipelineInputAssemblyStateCreateInfo lAssemblyStateCreateInfo{ vkpp::PrimitiveTopology::eTriangleList };

    constexpr vkpp::PipelineViewportStateCreateInfo lViewportStateCreateInfo{ 1, 1 };

    vkpp::PipelineRasterizationStateCreateInfo lRasterizationStateCreateInfo
    {
        vkpp::PolygonMode::eFill,
        vkpp::CullModeFlagBits::eFront,
        vkpp::FrontFace::eClockwise
    };

    constexpr vkpp::PipelineMultisampleStateCreateInfo lMultisampleStateCreateInfo;

    constexpr vkpp::PipelineDepthStencilStateCreateInfo lDepthStencilStateCreateInfo
    {
        vkpp::DepthTest::Enable, vkpp::DepthWrite::Enable,
        vkpp::CompareOp::eLess
    };

    constexpr vkpp::PipelineColorBlendAttachmentState lColorBlendAttachmentState;
    constexpr vkpp::PipelineColorBlendStateCreateInfo lColorBlendStateCreateInfo{ lColorBlendAttachmentState };

    constexpr std::array<vkpp::DynamicState, 2> lDynamicStates
    {
        vkpp::DynamicState::eViewport, vkpp::DynamicState::eScissor
    };

    const vkpp::PipelineDynamicStateCreateInfo lDynamicStateCreateInfo{ lDynamicStates };

    // Solid rendering pipeline
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
        lVertexInputStateCreateInfo.AddressOf(),
        lAssemblyStateCreateInfo.AddressOf(),
        nullptr,
        lViewportStateCreateInfo.AddressOf(),
        lRasterizationStateCreateInfo.AddressOf(),
        lMultisampleStateCreateInfo.AddressOf(),
        lDepthStencilStateCreateInfo.AddressOf(),
        lColorBlendStateCreateInfo.AddressOf(),
        lDynamicStateCreateInfo.AddressOf(),
        mPipelineLayouts.textured,
        mRenderPass,
        0
    };

    lVertexShaderModule = CreateShaderModule("Shader/SPV/quad.vert.spv");
    lFragmentShaderModule = CreateShaderModule("Shader/SPV/quad.frag.spv");

    mPipelines.debug = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);

    // Mirror
    lVertexShaderModule = CreateShaderModule("Shader/SPV/mirror.vert.spv");
    lFragmentShaderModule = CreateShaderModule("Shader/SPV/mirror.frag.spv");
    lRasterizationStateCreateInfo.cullMode = vkpp::CullModeFlagBits::eNone;

    mPipelines.mirror = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);

    // Flip culling
    lRasterizationStateCreateInfo.cullMode = vkpp::CullModeFlagBits::eBack;

    // Phong shading pipelines
    lGraphicsPipelineCreateInfo.SetLayout(mPipelineLayouts.shaded);

    // Scene
    lVertexShaderModule = CreateShaderModule("Shader/SPV/phong.vert.spv");
    lFragmentShaderModule = CreateShaderModule("Shader/SPV/phong.frag.spv");
    mPipelines.shaded = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    // Offscreen
    // Flip culling
    lRasterizationStateCreateInfo.cullMode = vkpp::CullModeFlagBits::eFront;
    lGraphicsPipelineCreateInfo.renderPass = mOffscreenFrame.renderpass;
    mPipelines.shadedOffscreen = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);
}


void OffScreenRenderer::CreateDescriptorPool(void)
{
    constexpr std::array<vkpp::DescriptorPoolSize, 2> lPoolSizes
    { {
        { vkpp::DescriptorType::eUniformBuffer, 4 },
        { vkpp::DescriptorType::eCombinedImageSampler, 4 }
    } };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool({ lPoolSizes, 4 });
}


void OffScreenRenderer::AllocateDescriptorSets(void)
{
    vkpp::DescriptorSetAllocateInfo lSetAllocateInfo
    {
        mDescriptorPool, mSetLayouts.textured
    };

    // Mirror plane descriptor set
    mDescriptorSets.mirror = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);

    // Debug quad
    mDescriptorSets.debugQuad = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);

    // Shaded descriptor sets
    lSetAllocateInfo.SetLayout(mSetLayouts.shaded);

    // Model
    // No texture
    mDescriptorSets.model = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);

    // Offscreen
    mDescriptorSets.offscreen = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);
}


void OffScreenRenderer::CreateUniformBuffers(void)
{
    const vkpp::BufferCreateInfo lBufferCreateInfo
    {
        sizeof(UBOVS),
        vkpp::BufferUsageFlagBits::eUniformBuffer
    };

    constexpr auto lMemFlags = vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent;

    // Mesh vertex shader uniform buffer block
    mUBORes.vsShaded.Reset(lBufferCreateInfo, lMemFlags);

    // Mirror plane vertex shader uniform buffer block
    mUBORes.vsMirror.Reset(lBufferCreateInfo, lMemFlags);

    // Offscreen vertex shader uniform buffer block
    mUBORes.vsOffscreen.Reset(lBufferCreateInfo, lMemFlags);

    // Debug quad vertex shader uniform buffer block
    mUBORes.vsDebugQuad.Reset(lBufferCreateInfo, lMemFlags);

    mUBORes.Map();
}


void OffScreenRenderer::UpdateUniformBuffers(void)
{
    const static auto lWidth = static_cast<float>(mSwapchain.extent.width);
    const static auto lHeight = static_cast<float>(mSwapchain.extent.height);

    const static glm::vec3 lCameraPos{ 0.0f, 1.1f, 0.0f };

    // Mesh
    mMVPMatrix.proj = glm::perspective(glm::radians(60.f), lWidth / lHeight, 0.1f, 256.0f);

    const auto& lViewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, mCurrentZoomLevel));
    mMVPMatrix.model = lViewMatrix * glm::translate(glm::mat4(), lCameraPos);
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    mMVPMatrix.model = glm::translate(mMVPMatrix.model, mMeshPos);

    std::memcpy(mUBORes.vsShadedMappedMem, &mMVPMatrix, sizeof(UBOVS));

    // Mirror
    mMVPMatrix.model = lViewMatrix * glm::translate(glm::mat4(), lCameraPos);
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.y), glm::vec3(0.0, 1.0f, 0.0f));
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.z), glm::vec3(0.0, 0.0f, 1.0f));

    std::memcpy(mUBORes.vsMirrorMappedMem, &mMVPMatrix, sizeof(UBOVS));

    // Debug quad
    mMVPMatrix.proj = glm::ortho(4.0f, 0.0f, 0.0f, 4.0f * lHeight / lWidth, -1.0f, 1.0f);
    mMVPMatrix.model = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));

    std::memcpy(mUBORes.vsDebugQuadMappedMem, &mMVPMatrix, sizeof(UBOVS));
}


void OffScreenRenderer::UpdateUniformBufferOffScreen(void)
{
    const static auto lWidth = static_cast<float>(mSwapchain.extent.width);
    const static auto lHeight = static_cast<float>(mSwapchain.extent.height);

    const static glm::vec3 lCameraPos{ 0.1f, 1.1f, 0.0f };

    mMVPMatrix.proj = glm::perspective(glm::radians(60.0f), lWidth / lHeight, 0.1f, 256.0f);
    const auto& lViewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, mCurrentZoomLevel));

    mMVPMatrix.model = lViewMatrix * glm::translate(glm::mat4(), lCameraPos);
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    mMVPMatrix.model = glm::scale(mMVPMatrix.model, glm::vec3(1.0f, -1.0f, 1.0f));
    mMVPMatrix.model = glm::translate(mMVPMatrix.model, mMeshPos);

    std::memcpy(mUBORes.vsOffscreenMappedMem, &mMVPMatrix, sizeof(UBOVS));
}


void OffScreenRenderer::UpdateDescriptorSets(void) const
{
    // Mirror plane descriptor set
    const vkpp::DescriptorBufferInfo lVSMirrorDescriptor{ mUBORes.vsMirror.buffer };
    const vkpp::DescriptorImageInfo lOffScreenDescriptor{ mOffscreenFrame.sampler, mOffscreenFrame.color.view, vkpp::ImageLayout::eShaderReadOnlyOptimal };
    const vkpp::DescriptorImageInfo lColorMapDescriptor{ mTextureSampler, mTextureRes.view, vkpp::ImageLayout::eShaderReadOnlyOptimal };

    const std::vector<vkpp::WriteDescriptorSetInfo> lWriteDescriptorSetInfos
    {
        // Binding 0: Vertex shader uniform buffer
        {
            mDescriptorSets.mirror, 0,
            vkpp::DescriptorType::eUniformBuffer,
            lVSMirrorDescriptor
        },
        // Binding 1: Fragment shader texture sampler
        {
            mDescriptorSets.mirror, 1,
            vkpp::DescriptorType::eCombinedImageSampler,
            lOffScreenDescriptor
        },
        // Binding 2: Fragment shader texture sampler
        {
            mDescriptorSets.mirror, 2,
            vkpp::DescriptorType::eCombinedImageSampler,
            lColorMapDescriptor
        }
    };

    mLogicalDevice.UpdateDescriptorSets(lWriteDescriptorSetInfos);

    // Debug quad
    const vkpp::DescriptorBufferInfo lVSDebugQuadDescriptor{ mUBORes.vsDebugQuad.buffer };
    const std::vector<vkpp::WriteDescriptorSetInfo> lDebugQuadWriteDescriptorSets
    {
        // Binding 0: Vertex shader uniform buffer
        {
            mDescriptorSets.debugQuad, 0,
            vkpp::DescriptorType::eUniformBuffer,
            lVSDebugQuadDescriptor
        },
        // Binding 1: Fragment shader texture sampler
        {
            mDescriptorSets.debugQuad, 1,
            vkpp::DescriptorType::eCombinedImageSampler,
            lOffScreenDescriptor
        }
    };

    mLogicalDevice.UpdateDescriptorSets(lDebugQuadWriteDescriptorSets);

    // Model
    // No texture
    const vkpp::DescriptorBufferInfo lVSShadedDescriptorInfo{ mUBORes.vsShaded.buffer };
    const vkpp::WriteDescriptorSetInfo lModelWriteDescriptorSet
    {
        mDescriptorSets.model, 0,
        vkpp::DescriptorType::eUniformBuffer,
        lVSShadedDescriptorInfo
    };

    mLogicalDevice.UpdateDescriptorSet(lModelWriteDescriptorSet);

    // Offscreen
    const vkpp::DescriptorBufferInfo lOffScreenBufferDescriptor{ mUBORes.vsOffscreen.buffer };
    const vkpp::WriteDescriptorSetInfo lOffScreenWriteDescriptorInfo
    {
        mDescriptorSets.offscreen, 0,
        vkpp::DescriptorType::eUniformBuffer,
        lOffScreenBufferDescriptor
    };

    mLogicalDevice.UpdateDescriptorSet(lOffScreenWriteDescriptorInfo);
}


void OffScreenRenderer::GenerateDebugQuad(void)
{
    // Vertex buffer
    const std::vector<VertexData> lVertexData
    {
        { { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }
    };

    const auto lVtxDataSize = sizeof(VertexData) * lVertexData.size();

    vkpp::BufferCreateInfo lBufferCreateInfo
    {
        lVtxDataSize,
        vkpp::BufferUsageFlagBits::eVertexBuffer
    };

    mDebugQuadVtxRes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lpMemMappped = mLogicalDevice.MapMemory(mDebugQuadVtxRes.memory);
    std::memcpy(lpMemMappped, lVertexData.data(), lVtxDataSize);
    mLogicalDevice.UnmapMemory(mDebugQuadVtxRes.memory);

    // Index buffer
    const std::vector<uint32_t> lIdxData{ 0, 1, 2, 2, 3, 0 };
    mDebugQuadIdxCount = static_cast<uint32_t>(lIdxData.size());
    const auto lIdxDataSize = sizeof(uint32_t) * mDebugQuadIdxCount;

    lBufferCreateInfo.SetSize(lIdxDataSize)
                     .SetUsage(vkpp::BufferUsageFlagBits::eIndexBuffer);

    mDebugQuadIdxRes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    lpMemMappped = mLogicalDevice.MapMemory(mDebugQuadIdxRes.memory);
    std::memcpy(lpMemMappped, lIdxData.data(), lIdxDataSize);
    mLogicalDevice.UnmapMemory(mDebugQuadIdxRes.memory);
}


void OffScreenRenderer::LoadTexture(const std::string& aFilename, vkpp::Format aTexFormat)
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


void OffScreenRenderer::CreateSampler()
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


void OffScreenRenderer::BuildCmdBuffers(void)
{
    constexpr vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo;

    constexpr vkpp::ClearValue lClearValues[]
    {
        { 1.0f, 0.0f },
        { 0.129411f, 0.156862f, 0.188235f, 1.0f }
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
            { 0, 0 },
            mSwapchain.extent
        };

        lDrawCmdBuffer.SetScissor(lScissor);

        // Debug display
        lDrawCmdBuffer.BindGraphicsPipeline(mPipelines.debug);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayouts.textured, 0, mDescriptorSets.debugQuad);
        lDrawCmdBuffer.BindVertexBuffer(mDebugQuadVtxRes.buffer);
        lDrawCmdBuffer.BindIndexBuffer(mDebugQuadIdxRes.buffer);
        lDrawCmdBuffer.DrawIndexed(mDebugQuadIdxCount);

        // Scene
        lDrawCmdBuffer.BindGraphicsPipeline(mPipelines.mirror);

        // Reflection plane
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayouts.textured, 0, mDescriptorSets.mirror);
        lDrawCmdBuffer.BindVertexBuffer(mPlane.mVtxBufferRes.buffer);
        lDrawCmdBuffer.BindIndexBuffer(mPlane.mIdxBufferRes.buffer);
        lDrawCmdBuffer.DrawIndexed(mPlane.mIndexCount);

        // Model
        lDrawCmdBuffer.BindGraphicsPipeline(mPipelines.shaded);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayouts.shaded, 0, mDescriptorSets.model);
        lDrawCmdBuffer.BindVertexBuffer(mMesh.mVtxBufferRes.buffer);
        lDrawCmdBuffer.BindIndexBuffer(mMesh.mIdxBufferRes.buffer);
        lDrawCmdBuffer.DrawIndexed(mMesh.mIndexCount);

        lDrawCmdBuffer.EndRenderPass();

        lDrawCmdBuffer.End();
    }
}


void OffScreenRenderer::CreateSemaphores(void)
{
    constexpr vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    mPresentCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mRenderingCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mOffScreenCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void OffScreenRenderer::CreateFences(void)
{
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    for (std::size_t lIndex = 0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
        mWaitFences.emplace_back(mLogicalDevice.CreateFence(lFenceCreateInfo));
}


void OffScreenRenderer::CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const
{
    const auto& lCopyCmd = BeginOneTimeCmdBuffer();

    const vkpp::BufferCopy lBufferCopy{ aSize };

    lCopyCmd.Copy(aDstBuffer, aSrcBuffer, lBufferCopy);

    EndOneTimeCmdBuffer(lCopyCmd);
}


template <vkpp::ImageLayout OldLayout, vkpp::ImageLayout NewLayout>
void OffScreenRenderer::TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange, const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask)
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


void OffScreenRenderer::Update(void)
{
    auto lIndex = mLogicalDevice.AcquireNextImage(mSwapchain.handle, mPresentCompleteSemaphore);

    /*mLogicalDevice.WaitForFence(mWaitFences[lIndex]);
    mLogicalDevice.ResetFence(mWaitFences[lIndex]);*/

    // The scene renderer command buffer has to wait for the offscreen rendering to be finished before we can use the framebuffer color image for sampling during
    // final rendering. To ensure this, we use a dedicated offscreen synchronization semaphore that will be signaled when offscreen rendering has been finished.
    // This is necessary as an implementation may start both command buffers at the same time, there is no guarantee that command buffers will be extended in the
    // order they have been submitted by the application.

    constexpr vkpp::PipelineStageFlags lWaitDstStageMask{ vkpp::PipelineStageFlagBits::eColorAttachmentOutput };

    // Offscreen rendering
    vkpp::SubmitInfo lSubmitInfo
    {
        1, mPresentCompleteSemaphore.AddressOf(),       // Wait for swap-chain presentation to finish.
        &lWaitDstStageMask,
        1, mOffscreenFrame.cmdbuffer.AddressOf(),
        1, mOffScreenCompleteSemaphore.AddressOf()      // Signal ready with offscreen semaphore.
    };

    mGraphicsQueue.handle.Submit(lSubmitInfo);

    // Scene rendering
    lSubmitInfo.pWaitSemaphores = mOffScreenCompleteSemaphore.AddressOf(); //  mPresentCompleteSemaphore.AddressOf();      // Wait for offscreen semaphore
    lSubmitInfo.pSignalSemaphores = mRenderingCompleteSemaphore.AddressOf();    // Signal ready with render complete semaphore
    lSubmitInfo.pCommandBuffers = mDrawCmdBuffers[lIndex].AddressOf();

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


vkpp::CommandBuffer OffScreenRenderer::BeginOneTimeCmdBuffer(void) const
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


void OffScreenRenderer::EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const
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

            // Vertex texture coordinates
            const auto lTexCoord = lpMesh->HasTextureCoords(0) ? lpMesh->mTextureCoords[0][lVtxIdx] : lZero3D;
            lVertexBuffer.emplace_back(lTexCoord.x);
            lVertexBuffer.emplace_back(lTexCoord.y);

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


void OffScreenRenderer::BuildOffScreenCmdBuffer(void)
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
            { mOffscreenFrame.width, mOffscreenFrame.height }
        },
        2, lClearValues
    };

    lCmdBuffer.Begin(lCmdBufferBeginInfo);

    lCmdBuffer.BeginRenderPass(lRenderPassBeginInfo);
    lCmdBuffer.BindVertexBuffer(mMesh.mVtxBufferRes.buffer);
    lCmdBuffer.BindIndexBuffer(mMesh.mIdxBufferRes.buffer);

    const vkpp::Viewport lViewport
    {
        0.0f, 0.0f,
        static_cast<float>(mOffscreenFrame.width), static_cast<float>(mOffscreenFrame.height)
    };

    lCmdBuffer.SetViewport(lViewport);

    const vkpp::Rect2D lScissor
    {
        { 0, 0 },
        { mOffscreenFrame.width, mOffscreenFrame.height }
    };

    lCmdBuffer.SetScissor(lScissor);

    lCmdBuffer.BindGraphicsPipeline(mPipelines.shadedOffscreen);
    lCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayouts.shaded, 0, mDescriptorSets.offscreen);

    lCmdBuffer.DrawIndexed(mMesh.mIndexCount);

    lCmdBuffer.EndRenderPass();

    lCmdBuffer.End();
}


}                   // End of namespace vkpp::sample.