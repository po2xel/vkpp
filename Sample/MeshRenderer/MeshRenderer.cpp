#include "MeshRenderer.h"



namespace vkpp::sample
{



MeshRenderer::MeshRenderer(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName, uint32_t aEngineVersion)
    : ExampleBase(aWindow, apAppName, aAppVersion, apEngineName, aEngineVersion),
      CWindowEvent(aWindow), CMouseMotionEvent(aWindow), CMouseWheelEvent(aWindow),
      mDepthRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mVertexBufferRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mIndexBufferRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mTextureRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mUBORes(mLogicalDevice, mPhysicalDeviceMemoryProperties)
{
    theApp.RegisterUpdateEvent([this](void)
    {
        Update();
    });

    mMouseWheelFunc = [this](Sint32 /*aPosX*/, Sint32 aPosY)
    {
        mCurrentZoomLevel *= (1.0f + aPosY * MINIMUM_ZOOM_LEVEL);
        UpdateUniformBuffer();
    };

    mMouseMotionFunc = [this](Uint32 aMouseState, Sint32 /*aPosX*/, Sint32 /*aPosY*/, Sint32 aRelativeX, Sint32 aRelativeY)
    {
        if (aMouseState & CMouseEvent::ButtonMask::eLMask)
        {
            mCurrentRotation.x += aRelativeY;
            mCurrentRotation.y += aRelativeX;

            UpdateUniformBuffer();
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

    CreateRenderPass();
    CreateDepthResource();
    CreateFramebuffers();

    CreateSetLayout();
    CreatePipelineLayout();
    CreateGraphicsPipeline();

    CreateDescriptorPool();
    AllocateDescriptorSet();

    LoadModel("Model/voyager.dae");

    LoadTexture("Model/voyager_bc3_unorm.ktx", vkpp::Format::eBC3_uNormBlock);
    CreateSampler();

    CreateUniformBuffer();
    UpdateUniformBuffer();

    UpdateDescriptorSets();

    BuildCmdBuffers();

    CreateSemaphores();
    CreateFences();
}


MeshRenderer::~MeshRenderer(void)
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

    mLogicalDevice.DestroyPipeline(mGraphicsPipeline);
    mLogicalDevice.DestroyPipelineLayout(mPipelineLayout);
    mLogicalDevice.DestroyDescriptorSetLayout(mSetLayout);

    mLogicalDevice.DestroyFramebuffers(mFramebuffers);
    mDepthRes.Reset();
    mLogicalDevice.DestroyRenderPass(mRenderPass);

    mLogicalDevice.FreeCommandBuffers(mCmdPool, mDrawCmdBuffers);
    mLogicalDevice.DestroyCommandPool(mCmdPool);
}


void MeshRenderer::CreateCmdPool(void)
{
    const vkpp::CommandPoolCreateInfo lCmdPoolCreateInfo
    {
        mGraphicsQueue.familyIndex,
        vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer
    };

    mCmdPool = mLogicalDevice.CreateCommandPool(lCmdPoolCreateInfo);
}


void MeshRenderer::AllocateCmdBuffers(void)
{
    const vkpp::CommandBufferAllocateInfo lCmdAllocateInfo
    {
        mCmdPool,
        static_cast<uint32_t>(mSwapchain.buffers.size()) 
    };

    mDrawCmdBuffers = mLogicalDevice.AllocateCommandBuffers(lCmdAllocateInfo);
}


void MeshRenderer::CreateRenderPass(void)
{
    const vkpp::AttachementDescription lAttachments[]
    {
        // Color Attachment
        {
            mSwapchain.surfaceFormat.format,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eStore,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::ePresentSrcKHR
        },
        // Depth Attachment
        {
            vkpp::Format::eD32sFloat,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eDontCare,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
        }
    };

    // Color attachment reference
    constexpr vkpp::AttachmentReference lColorRef
    {
        0, vkpp::ImageLayout::eColorAttachmentOptimal
    };

    // Depth attachment reference
    constexpr vkpp::AttachmentReference lDepthRef
    {
        1, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
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


void MeshRenderer::CreateDepthResource(void)
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


void MeshRenderer::CreateFramebuffers(void)
{
    for (auto& lSwapchain : mSwapchain.buffers)
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


void MeshRenderer::CreateSetLayout(void)
{
    constexpr vkpp::DescriptorSetLayoutBinding lSetLayoutBindings[]
    {
        // Binding 0: Vertex shader uniform buffer.
        {
            0,
            vkpp::DescriptorType::eUniformBuffer,
            vkpp::ShaderStageFlagBits::eVertex,
        },
        // Binding 1: Fragment shader combined sampler.
        {
            1,
            vkpp::DescriptorType::eCombinedImageSampler,
            vkpp::ShaderStageFlagBits::eFragment
        }
    };

    const vkpp::DescriptorSetLayoutCreateInfo lSetLayoutCreateInfo
    {
        2, lSetLayoutBindings
    };

    mSetLayout = mLogicalDevice.CreateDescriptorSetLayout(lSetLayoutCreateInfo);
}


void MeshRenderer::CreatePipelineLayout(void)
{
    const vkpp::PipelineLayoutCreateInfo lPipelineLayoutCreateInfo
    {
        1, mSetLayout.AddressOf()
    };

    mPipelineLayout = mLogicalDevice.CreatePipelineLayout(lPipelineLayoutCreateInfo);
}


void MeshRenderer::CreateGraphicsPipeline(void)
{
    const auto& lVertexShaderModule = CreateShaderModule("Shader/SPV/mesh.vert.spv");
    const auto& lFragmentShaderModule = CreateShaderModule("Shader/SPV/mesh.frag.spv");

    const vkpp::PipelineShaderStageCreateInfo lShaderStageCreateInfos[]
    {
        { vkpp::ShaderStageFlagBits::eVertex, lVertexShaderModule },
        { vkpp::ShaderStageFlagBits::eFragment, lFragmentShaderModule }
    };

    constexpr auto lVertexInputBinding = VertexData::GetBindingDescription();
    constexpr auto lVertexAttributeDescriptions = VertexData::GetAttributeDescriptions();
    const vkpp::PipelineVertexInputStateCreateInfo lVertexInputStateCreateInfo
    {
        1, lVertexInputBinding.AddressOf(),
        4, lVertexAttributeDescriptions.data()
    };

    constexpr vkpp::PipelineInputAssemblyStateCreateInfo lAssemblyStateCreateInfo
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
        vkpp::CullModeFlagBits::eBack,
        vkpp::FrontFace::eClockwise
    };

    constexpr vkpp::PipelineMultisampleStateCreateInfo lMultisampleStateCreateInfo;

    constexpr vkpp::PipelineDepthStencilStateCreateInfo lDepthStencilStateCreateInfo
    {
        DepthTest::Enable, DepthWrite::Enable,
        vkpp::CompareOp::eLess
    };

    constexpr vkpp::PipelineColorBlendAttachmentState lColorBlendAttachmentState;
    constexpr vkpp::PipelineColorBlendStateCreateInfo lColorBlendStateCreateInfo
    {
        1, lColorBlendAttachmentState.AddressOf()
    };

    constexpr vkpp::DynamicState lDynamicStates[]
    {
        vkpp::DynamicState::eViewport,
        vkpp::DynamicState::eScissor
    };

    constexpr vkpp::PipelineDynamicStateCreateInfo lDynamicStateCreateInfo
    {
        2, lDynamicStates
    };

    const vkpp::GraphicsPipelineCreateInfo lGraphicsPipelineCreateInfo
    {
        2, lShaderStageCreateInfos,
        lVertexInputStateCreateInfo.AddressOf(),
        lAssemblyStateCreateInfo.AddressOf(),
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

    mGraphicsPipeline = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);
}


void MeshRenderer::CreateDescriptorPool(void)
{
    constexpr vkpp::DescriptorPoolSize lDescriptorPoolSizes[]
    {
        { vkpp::DescriptorType::eUniformBuffer, 1 },                // One uniform buffer object
        { vkpp::DescriptorType::eCombinedImageSampler, 1}           // One combined image sampler
    };

    const vkpp::DescriptorPoolCreateInfo lDescriptorPoolCreateInfo
    {
        2, lDescriptorPoolSizes,
        1
    };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool(lDescriptorPoolCreateInfo);
}


void MeshRenderer::AllocateDescriptorSet(void)
{
    const vkpp::DescriptorSetAllocateInfo lSetAllocateInfo
    {
        mDescriptorPool,
        1, mSetLayout.AddressOf()
    };

    mDescriptorSet = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);
}


// Load a model from file using the ASSIMP model loader and generate all resources required to render the model.
void MeshRenderer::LoadModel(const std::string& aFilename)
{
    // Flags for loading the mesh
    constexpr static auto lAssimpFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices;

    Assimp::Importer lImporter;

    auto lpScene = lImporter.ReadFile(aFilename, lAssimpFlags);
    assert(lpScene != nullptr);

    std::vector<float> lVertexBuffer;
    std::vector<uint32_t> lIndexBuffer;

    // Iterate through all meshes in the file and extract the vertex components.
    for (unsigned int lMeshIdx = 0; lMeshIdx < lpScene->mNumMeshes; ++lMeshIdx)
    {
        const auto lpMesh = lpScene->mMeshes[lMeshIdx];

        for (unsigned int lVtxIdx = 0; lVtxIdx < lpMesh->mNumVertices; ++lVtxIdx)
        {
            // Vertex positions
            const auto lPos = lpMesh->mVertices[lVtxIdx];
            lVertexBuffer.emplace_back(lPos.x);
            lVertexBuffer.emplace_back(-lPos.y);                // Vulkan use a right handed NDC.
            lVertexBuffer.emplace_back(lPos.z);

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
            const auto lColor = lpMesh->HasVertexColors(0) ? lpMesh->mColors[0][lVtxIdx] : aiColor4D(1.0f);
            lVertexBuffer.emplace_back(lColor.r);
            lVertexBuffer.emplace_back(lColor.g);
            lVertexBuffer.emplace_back(lColor.b);
        }

        mIndexCount = static_cast<uint32_t>(lIndexBuffer.size());
        for (unsigned int lIdxIndex = 0; lIdxIndex < lpMesh->mNumFaces; ++lIdxIndex)
        {
            const auto& lFace = lpMesh->mFaces[lIdxIndex];

            if (lFace.mNumIndices != 3)
                continue;

            // Assume that all faces are triangulated
            lIndexBuffer.emplace_back(mIndexCount + lFace.mIndices[0]);
            lIndexBuffer.emplace_back(mIndexCount + lFace.mIndices[1]);
            lIndexBuffer.emplace_back(mIndexCount + lFace.mIndices[2]);
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

    mVertexBufferRes.Reset(lVtxBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    // Index buffer
    const vkpp::BufferCreateInfo lIdxBufferCreateInfo
    {
        lIdxBufferSize, vkpp::BufferUsageFlagBits::eIndexBuffer | vkpp::BufferUsageFlagBits::eTransferDst
    };

    mIndexBufferRes.Reset(lIdxBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    CopyBuffer(mVertexBufferRes.buffer, lVtxStagingBuffer.buffer, lVtxBufferSize);
    CopyBuffer(mIndexBufferRes.buffer, lIdxStagingBuffer.buffer, lIdxBufferSize);
}


void MeshRenderer::LoadTexture(const std::string& aFilename, vkpp::Format aTexFormat)
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


void MeshRenderer::CreateSampler()
{
    const vkpp::SamplerCreateInfo lSamplerCreateInfo
    {
        vkpp::Filter::eLinear, vkpp::Filter::eLinear,
        vkpp::SamplerMipmapMode::eLinear,
        vkpp::SamplerAddressMode::eClampToBorder, vkpp::SamplerAddressMode::eClampToEdge, vkpp::SamplerAddressMode::eClampToEdge,
        0.0f,
        Anisotropy::Enable,
        mPhysicalDeviceProperties.limits.maxSamplerAnisotropy,
        Compare::Disable,
        vkpp::CompareOp::eNever,
        0.0f, 0.0f,
        vkpp::BorderColor::eFloatOpaqueWhite
    };

    mTextureSampler = mLogicalDevice.CreateSampler(lSamplerCreateInfo);
}


void MeshRenderer::CreateUniformBuffer()
{
    // Vertex shader uniform buffer block
    constexpr vkpp::BufferCreateInfo lBufferCreateInfo
    {
        sizeof(UniformBufferObject),
        vkpp::BufferUsageFlagBits::eUniformBuffer
    };

    mUBORes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    // Map persistent
    mpMatrixUBOMapped = mLogicalDevice.MapMemory(mUBORes.memory, 0, VK_WHOLE_SIZE);
}


void MeshRenderer::UpdateUniformBuffer()
{
    const auto lWidth = static_cast<float>(mSwapchain.extent.width);
    const auto lHeight = static_cast<float>(mSwapchain.extent.height);

    const glm::vec3 lCameraPos{ 0.1f, 1.1f, 0.0f };

    mMVPMatrix.proj = glm::perspective(glm::radians(60.f), lWidth / lHeight, 0.001f, 256.0f);

    const auto& lViewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, mCurrentZoomLevel));
    mMVPMatrix.model = lViewMatrix * glm::translate(glm::mat4(), lCameraPos);
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    std::memcpy(mpMatrixUBOMapped, &mMVPMatrix, sizeof(UniformBufferObject));
}


void MeshRenderer::UpdateDescriptorSets(void) const
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


void MeshRenderer::BuildCmdBuffers(void)
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

        lDrawCmdBuffer.BindGraphicsPipeline(mGraphicsPipeline);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayout, 0, mDescriptorSet);

        lDrawCmdBuffer.DrawIndexed(mIndexCount);

        lDrawCmdBuffer.EndRenderPass();

        lDrawCmdBuffer.End();
    }
}


void MeshRenderer::CreateSemaphores(void)
{
    constexpr vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    mPresentCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mRenderingCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void MeshRenderer::CreateFences(void)
{
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    for (std::size_t lIndex = 0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
        mWaitFences.emplace_back(mLogicalDevice.CreateFence(lFenceCreateInfo));
}


void MeshRenderer::Update()
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


void MeshRenderer::CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const
{
    const auto& lCopyCmd = BeginOneTimeCmdBuffer();

    const vkpp::BufferCopy lBufferCopy{ aSize };

    lCopyCmd.Copy(aDstBuffer, aSrcBuffer, lBufferCopy);

    EndOneTimeCmdBuffer(lCopyCmd);
}


vkpp::CommandBuffer MeshRenderer::BeginOneTimeCmdBuffer(void) const
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


void MeshRenderer::EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const
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
void MeshRenderer::TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange, const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask)
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