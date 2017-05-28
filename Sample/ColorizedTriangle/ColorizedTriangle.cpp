#include "ColorizedTriangle/ColorizedTriangle.h"



namespace vkpp::sample
{




ColorizedTriangle::ColorizedTriangle(CWindow& aWindow, const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion)
    : ExampleBase(aWindow, apApplicationName, aApplicationVersion, apEngineName, aEngineVersion), CWindowEvent(aWindow),
      mDepthResources(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mVertexBufferResource(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mIndexBufferResource(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mUniformBufferResource(mLogicalDevice, mPhysicalDeviceMemoryProperties)
{
    mResizedFunc = [this](Sint32 /*aWidth*/, Sint32 /*aHeight*/)
    {
        // Ensure all operations on the device have been finished before destroying resources.
        mLogicalDevice.Wait();

        // Re-create swapchain.
        CreateSwapchain(mSwapchain);

        mDepthResources.Reset();
        CreateDepthResources();

        // Re-create framebuffers.
        for(auto& lFramebuffer : mFramebuffers)
            mLogicalDevice.DestroyFramebuffer(lFramebuffer);

        mFramebuffers.clear();
        CreateFramebuffers();

        // Command buffers need to be recreated as they reference to the destroyed framebuffer.
        mLogicalDevice.FreeCommandBuffers(mCommandPool, mDrawCmdBuffers);
        AllocateDrawCmdBuffers();
        BuildCommandBuffers();
    };

    CreateCommandPool();
    AllocateDrawCmdBuffers();

    CreateDepthResources();
    CreateSemaphores();
    CreateFences();

    CreateRenderPass();
    CreatePipelineCache();

    CreateFramebuffers();

    CreateDescriptorSetLayout();
    CreatePipelineLayout();

    CreateGraphicsPipeline();
    CreateShaderResources();

    CreateDescriptorPool();
    AllocateDescriptorSet();
    UpdateDescriptorSet();

    BuildCommandBuffers();

    theApp.RegisterUpdateEvent([this](void)
    {
        Update();
    });
}


ColorizedTriangle::~ColorizedTriangle(void)
{
    mLogicalDevice.Wait();

    // mLogicalDevice.FreeDescriptorSet(mDescriptorPool, mDescriptorSet);
    mLogicalDevice.DestroyDescriptorPool(mDescriptorPool);

    mLogicalDevice.DestroyPipeline(mGraphicsPipeline);

    mLogicalDevice.DestroyPipelineLayout(mPipelineLayout);
    mLogicalDevice.DestroyDescriptorSetLayout(mSetLayout);

    for (auto& lFramebuffer : mFramebuffers)
        mLogicalDevice.DestroyFramebuffer(lFramebuffer);

    mLogicalDevice.DestroyPipelineCache(mPipelineCache);
    mLogicalDevice.DestroyRenderPass(mRenderPass);

    for (auto& lFence : mWaitFences)
        mLogicalDevice.DestroyFence(lFence);

    mLogicalDevice.DestroySemaphore(mRenderCompleteSemaphore);
    mLogicalDevice.DestroySemaphore(mPresentCompleteSemaphore);

    mLogicalDevice.FreeCommandBuffers(mCommandPool, mDrawCmdBuffers);
    mLogicalDevice.DestroyCommandPool(mCommandPool);
}


void ColorizedTriangle::CreateCommandPool(void)
{
    const vkpp::CommandPoolCreateInfo lCmdCreateInfo
    {
        mGraphicsQueue.familyIndex,
        vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer
    };

    mCommandPool = mLogicalDevice.CreateCommandPool(lCmdCreateInfo);
}


void ColorizedTriangle::AllocateDrawCmdBuffers(void)
{
    const vkpp::CommandBufferAllocateInfo lCmdBufferAllocateInfo
    {
        mCommandPool,
        static_cast<uint32_t>(mSwapchain.buffers.size())
    };

    // Create one command buffer for each swapchain image and reuse for rendering.
    mDrawCmdBuffers = mLogicalDevice.AllocateCommandBuffers(lCmdBufferAllocateInfo);
}


void ColorizedTriangle::CreateDepthResources(void)
{
    const vkpp::ImageCreateInfo lImageCreateInfo
    {
        vkpp::ImageType::e2D,
        vkpp::Format::eD32sFloatS8uInt,
        mSwapchain.extent,
        vkpp::ImageUsageFlagBits::eDepthStencilAttachment
    };

    vkpp::ImageViewCreateInfo lImageViewCreateInfo
    {
        vkpp::ImageViewType::e2D,
        vkpp::Format::eD32sFloatS8uInt,
        {
            vkpp::ImageAspectFlagBits::eDepth | vkpp::ImageAspectFlagBits::eStencil,
            0, 1,
            0, 1
        }
    };

    mDepthResources.Reset(lImageCreateInfo, lImageViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);
}


void ColorizedTriangle::CreateSemaphores(void)
{
    constexpr vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    // Semaphore used to ensure that image presentation is complete before starting to submit again.
    mPresentCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);

    // Semaphore used to ensure that all commands submitted have been finished before submitting the image to the presentation queue.
    mRenderCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void ColorizedTriangle::CreateFences(void)
{
    // Create in signaled state so we don't wait on the first render of each command buffer.
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    for (int lIndex = 0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
        mWaitFences.emplace_back(mLogicalDevice.CreateFence(lFenceCreateInfo));
}


void ColorizedTriangle::CreateRenderPass(void)
{
    // Descriptors used for the attachments used by the renderpass.
    const std::array<vkpp::AttachementDescription, 2> lAttachmentDescriptions
    { {
        {
            mSwapchain.surfaceFormat.format,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eStore,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::ePresentSrcKHR
        },
        {
            vkpp::Format::eD32sFloatS8uInt,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eDontCare,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
        }
    } };

    // Setup attachment references.
    constexpr vkpp::AttachmentReference lColorReference
    {
        0,
        vkpp::ImageLayout::eColorAttachmentOptimal
    };

    constexpr vkpp::AttachmentReference lDepthReference
    {
        1,
        vkpp::ImageLayout::eDepthStencilAttachmentOptimal
    };

    // Setup a single subpass reference.
    const std::array<vkpp::SubpassDescription, 1> lSubpassDescriptions
    { {
        {
            vkpp::PipelineBindPoint::eGraphics,
            0, nullptr,                             // Input Attachments.
            1, lColorReference.AddressOf(),         // Color Attachments.
            nullptr,                                // Resolve Attachments.
            lDepthReference.AddressOf()
        }
    } };

    // Setup subpass dependencies.
    constexpr std::array<vkpp::SubpassDependency, 2> lSubpassDependencies
    { {
        // First dependency at the start of the renderpass does the transition from final to initial layout.
        {
            vkpp::subpass::External, 0,
            vkpp::PipelineStageFlagBits::eBottomOfPipe, vkpp::PipelineStageFlagBits::eColorAttachmentOutput,
            vkpp::AccessFlagBits::eMemoryRead, vkpp::AccessFlagBits::eColorAttachmentWrite,
            vkpp::DependencyFlagBits::eByRegion
        },
        // Sencond dependency at the end of the renderpass does the transition from the initial layout to the final layout.
        {
            0, vkpp::subpass::External,
            vkpp::PipelineStageFlagBits::eColorAttachmentOutput, vkpp::PipelineStageFlagBits::eBottomOfPipe,
            vkpp::AccessFlagBits::eColorAttachmentWrite, vkpp::AccessFlagBits::eMemoryRead,
            vkpp::DependencyFlagBits::eByRegion
        }
    } };

    const vkpp::RenderPassCreateInfo lRenderPassCreateInfo
    {
        lAttachmentDescriptions,
        lSubpassDescriptions,
        lSubpassDependencies
    };

    mRenderPass = mLogicalDevice.CreateRenderPass(lRenderPassCreateInfo);
}


void ColorizedTriangle::CreatePipelineCache(void)
{
    const vkpp::PipelineCacheCreateInfo lPipelineCacheCreateInfo;
    mPipelineCache = mLogicalDevice.CreatePipelineCache(lPipelineCacheCreateInfo);
}


void ColorizedTriangle::CreateFramebuffers(void)
{
    // Create a framebuffer for each swapchain image.
    for(auto& lSwapchainRes : mSwapchain.buffers)
    {
        const std::array<vkpp::ImageView, 2> lAttachments
        {
            lSwapchainRes.view,
            mDepthResources.view
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
}


void ColorizedTriangle::CreateDescriptorSetLayout(void)
{
    // Binding 0: Uniform Buffer (Vertex Shader).
    constexpr vkpp::DescriptorSetLayoutBinding lSetLayoutBinding
    {
        0, vkpp::DescriptorType::eUniformBuffer,
        1, vkpp::ShaderStageFlagBits::eVertex
    };

    const vkpp::DescriptorSetLayoutCreateInfo lSetLayoutCreateInfo
    {
        1, lSetLayoutBinding.AddressOf()
    };

    mSetLayout = mLogicalDevice.CreateDescriptorSetLayout(lSetLayoutCreateInfo);
}


void ColorizedTriangle::CreatePipelineLayout()
{
    const vkpp::PipelineLayoutCreateInfo lPipelineLayoutCreateInfo
    {
        1, mSetLayout.AddressOf()
    };

    mPipelineLayout = mLogicalDevice.CreatePipelineLayout(lPipelineLayoutCreateInfo);
}


void ColorizedTriangle::CreateGraphicsPipeline(void)
{
    const auto& lVertexShaderModule = CreateShaderModule("Shader/vert.spv");
    const auto& lFragmentShaderModule = CreateShaderModule("Shader/frag.spv");

    const std::vector<vkpp::PipelineShaderStageCreateInfo> lShaderStageCreateInfos
    {
        {
            vkpp::ShaderStageFlagBits::eVertex,
            lVertexShaderModule
        },
        {
            vkpp::ShaderStageFlagBits::eFragment,
            lFragmentShaderModule
        }
    };

    constexpr std::array<vkpp::VertexInputBindingDescription, 1> lVertxInputBinding{ VertexData::GetBindingDescription() };
    constexpr auto lVerexAttributeDescriptions = VertexData::GetAttributeDescriptions();

    const vkpp::PipelineVertexInputStateCreateInfo lVertexInputStateCreateInfo
    {
        lVertxInputBinding, lVerexAttributeDescriptions
    };

    constexpr vkpp::PipelineInputAssemblyStateCreateInfo lInputAssemblyStateCreateInfo { vkpp::PrimitiveTopology::eTriangleList };

    constexpr vkpp::PipelineViewportStateCreateInfo lViewportCreateInfo{ 1, 1 };

    constexpr vkpp::PipelineRasterizationStateCreateInfo lRasterizationStateCreateInfo
    {
        DepthClamp::Disable, RasterizerDiscard::Disable,
        vkpp::PolygonMode::eFill,
        vkpp::CullModeFlagBits::eBack,
        vkpp::FrontFace::eClockwise,
        DepthBias::Disable,
        0.0f, 0.0f, 0.0f,
        1.0f
    };

    constexpr vkpp::PipelineMultisampleStateCreateInfo lMultisampleStateCreateInfo;

    constexpr vkpp::PipelineDepthStencilStateCreateInfo lDepthStencilCreateInfo
    {
        DepthTest::Enable, DepthWrite::Enable,
        vkpp::CompareOp::eLess
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
        2, lShaderStageCreateInfos.data(),           // Pipeline Shader Stages.
        lVertexInputStateCreateInfo.AddressOf(),    // Vertex Input State.
        lInputAssemblyStateCreateInfo.AddressOf(),  // Input Assembly State.
        nullptr,                                    // Tessellation State.
        lViewportCreateInfo.AddressOf(),            // Viewport State.
        lRasterizationStateCreateInfo.AddressOf(),  // Rasterization State.
        lMultisampleStateCreateInfo.AddressOf(),    // Multisample State.
        lDepthStencilCreateInfo.AddressOf(),        // Depth Stencil State.
        lColorBlendStateCreateInfo.AddressOf(),     // Color Blend State.
        lDynamicStateCreateInfo.AddressOf(),        // Dynamic State.
        mPipelineLayout,                            // Pipeline Layout.
        mRenderPass,                                // Render Pass.
        0
    };

    mGraphicsPipeline = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);
}


void ColorizedTriangle::CreateShaderResources(void)
{
    CreateVertexBuffer();
    CreateIndexBuffer();

    CreateUniformBuffers();
    UpdateUniformBuffer();
}


void ColorizedTriangle::CreateDescriptorPool(void)
{
    constexpr vkpp::DescriptorPoolSize lPoolSize
    {
        vkpp::DescriptorType::eUniformBuffer,
        1                                           // Number of descriptors to allocate.
    };

    const vkpp::DescriptorPoolCreateInfo lDescriptorPoolCreateInfo
    {
        1, lPoolSize.AddressOf(),
        1                                           // Set the maximum number of descriptor sets that can be requested from this pool.
                                                    // Request beyond this limit will result in an error.
    };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool(lDescriptorPoolCreateInfo);
}


void ColorizedTriangle::AllocateDescriptorSet(void)
{
    const vkpp::DescriptorSetAllocateInfo lSetAllocateInfo
    {
        mDescriptorPool,
        1, mSetLayout.AddressOf()
    };

    mDescriptorSet = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);
}


void ColorizedTriangle::UpdateDescriptorSet(void) const
{
    // Update the descriptor set determining the shader binding points.
    // For every binding point used in a shader there needs to be one descriptor set matching that binding point.

    // Binding 0: uniform buffer.
    const vkpp::DescriptorBufferInfo lDescriptorBufferInfo
    {
        mUniformBufferResource.buffer,
        0,
        sizeof(UniformBufferObject)
    };

    const vkpp::WriteDescriptorSetInfo lWriteDescriptorSetInfo
    {
        mDescriptorSet,
        0,                          // Binding 0: uniform buffer.
                                    // Binds the uniform buffer to binding point 0.
        0,
        vkpp::DescriptorType::eUniformBuffer,
        lDescriptorBufferInfo
    };

    mLogicalDevice.UpdateDescriptorSet(lWriteDescriptorSetInfo);
}


// Build separate command buffer for every framebuffer image.
// All rendering commands are recorded once into command buffers that are then re-submitted to the queue.
// This allows to generate work upfront and from multiple threads.
void ColorizedTriangle::BuildCommandBuffers(void)
{
    constexpr vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo;

    // Set clear values for all framebuffer attachments with loadOp set to clear.
    // We use two attachments (color and depth) that are cleared at the start of the subpass
    // and as such we need to set clear values for both.
    constexpr std::array<vkpp::ClearValue, 2> lClearValues
    { {
        { 0.129411f, 0.156862f, 0.188235f, 1.0f },
        { 1.0f, 0.0f }
    } };

    for (std::size_t lIndex = 0; lIndex < mDrawCmdBuffers.size(); ++lIndex)
    {
        vkpp::RenderPassBeginInfo lRenderPassBeginInfo
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
        lDrawCmdBuffer.BindVertexBuffer(mVertexBufferResource.buffer, 0);
        lDrawCmdBuffer.BindIndexBuffer(mIndexBufferResource.buffer, 0, vkpp::IndexType::eUInt32);

        const vkpp::Viewport lViewport
        {
            0.0f, 0.0f,
            static_cast<float>(mSwapchain.extent.width), static_cast<float>(mSwapchain.extent.height),
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

        lDrawCmdBuffer.DrawIndexed(3);

        lDrawCmdBuffer.EndRenderPass();

        lDrawCmdBuffer.End();
    }
}


void ColorizedTriangle::CreateVertexBuffer(void)
{
    // Setup Vertices.
    const std::vector<VertexData> lVertexData
    {
        {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };

    const auto lVertexDataSize = static_cast<vkpp::DeviceSize>(sizeof(VertexData) * lVertexData.size());

    vkpp::BufferCreateInfo lBufferCreateInfo
    {
        lVertexDataSize,
        vkpp::BufferUsageFlagBits::eVertexBuffer | vkpp::BufferUsageFlagBits::eTransferDst
    };

    mVertexBufferResource.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    BufferResource lStagingResource{ mLogicalDevice, mPhysicalDeviceMemoryProperties };
    lBufferCreateInfo.SetUsage(vkpp::BufferUsageFlagBits::eTransferSrc);
    lStagingResource.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lMappedMemory = mLogicalDevice.MapMemory(lStagingResource.memory, 0, lVertexDataSize);
    std::memcpy(lMappedMemory, lVertexData.data(), lVertexDataSize);
    mLogicalDevice.UnmapMemory(lStagingResource.memory);

    CopyBuffer(lVertexDataSize, lStagingResource.buffer, mVertexBufferResource.buffer);
}


void ColorizedTriangle::CreateIndexBuffer(void)
{
    // Setup Indices.
    const std::vector<uint32_t> lIndexData{ 0, 1, 2 };
    const auto lIndexDataSize = static_cast<uint32_t>(sizeof(uint32_t) * lIndexData.size());

    vkpp::BufferCreateInfo lBufferCreateInfo
    {
        lIndexDataSize,
         vkpp::BufferUsageFlagBits::eIndexBuffer | vkpp::BufferUsageFlagBits::eTransferDst
    };

    mIndexBufferResource.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    BufferResource lStagingResource{ mLogicalDevice, mPhysicalDeviceMemoryProperties };
    lBufferCreateInfo.SetUsage(vkpp::BufferUsageFlagBits::eTransferSrc);
    lStagingResource.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);

    auto lMappedMemory = mLogicalDevice.MapMemory(lStagingResource.memory, 0, lIndexDataSize);
    std::memcpy(lMappedMemory, lIndexData.data(), lIndexDataSize);
    mLogicalDevice.UnmapMemory(lStagingResource.memory);

    CopyBuffer(lIndexDataSize, lStagingResource.buffer, mIndexBufferResource.buffer);
}


void ColorizedTriangle::CreateUniformBuffers(void)
{
    // Prepare and initialize an uniform buffer block containing shader uniforms.
    vkpp::BufferCreateInfo lBufferCreateInfo
    {
        sizeof(UniformBufferObject),
        vkpp::BufferUsageFlagBits::eUniformBuffer
    };

    mUniformBufferResource.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);
}


void ColorizedTriangle::CopyBuffer(DeviceSize aSize, const vkpp::Buffer& aSrcBuffer, vkpp::Buffer& aDstBuffer) const
{
    // Buffer copy operations have to be submitted to a queue, so we need a command buffer for them.
    // NOTE: Some devices offer a dedicated transfer queue (with only the transfer bit set) that may be faster when doing lots of copies.
    const auto& lCmdBuffer = BeginOneTimeCommandBuffer();

    const vkpp::BufferCopy lBufferCopy
    {
        0,      // Source Offset.
        0,      // Dst Offset.
        aSize
    };

    lCmdBuffer.Copy(aDstBuffer, aSrcBuffer, lBufferCopy);

    EndOneTimeCommandBuffer(lCmdBuffer);
}


void ColorizedTriangle::UpdateUniformBuffer(void) const
{
    auto lMappedMemory = mLogicalDevice.MapMemory(mUniformBufferResource.memory, 0, sizeof(UniformBufferObject));
    std::memcpy(lMappedMemory, &mMVPMatrix, sizeof(UniformBufferObject));

    // Note: Since we requested a host coherent memory for the uniform buffer, the write is instantly visible to the GPU.
    mLogicalDevice.UnmapMemory(mUniformBufferResource.memory);
}


vkpp::CommandBuffer ColorizedTriangle::BeginOneTimeCommandBuffer(void) const
{
    const vkpp::CommandBufferAllocateInfo lCmdBufferAllocateInfo
    {
        mCommandPool,
        1                   // Command buffer count.
    };

    auto lCmdBuffer = mLogicalDevice.AllocateCommandBuffer(lCmdBufferAllocateInfo);

    const vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo
    {
        vkpp::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    lCmdBuffer.Begin(lCmdBufferBeginInfo);

    return lCmdBuffer;
}


void ColorizedTriangle::EndOneTimeCommandBuffer(const vkpp::CommandBuffer& aCmdBuffer) const
{
    aCmdBuffer.End();

    const vkpp::SubmitInfo lSubmitInfo
    {
        aCmdBuffer
    };

    // Create fence to ensure that the command buffer has finished executing.
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo;
    const auto& lFence = mLogicalDevice.CreateFence(lFenceCreateInfo);

    mGraphicsQueue.handle.Submit(lSubmitInfo, lFence);

    // Wait for the fence to signal that command buffer has finished executing.
    mLogicalDevice.WaitForFence(lFence);

    mLogicalDevice.DestroyFence(lFence);
    mLogicalDevice.FreeCommandBuffer(mCommandPool, aCmdBuffer);
}


void ColorizedTriangle::Update(void)
{
    auto lIndex = mLogicalDevice.AcquireNextImage(mSwapchain.handle, mPresentCompleteSemaphore);

    // Use fence to wait until the command buffer has finished execution before using it again.
    mLogicalDevice.WaitForFence(mWaitFences[lIndex]);
    mLogicalDevice.ResetFence(mWaitFences[lIndex]);

    // Pipeline stage at which the queue submission will wait (via pWaitSemaphores).
    const vkpp::PipelineStageFlags lWaitDstStageMask{ vkpp::PipelineStageFlagBits::eColorAttachmentOutput };

    // The submit info structure specifies a command buffer queue submission batch.
    const vkpp::SubmitInfo lSubmitInfo
    {
        1, mPresentCompleteSemaphore.AddressOf(),       // Wait Semaphores.
        &lWaitDstStageMask,
        1, mDrawCmdBuffers[lIndex].AddressOf(),
        1, mRenderCompleteSemaphore.AddressOf()         // Signal Semaphores
    };

    // Submit the graphics queue passing a wait fence.
    mPresentQueue.handle.Submit(lSubmitInfo, mWaitFences[lIndex]);

    // Present the current color attachment to the swapchain.
    // Pass the semaphore signaled by the draw command buffer submission from the submit info as the wait semaphore for swapchain presentation.
    // This ensures that the image is not presented to the windowing system until all commands have been submitted.
    const vkpp::khr::PresentInfo lPresentInfo
    {
        1, mRenderCompleteSemaphore.AddressOf(),
        1, mSwapchain.handle.AddressOf(), &lIndex
    };

    mPresentQueue.handle.Present(lPresentInfo);
}



}                   // End of namespace vkpp::sample.