#include "ParticleFire.h"

#include <chrono>
#include <cmath>
#include <algorithm>



namespace vkpp::sample
{



ParticleFire::ParticleFire(CWindow & aWindow, const char * apAppName, uint32_t aAppVersion, const char * apEngineName, uint32_t aEngineVersion)
    : ExampleBase{aWindow, apAppName, aAppVersion, apEngineName, aEngineVersion},
      CWindowEvent{aWindow}, CMouseMotionEvent{aWindow}, CMouseWheelEvent{aWindow},
      mDepthRes{ mLogicalDevice, mPhysicalDeviceMemoryProperties },
      mSmokeTexRes{ mLogicalDevice, mPhysicalDeviceMemoryProperties },
      mFireTexRes{ mLogicalDevice, mPhysicalDeviceMemoryProperties },
      mColorMapTexRes{ mLogicalDevice, mPhysicalDeviceMemoryProperties },
      mNormapMapTexRes{ mLogicalDevice, mPhysicalDeviceMemoryProperties },
      mUboEnv{ mLogicalDevice, mPhysicalDeviceMemoryProperties },
      mUboVS{ mLogicalDevice, mPhysicalDeviceMemoryProperties },
      mVtxBufferRes{ mLogicalDevice, mPhysicalDeviceMemoryProperties },
      mIdxBufferRes{ mLogicalDevice, mPhysicalDeviceMemoryProperties },
      mParticleBufferRes{ mLogicalDevice, mPhysicalDeviceMemoryProperties }
{
    theApp.RegisterUpdateEvent([this](void)
    {
        UpdateUniformBuffers();
        UpdateParticles();
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
    CreatePipelines();

    CreateDescriptorPool();
    AllocateDescriptorSets();

    // Particles
    LoadTexture("Texture/particle_fire.ktx", vkpp::Format::eRGBA8uNorm, mTexFire, mFireTexRes);
    LoadTexture("Texture/particle_smoke.ktx", vkpp::Format::eRGBA8uNorm, mTexSmoke, mSmokeTexRes);

    // Normal map
    LoadTexture("Texture/fireplace_normalmap_bc3_unorm.ktx", vkpp::Format::eBC3_uNormBlock, mTexNormalMap, mNormapMapTexRes);
    LoadTexture("Texture/fireplace_colormap_bc3_unorm.ktx", vkpp::Format::eBC3_uNormBlock, mTexColorMap, mColorMapTexRes);

    CreateSampler();
    CreateUniformBuffers();
    UpdateUniformBuffers();
    UpdateDescriptorSets();

    LoadModel("Model/fireplace.obj");
    CreateParticles();

    BuildCmdBuffers();

    CreateSemaphores();
    CreateFences();
}


ParticleFire::~ParticleFire(void)
{
    mLogicalDevice.Wait();

    mLogicalDevice.DestroySemaphore(mRenderingCompleteSemaphore);
    mLogicalDevice.DestroySemaphore(mPresentCompleteSemaphore);

    mLogicalDevice.DestroyFences(mWaitFences);

    mParticleBufferRes.Reset();
    mIdxBufferRes.Reset();
    mVtxBufferRes.Reset();

    mUboEnv.Unmap();
    mUboVS.Unmap();

    mUboEnv.Reset();
    mUboVS.Reset();

    mLogicalDevice.DestroySampler(mTextureSampler);

    mNormapMapTexRes.Reset();
    mColorMapTexRes.Reset();
    mFireTexRes.Reset();
    mSmokeTexRes.Reset();

    mLogicalDevice.DestroyDescriptorPool(mDescriptorPool);

    mLogicalDevice.DestroyPipeline(mPipelines.particle);
    mLogicalDevice.DestroyPipeline(mPipelines.normalmap);

    mLogicalDevice.DestroyPipelineLayout(mPipelineLayout);
    mLogicalDevice.DestroyDescriptorSetLayout(mSetLayout);
    mLogicalDevice.DestroyFramebuffers(mFramebuffers);
    mDepthRes.Reset();
    mLogicalDevice.DestroyRenderPass(mRenderPass);
    mLogicalDevice.FreeCommandBuffers(mCmdPool, mDrawCmdBuffers);
    mLogicalDevice.DestroyCommandPool(mCmdPool);
}


void ParticleFire::CreateCmdPool(void)
{
    const vkpp::CommandPoolCreateInfo lCmdCreateInfo
    {
        mGraphicsQueue.familyIndex,
        vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer
    };

    mCmdPool = mLogicalDevice.CreateCommandPool(lCmdCreateInfo);
}


void ParticleFire::AllocateCmdBuffers(void)
{
    const vkpp::CommandBufferAllocateInfo lCmdBufferAllocateInfo
    {
        mCmdPool,
        static_cast<uint32_t>(mSwapchain.buffers.size())
    };

    mDrawCmdBuffers = mLogicalDevice.AllocateCommandBuffers(lCmdBufferAllocateInfo);
}


void ParticleFire::CreateRenderPass(void)
{
    const std::vector<vkpp::AttachementDescription> lAttachments
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
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eStore,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
        }
    };

    constexpr vkpp::AttachmentReference lColorRef{ 0, vkpp::ImageLayout::eColorAttachmentOptimal };
    constexpr vkpp::AttachmentReference lDepthRef{ 1, vkpp::ImageLayout::eDepthStencilAttachmentOptimal };

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
        lAttachments, lSubpassDescriptions, lSubpassDependencies
    };

    mRenderPass = mLogicalDevice.CreateRenderPass(lRenderPassCreateInfo);
}


void ParticleFire::CreateDepthRes(void)
{
    const vkpp::ImageCreateInfo lImageCreateInfo
    {
        vkpp::ImageType::e2D,
        vkpp::Format::eD32sFloat,
        mSwapchain.extent,
        vkpp::ImageUsageFlagBits::eDepthStencilAttachment
    };

    vkpp::ImageViewCreateInfo lViewCreateInfo
    {
        vkpp::ImageViewType::e2D,
        vkpp::Format::eD32sFloat,
        {
            vkpp::ImageAspectFlagBits::eDepth,
            0, 1,
            0, 1
        }
    };

    mDepthRes.Reset(lImageCreateInfo, lViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);
}


void ParticleFire::CreateFramebuffers(void)
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


void ParticleFire::CreateSetLayouts(void)
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
        // Binding 2: Fragment shader image sampler
        {
            2,
            vkpp::DescriptorType::eCombinedImageSampler,
            vkpp::ShaderStageFlagBits::eFragment
        }
    };

    mSetLayout = mLogicalDevice.CreateDescriptorSetLayout(lSetLayoutBindings);
}


void ParticleFire::CreatePipelineLayout(void)
{
    mPipelineLayout = mLogicalDevice.CreatePipelineLayout(mSetLayout);
}


void ParticleFire::CreatePipelines(void)
{
    std::array<vkpp::PipelineShaderStageCreateInfo, 2> lShaderStageCreateInfos
    { {
        { vkpp::ShaderStageFlagBits::eVertex },
        { vkpp::ShaderStageFlagBits::eFragment }
    } };

    vkpp::PipelineVertexInputStateCreateInfo lInputStateCreateInfo;

    vkpp::PipelineInputAssemblyStateCreateInfo lInputAssemblyStateCreateInfo{ vkpp::PrimitiveTopology::eTriangleList };
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
        vkpp::DepthTest::Enable, vkpp::DepthWrite::Enable,
        vkpp::CompareOp::eLess
    };

    vkpp::PipelineColorBlendAttachmentState lColorBlendAttachmentState;
    const vkpp::PipelineColorBlendStateCreateInfo lColorBlendStateCreateInfo{ lColorBlendAttachmentState };

    constexpr std::array<vkpp::DynamicState, 2> lDynamicStates
    {
        vkpp::DynamicState::eViewport, vkpp::DynamicState::eScissor
    };

    const vkpp::PipelineDynamicStateCreateInfo lDynamicStateCreateInfo{ lDynamicStates };

    const vkpp::GraphicsPipelineCreateInfo lGraphicsPipelineCreateInfo
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
        mPipelineLayout,
        mRenderPass,
        0
    };

    auto& lVertexShaderModule = lShaderStageCreateInfos[0].module;
    auto& lFragmentShaderModule = lShaderStageCreateInfos[1].module;

    // Environment rendering pipeline (normal mapped)
    {
        // Vertex input state
        constexpr auto lInputBindingDescription = NormalMapVtxData::GetBindingDescription();
        constexpr auto lInputAttributeDescriptions = NormalMapVtxData::GetAttributeDescriptions();

        lInputStateCreateInfo
            .SetBindingDescription(lInputBindingDescription)
            .SetAttributeDescriptions(lInputAttributeDescriptions);

        // Shaders
        lVertexShaderModule = mLogicalDevice.CreateShaderModule("Shader/SPV/normalmap.vert.spv");
        lFragmentShaderModule = mLogicalDevice.CreateShaderModule("Shader/SPV/normalmap.frag.spv");

        mPipelines.normalmap = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

        mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
        mLogicalDevice.DestroyShaderModule(lVertexShaderModule);
    }

    // Particle rendering pipeline
    {
        // Vertex input state
        constexpr auto lInputBindingDescription = ParticleVtxData::GetBindingDescription();
        constexpr auto lInputAttributeDescriptions = ParticleVtxData::GetAttributeDescriptions();

        lInputStateCreateInfo
            .SetBindingDescription(lInputBindingDescription)
            .SetAttributeDescriptions(lInputAttributeDescriptions);

        // Don't write to depth buffer
        lDepthStencilStateCreateInfo.DisableDepthTest();

        // Premulitplied alpha
        lColorBlendAttachmentState
            .EnableBlendOp()
            .SetColorBlend(vkpp::BlendFactor::eOne, vkpp::BlendFactor::eOneMinusSrcAlpha, vkpp::BlendOp::eAdd)
            .SetAlphaBlend(vkpp::BlendFactor::eOne, vkpp::BlendFactor::eZero, vkpp::BlendOp::eAdd);

        // Shaders
        lVertexShaderModule = mLogicalDevice.CreateShaderModule("Shader/SPV/particle.vert.spv");
        lFragmentShaderModule = mLogicalDevice.CreateShaderModule("Shader/SPV/particle.frag.spv");

        lInputAssemblyStateCreateInfo.SetTopology(vkpp::PrimitiveTopology::ePointList);

        mPipelines.particle = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

        mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
        mLogicalDevice.DestroyShaderModule(lVertexShaderModule);
    }
}


void ParticleFire::CreateDescriptorPool(void)
{
    // Example uses one UBO and two image samplers.
    constexpr std::array<vkpp::DescriptorPoolSize, 2> lPoolSizes
    { {
        { vkpp::DescriptorType::eUniformBuffer, 2 },
        { vkpp::DescriptorType::eCombinedImageSampler, 4 }
    } };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool({ lPoolSizes, 2 });
}


void ParticleFire::AllocateDescriptorSets(void)
{
    const vkpp::DescriptorSetAllocateInfo lSetAllocateInfo
    {
        mDescriptorPool, mSetLayout
    };

    mDescriptorSets.normalmap = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);
    mDescriptorSets.particle = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);
}


void ParticleFire::LoadTexture(const std::string& aFilename, vkpp::Format aTexFormat, Texture& aTexture, ImageResource& aTexRes) const
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


template <vkpp::ImageLayout OldLayout, vkpp::ImageLayout NewLayout>
void ParticleFire::TransitionImageLayout(const vkpp::CommandBuffer& aCmdBuffer, const vkpp::Image& aImage, const vkpp::ImageSubresourceRange& aImageSubRange, const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask)
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


vkpp::CommandBuffer ParticleFire::BeginOneTimeCmdBuffer(void) const
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


void ParticleFire::EndOneTimeCmdBuffer(const vkpp::CommandBuffer& aCmdBuffer) const
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


void ParticleFire::CreateSampler(void)
{
    const vkpp::SamplerCreateInfo lSamplerCreateInfo
    {
        vkpp::Filter::eLinear,
        vkpp::Filter::eLinear,
        vkpp::SamplerMipmapMode::eLinear,
        vkpp::SamplerAddressMode::eClampToEdge,
        vkpp::SamplerAddressMode::eClampToEdge,
        vkpp::SamplerAddressMode::eClampToEdge,
        0.0f,
        vkpp::Anisotropy::Enable,
        mPhysicalDeviceProperties.limits.maxSamplerAnisotropy,
        vkpp::Compare::Disable,
        vkpp::CompareOp::eNever,
        0.0f,
        static_cast<float>(mTexFire.mipLevels),
        vkpp::BorderColor::eFloatTransparentBlack
    };

    mTextureSampler = mLogicalDevice.CreateSampler(lSamplerCreateInfo);
}


void ParticleFire::UpdateDescriptorSets(void)
{
    // Image descriptor for the particle texture.
    {
        const vkpp::DescriptorImageInfo lTexDescriptorSmoke{ mTextureSampler, mSmokeTexRes.view, mTexSmoke.layout };
        const vkpp::DescriptorImageInfo lTexDescriptorFire{ mTextureSampler, mFireTexRes.view, mTexFire.layout };

        // Buffer descriptor
        const vkpp::DescriptorBufferInfo lVSUbo{ mUboVS.buffer };

        const std::array<vkpp::WriteDescriptorSetInfo, 3> lWriteDescriptorSetInfos
        { {
            // Binding 0: Vertex shader uniform buffer
            {
                mDescriptorSets.particle, 0,
                vkpp::DescriptorType::eUniformBuffer,
                lVSUbo
            },
            // Binding 1: Smoke texture
            {
                mDescriptorSets.particle, 1,
                vkpp::DescriptorType::eCombinedImageSampler,
                lTexDescriptorSmoke
            },
            // Binding 2: Fire texture
            {
                mDescriptorSets.particle, 2,
                vkpp::DescriptorType::eCombinedImageSampler,

                lTexDescriptorFire
            }
        } };

        mLogicalDevice.UpdateDescriptorSets(lWriteDescriptorSetInfos);
    }

    // Image descriptor for the color map texture.
    {
        const vkpp::DescriptorImageInfo lTexDescriptorColorMap{ mTextureSampler, mColorMapTexRes.view, mTexColorMap.layout };
        const vkpp::DescriptorImageInfo lTexDescriptorNormalMap{ mTextureSampler, mNormapMapTexRes.view, mTexNormalMap.layout };
        const vkpp::DescriptorBufferInfo lEnvUbo{ mUboEnv.buffer };

        const std::array<vkpp::WriteDescriptorSetInfo, 3> lWriteDescriptorSetInfos
        { {
            // Binding 0: Vertex shader uniform buffer
            {
                mDescriptorSets.normalmap, 0,
                vkpp::DescriptorType::eUniformBuffer,
                lEnvUbo
            },
            // Binding 1: Smoke texture
            {
                mDescriptorSets.normalmap, 1,
                vkpp::DescriptorType::eCombinedImageSampler,
                lTexDescriptorColorMap
            },
            // Binding 2: Fire texture
            {
                mDescriptorSets.normalmap, 2,
                vkpp::DescriptorType::eCombinedImageSampler,
                lTexDescriptorNormalMap
            }
        } };

        mLogicalDevice.UpdateDescriptorSets(lWriteDescriptorSetInfos);
    }
}


void ParticleFire::LoadModel(const std::string& aFilename)
{
    // Flags for loading the mesh
    constexpr static auto lAssimpFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;
    constexpr static auto lScale = 10.0f;

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

        for (unsigned int lVtxIdx = 0; lVtxIdx < lpMesh->mNumVertices; ++lVtxIdx)
        {
            // Vertex positions
            const auto lPos = lpMesh->mVertices[lVtxIdx];
            lVertexBuffer.emplace_back(lPos.x * lScale);
            lVertexBuffer.emplace_back(-lPos.y * lScale);                // Vulkan use a right handed NDC.
            lVertexBuffer.emplace_back(lPos.z * lScale);

            // Vertex texture coordinates
            const auto lTexCoord = lpMesh->mTextureCoords[0][lVtxIdx];
            lVertexBuffer.emplace_back(lTexCoord.x);
            lVertexBuffer.emplace_back(lTexCoord.y);

            // Vertex normal
            const auto lNormal = lpMesh->mNormals[lVtxIdx];
            lVertexBuffer.emplace_back(lNormal.x);
            lVertexBuffer.emplace_back(lNormal.y);
            lVertexBuffer.emplace_back(lNormal.z);

            // Vertex color
            /*const auto lColor = lpMesh->HasVertexColors(0) ? lpMesh->mColors[0][lVtxIdx] : aiColor4D(1.0f);
            lVertexBuffer.emplace_back(lColor.r);
            lVertexBuffer.emplace_back(lColor.g);
            lVertexBuffer.emplace_back(lColor.b);*/

            // Tangent
            const auto lTangent = lpMesh->HasTangentsAndBitangents() ? lpMesh->mTangents[lVtxIdx] : lZero3D;
            lVertexBuffer.emplace_back(lTangent.x);
            lVertexBuffer.emplace_back(lTangent.y);
            lVertexBuffer.emplace_back(lTangent.z);

            // BiTangent
            const auto lBiTagent = lpMesh->HasTangentsAndBitangents() ? lpMesh->mBitangents[lVtxIdx] : lZero3D;
            lVertexBuffer.emplace_back(lBiTagent.x);
            lVertexBuffer.emplace_back(lBiTagent.y);
            lVertexBuffer.emplace_back(lBiTagent.z);
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


void ParticleFire::CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const
{
    const auto& lCopyCmd = BeginOneTimeCmdBuffer();

    const vkpp::BufferCopy lBufferCopy{ aSize };

    lCopyCmd.Copy(aDstBuffer, aSrcBuffer, lBufferCopy);

    EndOneTimeCmdBuffer(lCopyCmd);
}


// Prepare and initialize uniform buffer containing shader uniforms.
void ParticleFire::CreateUniformBuffers(void)
{
    constexpr auto lMemFlags = vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent;

    vkpp::BufferCreateInfo lBufferCreateInfo{ sizeof(UboEnv), vkpp::BufferUsageFlagBits::eUniformBuffer };
    mUboEnv.Reset(lBufferCreateInfo, lMemFlags);

    lBufferCreateInfo.SetSize(sizeof(UboVS));
    mUboVS.Reset(lBufferCreateInfo, lMemFlags);

    mpUboEnvMapped = mUboEnv.Map();
    mpUboVSMapped = mUboVS.Map();
}


void ParticleFire::UpdateUniformBuffers(void)
{
    auto lWidth = static_cast<float>(mSwapchain.extent.width);
    auto lHeight = static_cast<float>(mSwapchain.extent.height);

    const static glm::vec3 lCameraPos{ 0.f, 15.f, 0.f };

    // Vertex shader
    mVSMatrix.proj = glm::perspective(glm::radians(60.f), lWidth / lHeight, 0.001f, 256.0f);

    const auto& lViewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, mCurrentZoomLevel));
    mVSMatrix.model = lViewMatrix * glm::translate(glm::mat4(), lCameraPos);
    mVSMatrix.model = glm::rotate(mVSMatrix.model, glm::radians(mCurrentRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    mVSMatrix.model = glm::rotate(mVSMatrix.model, glm::radians(mCurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    mVSMatrix.model = glm::rotate(mVSMatrix.model, glm::radians(mCurrentRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    mVSMatrix.viewportDim = { lWidth, lHeight };

    std::memcpy(mpUboVSMapped, &mVSMatrix, sizeof(UboVS));

    // Environment
    mEnvMatrix.proj = mVSMatrix.proj;
    mEnvMatrix.model = mVSMatrix.model;
    mEnvMatrix.normal = glm::inverseTranspose(mEnvMatrix.model);
    mEnvMatrix.cameraPos.z = mCurrentZoomLevel;

    const auto lTimer = std::chrono::system_clock::now();
    mEnvMatrix.lightPos.x = static_cast<float>(std::sin(lTimer.time_since_epoch().count()));
    mEnvMatrix.cameraPos.z = static_cast<float>(std::cos(lTimer.time_since_epoch().count()));

    std::memcpy(mpUboEnvMapped, &mEnvMatrix, sizeof(UboEnv));
}


void ParticleFire::BuildCmdBuffers(void)
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

        const vkpp::Scissor lScissor
        {
            0, 0,
            mSwapchain.extent
        };

        lDrawCmdBuffer.SetScissor(lScissor);

        // Environment
        lDrawCmdBuffer.BindGraphicsPipeline(mPipelines.normalmap);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayout, 0, mDescriptorSets.normalmap);
        lDrawCmdBuffer.BindVertexBuffer(mVtxBufferRes.buffer);
        lDrawCmdBuffer.BindIndexBuffer(mIdxBufferRes.buffer);
        lDrawCmdBuffer.DrawIndexed(mIndexCount);

        // Particle system (no index buffer)
        lDrawCmdBuffer.BindGraphicsPipeline(mPipelines.particle);
        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayout, 0, mDescriptorSets.particle);
        lDrawCmdBuffer.BindVertexBuffer(mParticleBufferRes.buffer);
        lDrawCmdBuffer.Draw(PARTICLE_COUNT);

        lDrawCmdBuffer.EndRenderPass();

        lDrawCmdBuffer.End();
    }
}


void ParticleFire::CreateSemaphores(void)
{
    constexpr vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    mPresentCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mRenderingCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void ParticleFire::CreateFences(void)
{
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    for (std::size_t lIndex = 0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
        mWaitFences.emplace_back(mLogicalDevice.CreateFence(lFenceCreateInfo));
}


void ParticleFire::Update(void)
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


float Rnd(float aRange)
{
    return aRange * (rand() / float(RAND_MAX));
}


void ParticleFire::InitParticle(ParticleVtxData& aParticle, const glm::vec3& aEmitterPos) const
{
    aParticle.vel = { 0.f, mMinVel.y + Rnd(mMaxVel.y - mMinVel.y), 0.f, 0.f };
    aParticle.inAlpha = Rnd(0.75f);
    aParticle.inSize = 1.0f + Rnd(0.5f);
    aParticle.inColor = glm::vec4{ 1.f };
    aParticle.inType = PARTICLE_TYPE_FLAME;
    aParticle.inRotation = Rnd(2.f * float(M_PI));
    aParticle.rotationSpeed = Rnd(2.f) - Rnd(2.f);

    // Get Random sphere point
    auto lTheta = Rnd(2.f * float(M_PI));
    auto lPhi = Rnd(float(M_PI)) - float(M_PI) / 2.f;
    auto lRadius = Rnd(FLAME_RADIUS);

    aParticle.inPos.x = lRadius * std::cos(lTheta) * std::cos(lPhi);
    aParticle.inPos.y = lRadius * std::sin(lPhi);
    aParticle.inPos.z = lRadius * std::sin(lTheta) * std::cos(lPhi);

    aParticle.inPos += glm::vec4(aEmitterPos, 0.f);
}


void ParticleFire::CreateParticles(void)
{
    srand(unsigned(time(nullptr)));

    std::for_each(mParticleBuffer.begin(), mParticleBuffer.end(), [this](auto& aParticle)
    {
        InitParticle(aParticle, mEmitterPos);
        aParticle.inAlpha = 1.0f - float(std::abs(aParticle.inPos.y) / (FLAME_RADIUS * 2.f));
    });

    const vkpp::BufferCreateInfo lBufferCreateInfo
    {
        PARTICLE_BUFFER_SIZE,
        vkpp::BufferUsageFlagBits::eVertexBuffer
    };

    mParticleBufferRes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    mpParticleMapped = mParticleBufferRes.Map();

    std::memcpy(mpParticleMapped, mParticleBuffer.data(), PARTICLE_BUFFER_SIZE);
}


void ParticleFire::TransitionParticle(ParticleVtxData& aParticle) const
{
    switch (aParticle.inType)
    {
    case PARTICLE_TYPE_FLAME:
        // Flame particles have a chane of turning into smoke
        if (Rnd(1.0f) < 0.05f)
        {
            aParticle.inAlpha = 0.f;
            aParticle.inColor = glm::vec4{ 0.25f + Rnd(0.25f) };
            aParticle.inPos.x *= 0.5f;
            aParticle.inPos.z *= 0.5f;
            aParticle.vel = { Rnd(1.f) - Rnd(1.f), mMinVel.y * 2 + Rnd(mMaxVel.y - mMinVel.y), Rnd(1.f) - Rnd(1.f), 0.f };
            aParticle.inSize = 1.f + Rnd(0.5f);
            aParticle.rotationSpeed = Rnd(1.f) - Rnd(1.f);
            aParticle.inType = PARTICLE_TYPE_SMOKE;
        }
        else
        {
            InitParticle(aParticle, mEmitterPos);
        }
        break;

    case  PARTICLE_TYPE_SMOKE:
        // Re-spawn at end of life.
        InitParticle(aParticle, mEmitterPos);
        break;
    }
}


void ParticleFire::UpdateParticles(void)
{
    constexpr auto lParticleTimer = 0.0006f;

    for (auto& lParticle : mParticleBuffer)
    {
        switch (lParticle.inType)
        {
        case PARTICLE_TYPE_FLAME:
            lParticle.inPos.y -= lParticle.vel.y * lParticleTimer * 3.5f;
            lParticle.inAlpha += lParticleTimer * 0.25f;
            lParticle.inSize -= lParticleTimer * 0.5f;
            break;

        case PARTICLE_TYPE_SMOKE:
            lParticle.inPos -= lParticle.vel * lParticleTimer * 1.f;
            lParticle.inAlpha += lParticleTimer * 1.25f;
            lParticle.inSize += lParticleTimer * 0.125f;
            lParticle.inColor -= lParticleTimer * 0.05f;
            break;
        }

        lParticle.inRotation += lParticleTimer * lParticle.rotationSpeed;

        // Transition particle state
        if (lParticle.inAlpha > 2.f)
            TransitionParticle(lParticle);
    }

    std::memcpy(mpParticleMapped, mParticleBuffer.data(), PARTICLE_BUFFER_SIZE);
}


}                   // End of namespace vkpp::sample.

