#include "TexturedCube.h"



namespace vkpp::sample
{



TexturedCube::TexturedCube(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName, uint32_t aEngineVersion)
    : ExampleBase(aWindow, apAppName, aAppVersion, apEngineName, aEngineVersion),
      CWindowEvent(aWindow),
      CMouseWheelEvent(aWindow),
      CMouseMotionEvent(aWindow),
      mDepthRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mTextureResource(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mSkyModel(*this, mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mSphereModel(*this, mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mTeapotModel(*this, mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mTorusKnotModel(*this, mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mObjectUBORes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mSkyboxUBORes(mLogicalDevice, mPhysicalDeviceMemoryProperties)
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
        AllocateCommandBuffers();
        BuildCommandBuffers();
    };

    CreateCommandPool();
    AllocateCommandBuffers();

    CreateDepthResource();
    CreateRenderPass();
    CreateFramebuffers();

    CreateSetLayout();
    CreatePipelineLayout();

    CreateGraphicsPipeline();

    CreateDescriptorPool();
    AllocateDescriptorSets();

    // TODO: Vulkan core supports three different compressed texture formats.
    //       As the support differs among implementations, it needs to check device features and select a proper format and file.
    LoadTexture("Texture/cubemap_yokohama_bc3_unorm.ktx", vkpp::Format::eBC2_uNormBlock);
    CreateSampler();

    LoadMeshes();

    CreateUniformBuffers();
    MapUniformBuffers();
    UpdateUniformBuffers();

    UpdateDescriptorSets();

    BuildCommandBuffers();

    CreateSemaphores();
    CreateFences();
}


TexturedCube::~TexturedCube(void)
{
    mLogicalDevice.Wait();

    mLogicalDevice.DestroySemaphore(mRenderingCompleteSemaphore);
    mLogicalDevice.DestroySemaphore(mPresentCompleteSemaphore);

    mLogicalDevice.DestroyFences(mWaitFences);

    UnmapUniformBuffers();

    mSkyboxUBORes.Reset();
    mObjectUBORes.Reset();

    mLogicalDevice.DestroySampler(mTextureSampler);
    mTextureResource.Reset();

    mLogicalDevice.DestroyDescriptorPool(mDescriptorPool);

    mLogicalDevice.DestroyPipeline(mReflectPipeline);
    mLogicalDevice.DestroyPipeline(mSkyboxPipeline);

    mLogicalDevice.DestroyPipelineLayout(mPipelineLayout);
    mLogicalDevice.DestroyDescriptorSetLayout(mSetLayout);

    mLogicalDevice.DestroyFramebuffers(mFramebuffers);

    mLogicalDevice.DestroyRenderPass(mRenderPass);
    mDepthRes.Reset();

    mLogicalDevice.FreeCommandBuffers(mCmdPool, mDrawCmdBuffers);
    mLogicalDevice.DestroyCommandPool(mCmdPool);
}


void TexturedCube::CreateCommandPool(void)
{
    const vkpp::CommandPoolCreateInfo lCmdPoolCreateInfo
    {
        mGraphicsQueue.familyIndex,
        vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer
    };

    mCmdPool = mLogicalDevice.CreateCommandPool(lCmdPoolCreateInfo);
}


void TexturedCube::AllocateCommandBuffers(void)
{
    const vkpp::CommandBufferAllocateInfo lCmdBufferAllocateInfo
    {
        mCmdPool,
        static_cast<uint32_t>(mSwapchain.buffers.size())
    };

    mDrawCmdBuffers = mLogicalDevice.AllocateCommandBuffers(lCmdBufferAllocateInfo);
}


void TexturedCube::CreateDepthResource(void)
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


void TexturedCube::CreateRenderPass(void)
{
    const std::array<vkpp::AttachementDescription, 2> lAttachements
    { {
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
    } };

    // Color attachment references.
    constexpr std::array<vkpp::AttachmentReference, 1> lColorRef
    { {
        {
            0, vkpp::ImageLayout::eColorAttachmentOptimal
        }
    } };

    // Depth attachment reference.
    constexpr vkpp::AttachmentReference lDepthRef
    {
        1, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
    };

    const std::array<vkpp::SubpassDescription, 1> lSubpassDescriptions
    { {
        {
            vkpp::PipelineBindPoint::eGraphics,
            lColorRef,
            lDepthRef.AddressOf()
        }
    } };

    constexpr std::array<vkpp::SubpassDependency, 2> lSubpassDependencies
    { {
        {
            vkpp::subpass::External, 0,
            vkpp::PipelineStageFlagBits::eBottomOfPipe, vkpp::PipelineStageFlagBits::eColorAttachmentOutput,
            vkpp::AccessFlagBits::eMemoryRead, vkpp::AccessFlagBits::eColorAttachmentWrite
        },
        {
            0, vkpp::subpass::External,
            vkpp::PipelineStageFlagBits::eColorAttachmentOutput, vkpp::PipelineStageFlagBits::eBottomOfPipe,
            vkpp::AccessFlagBits::eColorAttachmentWrite, vkpp::AccessFlagBits::eMemoryRead
        }
    } };

    const vkpp::RenderPassCreateInfo lRenderPassCreateInfo
    {
        lAttachements, lSubpassDescriptions, lSubpassDependencies
    };

    mRenderPass = mLogicalDevice.CreateRenderPass(lRenderPassCreateInfo);
}


void TexturedCube::CreateFramebuffers(void)
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


void TexturedCube::CreateSetLayout(void)
{
    constexpr std::array<vkpp::DescriptorSetLayoutBinding, 2> lSetLayoutBindings
    { {
        // Binding 0: Vertex shader uniform buffer. (MVP matrix)
        {
            0,
            vkpp::DescriptorType::eUniformBuffer,
            vkpp::ShaderStageFlagBits::eVertex
        },
        // Binding 1: Fragment shader image sampler.
        {
            1,
            vkpp::DescriptorType::eCombinedImageSampler,
            vkpp::ShaderStageFlagBits::eFragment
        }
    } };

    const vkpp::DescriptorSetLayoutCreateInfo lSetLayoutCreateInfo{ lSetLayoutBindings };
    mSetLayout = mLogicalDevice.CreateDescriptorSetLayout(lSetLayoutCreateInfo);
}


void TexturedCube::CreatePipelineLayout(void)
{
    const vkpp::PipelineLayoutCreateInfo lPipelineLayoutCreateInfo
    {
        1, mSetLayout.AddressOf()
    };

    mPipelineLayout = mLogicalDevice.CreatePipelineLayout(lPipelineLayoutCreateInfo);
}


void TexturedCube::CreateGraphicsPipeline(void)
{
    std::array<vkpp::PipelineShaderStageCreateInfo, 2> lShaderStageCreateInfo
    { {
        {
            vkpp::ShaderStageFlagBits::eVertex
        },
        {
            vkpp::ShaderStageFlagBits::eFragment
        }
    } };

    constexpr std::array<vkpp::VertexInputBindingDescription, 1> lVertexInputBindingDescriptions{ VertexData::GetInputBindingDescription() };
    constexpr auto lVertexAttributeDescriptions = VertexData::GetAttributeDescriptions();

    const vkpp::PipelineVertexInputStateCreateInfo lInputStateCreateInfo
    {
        lVertexInputBindingDescriptions,
        lVertexAttributeDescriptions
    };

    constexpr vkpp::PipelineInputAssemblyStateCreateInfo lInputAssemblyStateCreateInfo
    {
        vkpp::PrimitiveTopology::eTriangleList
    };

    constexpr vkpp::PipelineViewportStateCreateInfo lViewportStateCreateInfo{ 1, 1 };

    vkpp::PipelineRasterizationStateCreateInfo lRasterizationStateCreateInfo
    {
        vkpp::PolygonMode::eFill,
        vkpp::CullModeFlagBits::eBack,
        vkpp::FrontFace::eCounterClockwise,
    };

    constexpr vkpp::PipelineMultisampleStateCreateInfo lMultisampleStateCreateInfo;

    vkpp::PipelineDepthStencilStateCreateInfo lDepthStencilStateCreateInfo;

    constexpr vkpp::PipelineColorBlendAttachmentState lColorBlendAttachmentState;
    const vkpp::PipelineColorBlendStateCreateInfo lColorBlendStateCreateInfo
    {
        1, lColorBlendAttachmentState.AddressOf()
    };

    constexpr std::array<vkpp::DynamicState, 2> lDynamicStates
    {
        vkpp::DynamicState::eViewport,
        vkpp::DynamicState::eScissor
    };

    const vkpp::PipelineDynamicStateCreateInfo lDynamicStateCreateInfo{ lDynamicStates };

    vkpp::GraphicsPipelineCreateInfo lGraphicsPipelineCreateInfo
    {
        2, lShaderStageCreateInfo.data(),
        lInputStateCreateInfo.AddressOf(),
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

    // Skybox pipeline (background cube)
    lGraphicsPipelineCreateInfo.flags = vkpp::PipelineCreateFlagBits::eAllowDerivatives;

    lShaderStageCreateInfo[0].module = CreateShaderModule("Shader/SPV/skybox.vert.spv");
    lShaderStageCreateInfo[1].module = CreateShaderModule("Shader/SPV/skybox.frag.spv");

    mSkyboxPipeline = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lShaderStageCreateInfo[0].module);
    mLogicalDevice.DestroyShaderModule(lShaderStageCreateInfo[1].module);

    // Cube map reflect pipeline.
    lShaderStageCreateInfo[0].module = CreateShaderModule("Shader/SPV/reflect.vert.spv");
    lShaderStageCreateInfo[1].module = CreateShaderModule("Shader/SPV/reflect.frag.spv");

    lGraphicsPipelineCreateInfo.flags = vkpp::PipelineCreateFlagBits::eDerivative;
    lGraphicsPipelineCreateInfo.basePipelineHandle = mSkyboxPipeline;

    // Enable depth test and write
    lDepthStencilStateCreateInfo.EnableDepthTest(vkpp::CompareOp::eLess);

    // Flip cull mode
    lRasterizationStateCreateInfo.SetCullMode(vkpp::CullModeFlagBits::eFront);

    mReflectPipeline = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lShaderStageCreateInfo[0].module);
    mLogicalDevice.DestroyShaderModule(lShaderStageCreateInfo[1].module);
}


void TexturedCube::CreateDescriptorPool(void)
{
    constexpr std::array<vkpp::DescriptorPoolSize, 2> lPoolSizes
    { {
        { vkpp::DescriptorType::eUniformBuffer, 2 },
        { vkpp::DescriptorType::eCombinedImageSampler, 2}
    } };

    const vkpp::DescriptorPoolCreateInfo lDescriptorPoolCreateInfo
    {
        lPoolSizes, 2
    };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool(lDescriptorPoolCreateInfo);
}


void TexturedCube::AllocateDescriptorSets(void)
{
    const vkpp::DescriptorSetAllocateInfo lSetAllocateInfo
    {
        mDescriptorPool,
        1, mSetLayout.AddressOf()
    };

    // 3D object descriptor set
    mDescriptorSets.object = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);

    // Sky box descriptor set
    mDescriptorSets.skybox = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);
}


void TexturedCube::LoadTexture(const std::string& aFilename, vkpp::Format aFormat)
{
    const gli::texture_cube lTexCube{ gli::load(aFilename) };
    assert(!lTexCube.empty());

    mCubeMap.width = lTexCube.extent().x;
    mCubeMap.height = lTexCube.extent().y;
    mCubeMap.mipLevels = static_cast<uint32_t>(lTexCube.levels());

    // Create a host-visible staging buffer that contains the raw image data.
    const vkpp::BufferCreateInfo lStagingBufferCreateInfo
    {
        lTexCube.size(),
        vkpp::BufferUsageFlagBits::eTransferSrc
    };

    BufferResource lStagingBuffer{ mLogicalDevice, mPhysicalDeviceMemoryProperties };
    lStagingBuffer.Reset(lStagingBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lMappedMem = mLogicalDevice.MapMemory(lStagingBuffer.memory, 0, lTexCube.size());
    std::memcpy(lMappedMem, lTexCube.data(), lTexCube.size());
    mLogicalDevice.UnmapMemory(lStagingBuffer.memory);

    // Setup buffer copy regions for each face including all of its mip-levels.
    std::vector<vkpp::BufferImageCopy> lBufferCopyRegions;
    uint32_t lOffset{ 0 };

    for (uint32_t lFace = 0; lFace < 6; ++lFace)
    {
        for (uint32_t lLevel = 0; lLevel < mCubeMap.mipLevels; ++lLevel)
        {
            const vkpp::BufferImageCopy lBufferCopyRegion
            {
                lOffset,
                {
                    vkpp::ImageAspectFlagBits::eColor,
                    lLevel,
                    lFace
                },
                { 0, 0, 0},
                {
                    static_cast<uint32_t>(lTexCube[lFace][lLevel].extent().x),
                    static_cast<uint32_t>(lTexCube[lFace][lLevel].extent().y),
                    1
                }
            };

            lBufferCopyRegions.emplace_back(lBufferCopyRegion);

            // Increase offset into staging bffer for next level/face.
            lOffset += static_cast<uint32_t>(lTexCube[lFace][lLevel].size());
        }
    }

    // Create optimal tiled target image.
    const vkpp::ImageCreateInfo lImageCreateInfo
    {
        vkpp::ImageType::e2D,
        aFormat,
        { mCubeMap.width, mCubeMap.height, 1 },
        vkpp::ImageUsageFlagBits::eSampled | vkpp::ImageUsageFlagBits::eTransferDst,
        vkpp::ImageLayout::eUndefined,
        vkpp::ImageTiling::eOptimal,
        vkpp::SampleCountFlagBits::e1,
        mCubeMap.mipLevels,
        6,                                                      // Cube faces count as array layers in Vulkan.
        vkpp::ImageCreateFlagBits::eCubeCompatible              // This flag is required for cube map images.
    };

    vkpp::ImageViewCreateInfo lImageViewCreateInfo
    {
        vkpp::ImageViewType::eCube,
        aFormat,
        {
            vkpp::ImageAspectFlagBits::eColor,
            0, mCubeMap.mipLevels,
            0, 6                                            // 6 array layers (faces)
        }
    };

    mTextureResource.Reset(lImageCreateInfo, lImageViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    // Image barrier for optimal image (target).
    // Set initial layout for all array layers (faces) and mip-levels of the optimal (target) tiled texture.
    const vkpp::ImageSubresourceRange lImageSubRange
    {
        vkpp::ImageAspectFlagBits::eColor,
        0, mCubeMap.mipLevels,
        0, 6
    };

    auto lCopyCmd = BeginOneTimeCommandBuffer();

    TransitionImageLayout<vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eTransferDstOptimal>
    (
        lCopyCmd,
        mTextureResource.image,
        lImageSubRange,
        vkpp::DefaultFlags,
        vkpp::AccessFlagBits::eTransferWrite
    );

    // Copy the cube map faces and mip-levels from the staging buffer to the optimal tiled image.
    lCopyCmd.Copy(mTextureResource.image, vkpp::ImageLayout::eTransferDstOptimal, lStagingBuffer.buffer, lBufferCopyRegions);

    // Change texture image layout to shader read after all faces and mip-levels have been copied.
    TransitionImageLayout<vkpp::ImageLayout::eTransferDstOptimal, vkpp::ImageLayout::eShaderReadOnlyOptimal>
    (
        lCopyCmd,
        mTextureResource.image,
        lImageSubRange,
        vkpp::AccessFlagBits::eTransferWrite,
        vkpp::AccessFlagBits::eShaderRead
    );

    EndOneTimeCommandBuffer(lCopyCmd);
}


void TexturedCube::CreateSampler(void)
{
    const vkpp::SamplerCreateInfo lSamplerCreateInfo
    {
        vkpp::Filter::eLinear, vkpp::Filter::eLinear,
        vkpp::SamplerMipmapMode::eLinear,
        vkpp::SamplerAddressMode::eClampToEdge, vkpp::SamplerAddressMode::eClampToEdge, vkpp::SamplerAddressMode::eClampToBorder,
        0.0f,
        Anisotropy::Enable,
        mPhysicalDeviceProperties.limits.maxSamplerAnisotropy,
        Compare::Disable,
        vkpp::CompareOp::eNever,
        0.0f,
        static_cast<float>(mCubeMap.mipLevels),
        vkpp::BorderColor::eFloatOpaqueWhite
    };

    mTextureSampler = mLogicalDevice.CreateSampler(lSamplerCreateInfo);
}


void TexturedCube::LoadMeshes(void)
{
    mSkyModel.LoadMode("Model/cube.obj", glm::vec3(0.05f));

    mSphereModel.LoadMode("Model/sphere.obj", glm::vec3(0.05f));
    mTeapotModel.LoadMode("Model/teapot.dae", glm::vec3(0.05f));
    mTorusKnotModel.LoadMode("Model/torusknot.obj", glm::vec3(0.05f));
}


void TexturedCube::CreateUniformBuffers(void)
{
    // Object vertex shader uniform buffer.
    constexpr vkpp::BufferCreateInfo lBufferCreateInfo
    {
        sizeof(UniformBufferObject),
        vkpp::BufferUsageFlagBits::eUniformBuffer
    };

    mObjectUBORes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);
    mSkyboxUBORes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);
}


void TexturedCube::UpdateUniformBuffers(void)
{
    const auto lWidth = static_cast<float>(mSwapchain.extent.width);
    const auto lHeight = static_cast<float>(mSwapchain.extent.height);

    const glm::vec3 lCameraPos;

    // 3D object
    {
        mMVPMatrix.projection = glm::perspective(glm::radians(60.0f), lWidth / lHeight, 0.001f, 256.0f);

        const auto& lViewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, mCurrentZoomLevel));
        mMVPMatrix.model = lViewMatrix * glm::translate(glm::mat4(), lCameraPos);
        mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        std::memcpy(mpObjectUBOMapped, &mMVPMatrix, sizeof(UniformBufferObject));
    }

    // Skybox
    {
        mMVPMatrix.projection = glm::perspective(glm::radians(60.0f), lWidth / lHeight, 0.001f, 256.0f);
        mMVPMatrix.model = glm::translate(glm::mat4(), glm::vec3());
        mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        std::memcpy(mpSkyboxUBOMapped, &mMVPMatrix, sizeof(UniformBufferObject));
    }
}


void TexturedCube::MapUniformBuffers(void)
{
    mpObjectUBOMapped = mLogicalDevice.MapMemory(mObjectUBORes.memory, 0, sizeof(UniformBufferObject));
    mpSkyboxUBOMapped = mLogicalDevice.MapMemory(mSkyboxUBORes.memory, 0, sizeof(UniformBufferObject));
}


void TexturedCube::UnmapUniformBuffers(void) const
{
    mLogicalDevice.UnmapMemory(mSkyboxUBORes.memory);
    mLogicalDevice.UnmapMemory(mObjectUBORes.memory);
}


void TexturedCube::UpdateDescriptorSets(void)
{
    // Image descriptor for the cube map texture
    const vkpp::DescriptorImageInfo lTextureDescriptor
    {
        mTextureSampler,
        mTextureResource.view,
        mCubeMap.layout
    };

    const vkpp::DescriptorBufferInfo lObjectBufferDescriptor
    {
        mObjectUBORes.buffer,
        0,
        sizeof(UniformBufferObject)
    };

    // 3D object descriptor set
    const std::array<vkpp::WriteDescriptorSetInfo, 2> lObjectWriteDescriptorSets
    { {
        // Binding 0: Vertex Shader uniform buffer
        {
            mDescriptorSets.object,
            0,
            0,
            vkpp::DescriptorType::eUniformBuffer,
            lObjectBufferDescriptor
        },
        // Binding 1: Fragment shader cube-map sampler
        {
            mDescriptorSets.object,
            1,
            0,
            vkpp::DescriptorType::eCombinedImageSampler,
            lTextureDescriptor
        }
    } };

    mLogicalDevice.UpdateDescriptorSets(lObjectWriteDescriptorSets);

    // Skybox descriptor set
    const vkpp::DescriptorBufferInfo lSkyboxBufferDescriptor
    {
        mSkyboxUBORes.buffer,
        0,
        sizeof(UniformBufferObject)
    };

    const std::array<vkpp::WriteDescriptorSetInfo, 2> lSkyboxDescriptorSets
    { {
        // Binding 0: Vertex shader uniform buffer
        {
            mDescriptorSets.skybox,
            0,
            0,
            vkpp::DescriptorType::eUniformBuffer,
            lSkyboxBufferDescriptor
        },
        // Binding 1: Fragment shader cube-map sampler
        {
            mDescriptorSets.skybox,
            1,
            0,
            vkpp::DescriptorType::eCombinedImageSampler,
            lTextureDescriptor
        }
    } };

    mLogicalDevice.UpdateDescriptorSets(lSkyboxDescriptorSets);
}


void TexturedCube::BuildCommandBuffers(void)
{
    constexpr vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo;

    constexpr std::array<vkpp::ClearValue, 2> lClearValues
    { {
        { 0.129411f, 0.156862f, 0.188235f, 1.0f },
        { 1.0f, 0.0f }
    } };

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
            lClearValues
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

        // Skybox
        lDrawCmdBuffer.BindVertexBuffer(mSkyModel.vertices.buffer, 0);
        lDrawCmdBuffer.BindIndexBuffer(mSkyModel.indices.buffer, 0, vkpp::IndexType::eUInt32);
        lDrawCmdBuffer.BindGraphicsPipeline(mSkyboxPipeline);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayout, 0, mDescriptorSets.skybox);
        lDrawCmdBuffer.DrawIndexed(mSkyModel.indexCount);

        // 3D objects
        lDrawCmdBuffer.BindVertexBuffer(mSphereModel.vertices.buffer, 0);
        lDrawCmdBuffer.BindIndexBuffer(mSphereModel.indices.buffer, 0, vkpp::IndexType::eUInt32);
        lDrawCmdBuffer.BindGraphicsPipeline(mReflectPipeline);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayout, 0, mDescriptorSets.object);
        lDrawCmdBuffer.DrawIndexed(mSphereModel.indexCount);

        /*lDrawCmdBuffer.BindVertexBuffer(mTeapotModel.vertices.buffer, 0);
        lDrawCmdBuffer.BindIndexBuffer(mTeapotModel.indices.buffer, 0, vkpp::IndexType::eUInt32);
        lDrawCmdBuffer.DrawIndexed(mTeapotModel.indexCount);*/

        /*lDrawCmdBuffer.BindVertexBuffer(mTorusKnotModel.vertices.buffer, 0);
        lDrawCmdBuffer.BindIndexBuffer(mTorusKnotModel.indices.buffer, 0, vkpp::IndexType::eUInt32);
        lDrawCmdBuffer.DrawIndexed(mTorusKnotModel.indexCount);*/

        lDrawCmdBuffer.EndRenderPass();

        lDrawCmdBuffer.End();
    }
}


void TexturedCube::CreateSemaphores(void)
{
    constexpr vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    mPresentCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mRenderingCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void TexturedCube::CreateFences(void)
{
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    for (std::size_t lIndex = 0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
        mWaitFences.emplace_back(mLogicalDevice.CreateFence(lFenceCreateInfo));
}


void TexturedCube::Update(void)
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


vkpp::CommandBuffer TexturedCube::BeginOneTimeCommandBuffer(void) const
{
    const vkpp::CommandBufferAllocateInfo lCmdAllocateInfo
    {
        mCmdPool, 1
    };

    auto lCmdBuffer = mLogicalDevice.AllocateCommandBuffer(lCmdAllocateInfo);

    constexpr vkpp::CommandBufferBeginInfo lCmdBeginInfo{ vkpp::CommandBufferUsageFlagBits::eOneTimeSubmit };
    lCmdBuffer.Begin(lCmdBeginInfo);

    return lCmdBuffer;
}


void TexturedCube::EndOneTimeCommandBuffer(const vkpp::CommandBuffer& aCmdBuffer) const
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


template <vkpp::ImageLayout OldLayout, vkpp::ImageLayout NewLayout>
void TexturedCube::TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange, const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask)
{
    // Create an image varrier object
    const vkpp::ImageMemoryBarrier lImageMemBarrier
    {
        aSrcAccessMask, aDstAccessMask,
        OldLayout, NewLayout,
        aImage, aImageSubRange
    };

    // Put barrier inside setup command buffer.
    aCmdBuffer.PipelineBarrier(vkpp::PipelineStageFlagBits::eTopOfPipe, vkpp::PipelineStageFlagBits::eTopOfPipe, vkpp::DependencyFlagBits::eByRegion, lImageMemBarrier);
}


void TexturedCube::CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const
{
    const auto& lCmdBuffer = BeginOneTimeCommandBuffer();

    const vkpp::BufferCopy lBufferCopy
    {
        0, 0,
        aSize
    };

    lCmdBuffer.Copy(aDstBuffer, aSrcBuffer, lBufferCopy);

    EndOneTimeCommandBuffer(lCmdBuffer);
}


Model::Model(const TexturedCube& aTexturedCubeSample, const vkpp::LogicalDevice& aDevice, const vkpp::PhysicalDeviceMemoryProperties& aPhysicalDeviceMemProperties)
    : texturedCubeSample(aTexturedCubeSample), device(aDevice), memProperties(aPhysicalDeviceMemProperties), vertices(aDevice, aPhysicalDeviceMemProperties), indices(aDevice, aPhysicalDeviceMemProperties)
{}


void Model::LoadMode(const std::string& aFilename, const glm::vec3& aScale, unsigned int aImporterFlags)
{
    Assimp::Importer lImporter;

    auto lpAIScene = lImporter.ReadFile(aFilename, aImporterFlags);
    assert(lpAIScene != nullptr);

    std::vector<float> lVertexBuffer;
    std::vector<uint32_t> lIndexBuffer;

    for (unsigned int lIndex = 0; lIndex < lpAIScene->mNumMeshes; ++lIndex)
    {
        const auto lpAIMesh = lpAIScene->mMeshes[lIndex];

        modelParts.emplace_back(vertexCount, lpAIMesh->mNumVertices, indexCount, lpAIMesh->mNumFaces * 3);
        vertexCount += lpAIScene->mMeshes[lIndex]->mNumVertices;
        indexCount += lpAIScene->mMeshes[lIndex]->mNumFaces * 3;

        /*aiColor3D lColor;
        lpAIScene->mMaterials[lpAIMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, lColor);*/

        const aiVector3D lZero3D;

        for (unsigned int lVtxIndex = 0; lVtxIndex < lpAIMesh->mNumVertices; ++lVtxIndex)
        {
            // Vertex positions.
            const auto lPos = lpAIMesh->mVertices[lVtxIndex];
            lVertexBuffer.emplace_back(lPos.x * aScale.x);
            lVertexBuffer.emplace_back(-lPos.y * aScale.y);
            lVertexBuffer.emplace_back(lPos.z * aScale.z);

            // Vertex normals.
            const auto lNormal = lpAIMesh->mNormals[lVtxIndex];
            lVertexBuffer.emplace_back(lNormal.x);
            lVertexBuffer.emplace_back(-lNormal.y);
            lVertexBuffer.emplace_back(lNormal.z);

            // Vertex texture coordinates.
            const auto lTexCoord = lpAIMesh->HasTextureCoords(0) ? lpAIMesh->mTextureCoords[0][lVtxIndex] : lZero3D;
            lVertexBuffer.emplace_back(lTexCoord.x);
            lVertexBuffer.emplace_back(lTexCoord.y);

            // Vertex color.
            /*lVertexBuffer.emplace_back(lColor.r);
            lVertexBuffer.emplace_back(lColor.g);
            lVertexBuffer.emplace_back(lColor.b);*/

            dim.max.x = std::max(lPos.x, dim.max.x);
            dim.max.y = std::max(lPos.x, dim.max.y);
            dim.max.z = std::max(lPos.x, dim.max.z);

            dim.min.x = std::min(lPos.x, dim.min.x);
            dim.min.y = std::min(lPos.x, dim.min.y);
            dim.min.z = std::min(lPos.x, dim.min.z);
        }

        dim.size = dim.max - dim.min;

        auto lIndexBase = static_cast<uint32_t>(lIndexBuffer.size());
        for (unsigned int lIdxIndex = 0; lIdxIndex < lpAIMesh->mNumFaces; ++lIdxIndex)
        {
            const auto& lFace = lpAIMesh->mFaces[lIdxIndex];

            if (lFace.mNumIndices != 3)
                continue;

            lIndexBuffer.emplace_back(lIndexBase + lFace.mIndices[0]);
            lIndexBuffer.emplace_back(lIndexBase + lFace.mIndices[1]);
            lIndexBuffer.emplace_back(lIndexBase + lFace.mIndices[2]);
        }
    }

    // Use Staging buffers to move vertex and index buffer to device local memory.
    // Vertex Buffer.
    auto lVtxBufferSize = static_cast<uint32_t>(lVertexBuffer.size()) * sizeof(float);
    const vkpp::BufferCreateInfo lVtxStagingCreateInfo
    {
        lVtxBufferSize,
        vkpp::BufferUsageFlagBits::eTransferSrc
    };

    BufferResource lStagingVtxBufferRes{ device, memProperties };
    lStagingVtxBufferRes.Reset(lVtxStagingCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lMappedMem = device.MapMemory(lStagingVtxBufferRes.memory, 0, lVtxBufferSize);
    std::memcpy(lMappedMem, lVertexBuffer.data(), lVtxBufferSize);
    device.UnmapMemory(lStagingVtxBufferRes.memory);

    // Index Buffer.
    auto lIdxBufferSize = static_cast<uint32_t>(lIndexBuffer.size()) * sizeof(uint32_t);
    const vkpp::BufferCreateInfo lIdxStagingCreateInfo
    {
        lIdxBufferSize,
        vkpp::BufferUsageFlagBits::eTransferSrc
    };

    BufferResource lStagingIdxBufferRes{ device, memProperties };
    lStagingIdxBufferRes.Reset(lIdxStagingCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);
    lMappedMem = device.MapMemory(lStagingIdxBufferRes.memory, 0, lIdxBufferSize);
    std::memcpy(lMappedMem, lIndexBuffer.data(), lIdxBufferSize);
    device.UnmapMemory(lStagingIdxBufferRes.memory);

    // Create device local target buffers.
    // Vertex Buffer.
    const vkpp::BufferCreateInfo lVtxCreateInfo
    {
        lVtxBufferSize,
        vkpp::BufferUsageFlagBits::eVertexBuffer | vkpp::BufferUsageFlagBits::eTransferDst
    };

    vertices.Reset(lVtxCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    // Index Buffer.
    const vkpp::BufferCreateInfo lIdxCreateInfo
    {
        lIdxBufferSize,
        vkpp::BufferUsageFlagBits::eIndexBuffer | vkpp::BufferUsageFlagBits::eTransferDst
    };

    indices.Reset(lIdxCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    texturedCubeSample.CopyBuffer(vertices.buffer, lStagingVtxBufferRes.buffer, lVtxBufferSize);
    texturedCubeSample.CopyBuffer(indices.buffer, lStagingIdxBufferRes.buffer, lIdxBufferSize);
}


}                   // End of namespace vkpp::sample.