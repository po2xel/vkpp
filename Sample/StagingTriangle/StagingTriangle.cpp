#include "StagingTriangle.h"



namespace sample
{



const VertexData gVertexData[]
{
    { { 0.0f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
    { { 0.5f, 0.5f },{ 0.0f, 1.0f, 0.0f } },
    { { -0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } }
};



StagingTriangle::StagingTriangle(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion)
    : Application(apApplicationName, aApplicationVersion, apEngineName, aEngineVersion), mRenderingResources(DefaultResourceSize)
{
    CreateCommandPool();
    CreateRenderingResources();
    CreateRenderPass();
    CreatePipeline();

    CreateVertexBuffer();
}


StagingTriangle::~StagingTriangle(void)
{
    mLogicalDevice.Wait();

    mLogicalDevice.FreeMemory(mVertexBufferMemory);
    mLogicalDevice.DestroyBuffer(mVertexBuffer);

    mLogicalDevice.DestroyPipeline(mGraphicsPipeline);

    mLogicalDevice.DestroyRenderPass(mRenderPass);

    for (auto& lRenderingResource : mRenderingResources)
    {
        mLogicalDevice.DestroyFence(lRenderingResource.mFence);
        mLogicalDevice.DestroySemaphore(lRenderingResource.mFinishedRenderingSemaphore);
        mLogicalDevice.DestroySemaphore(lRenderingResource.mImageAvailSemaphore);
        mLogicalDevice.FreeCommandBuffer(mCommandPool, lRenderingResource.mCommandBuffer);
        mLogicalDevice.DestroyFramebuffer(lRenderingResource.mFramebuffer);
    }

    mLogicalDevice.DestroyCommandPool(mCommandPool);
}


void StagingTriangle::CreateCommandPool(void)
{
    const vkpp::CommandPoolCreateInfo lCommandPoolCreateInfo
    {
        mGraphicsQueue.mFamilyIndex, vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer
    };

    mCommandPool = mLogicalDevice.CreateCommandPool(lCommandPoolCreateInfo);
}


void StagingTriangle::CreateRenderingResources(void)
{
    for(auto& lRenderingResource : mRenderingResources)
    {
        AllocateCommandBuffers(lRenderingResource.mCommandBuffer);
        CreateSemaphore(lRenderingResource.mImageAvailSemaphore); 
        CreateSemaphore(lRenderingResource.mFinishedRenderingSemaphore);
        CreateFence(lRenderingResource.mFence);
    }
}


void StagingTriangle::AllocateCommandBuffers(vkpp::CommandBuffer& aCommandBuffer) const
{
    const vkpp::CommandBufferAllocateInfo lCommandBufferAllocateInfo
    {
        mCommandPool, 1
    };

    aCommandBuffer = mLogicalDevice.AllocateCommandBuffer(lCommandBufferAllocateInfo);
}


void StagingTriangle::CreateSemaphore(vkpp::Semaphore& aSemaphore) const
{
    vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    aSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void StagingTriangle::CreateFence(vkpp::Fence& aFence) const
{
    vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    aFence = mLogicalDevice.CreateFence(lFenceCreateInfo);
}


void StagingTriangle::CreateRenderPass(void)
{
    const vkpp::AttachementDescription lAttachementDescriptions[]
    {
        {
            mSwapchain.mSurfaceFormat.format, vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eStore,
            vkpp::AttachmentLoadOp::eDontCare, vkpp::AttachmentStoreOp::eDontCare,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::ePresentSrcKHR
        }
    };

    const vkpp::AttachmentReference lColorAttachments[]
    {
        {0, vkpp::ImageLayout::eColorAttachmentOptimal}
    };

    const vkpp::SubpassDescription lSubpassDescriptions[]
    {
        {
            vkpp::PipelineBindPoint::eGraphics,
            0, nullptr,
            1, lColorAttachments
        }
    };

    const vkpp::SubpassDependency lSubpassDependencies[]
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
        1, lAttachementDescriptions,
        1, lSubpassDescriptions,
        2, lSubpassDependencies
    };

    mRenderPass = mLogicalDevice.CreateRenderPass(lRenderPassCreateInfo);
}


void StagingTriangle::CreatePipeline(void)
{
    const auto& lVertexShaderModule = CreateShaderModule("../Shader/vert.spv");
    const auto& lFragmentShaderModule = CreateShaderModule("../Shader/frag.spv");

    const vkpp::PipelineShaderStageCreateInfo lPipelineShaderStageCreateInfos[]
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

    const vkpp::VertexInputBindingDescription lVertexInputBindingDescriptions[]
    {
        {
            0, sizeof(VertexData), vkpp::VertexInputRate::eVertex
        }
    };

    const vkpp::VertexInputAttributeDescription lVertexInputAttributeDescriptions[]
    {
        {
            0, lVertexInputBindingDescriptions[0].binding, vkpp::Format::eRG32sFloat, offsetof(VertexData, inPosition)
        },
        {
            1, lVertexInputBindingDescriptions[0].binding, vkpp::Format::eRGB32sFloat, offsetof(VertexData, inColor)
        }
    };

    const vkpp::PipelineVertexInputStateCreateInfo lVertexInputStateCreateInfo
    {
        1, lVertexInputBindingDescriptions,
        2, lVertexInputAttributeDescriptions
    };

    const vkpp::PipelineInputAssemblyStateCreateInfo lInputAssemblyStateCreateInfo
    {
        vkpp::PrimitiveTopology::eTriangleList
    };

    const vkpp::PipelineViewportStateCreateInfo lViewportStateCreateInfo
    {
        1, nullptr,
        1, nullptr
    };

    const vkpp::PipelineRasterizationStateCreateInfo lRasterizationStateCreateInfo
    {
        VK_FALSE,  VK_FALSE,
        vkpp::PolygonMode::eFill, vkpp::CullModeFlagBits::eBack, vkpp::FrontFace::eClockwise,
        VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f
    };

    const vkpp::PipelineMultisampleStateCreateInfo lMultisampleStateCreateInfo
    {
        vkpp::SampleCountFlagBits::e1,
        VK_FALSE, 1.0f, nullptr,
        VK_FALSE, VK_FALSE
    };

    const vkpp::PipelineColorBlendAttachmentState lColorBlendAttachmentState
    {
        VK_FALSE, vkpp::BlendFactor::eOne, vkpp::BlendFactor::eZero, vkpp::BlendOp::eAdd,
        vkpp::BlendFactor::eOne, vkpp::BlendFactor::eZero, vkpp::BlendOp::eAdd,
        vkpp::ColorComponentFlags(vkpp::ColorComponentFlagBits::eR) | vkpp::ColorComponentFlagBits::eG | vkpp::ColorComponentFlagBits::eB | vkpp::ColorComponentFlagBits::eA
    };

    const vkpp::PipelineColorBlendStateCreateInfo lColorBlendStateCreateInfo
    {
        VK_FALSE, vkpp::LogicalOp::eCopy,
        1, lColorBlendAttachmentState.AddressOf(),
        {0.0f, 0.0f, 0.0f, 0.0f}
    };

    const std::array<vkpp::DynamicState, 2> lDynamicStates
    {
        vkpp::DynamicState::eViewport, vkpp::DynamicState::eScissor
    };

    const vkpp::PipelineDynamicStateCreateInfo lDynamicStateCreateInfo{ lDynamicStates };

    const vkpp::PipelineLayoutCreateInfo lPipelineLayoutCreateInfo
    {
        0, nullptr,
        0, nullptr
    };

    const auto& lPipelineLayout = mLogicalDevice.CreatePipelineLayout(lPipelineLayoutCreateInfo);

    const vkpp::GraphicsPipelineCreateInfo lGraphicsPipelineCreateInfo
    (
        2, lPipelineShaderStageCreateInfos,
        lVertexInputStateCreateInfo.AddressOf(),
        lInputAssemblyStateCreateInfo.AddressOf(),
        nullptr,
        lViewportStateCreateInfo.AddressOf(),
        lRasterizationStateCreateInfo.AddressOf(),
        lMultisampleStateCreateInfo.AddressOf(),
        nullptr,
        lColorBlendStateCreateInfo.AddressOf(),
        lDynamicStateCreateInfo.AddressOf(),
        lPipelineLayout,
        mRenderPass, 0

    );

    mGraphicsPipeline = mLogicalDevice.CreateGraphicsPipeline(1, lGraphicsPipelineCreateInfo.AddressOf());

    mLogicalDevice.DestroyPipelineLayout(lPipelineLayout);
    DestroyShaderModule(lFragmentShaderModule);
    DestroyShaderModule(lVertexShaderModule);
}


//void StagingTriangle::CreateVertexBuffer(void)
//{
//    vkpp::BufferCreateInfo lVertexBufferCreateInfo
//    {
//        sizeof(gVertexData), vkpp::BufferUsageFlags(vkpp::BufferUsageFlagBits::eVertexBuffer) | vkpp::BufferUsageFlagBits::eTransferDst
//    };
//
//    mVertexBufferMemory = AllocateBufferMemory(mVertexBuffer, vkpp::MemoryPropertyFlags(vkpp::MemoryPropertyFlagBits::eHostVisible) | vkpp::MemoryPropertyFlagBits::eHostCoherent);
//    // mVertexBufferMemory = AllocateBufferMemory(mVertexBuffer, vkpp::MemoryPropertyFlagBits::eDeviceLocal);
//
//    auto lMappedMemory = mLogicalDevice.MapMemory(mVertexBufferMemory, 0, sizeof(gVertexData));
//    std::memcpy(lMappedMemory, gVertexData, sizeof(gVertexData));
//    mLogicalDevice.UnmapMemory(mVertexBufferMemory);
//
//    /*vkpp::MappedMemoryRange lFlushRange{ mVertexBufferMemory };
//    mLogicalDevice.FlushMappedMemoryRange(lFlushRange);
//    mLogicalDevice.UnmapMemory(mVertexBufferMemory);*/
//
//    mVertexBuffer = mLogicalDevice.CreateBuffer(lVertexBufferCreateInfo);
//    mLogicalDevice.BindBufferMemory(mVertexBuffer, mVertexBufferMemory);
//}


void StagingTriangle::CreateVertexBuffer(void)
{
    vkpp::BufferCreateInfo lVertexBufferCreateInfo
    {
        sizeof(gVertexData), vkpp::BufferUsageFlags(vkpp::BufferUsageFlagBits::eVertexBuffer) | vkpp::BufferUsageFlagBits::eTransferDst
    };

    mVertexBuffer = mLogicalDevice.CreateBuffer(lVertexBufferCreateInfo);
    mVertexBufferMemory = AllocateBufferMemory(mVertexBuffer, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    mLogicalDevice.BindBufferMemory(mVertexBuffer, mVertexBufferMemory);

    vkpp::BufferCreateInfo lStagingBufferCreateInfo
    {
        sizeof(gVertexData), vkpp::BufferUsageFlagBits::eTransferSrc
    };

    auto lStagingBuffer = mLogicalDevice.CreateBuffer(lStagingBufferCreateInfo);
    auto lStagingBufferMemory = AllocateBufferMemory(lStagingBuffer, vkpp::MemoryPropertyFlags(vkpp::MemoryPropertyFlagBits::eHostVisible) | vkpp::MemoryPropertyFlagBits::eHostCoherent);
    mLogicalDevice.BindBufferMemory(lStagingBuffer, lStagingBufferMemory);

    auto lMappedMemory = mLogicalDevice.MapMemory(lStagingBufferMemory, 0, sizeof(gVertexData));
    std::memcpy(lMappedMemory, gVertexData, sizeof(gVertexData));
    mLogicalDevice.UnmapMemory(lStagingBufferMemory);

    CopyBuffer(mVertexBuffer, lStagingBuffer, sizeof(gVertexData));

    mLogicalDevice.FreeMemory(lStagingBufferMemory);
    mLogicalDevice.DestroyBuffer(lStagingBuffer);
}


void StagingTriangle::CopyBuffer(vkpp::Buffer& aDstBuffer, const vkpp::Buffer& aSrcBuffer, vkpp::DeviceSize aSize) const
{
    const vkpp::CommandBufferAllocateInfo lAllocateInfo
    {
        mCommandPool, 1
    };

    auto lCommandBuffer = mLogicalDevice.AllocateCommandBuffer(lAllocateInfo);

    const vkpp::CommandBufferBeginInfo lCmdBeginInfo
    {
        vkpp::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    lCommandBuffer.Begin(lCmdBeginInfo);

    vkpp::BufferCopy lBufferCopy
    {
        0, 0, aSize
    };

    lCommandBuffer.Copy(aDstBuffer, aSrcBuffer, lBufferCopy);

    lCommandBuffer.End();

    const vkpp::SubmitInfo lSubmitInfo{ lCommandBuffer };

    const vkpp::FenceCreateInfo lFenceCreateInfo;

    auto lFence = mLogicalDevice.CreateFence(lFenceCreateInfo);

    mGraphicsQueue.mQueue.Submit(lSubmitInfo, lFence);

    mLogicalDevice.WaitForFence(lFence);

    mLogicalDevice.DestroyFence(lFence);
    mLogicalDevice.FreeCommandBuffer(mCommandPool, lCommandBuffer);
}


vkpp::DeviceMemory StagingTriangle::AllocateBufferMemory(const vkpp::Buffer& aBuffer, const vkpp::MemoryPropertyFlags& aMemoryProperties) const
{
    const auto& lBufferMemRequirements = mLogicalDevice.GetBufferMemoryRequirements(aBuffer);

    const auto& lDeviceMemoryProperties = mPhysicalDevice.GetMemoryProperties();

    for(uint32_t i = 0; i < lDeviceMemoryProperties.memoryTypeCount; ++i)
    {
        if (lBufferMemRequirements.memoryTypeBits & (1 << i) && (lDeviceMemoryProperties.memoryTypes[i].propertyFlags & aMemoryProperties) == aMemoryProperties)
        {
            vkpp::MemoryAllocateInfo lMemoryAllocationInfo
            {
                lBufferMemRequirements.size, i
            };

            return mLogicalDevice.AllocateMemory(lMemoryAllocationInfo);
        }
    }

    assert(false);
    return nullptr;
}


vkpp::Framebuffer StagingTriangle::CreateFramebuffer(const vkpp::ImageView& aImageView) const
{
    vkpp::FramebufferCreateInfo lFramebufferCreateInfo
    {
        mRenderPass, 1, aImageView.AddressOf(),
        mSwapchain.mExtent.width, mSwapchain.mExtent.height
    };

    return mLogicalDevice.CreateFramebuffer(lFramebufferCreateInfo);
}


void StagingTriangle::PrepareFrame(vkpp::Framebuffer& aFramebuffer, const vkpp::CommandBuffer& aCommandBuffer, const vkpp::ImageView& aImageView) const
{
    if (aFramebuffer)
        mLogicalDevice.DestroyFramebuffer(aFramebuffer);

    aFramebuffer = CreateFramebuffer(aImageView);

    const vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo
    {
        vkpp::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    aCommandBuffer.Begin(lCmdBufferBeginInfo);

    const vkpp::ClearValue lClearValue
    {
        { 0.129411f, 0.156862f, 0.188235f, 1.0f }
    };

    const vkpp::RenderPassBeginInfo lRenderPassBeginInfo
    {
        mRenderPass,
        aFramebuffer,
        {
            {0, 0},
            mSwapchain.mExtent
        },
        1, &lClearValue
    };

    aCommandBuffer.BeginRenderPass(lRenderPassBeginInfo, vkpp::SubpassContents::eInline);
    aCommandBuffer.BindVertexBuffer(mVertexBuffer, 0);

    const vkpp::Viewport lViewport
    {
        0.0f, 0.0f,
        static_cast<float>(mSwapchain.mExtent.width), static_cast<float>(mSwapchain.mExtent.height),
        0.0f, 1.0f
    };

    const vkpp::Rect2D lScissor
    {
        {0, 0},
        mSwapchain.mExtent
    };

    aCommandBuffer.SetViewport(lViewport);
    aCommandBuffer.SetScissor(lScissor);

    aCommandBuffer.BindGraphicsPipeline(mGraphicsPipeline);

    aCommandBuffer.Draw(3, 1, 0, 0);

    aCommandBuffer.EndRenderPass();

    aCommandBuffer.End();
}


void StagingTriangle::DrawFrame(void)
{
    static std::size_t lResourceIndex{ 0 };
    auto& lCurrentRenderingResource = mRenderingResources[lResourceIndex];

    lResourceIndex = (lResourceIndex + 1) % DefaultResourceSize;

    mLogicalDevice.WaitForFence(lCurrentRenderingResource.mFence, false, UINT64_MAX);
    mLogicalDevice.ResetFence(lCurrentRenderingResource.mFence);

    auto lImageIndex = mLogicalDevice.AcquireNextImage(mSwapchain.Handle, UINT64_MAX, lCurrentRenderingResource.mImageAvailSemaphore, nullptr);
    PrepareFrame(lCurrentRenderingResource.mFramebuffer, lCurrentRenderingResource.mCommandBuffer, mSwapchain.mSwapchainImageViews[lImageIndex]);

    vkpp::PipelineStageFlags lWaitDstStageMask{ vkpp::PipelineStageFlagBits::eColorAttachmentOutput };

    vkpp::SubmitInfo lSubmitInfo
    {
        1, lCurrentRenderingResource.mImageAvailSemaphore.AddressOf(),
        &lWaitDstStageMask,
        1, lCurrentRenderingResource.mCommandBuffer.AddressOf(),
        1, lCurrentRenderingResource.mFinishedRenderingSemaphore.AddressOf()
    };

    mPresentQueue.mQueue.Submit(1, lSubmitInfo.AddressOf(), lCurrentRenderingResource.mFence);

    vkpp::khr::PresentInfo lPresentInfo
    {
        1, lCurrentRenderingResource.mFinishedRenderingSemaphore.AddressOf(),
        1, mSwapchain.Handle.AddressOf(), &lImageIndex
    };

    mPresentQueue.mQueue.Present(lPresentInfo);
}



}                   // End of namespace sample.
