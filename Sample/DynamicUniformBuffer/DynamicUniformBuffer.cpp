#include "DynamicUniformBuffer.h"

#include <random>
#include <cmath>



namespace vkpp::sample
{



// Wrapper functions for aligned memory allocation.
// C++17 added a standard cross-platform function, namely std::aligned_alloc, to do this, but it is not supported in VS2017.
void* AlignedAlloc(std::size_t aAlignment, std::size_t aSize)
{
#ifdef _WIN32
    return _aligned_malloc(aSize, aAlignment);
#else
    static_assert(false);
#endif
}


void AlignedFree(void* apData)
{
#ifdef _WIN32
    _aligned_free(apData);
#else
    static_assert(false);
#endif
}



DynamicUniformBuffer::DynamicUniformBuffer(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName, uint32_t aEngineVersion)
    : ExampleBase(aWindow, apAppName, aAppVersion, apEngineName, aEngineVersion),
      CWindowEvent(aWindow), CMouseMotionEvent(aWindow), CMouseWheelEvent(aWindow),
      mDepthRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mVtxBufferRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mIdxBufferRes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mUBORes(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mDynamicRes(mLogicalDevice, mPhysicalDeviceMemoryProperties)
{
    theApp.RegisterUpdateEvent([this](void)
    {
        UpdateDynamicUniformBuffer();
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

    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateUniformBuffer();
    UpdateUniformBuffer();
    UpdateDynamicUniformBuffer();

    UpdateDescriptorSets();

    BuildCmdBuffers();

    CreateSemaphores();
    CreateFences();
}


DynamicUniformBuffer::~DynamicUniformBuffer(void)
{
    mLogicalDevice.Wait();

    AlignedFree(mpUBODataDynamic);

    mLogicalDevice.DestroySemaphore(mRenderingCompleteSemaphore);
    mLogicalDevice.DestroySemaphore(mPresentCompleteSemaphore);

    mLogicalDevice.DestroyFences(mWaitFences);

    mLogicalDevice.UnmapMemory(mUBORes.memory);
    mLogicalDevice.UnmapMemory(mDynamicRes.memory);

    mDynamicRes.Reset();
    mUBORes.Reset();

    mIdxBufferRes.Reset();
    mVtxBufferRes.Reset();

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


void DynamicUniformBuffer::CreateCmdPool(void)
{
    const vkpp::CommandPoolCreateInfo lCmdCreateInfo
    {
        mGraphicsQueue.familyIndex,
        vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer
    };

    mCmdPool = mLogicalDevice.CreateCommandPool(lCmdCreateInfo);
}


void DynamicUniformBuffer::AllocateCmdBuffers(void)
{
    const vkpp::CommandBufferAllocateInfo lCmdAllocateInfo
    {
        mCmdPool,
        static_cast<uint32_t>(mSwapchain.buffers.size())            // Create one command buffer for each swap-chain image and reuse for rendering.
    };

    mDrawCmdBuffers = mLogicalDevice.AllocateCommandBuffers(lCmdAllocateInfo);
}


void DynamicUniformBuffer::CreateRenderPass(void)
{
    const std::vector<vkpp::AttachementDescription> lAttachments
    {
        // Depth attachment
        {
            vkpp::Format::eD32sFloat,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eDontCare,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
        },
        // Color attachment
        {
            mSwapchain.surfaceFormat.format,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eStore,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::ePresentSrcKHR
        }
    };

    // Color attachment reference
    constexpr vkpp::AttachmentReference lColorRef
    {
        1, vkpp::ImageLayout::eColorAttachmentOptimal
    };

    // Depth attachment reference
    constexpr vkpp::AttachmentReference lDepthRef
    {
        0, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
    };

    const std::vector<vkpp::SubpassDescription> lSubpassDescription
    {
        {
            vkpp::PipelineBindPoint::eGraphics,
            1, lColorRef.AddressOf(),
            lDepthRef.AddressOf()
         }
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
        lAttachments, lSubpassDescription, lSubpassDependencies 
    };

    mRenderPass = mLogicalDevice.CreateRenderPass(lRenderPassCreateInfo);
}


void DynamicUniformBuffer::CreateDepthResource(void)
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


void DynamicUniformBuffer::CreateFramebuffers(void)
{
    for (auto& lSwapchainRes : mSwapchain.buffers)
    {
        const std::array<vkpp::ImageView, 2> lAttachments
        {
            mDepthRes.view,
            lSwapchainRes.view,
        };

        const vkpp::FramebufferCreateInfo lFramebufferCreateInfo
        {
            mRenderPass,
            lAttachments,
            mSwapchain.extent.width,
            mSwapchain.extent.height
        };

        mFramebuffers.emplace_back(mLogicalDevice.CreateFramebuffer(lFramebufferCreateInfo));
    }

    //// Depth/stencil attachment is the same for all frame buffers.
    //std::array<vkpp::ImageView, 2> lAttachments{ mDepthRes.view };

    //const vkpp::FramebufferCreateInfo lFramebufferCreateInfo
    //{
    //    mRenderPass,
    //    lAttachments,
    //    mSwapchain.extent
    //};

    //// Create frame buffers for every swap chain image.
    //for (auto& lSwapchain : mSwapchain.buffers)
    //{
    //    lAttachments[1] = lSwapchain.view;

    //    mFramebuffers.emplace_back(mLogicalDevice.CreateFramebuffer(lFramebufferCreateInfo));
    //}
}


void DynamicUniformBuffer::CreateSetLayout(void)
{
    const std::vector<vkpp::DescriptorSetLayoutBinding> lSetLayoutBindings
    {
        {
            0,
            vkpp::DescriptorType::eUniformBuffer,
            vkpp::ShaderStageFlagBits::eVertex
        },
        {
            1,
            vkpp::DescriptorType::eUniformBufferDynamic,
            vkpp::ShaderStageFlagBits::eVertex
        }
    };

    const vkpp::DescriptorSetLayoutCreateInfo lSetLayoutCreateInfo{ lSetLayoutBindings };

    mSetLayout = mLogicalDevice.CreateDescriptorSetLayout(lSetLayoutCreateInfo);
}


void DynamicUniformBuffer::CreatePipelineLayout(void)
{
    mPipelineLayout = mLogicalDevice.CreatePipelineLayout({ 1, mSetLayout.AddressOf() });
}


void DynamicUniformBuffer::CreateGraphicsPipeline(void)
{
    const auto& lVertexShaderModule = CreateShaderModule("Shader/SPV/base.vert.spv");
    const auto& lFragmentShaderModule = CreateShaderModule("Shader/SPV/base.frag.spv");

    const std::vector<vkpp::PipelineShaderStageCreateInfo> lShaderStageCreateInfos
    {
        { vkpp::ShaderStageFlagBits::eVertex, lVertexShaderModule },
        { vkpp::ShaderStageFlagBits::eFragment, lFragmentShaderModule }
    };

    constexpr auto lVertexInputBinding = VertexData::GetBindingDescription();
    constexpr auto lVertexAttributes = VertexData::GetAttributeDescriptions();
    const vkpp::PipelineVertexInputStateCreateInfo lVertexInputStateCreateInfo
    {
        lVertexAttributes,
        1, lVertexInputBinding.AddressOf()
    };

    constexpr vkpp::PipelineInputAssemblyStateCreateInfo lAssemblyStateCreateInfo{ vkpp::PrimitiveTopology::eTriangleList };

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
        vkpp::CompareOp::eLess,
    };

    constexpr vkpp::PipelineColorBlendAttachmentState lColorBlendAttachmentState;
    constexpr vkpp::PipelineColorBlendStateCreateInfo lColorBlendStateCreateInfo
    {
        1, lColorBlendAttachmentState.AddressOf()
    };

    constexpr std::array<vkpp::DynamicState, 2> lDynamicStates
    {
        vkpp::DynamicState::eViewport, vkpp::DynamicState::eScissor
    };

    const vkpp::PipelineDynamicStateCreateInfo lDynamicStateCreateInfo{ lDynamicStates };

    const vkpp::GraphicsPipelineCreateInfo lGraphicsPipelineCreateInfo
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
        mPipelineLayout,
        mRenderPass,
        0
    };

    mGraphicsPipeline = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);
}


void DynamicUniformBuffer::CreateDescriptorPool(void)
{
    constexpr std::array<vkpp::DescriptorPoolSize, 2> lDescriptorPoolSizes
    { {
        { vkpp::DescriptorType::eUniformBuffer, 1},
        { vkpp::DescriptorType::eUniformBufferDynamic, 1}
    } };

    const vkpp::DescriptorPoolCreateInfo lDescriptorPoolCreateInfo
    {
        lDescriptorPoolSizes, 1
    };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool(lDescriptorPoolCreateInfo);
}


void DynamicUniformBuffer::AllocateDescriptorSet(void)
{
    const vkpp::DescriptorSetAllocateInfo lSetAllocateInfo
    {
        mDescriptorPool,
        1, mSetLayout.AddressOf()
    };

    mDescriptorSet = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);
}


void DynamicUniformBuffer::CreateVertexBuffer(void)
{
    // Setup vertices for a colored cube.
    const std::vector<VertexData> lVertices
    {
        { { -1.0f, -1.0f,  1.0f },{ 1.0f, 0.0f, 0.0f } },
        { { 1.0f, -1.0f,  1.0f },{ 0.0f, 1.0f, 0.0f } },
        { { 1.0f,  1.0f,  1.0f },{ 0.0f, 0.0f, 1.0f } },
        { { -1.0f,  1.0f,  1.0f },{ 0.0f, 0.0f, 0.0f } },
        { { -1.0f, -1.0f, -1.0f },{ 1.0f, 0.0f, 0.0f } },
        { { 1.0f, -1.0f, -1.0f },{ 0.0f, 1.0f, 0.0f } },
        { { 1.0f,  1.0f, -1.0f },{ 0.0f, 0.0f, 1.0f } },
        { { -1.0f,  1.0f, -1.0f },{ 0.0f, 0.0f, 0.0f } }
    };

    const auto lVtxDataSize = static_cast<vkpp::DeviceSize>(sizeof(VertexData) * lVertices.size());

    const vkpp::BufferCreateInfo lBufferCreateInfo
    {
        lVtxDataSize, vkpp::BufferUsageFlagBits::eVertexBuffer
    };

    mVtxBufferRes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lpMappedMem = mLogicalDevice.MapMemory(mVtxBufferRes.memory, 0, lVtxDataSize);
    std::memcpy(lpMappedMem, lVertices.data(), lVtxDataSize);
    mLogicalDevice.UnmapMemory(mVtxBufferRes.memory);
}


void DynamicUniformBuffer::CreateIndexBuffer(void)
{
    // Setup indices for a colored cube.
    const std::vector<uint32_t> lIndices
    {
        0,1,2, 2,3,0, 1,5,6, 6,2,1, 7,6,5, 5,4,7, 4,0,3, 3,7,4, 4,5,1, 1,0,4, 3,2,6, 6,7,3,
    };

    mIndexCount = static_cast<uint32_t>(lIndices.size());
    const auto lIdxDataSize = static_cast<uint32_t>(sizeof(uint32_t) * lIndices.size());

    const vkpp::BufferCreateInfo lBufferCreateInfo
    {
        lIdxDataSize, vkpp::BufferUsageFlagBits::eIndexBuffer
    };

    mIdxBufferRes.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lpMappedMem = mLogicalDevice.MapMemory(mIdxBufferRes.memory, 0, lIdxDataSize);
    std::memcpy(lpMappedMem, lIndices.data(), lIdxDataSize);
    mLogicalDevice.UnmapMemory(mIdxBufferRes.memory);
}


void DynamicUniformBuffer::CreateUniformBuffer(void)
{
    // Allocate data for the dynamic uniform buffer object.
    // We allocate this manually as the alignment of the offset differs between GPUs.

    // Calculate required alignment depending on device limits.
    const auto lUBOAlignment = mPhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
    mDynamicAlignment = sizeof(glm::mat4) / lUBOAlignment * lUBOAlignment + sizeof(glm::mat4) % lUBOAlignment > 0 ? lUBOAlignment : 0;

    mDynamicBufferSize = OBJECT_INSTANCES * mDynamicAlignment;
    mpUBODataDynamic = static_cast<glm::mat4*>(AlignedAlloc(mDynamicAlignment, mDynamicBufferSize));
    assert(mpUBODataDynamic != nullptr);

    // Vertex shader uniform buffer lock.

    // Static shared uniform buffer object with projection and view matrix.
    constexpr vkpp::BufferCreateInfo lStaticBufferCreateInfo
    {
        sizeof(UniformBufferObject), vkpp::BufferUsageFlagBits::eUniformBuffer
    };

    mUBORes.Reset(lStaticBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    // Uniform buffer object with per-object matrices.
    const vkpp::BufferCreateInfo lDynamicBufferCreateInfo
    {
        static_cast<uint32_t>(mDynamicBufferSize), vkpp::BufferUsageFlagBits::eUniformBuffer
    };

    mDynamicRes.Reset(lDynamicBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible);

    mpMatrixUBOMapped = mLogicalDevice.MapMemory(mUBORes.memory, 0, sizeof(UniformBufferObject));
    mpDynamicUBOMapped = mLogicalDevice.MapMemory(mDynamicRes.memory, 0, mDynamicBufferSize);

    // Prepare per-object matrices with offsets and random rotations.
    std::random_device lRndDev;
    std::mt19937 lRndGen(static_cast<uint32_t>(lRndDev()));
    std::normal_distribution<float> lRndDist{ -1.0f, 1.0f };

    for (uint32_t lIdx = 0; lIdx < OBJECT_INSTANCES; ++lIdx)
    {
        mRotations[lIdx] = glm::vec3(lRndDist(lRndGen), lRndDist(lRndGen), lRndDist(lRndGen));//  *2.0f * static_cast<float>(M_PI);
        mRotationSpeeds[lIdx] = glm::vec3(lRndDist(lRndGen), lRndDist(lRndGen), lRndDist(lRndGen));
    }
}


void DynamicUniformBuffer::CreateSemaphores(void)
{
    constexpr vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    mPresentCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mRenderingCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void DynamicUniformBuffer::CreateFences(void)
{
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    for (std::size_t lIndex = 0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
        mWaitFences.emplace_back(mLogicalDevice.CreateFence(lFenceCreateInfo));
}


void DynamicUniformBuffer::UpdateUniformBuffer(void)
{
    const auto lWidth = static_cast<float>(mSwapchain.extent.width);
    const auto lHeight = static_cast<float>(mSwapchain.extent.height);

    // Projection
    mUniformBufferObject.proj = glm::perspective(glm::radians(60.0f), lWidth / lHeight, 0.001f, 256.0f);

    // View
    mUniformBufferObject.view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, mCurrentZoomLevel));
    mUniformBufferObject.view = glm::rotate(mUniformBufferObject.view, glm::radians(mCurrentRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    mUniformBufferObject.view = glm::rotate(mUniformBufferObject.view, glm::radians(mCurrentRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    mUniformBufferObject.view = glm::rotate(mUniformBufferObject.view, glm::radians(mCurrentRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Only update the matrices part of the uniform buffer.
    std::memcpy(mpMatrixUBOMapped, &mUniformBufferObject, sizeof(mUniformBufferObject));
}


void DynamicUniformBuffer::UpdateDynamicUniformBuffer(void)
{
    // Dynamic ubo with per-object model matrices indexed by offsets in command buffer.
    auto lDim = static_cast<uint32_t>(pow(OBJECT_INSTANCES, (1.0f / 3.0f)));
    glm::vec3 lOffset(5.0f);

    static auto lAnimationTimer{ 1.0f };
    lAnimationTimer += 1.0f / 60.0f;

    for (uint32_t x = 0; x < lDim; x++)
    {
        for (uint32_t y = 0; y < lDim; y++)
        {
            for (uint32_t z = 0; z < lDim; z++)
            {
                uint32_t lIndex = x * lDim * lDim + y * lDim + z;

                // Aligned offset
                glm::mat4* lModelMat = (glm::mat4*)(((uint64_t)mpUBODataDynamic + (lIndex * mDynamicAlignment)));

                // Update rotations
                mRotations[lIndex] += lAnimationTimer * mRotationSpeeds[lIndex];

                // Update matrices
                glm::vec3 pos = glm::vec3(-((lDim * lOffset.x) / 2.0f) + lOffset.x / 2.0f + x * lOffset.x, -((lDim * lOffset.y) / 2.0f) + lOffset.y / 2.0f + y * lOffset.y, -((lDim * lOffset.z) / 2.0f) + lOffset.z / 2.0f + z * lOffset.z);
                *lModelMat = glm::translate(glm::mat4(), pos);
                *lModelMat = glm::rotate(*lModelMat, mRotations[lIndex].x, glm::vec3(1.0f, 1.0f, 0.0f));
                *lModelMat = glm::rotate(*lModelMat, mRotations[lIndex].y, glm::vec3(0.0f, 1.0f, 0.0f));
                *lModelMat = glm::rotate(*lModelMat, mRotations[lIndex].z, glm::vec3(0.0f, 0.0f, 1.0f));
            }
        }
    }

    lAnimationTimer = 0.0f;

    std::memcpy(mpDynamicUBOMapped, mpUBODataDynamic, mDynamicBufferSize);

    // Flush to make changes visible to the device.
    mLogicalDevice.FlushMappedMemoryRange({ mDynamicRes.memory, 0, mDynamicBufferSize });
}


void DynamicUniformBuffer::BuildCmdBuffers(void)
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
        lDrawCmdBuffer.BindVertexBuffer(mVtxBufferRes.buffer, 0);
        lDrawCmdBuffer.BindIndexBuffer(mIdxBufferRes.buffer, 0, vkpp::IndexType::eUInt32);

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

        // Render multiple objects using different mode matrices into the ubo containing all mode matrices.
        for (uint32_t lIdx = 0; lIdx < OBJECT_INSTANCES; ++lIdx)
        {
            // One dynamic offset per dynamic descriptor to offset into the ubo containing all model matrices.
            auto lDynamicOffset = lIdx * static_cast<uint32_t>(mDynamicAlignment);

            // Bind the descriptor set for rendering a mesh using the dynamic offset.
            lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayout, 0, mDescriptorSet, lDynamicOffset);

            lDrawCmdBuffer.DrawIndexed(mIndexCount);
        }

        lDrawCmdBuffer.EndRenderPass();

        lDrawCmdBuffer.End();
    }
}


void DynamicUniformBuffer::Update()
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


void DynamicUniformBuffer::UpdateDescriptorSets(void) const
{
    const vkpp::DescriptorBufferInfo lStaticUniformDescriptor
    {
        mUBORes.buffer, 0
    };

    const vkpp::DescriptorBufferInfo lDynamicUniformDescriptor
    {
        mDynamicRes.buffer, 0
    };

    const std::vector<vkpp::WriteDescriptorSetInfo> lWriteDescriptorSets
    {
        // Binding 0: Project/View matrix uniform buffer.
        {
            mDescriptorSet,
            0,
            vkpp::DescriptorType::eUniformBuffer,
            lStaticUniformDescriptor
        },
        // Binding 1: Instance matrix as dynamic uniform buffer.
        {
            mDescriptorSet,
            1,
            vkpp::DescriptorType::eUniformBufferDynamic,
            lDynamicUniformDescriptor
        }
    };

    mLogicalDevice.UpdateDescriptorSets(lWriteDescriptorSets);
}


}                   // End of namespace vkpp::sample.