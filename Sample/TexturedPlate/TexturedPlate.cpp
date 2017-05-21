#include "TexturedPlate.h"



namespace vkpp::sample
{

    

TexturedPlate::TexturedPlate(CWindow& aWindow, const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion)
    : ExampleBase(aWindow, apApplicationName, aApplicationVersion, apEngineName, aEngineVersion),
      CWindowEvent(aWindow),
      CMouseWheelEvent(aWindow),
      CMouseMotionEvent(aWindow),
      mDepthResource(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mTextureResource(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mVertexBufferRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mIndexBufferRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mUniformBufferRes(mLogicalDevice, mPhysicalDeviceMemoryProperties)
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

        mDepthResource.Reset();
        CreateDepthResource();

        // Re-create framebuffers.
        for (auto& lFramebuffer : mFramebuffers)
            mLogicalDevice.DestroyFramebuffer(lFramebuffer);

        mFramebuffers.clear();
        CreateFramebuffer();

        // Command buffers need to be recreated as they reference to the destroyed framebuffer.
        mLogicalDevice.FreeCommandBuffers(mCmdPool, mDrawCmdBuffers);
        AllocateCmdBuffers();
        BuildCommandBuffers();
    };

    CreateCmdPool();
    AllocateCmdBuffers();

    CreateRenderPass();
    CreateDepthResource();

    CreateFramebuffer();

    CreateSetLayout();
    CreatePipelineLayout();

    CreateGraphicsPipeline();

    CreateDescriptorPool();
    AllocateDescriptorSet();

    // TODO: Vulkan core support three different compressed texture formats.
    // As the support differs among implementations, it is needed to check device features and select a proper format and file.
    LoadTexture("Texture/metalplate01_bc2_unorm.ktx", vkpp::Format::eBC2_uNormBlock);
    CreateSampler();

    CreateUniformBuffer();
    UpdateUniformBuffer();

    UpdateDescriptorSet();

    CreateVertexBuffer();
    CreateIndexBuffer();

    BuildCommandBuffers();

    CreateSemaphores();
    CreateFences();
}


TexturedPlate::~TexturedPlate(void)
{
    mLogicalDevice.Wait();

    for (auto& lFence : mWaitFences)
        mLogicalDevice.DestroyFence(lFence);

    mLogicalDevice.DestroySemaphore(mPresentCompleteSemaphore);
    mLogicalDevice.DestroySemaphore(mRenderCompleteSemaphore);

    mIndexBufferRes.Reset();
    mVertexBufferRes.Reset();

    mUniformBufferRes.Reset();

    mLogicalDevice.DestroySampler(mTextureSampler);

    mLogicalDevice.DestroyDescriptorPool(mDescriptorPool);

    mLogicalDevice.DestroyPipeline(mGraphicsPipeline);

    mLogicalDevice.DestroyPipelineLayout(mPipelineLayout);
    mLogicalDevice.DestroyDescriptorSetLayout(mSetLayout);

    for (auto& lFramebuffer : mFramebuffers)
        mLogicalDevice.DestroyFramebuffer(lFramebuffer);

    mDepthResource.Reset();

    mLogicalDevice.DestroyRenderPass(mRenderPass);

    mLogicalDevice.FreeCommandBuffers(mCmdPool, mDrawCmdBuffers);
    mLogicalDevice.DestroyCommandPool(mCmdPool);
}


void TexturedPlate::CreateCmdPool(void)
{
    const vkpp::CommandPoolCreateInfo lCmdPoolCreateInfo
    {
        mGraphicsQueue.familyIndex,
        vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer
    };

    mCmdPool = mLogicalDevice.CreateCommandPool(lCmdPoolCreateInfo);
}


void TexturedPlate::AllocateCmdBuffers(void)
{
    const vkpp::CommandBufferAllocateInfo lCmdAllocateInfo
    {
        mCmdPool,
        static_cast<uint32_t>(mSwapchain.buffers.size())
    };

    mDrawCmdBuffers = mLogicalDevice.AllocateCommandBuffers(lCmdAllocateInfo);
}


void TexturedPlate::CreateRenderPass(void)
{
    const std::array<vkpp::AttachementDescription, 2> lAttachments
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

    constexpr vkpp::AttachmentReference  lColorRef
    {
        0, vkpp::ImageLayout::eColorAttachmentOptimal
    };

    constexpr vkpp::AttachmentReference lDepthRef
    {
        1, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
    };

    constexpr std::array<vkpp::SubpassDescription, 1> lSubpassDescriptions
    { {
        {
            vkpp::PipelineBindPoint::eGraphics,
            0, nullptr,
            1, lColorRef.AddressOf(),
            nullptr,
            lDepthRef.AddressOf()
        }
    } };

    constexpr std::array<vkpp::SubpassDependency, 2> lSubpassDependencies
    { {
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
    } };

    const vkpp::RenderPassCreateInfo lRenderPassCreateInfo
    {
        lAttachments, lSubpassDescriptions, lSubpassDependencies
    };

    mRenderPass = mLogicalDevice.CreateRenderPass(lRenderPassCreateInfo);
}


void TexturedPlate::CreateDepthResource(void)
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

    mDepthResource.Reset(lImageCreateInfo, lImageViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);
}


void TexturedPlate::CreateFramebuffer(void)
{
    for (auto& lSwapchain : mSwapchain.buffers)
    {
        const std::array<vkpp::ImageView, 2> lAttachments
        {
            lSwapchain.view,
            mDepthResource.view
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


void TexturedPlate::CreateSetLayout(void)
{
    constexpr std::array<vkpp::DescriptorSetLayoutBinding, 2> lSetLayoutBindings
    { {
        // Binding 0: Vertex shader uniform buffer. (MVP matrix)
        {
            0,                                          // Binding
            vkpp::DescriptorType::eUniformBuffer,
            1,                                          // Descriptor count
            vkpp::ShaderStageFlagBits::eVertex
        },
        // Binding 1: Fragment shader image sampler.
        {
            1,                                          // Binding
            vkpp::DescriptorType::eCombinedImageSampler,
            1,                                          // Descriptor count
            vkpp::ShaderStageFlagBits::eFragment
        }
    } };

    const vkpp::DescriptorSetLayoutCreateInfo lSetLayoutCreateInfo{ lSetLayoutBindings };
    mSetLayout = mLogicalDevice.CreateDescriptorSetLayout(lSetLayoutCreateInfo);
}


void TexturedPlate::CreatePipelineLayout(void)
{
    const vkpp::PipelineLayoutCreateInfo lPipelineLayoutCreateInfo
    {
        1, mSetLayout.AddressOf()
    };

    mPipelineLayout = mLogicalDevice.CreatePipelineLayout(lPipelineLayoutCreateInfo);
}


void TexturedPlate::CreateGraphicsPipeline(void)
{
    const auto& lVertexShader = CreateShaderModule("Shader/SPV/texture.vert.spv");
    const auto& lFragmentShader = CreateShaderModule("Shader/SPV/texture.frag.spv");

    const std::vector<vkpp::PipelineShaderStageCreateInfo> lShaderStageCreateInfos
    {
        {
            vkpp::ShaderStageFlagBits::eVertex,
            lVertexShader
        },
        {
            vkpp::ShaderStageFlagBits::eFragment,
            lFragmentShader
        }
    };

    constexpr std::array<vkpp::VertexInputBindingDescription, 1> lVertexInputBindingDescriptions{ VertexData::GetBindingDescription() };
    constexpr auto lVertexAtributeDescriptions = VertexData::GetAttributeDescriptions();

    const vkpp::PipelineVertexInputStateCreateInfo lInputStateCreateInfo
    {
        lVertexInputBindingDescriptions,
        lVertexAtributeDescriptions
    };

    constexpr vkpp::PipelineInputAssemblyStateCreateInfo lInputAssemblyState{ vkpp::PrimitiveTopology::eTriangleList };

    constexpr vkpp::PipelineViewportStateCreateInfo lViewportCreateInfo{ 1, 1 };

    constexpr vkpp::PipelineRasterizationStateCreateInfo lRasterizationStateCreateInfo
    {
        VK_FALSE, VK_FALSE,
        vkpp::PolygonMode::eFill,
        vkpp::CullModeFlagBits::eNone,
        vkpp::FrontFace::eCounterClockwise,
        VK_FALSE,
        0.0f, 0.0f, 1.0f,
        1.0f
    };

    constexpr vkpp::PipelineMultisampleStateCreateInfo lMultisampleStateCreateInfo;

    constexpr vkpp::PipelineDepthStencilStateCreateInfo lDepthStencilStateCreateInfo
    {
        VK_TRUE, VK_TRUE,
        vkpp::CompareOp::eLessOrEqual
    };

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

    const vkpp::GraphicsPipelineCreateInfo lGraphicsPipelineCreateInfo
    {
        2, lShaderStageCreateInfos.data(),
        lInputStateCreateInfo.AddressOf(),
        lInputAssemblyState.AddressOf(),
        nullptr,
        lViewportCreateInfo.AddressOf(),
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

    mLogicalDevice.DestroyShaderModule(lFragmentShader);
    mLogicalDevice.DestroyShaderModule(lVertexShader);
}


void TexturedPlate::CreateDescriptorPool(void)
{
    constexpr std::array<vkpp::DescriptorPoolSize, 2> lPoolSizes
    { {
        { vkpp::DescriptorType::eUniformBuffer, 1},
        { vkpp::DescriptorType::eCombinedImageSampler, 1}
    } };

    const vkpp::DescriptorPoolCreateInfo lPoolCreateInfo
    {
        lPoolSizes, 2
    };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool(lPoolCreateInfo);
}


void TexturedPlate::AllocateDescriptorSet(void)
{
    const vkpp::DescriptorSetAllocateInfo lSetAllocateInfo
    {
        mDescriptorPool,
        1, mSetLayout.AddressOf()
    };

    mDescriptorSet = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);
}


void TexturedPlate::LoadTexture(const std::string& aFilename, vkpp::Format mTexFormat)
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
            {0, 0, 0},
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
        mTexFormat,
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
        mTexFormat,
        // The subresource range describes the set of mip-levels (and array layers) that can be accessed through this image view.
        // It is possible to create multiple image views for a single image referring to different (and/or overlapping) ranges of the image.
        {
            vkpp::ImageAspectFlagBits::eColor,
            0, mTexture.mipLevels,
            0, 1
        }
    };

    mTextureResource.Reset(lImageCreateInfo, lImageViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

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
        lCopyCmd, mTextureResource.image,
        lImageSubRange,
        vkpp::DefaultFlags,                         // srcAccessMask = 0: Only valid as initial layout, memory contents are not preserved.
                                                    //                    Can be accessed directly, no source dependency required.
        vkpp::AccessFlagBits::eTransferWrite        // dstAccessMask: Transfer destination (copy, blit).
                                                    //                Make sure any write operation to the image has been finished.
    );

    // Copy all mip-levels from staging buffer.
    lCopyCmd.Copy(mTextureResource.image, vkpp::ImageLayout::eTransferDstOptimal, lStagingBufferRes.buffer, lBufferCopyRegions);

    // Transfer texture image layout to shader read after all mip-levels have been copied.
    TransitionImageLayout<vkpp::ImageLayout::eTransferDstOptimal, vkpp::ImageLayout::eShaderReadOnlyOptimal>
    (
        lCopyCmd, mTextureResource.image,
        lImageSubRange,
        vkpp::AccessFlagBits::eTransferWrite,       // srcAccessMask: Old layout is transfer destination.
                                                    //                Make sure any write operation to the destination image has been finished.
        vkpp::AccessFlagBits::eShaderRead           // dstAccessMask: Shader read, like sampler, input attachment.
    );

    EndOneTimeCmdBuffer(lCopyCmd);

    lStagingBufferRes.Reset();
}


// In Vulkan, textures are accessed by samplers. This separates all the sampling information from the texture data.
// This means it is possible to have multiple sampler objects for the same texture with different settings.
void TexturedPlate::CreateSampler(void)
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
        VK_TRUE,                                                    // anisotropyEnable
        mPhysicalDeviceProperties.limits.maxSamplerAnisotropy,      // maxAnisotropy
        VK_FALSE,                                                   // compareEnable,
        vkpp::CompareOp::eNever,                                    // compareOp
        0.0f,                                                       // minLoad
        static_cast<float>(mTexture.mipLevels),                     // maxLoad: Set max level-of-detail to mip-level count of the texture.
        vkpp::BorderColor::eFloatOpaqueWhite                        // borderColor
    };

    mTextureSampler = mLogicalDevice.CreateSampler(lSamplerCreateInfo);
}


void TexturedPlate::UpdateDescriptorSet(void)
{
    // Setup a descriptor image info for the current texture to be used as a combined image sampler.
    const vkpp::DescriptorImageInfo lTextureDescriptor
    {
        mTextureSampler,                    // Tells the pipeline how to sample the texture, including repeat, border, etc.
        mTextureResource.view,              // Images are never directly accessed by the shader, but rather through views defining the sub-resources.
        mTexture.layout                     // The current layout of the image. Note: It should always fit the actual use, e.g. shader read.
    };

    const vkpp::DescriptorBufferInfo lDescriptorBufferInfo
    {
        mUniformBufferRes.buffer,
        0,
        sizeof(UniformBufferObject)
    };

    const std::array<vkpp::WriteDescriptorSetInfo, 2> lWriteDescriptorSets
    { {
        // Binding 0: Vertex shader uniform buffer.
        {
            mDescriptorSet,
            0,                                      // dstBindig
            0,                                      // dstArrayElement
            vkpp::DescriptorType::eUniformBuffer,
            lDescriptorBufferInfo
        },
        // Binding 1: Fragment shader texture sampler.
        //            Fragment shader: layout(binding = 1) uniform sampler2D samplerColor;
        {
            mDescriptorSet,
            1,                                              // binding
            0,                                              // dstArrayElement
            vkpp::DescriptorType::eCombinedImageSampler,    // The descriptor set will use a combined image sampler (sampler and image could be split).
            lTextureDescriptor
        }
    } };

    mLogicalDevice.UpdateDescriptorSets(lWriteDescriptorSets);
}


void TexturedPlate::CreateVertexBuffer(void)
{
    // Setup vertices for a single uv-mapped quad made from two triangles.
    std::array<VertexData, 4> lVertices
    { {
        {{1.0f, 1.0f, 0.0f}, {2.0f, 2.0f}, {0.0f, 0.0f, 1.0f}},
        {{-1.0f, 1.0f, 0.0f}, {0.0f, 2.0f}, {0.0f, 0.0f, 1.0f}},
        {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{1.0f, -1.0f, 0.0f}, {2.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    } };

    const auto lVertexDataSize = sizeof(lVertices);

    const vkpp::BufferCreateInfo lBufferCreateInfo
    {
        lVertexDataSize,
        vkpp::BufferUsageFlagBits::eVertexBuffer
    };

    mVertexBufferRes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lMappedMem = mLogicalDevice.MapMemory(mVertexBufferRes.memory, 0, lVertexDataSize);
    std::memcpy(lMappedMem, lVertices.data(), lVertexDataSize);
    mLogicalDevice.UnmapMemory(mVertexBufferRes.memory);
}


void TexturedPlate::CreateIndexBuffer(void)
{
    // Setup indices.
    constexpr std::array<uint32_t, 6> lIndices
    {
        0, 3, 2,
        0, 2, 1
    };

    constexpr auto lIndexDataSize = sizeof(lIndices);

    const vkpp::BufferCreateInfo lIndexCreateInfo
    {
        lIndexDataSize,
        vkpp::BufferUsageFlagBits::eIndexBuffer
    };

    mIndexBufferRes.Reset(lIndexCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lMappedMem = mLogicalDevice.MapMemory(mIndexBufferRes.memory, 0, lIndexDataSize);
    std::memcpy(lMappedMem, lIndices.data(), lIndexDataSize);
    mLogicalDevice.UnmapMemory(mIndexBufferRes.memory);
}


void TexturedPlate::CreateUniformBuffer(void)
{
    constexpr vkpp::BufferCreateInfo lBufferCreateInfo
    {
        sizeof(UniformBufferObject),
        vkpp::BufferUsageFlagBits::eUniformBuffer
    };

    mUniformBufferRes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);
}


void TexturedPlate::UpdateUniformBuffer(void)
{
    const auto lWidth = static_cast<float>(mSwapchain.extent.width);
    const auto lHeight = static_cast<float>(mSwapchain.extent.height);

    const glm::vec3 lCameraPos;

    mMVPMatrix.projection = glm::perspective(glm::radians(60.f), lWidth / lHeight, 0.001f, 256.0f);

    const auto& lViewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, mCurrentZoomLevel));
    mMVPMatrix.model = lViewMatrix * glm::translate(glm::mat4(), lCameraPos);
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    mMVPMatrix.model = glm::rotate(mMVPMatrix.model, glm::radians(mCurrentRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    mMVPMatrix.viewPos = glm::vec4(0.0f, 0.0f, -mCurrentZoomLevel, 0.0f);

    auto lMappedMem = mLogicalDevice.MapMemory(mUniformBufferRes.memory, 0, sizeof(UniformBufferObject));
    std::memcpy(lMappedMem, &mMVPMatrix, sizeof(UniformBufferObject));
    mLogicalDevice.UnmapMemory(mUniformBufferRes.memory);
}


vkpp::CommandBuffer TexturedPlate::BeginOneTimeCmdBuffer(void) const
{
    const vkpp::CommandBufferAllocateInfo lCmdAllocateInfo
    {
        mCmdPool, 1
    };

    auto lCmdBuffer = mLogicalDevice.AllocateCommandBuffer(lCmdAllocateInfo);

    constexpr vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo{ vkpp::CommandBufferUsageFlagBits::eOneTimeSubmit };
    lCmdBuffer.Begin(lCmdBufferBeginInfo);

    return lCmdBuffer;
}


void TexturedPlate::EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const
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


// Create an image barrier for changing the layout of an image and put it into an active command buffer.
template <vkpp::ImageLayout OldLayout, vkpp::ImageLayout NewLayout>
void TexturedPlate::TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange,
    const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask)
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


void TexturedPlate::CreateSemaphores(void)
{
    constexpr vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    mPresentCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mRenderCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void TexturedPlate::CreateFences(void)
{
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    for (std::size_t lIndex = 0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
        mWaitFences.emplace_back(mLogicalDevice.CreateFence(lFenceCreateInfo));
}


void TexturedPlate::BuildCommandBuffers(void)
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
                {0, 0},
                mSwapchain.extent
            },
            lClearValues
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
            {0, 0},
            mSwapchain.extent
        };

        lDrawCmdBuffer.SetScissor(lScissor);

        lDrawCmdBuffer.BindGraphicsPipeline(mGraphicsPipeline);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayout, 0, mDescriptorSet);

        lDrawCmdBuffer.DrawIndexed(6);

        lDrawCmdBuffer.EndRenderPass();

        lDrawCmdBuffer.End();
    }
}


void TexturedPlate::Update(void)
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
        1, mRenderCompleteSemaphore.AddressOf()
    };

    mPresentQueue.handle.Submit(lSubmitInfo, mWaitFences[lIndex]);

    const vkpp::khr::PresentInfo lPresentInfo
    {
        1, mRenderCompleteSemaphore.AddressOf(),
        1, mSwapchain.handle.AddressOf(),
        &lIndex
    };

    mPresentQueue.handle.Present(lPresentInfo);
}



}                   // End of namespace vkpp::sample.
