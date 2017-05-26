#include "TexturedArray.h"



namespace vkpp::sample
{



TexturedArray::TexturedArray(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngingName, uint32_t aEngineVersion)
    : ExampleBase(aWindow, apAppName, aAppVersion, apEngingName, aEngineVersion),
      CWindowEvent(aWindow), CMouseMotionEvent(aWindow), CMouseWheelEvent(aWindow),
      mDepthRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mTextureRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mVertexBufferRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mIndexBufferRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mUBORes(mLogicalDevice, mPhysicalDeviceMemoryProperties)
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

    mResizedFunc = [this](Sint32 /*aWidth*/, Sint32 /*aHeight*/)
    {
        // Ensure all operations on the device have been finished before destroying resources.
        mLogicalDevice.Wait();

        // Re-create swapchain.
        CreateSwapchain(mSwapchain);

        mDepthRes.Reset();
        CreateDepthResource();

        // Re-create framebuffers.
        for (auto& lFramebuffer : mFramebuffers)
            mLogicalDevice.DestroyFramebuffer(lFramebuffer);

        mFramebuffers.clear();
        CreateFramebuffers();

        // Command buffers need to be recreated as they reference to the destroyed framebuffer.
        mLogicalDevice.FreeCommandBuffers(mCmdPool, mDrawCmdBuffers);
        AllocateCmdBuffers();
        BuildCmdBuffers();
    };

    CreateCmdPool();
    AllocateCmdBuffers();

    CreateDepthResource();
    CreateRenderPass();
    CreateFramebuffers();

    CreateSetLayout();
    CreatePipelineLayout();
    CreateGraphicsPipeline();

    CreateDescriptorPool();
    AllocateDescriptorSet();

    // TODO: Vulkan core supports three different compressed texture formats.
    //       As the support differs among implementations, it needs to check device features and select a proper format and file.
    LoadTexture("Texture/texturearray_bc3_unorm.ktx", vkpp::Format::eBC3_uNormBlock);
    CreateSampler();

    CreateVertexBuffer();
    CreateIndexBuffer();

    CreateUniformBuffer();
    UpdateUniformBuffers();

    UpdateDescriptorSets();

    BuildCmdBuffers();

    CreateSemaphores();
    CreateFences();
}


TexturedArray::~TexturedArray(void)
{
    mLogicalDevice.Wait();

    mLogicalDevice.DestroySemaphore(mRenderingCompleteSemaphore);
    mLogicalDevice.DestroySemaphore(mPresentCompleteSemaphore);

    mLogicalDevice.DestroyFences(mWaitFences);

    mLogicalDevice.UnmapMemory(mUBORes.memory);

    mUBORes.Reset();

    mIndexBufferRes.Reset();
    mVertexBufferRes.Reset();

    mLogicalDevice.DestroySampler(mTextureSampler);
    mTextureRes.Reset();

    mLogicalDevice.DestroyDescriptorPool(mDescriptorPool);

    mLogicalDevice.DestroyPipeline(mPipeline);

    mLogicalDevice.DestroyPipelineLayout(mPipelineLayout);
    mLogicalDevice.DestroyDescriptorSetLayout(mSetLayout);

    mLogicalDevice.DestroyFramebuffers(mFramebuffers);
    mLogicalDevice.DestroyRenderPass(mRenderPass);
    mDepthRes.Reset();

    mLogicalDevice.FreeCommandBuffers(mCmdPool, mDrawCmdBuffers);
    mLogicalDevice.DestroyCommandPool(mCmdPool);
}


void TexturedArray::CreateCmdPool(void)
{
    const vkpp::CommandPoolCreateInfo lCmdPoolCreateInfo
    {
        mGraphicsQueue.familyIndex,
        vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer
    };

    mCmdPool = mLogicalDevice.CreateCommandPool(lCmdPoolCreateInfo);
}


void TexturedArray::AllocateCmdBuffers(void)
{
    const vkpp::CommandBufferAllocateInfo lCmdAllocateInfo
    {
        mCmdPool,
        static_cast<uint32_t>(mSwapchain.buffers.size())
    };

    mDrawCmdBuffers = mLogicalDevice.AllocateCommandBuffers(lCmdAllocateInfo);
}


void TexturedArray::CreateDepthResource(void)
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
            0, 1
        }
    };

    mDepthRes.Reset(lImageCreateInfo, lImageViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);
}


void TexturedArray::CreateRenderPass(void)
{
    const vkpp::AttachementDescription lAttachments[]
    {
        // Color attachment
        {
            mSwapchain.surfaceFormat.format,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear,
            vkpp::AttachmentStoreOp::eStore,
            vkpp::ImageLayout::eUndefined,
            vkpp::ImageLayout::ePresentSrcKHR
        },
        // Depth attachment
        {
            vkpp::Format::eD32sFloat,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear,
            vkpp::AttachmentStoreOp::eDontCare,
            vkpp::ImageLayout::eUndefined,
            vkpp::ImageLayout::eDepthStencilAttachmentOptimal
        }
    };

    // Color attachment reference
    constexpr vkpp::AttachmentReference lColorRef
    {
        0,
        vkpp::ImageLayout::eColorAttachmentOptimal
    };

    // Depth attachment reference
    constexpr vkpp::AttachmentReference lDepthRef
    {
        1,
        vkpp::ImageLayout::eDepthStencilAttachmentOptimal
    };

    constexpr vkpp::SubpassDescription lSubpassDescription
    {
        vkpp::PipelineBindPoint::eGraphics,
        1, lColorRef.AddressOf(),
        lDepthRef.AddressOf()
    };

    constexpr vkpp::SubpassDependency lSubpassDependencies[]
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
        2, lAttachments,
        1, lSubpassDescription.AddressOf(),
        2, lSubpassDependencies
    };

    mRenderPass = mLogicalDevice.CreateRenderPass(lRenderPassCreateInfo);
}


void TexturedArray::CreateFramebuffers(void)
{
    for (auto lSwapchain : mSwapchain.buffers)
    {
        const std::array<vkpp::ImageView, 2> lAttachments
        {
            lSwapchain.view,
            mDepthRes.view
        };

        const vkpp::FramebufferCreateInfo lFramebufferCreateInfo
        {
            mRenderPass,
            lAttachments,
            mSwapchain.extent
        };

        mFramebuffers.emplace_back(mLogicalDevice.CreateFramebuffer(lFramebufferCreateInfo));
    }
}


void TexturedArray::CreateSetLayout(void)
{
    constexpr vkpp::DescriptorSetLayoutBinding lSetLayoutBindings[]
    {
        { 0, vkpp::DescriptorType::eUniformBuffer, vkpp::ShaderStageFlagBits::eVertex },            // Binding 0: Vertex shader uniform buffer (MVP matrix).
        { 1, vkpp::DescriptorType::eCombinedImageSampler, vkpp::ShaderStageFlagBits::eFragment }    // Binding 1: Fragment shader image sampler (texture array).
    };

    constexpr vkpp::DescriptorSetLayoutCreateInfo lSetLayoutCreateInfo
    {
        2, lSetLayoutBindings
    };

    mSetLayout = mLogicalDevice.CreateDescriptorSetLayout(lSetLayoutCreateInfo);
}


void TexturedArray::CreatePipelineLayout(void)
{
    const vkpp::PipelineLayoutCreateInfo lPipelineLayoutCreateInfo
    {
        1, mSetLayout.AddressOf()
    };

    mPipelineLayout = mLogicalDevice.CreatePipelineLayout(lPipelineLayoutCreateInfo);
}


void TexturedArray::CreateGraphicsPipeline(void)
{
    const auto& lVertexShaderModule = CreateShaderModule("Shader/SPV/instancing.vert.spv");
    const auto& lFragmentShaderModule = CreateShaderModule("Shader/SPV/instancing.frag.spv");

    const vkpp::PipelineShaderStageCreateInfo lShaderStageCreateInfos[]
    {
        { vkpp::ShaderStageFlagBits::eVertex, lVertexShaderModule },
        { vkpp::ShaderStageFlagBits::eFragment, lFragmentShaderModule }
    };

    constexpr auto lVertexInputBinding = VertexData::GetInputBindingDescription();
    constexpr auto lVertexAttributeDescriptions = VertexData::GetAttributeDescriptions();
    const vkpp::PipelineVertexInputStateCreateInfo lVertexInputStateCreateInfo
    {
        1, lVertexInputBinding.AddressOf(),
        2, lVertexAttributeDescriptions.data()
    };

    constexpr vkpp::PipelineInputAssemblyStateCreateInfo lInputAssemblyStateCreateInfo
    {
        vkpp::PrimitiveTopology::eTriangleList
    };

    constexpr vkpp::PipelineViewportStateCreateInfo lViewportStateCreateInfo
    {
        1, 1
    };

    constexpr vkpp::PipelineRasterizationStateCreateInfo lRasterizationStateCreateInfo
    {
        vkpp::PolygonMode::eFill,
        vkpp::CullModeFlagBits::eNone,
        vkpp::FrontFace::eClockwise
    };

    constexpr vkpp::PipelineMultisampleStateCreateInfo lMultisampleStateCreateInfo;

    constexpr vkpp::PipelineDepthStencilStateCreateInfo lDepthStencilStateCreateInfo
    {
        VK_TRUE, VK_TRUE,
        vkpp::CompareOp::eLess
    };

    constexpr vkpp::PipelineColorBlendAttachmentState lColorBlendAttachmentState;
    constexpr vkpp::PipelineColorBlendStateCreateInfo lColorBlendStateCreateInfo
    {
        1, lColorBlendAttachmentState.AddressOf()
    };

    constexpr vkpp::DynamicState lDynamicStates[]
    {
        vkpp::DynamicState::eViewport, vkpp::DynamicState::eScissor
    };

    constexpr vkpp::PipelineDynamicStateCreateInfo lDynamicStateCreateInfo
    {
        2, lDynamicStates
    };

    const vkpp::GraphicsPipelineCreateInfo lGraphicsPipelineCreateInfo
    {
        2, lShaderStageCreateInfos,
        lVertexInputStateCreateInfo.AddressOf(),
        lInputAssemblyStateCreateInfo.AddressOf(),
        nullptr,
        lViewportStateCreateInfo.AddressOf(),
        lRasterizationStateCreateInfo.AddressOf(),
        lMultisampleStateCreateInfo.AddressOf(),
        lDepthStencilStateCreateInfo.AddressOf(),
        lColorBlendStateCreateInfo.AddressOf(),
        lDynamicStateCreateInfo.AddressOf(),
        mPipelineLayout,
        mRenderPass,
        0
    };

    mPipeline = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);
}


void TexturedArray::CreateDescriptorPool(void)
{
    constexpr vkpp::DescriptorPoolSize lPoolSizes[]
    {
        { vkpp::DescriptorType::eUniformBuffer, 1 },
        { vkpp::DescriptorType::eCombinedImageSampler, 1}
    };

    const vkpp::DescriptorPoolCreateInfo lDescriptorPoolCreateInfo
    {
        2, lPoolSizes, 2
    };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool(lDescriptorPoolCreateInfo);
}


void TexturedArray::AllocateDescriptorSet(void)
{
    const vkpp::DescriptorSetAllocateInfo lSetAllocateInfo
    {
        mDescriptorPool,
        1, mSetLayout.AddressOf()
    };

    mDescriptorSet = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);
}


void TexturedArray::LoadTexture(const std::string& aFilename, vkpp::Format aFormat)
{
    const gli::texture2d_array lTex2DArray{ gli::load(aFilename) };
    assert(!lTex2DArray.empty());

    mTexture.width = lTex2DArray.extent().x;
    mTexture.height = lTex2DArray.extent().y;
    mTexture.layerCount = static_cast<uint32_t>(lTex2DArray.layers());

    // Create a host-visible staging buffer that contains the raw image data.
    const vkpp::BufferCreateInfo lStagingBufferCreateInfo
    {
        lTex2DArray.size(), vkpp::BufferUsageFlagBits::eTransferSrc
    };

    BufferResource lStagingBufferRes{ mLogicalDevice, mPhysicalDeviceMemoryProperties };
    lStagingBufferRes.Reset(lStagingBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lMappedMem = mLogicalDevice.MapMemory(lStagingBufferRes.memory, 0, lTex2DArray.size());
    std::memcpy(lMappedMem, lTex2DArray.data(), lTex2DArray.size());
    mLogicalDevice.UnmapMemory(lStagingBufferRes.memory);

    // Setup buffer copy regions for array layers.
    std::vector<vkpp::BufferImageCopy> lBufferCopyRegions;
    uint32_t lOffset{ 0 };

    for (uint32_t lLayer = 0; lLayer < mTexture.layerCount; ++lLayer)
    {
        const vkpp::BufferImageCopy lBufferCopyRegion
        {
            lOffset,
            {
                vkpp::ImageAspectFlagBits::eColor,
                0,
                lLayer,                                 // base array layer
                1
            },
            { 0, 0, 0 },
            {
                static_cast<uint32_t>(lTex2DArray[lLayer][0].extent().x),
                static_cast<uint32_t>(lTex2DArray[lLayer][0].extent().y),
                1
            }
        };

        lBufferCopyRegions.emplace_back(lBufferCopyRegion);

        // Increase offset into staging buffer for next level / face.
        lOffset += static_cast<uint32_t>(lTex2DArray[lLayer].size());
    }

    // Create optimal tiled target image.
    const vkpp::ImageCreateInfo lImageCreateInfo
    {
        vkpp::ImageType::e2D,
        aFormat,
        { mTexture.width, mTexture.height, 1 },
        vkpp::ImageUsageFlagBits::eSampled | vkpp::ImageUsageFlagBits::eTransferDst,
        vkpp::ImageLayout::eUndefined, vkpp::ImageTiling::eOptimal,
        vkpp::SampleCountFlagBits::e1,
        1,
        mTexture.layerCount
    };

    vkpp::ImageViewCreateInfo lImageViewCreateInfo
    {
        vkpp::ImageViewType::e2DArray,
        aFormat,
        { vkpp::ImageAspectFlagBits::eColor, 0, 1, 0, mTexture.layerCount },
        vkpp::IdentityComponentMapping
    };

    mTextureRes.Reset(lImageCreateInfo, lImageViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    // Image barrier for optimal image (target).
    // Set initial layout for all array layers of the optimal (target) tiled texture.
    const vkpp::ImageSubresourceRange lImageSubRange
    {
        vkpp::ImageAspectFlagBits::eColor,
        0, 1,
        0, mTexture.layerCount
    };

    const auto& lCopyCmd = BeginOneTimeCmdBuffer();

    TransitionImageLayout<vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eTransferDstOptimal>
    (
        lCopyCmd,
        mTextureRes.image,
        lImageSubRange,
        vkpp::DefaultFlags,
        vkpp::AccessFlagBits::eTransferWrite
    );

    // Copy the array layers from the staging buffers to the optimal tiled image.
    lCopyCmd.Copy(mTextureRes.image, vkpp::ImageLayout::eTransferDstOptimal, lStagingBufferRes.buffer, lBufferCopyRegions);

    // Change texture image layout to shader read after all layers haven been copied.
    TransitionImageLayout<vkpp::ImageLayout::eTransferDstOptimal, vkpp::ImageLayout::eShaderReadOnlyOptimal>
    (
        lCopyCmd,
        mTextureRes.image,
        lImageSubRange,
        vkpp::AccessFlagBits::eTransferWrite,
        vkpp::AccessFlagBits::eShaderRead
    );

    EndOneTimeCmdBuffer(lCopyCmd);
}


void TexturedArray::CreateSampler(void)
{
    const vkpp::SamplerCreateInfo lSamplerCreateInfo
    {
        vkpp::Filter::eLinear, vkpp::Filter::eLinear,
        vkpp::SamplerMipmapMode::eLinear,
        vkpp::SamplerAddressMode::eClampToBorder, vkpp::SamplerAddressMode::eClampToEdge, vkpp::SamplerAddressMode::eClampToEdge,
        0.0f,
        VK_TRUE,
        mPhysicalDeviceProperties.limits.maxSamplerAnisotropy,
        VK_FALSE,
        vkpp::CompareOp::eNever,
        0.0f, 0.0f,
        vkpp::BorderColor::eFloatOpaqueWhite
    };

    mTextureSampler = mLogicalDevice.CreateSampler(lSamplerCreateInfo);
}


void TexturedArray::CreateVertexBuffer(void)
{
    // Setup vertices for a single UV-mapped quad form two triangles.
    const VertexData lVertices[]
    {
        { { 2.5f, 2.5f, 0.0f }, { 1.0f, 1.0f } },
        { { -2.5f, 2.5f, 0.0f }, { 0.0f, 1.0f } },
        { { -2.5f, -2.5f, 0.0f }, { 0.0f, 0.0f } },
        { { 2.5f, -2.5f, 0.0f }, { 1.0f, 0.0f } }
    };

    constexpr auto lVertexDataSize = sizeof lVertices;

    constexpr vkpp::BufferCreateInfo lBufferCreateInfo
    {
        lVertexDataSize,
        vkpp::BufferUsageFlagBits::eVertexBuffer | vkpp::BufferUsageFlagBits::eTransferDst
    };

    mVertexBufferRes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lMappedMem = mLogicalDevice.MapMemory(mVertexBufferRes.memory, 0, lVertexDataSize);
    std::memcpy(lMappedMem, lVertices, lVertexDataSize);
    mLogicalDevice.UnmapMemory(mVertexBufferRes.memory);
}


void TexturedArray::CreateIndexBuffer(void)
{
    constexpr uint32_t lIndices[]{ 0, 1, 2, 2, 3, 0 };

    constexpr auto lIndexDataSize = sizeof lIndices;

    constexpr vkpp::BufferCreateInfo lBufferCreateInfo
    {
        lIndexDataSize,
        vkpp::BufferUsageFlagBits::eVertexBuffer | vkpp::BufferUsageFlagBits::eTransferDst
    };

    mIndexBufferRes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lMappedMem = mLogicalDevice.MapMemory(mIndexBufferRes.memory, 0, lIndexDataSize);
    std::memcpy(lMappedMem, lIndices, lIndexDataSize);
    mLogicalDevice.UnmapMemory(mIndexBufferRes.memory);
}


void TexturedArray::CreateUniformBuffer(void)
{
    // Array indices and model matrices are fixed.
    constexpr auto lOffset{ -1.5f };
    const auto lCenter{ mTexture.layerCount * lOffset / 2 };

    glm::mat4 lModelMatrix;

    for (uint32_t lLayer = 0; lLayer < mTexture.layerCount; ++lLayer)
    {
        // Instance model matrix
        lModelMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, lLayer * lOffset - lCenter, 0.0f));
        lModelMatrix = glm::rotate(lModelMatrix, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        mUniformBufferObject.instances.emplace_back(lModelMatrix, lLayer);
    }

    // Vertex shader uniform buffer block
    constexpr auto lMatrixeDataSize{ sizeof mUniformBufferObject.matrices };
    const auto lInstanceDataSize{ mTexture.layerCount * sizeof(UboInstanceData) };
    const auto lUboSize = lMatrixeDataSize + lInstanceDataSize;

    const vkpp::BufferCreateInfo lBufferCreateInfo
    {
        lUboSize,
        vkpp::BufferUsageFlagBits::eUniformBuffer
    };

    mUBORes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    // Update instanced part of the uniform buffer.
    auto lpMappedMem = mLogicalDevice.MapMemory(mUBORes.memory, lMatrixeDataSize, lInstanceDataSize);
    std::memcpy(lpMappedMem, mUniformBufferObject.instances.data(), lInstanceDataSize);
    mLogicalDevice.UnmapMemory(mUBORes.memory);

    // Map persistent
    mpMatrixUBOMapped = mLogicalDevice.MapMemory(mUBORes.memory, 0, lMatrixeDataSize);
}


// Only update the uniform buffer block part containing the global matrices.
void TexturedArray::UpdateUniformBuffers(void)
{
    const auto lWidth = static_cast<float>(mSwapchain.extent.width);
    const auto lHeight = static_cast<float>(mSwapchain.extent.height);

    auto& lMatrices = mUniformBufferObject.matrices;

    // Projection
    lMatrices.projection = glm::perspective(glm::radians(60.0f), lWidth / lHeight, 0.001f, 256.0f);

    // View
    lMatrices.view = glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, mCurrentZoomLevel));
    lMatrices.view = glm::rotate(lMatrices.view, glm::radians(mCurrentRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    lMatrices.view = glm::rotate(lMatrices.view, glm::radians(mCurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    lMatrices.view = glm::rotate(lMatrices.view, glm::radians(mCurrentRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Only update the matrices part of the uniform buffer.
    std::memcpy(mpMatrixUBOMapped, &lMatrices, sizeof(lMatrices));
}


void TexturedArray::UpdateDescriptorSets(void) const
{
    // Image descriptor for texture array
    const vkpp::DescriptorImageInfo lTexDescriptor
    {
        mTextureSampler,
        mTextureRes.view,
        mTexture.layout
    };

    // Uniform buffer descriptor
    const vkpp::DescriptorBufferInfo lBufferDescriptor
    {
        mUBORes.buffer,
        0,
        sizeof(UniformBufferObject)
    };

    const vkpp::WriteDescriptorSetInfo lWriteDescriptorSetInfos[]
    {
        // Binding 0: Vertex shader uniform buffer
        {
            mDescriptorSet,
            0,
            0,
            vkpp::DescriptorType::eUniformBuffer,
            lBufferDescriptor
        },
        // Binding 1: Fragment shader texture array sampler.
        {
            mDescriptorSet,
            1,
            0,
            vkpp::DescriptorType::eCombinedImageSampler,
            lTexDescriptor
        }
    };

    mLogicalDevice.UpdateDescriptorSets(2, lWriteDescriptorSetInfos);
}


void TexturedArray::BuildCmdBuffers()
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
        lDrawCmdBuffer.BindVertexBuffer(mVertexBufferRes.buffer, 0);
        lDrawCmdBuffer.BindIndexBuffer(mIndexBufferRes.buffer, 0, vkpp::IndexType::eUInt32);

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

        lDrawCmdBuffer.BindGraphicsPipeline(mPipeline);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayout, 0, mDescriptorSet);

        lDrawCmdBuffer.DrawIndexed(6, mTexture.layerCount);

        lDrawCmdBuffer.EndRenderPass();

        lDrawCmdBuffer.End();
    }
}


void TexturedArray::CreateSemaphores(void)
{
    constexpr vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    mPresentCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mRenderingCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void TexturedArray::CreateFences(void)
{
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    for (std::size_t lIndex = 0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
        mWaitFences.emplace_back(mLogicalDevice.CreateFence(lFenceCreateInfo));
}


void TexturedArray::Update(void)
{
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

    const vkpp::khr::PresentInfo lPresentInfo
    {
        1, mRenderingCompleteSemaphore.AddressOf(),
        1, mSwapchain.handle.AddressOf(),
        &lIndex
    };

    mPresentQueue.handle.Present(lPresentInfo);
}


vkpp::CommandBuffer TexturedArray::BeginOneTimeCmdBuffer(void) const
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


void TexturedArray::EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const
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


template <vkpp::ImageLayout OldLayour, vkpp::ImageLayout NewLayout>
void TexturedArray::TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange, const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask)
{
    // Create an image barrier.
    const vkpp::ImageMemoryBarrier lImageMemoryBarrier
    {
        aSrcAccessMask, aDstAccessMask,
        OldLayour, NewLayout,
        aImage,
        aImageSubRange
    };

    // Put barrier inside setup command buffer.
    aCmdBuffer.PipelineBarrier(vkpp::PipelineStageFlagBits::eTopOfPipe, vkpp::PipelineStageFlagBits::eTopOfPipe, vkpp::DependencyFlagBits::eByRegion, lImageMemoryBarrier);
}


}                   // End of namespace vkpp::sample.
