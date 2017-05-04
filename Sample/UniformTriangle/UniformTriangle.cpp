
#include "UniformTriangle.h"



namespace sample
{



const VertexData gVertexData[]
{
    { { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
    { { 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f } },
    { { 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } },
    { { -0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f } }
};

const uint16_t gIndices[]
{
    0, 1, 2, 2, 3, 0
};



UniformTriangle::UniformTriangle(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion)
    : Application(apApplicationName, aApplicationVersion, apEngineName, aEngineVersion), mRenderingResources(DefaultResourceSize)
{
    CreateCommandPool();
    CreateRenderingResources();
    CreateRenderPass();
    CreateGraphicsPipeline();

    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateUniformBuffer();

    CreateDescriptorPool();
    CreateDescriptorSet();
}


UniformTriangle::~UniformTriangle(void)
{
    mLogicalDevice.Wait();

    // mLogicalDevice.FreeDescriptorSet(mDescriptorPool, mDescriptorSet);
    mLogicalDevice.DestroyDescriptorPool(mDescriptorPool);

    mLogicalDevice.FreeMemory(mUniformBufferMemory);
    mLogicalDevice.DestroyBuffer(mUniformBuffer);

    mLogicalDevice.FreeMemory(mUniformStagingBufferMemory);
    mLogicalDevice.DestroyBuffer(mUniformStagingBuffer);

    mLogicalDevice.FreeMemory(mIndexBufferMemory);
    mLogicalDevice.DestroyBuffer(mIndexBuffer);

    mLogicalDevice.FreeMemory(mVertexBufferMemory);
    mLogicalDevice.DestroyBuffer(mVertexBuffer);

    mLogicalDevice.DestroyPipeline(mGraphicsPipeline);
    mLogicalDevice.DestroyPipelineLayout(mPipelineLayout);
    mLogicalDevice.DestroyDescriptorSetLayout(mDescriptorSetLayout);
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


void UniformTriangle::CreateCommandPool(void)
{
    const vkpp::CommandPoolCreateInfo lCommandPoolCreateInfo
    {
        mGraphicsQueue.mFamilyIndex, vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer
    };

    mCommandPool = mLogicalDevice.CreateCommandPool(lCommandPoolCreateInfo);
}


void UniformTriangle::CreateRenderingResources(void)
{
    for(auto& lRenderingResource : mRenderingResources)
    {
        AllocateCommandBuffers(lRenderingResource.mCommandBuffer);
        CreateSemaphore(lRenderingResource.mImageAvailSemaphore); 
        CreateSemaphore(lRenderingResource.mFinishedRenderingSemaphore);
        CreateFence(lRenderingResource.mFence);
    }
}


void UniformTriangle::AllocateCommandBuffers(vkpp::CommandBuffer& aCommandBuffer) const
{
    const vkpp::CommandBufferAllocateInfo lCommandBufferAllocateInfo
    {
        mCommandPool, 1
    };

    aCommandBuffer = mLogicalDevice.AllocateCommandBuffer(lCommandBufferAllocateInfo);
}


void UniformTriangle::CreateSemaphore(vkpp::Semaphore& aSemaphore) const
{
    vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    aSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void UniformTriangle::CreateFence(vkpp::Fence& aFence) const
{
    vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    aFence = mLogicalDevice.CreateFence(lFenceCreateInfo);
}


void UniformTriangle::CreateRenderPass(void)
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


vkpp::DescriptorSetLayout UniformTriangle::CreateDescriptorSetLayout(void) const
{
    const vkpp::DescriptorSetLayoutBinding lSetLayoutBinding
    {
        0, vkpp::DescriptorType::eUniformBuffer,
        1, vkpp::ShaderStageFlagBits::eVertex
    };

    const vkpp::DescriptorSetLayoutCreateInfo lSetLayoutCreateInfo
    {
        1, lSetLayoutBinding.AddressOf()
    };

    return mLogicalDevice.CreateDescriptorSetLayout(lSetLayoutCreateInfo);
}


void UniformTriangle::CreateGraphicsPipeline(void)
{
    const auto& lVertexShaderModule = CreateShaderModule("Shader/vert.spv");
    const auto& lFragmentShaderModule = CreateShaderModule("Shader/frag.spv");

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
            0, lVertexInputBindingDescriptions[0].binding, vkpp::Format::eRG32SFloat, offsetof(VertexData, inPosition)
        },
        {
            1, lVertexInputBindingDescriptions[0].binding, vkpp::Format::eRGB32SFloat, offsetof(VertexData, inColor)
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
        vkpp::PolygonMode::eFill, vkpp::CullModeFlagBits::eBack, vkpp::FrontFace::eCounterClockwise,
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
        vkpp::ColorComponentFlagBits::eR | vkpp::ColorComponentFlagBits::eG | vkpp::ColorComponentFlagBits::eB | vkpp::ColorComponentFlagBits::eA
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

    mDescriptorSetLayout = CreateDescriptorSetLayout();

    const vkpp::PipelineLayoutCreateInfo lPipelineLayoutCreateInfo
    {
        1, mDescriptorSetLayout.AddressOf(),
        0, nullptr
    };

    mPipelineLayout = mLogicalDevice.CreatePipelineLayout(lPipelineLayoutCreateInfo);

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
        mPipelineLayout,
        mRenderPass, 0

    );

    mGraphicsPipeline = mLogicalDevice.CreateGraphicsPipeline(1, lGraphicsPipelineCreateInfo.AddressOf());

    DestroyShaderModule(lFragmentShaderModule);
    DestroyShaderModule(lVertexShaderModule);
}


//void StagingTriangle::CreateVertexBuffer(void)
//{
//    vkpp::BufferCreateInfo lVertexBufferCreateInfo
//    {
//        sizeof(gVertexData), vkpp::BufferUsageFlagBits::eVertexBuffer | vkpp::BufferUsageFlagBits::eTransferDst
//    };
//
//    mVertexBufferMemory = AllocateBufferMemory(mVertexBuffer, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);
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


void UniformTriangle::CreateVertexBuffer(void)
{
    vkpp::BufferCreateInfo lVertexBufferCreateInfo
    {
        sizeof(gVertexData), vkpp::BufferUsageFlagBits::eVertexBuffer | vkpp::BufferUsageFlagBits::eTransferDst
    };

    mVertexBuffer = mLogicalDevice.CreateBuffer(lVertexBufferCreateInfo);
    mVertexBufferMemory = AllocateBufferMemory(mVertexBuffer, vkpp::MemoryPropertyFlagBits::eDeviceLocal);

    mLogicalDevice.BindBufferMemory(mVertexBuffer, mVertexBufferMemory);

    vkpp::BufferCreateInfo lStagingBufferCreateInfo
    {
        sizeof(gVertexData), vkpp::BufferUsageFlagBits::eTransferSrc
    };

    auto lStagingBuffer = mLogicalDevice.CreateBuffer(lStagingBufferCreateInfo);
    auto lStagingBufferMemory = AllocateBufferMemory(lStagingBuffer, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);
    mLogicalDevice.BindBufferMemory(lStagingBuffer, lStagingBufferMemory);

    auto lMappedMemory = mLogicalDevice.MapMemory(lStagingBufferMemory, 0, sizeof(gVertexData));
    std::memcpy(lMappedMemory, gVertexData, sizeof(gVertexData));
    mLogicalDevice.UnmapMemory(lStagingBufferMemory);

    CopyBuffer(mVertexBuffer, lStagingBuffer, sizeof(gVertexData));

    mLogicalDevice.FreeMemory(lStagingBufferMemory);
    mLogicalDevice.DestroyBuffer(lStagingBuffer);
}


void UniformTriangle::CreateIndexBuffer(void)
{
    const vkpp::BufferCreateInfo lStagingBufferCreateInfo
    {
        sizeof(gIndices), vkpp::BufferUsageFlagBits::eTransferSrc
    };

    auto lStagingBuffer = mLogicalDevice.CreateBuffer(lStagingBufferCreateInfo);
    auto lStagingBufferMemory = AllocateBufferMemory(lStagingBuffer, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);
    mLogicalDevice.BindBufferMemory(lStagingBuffer, lStagingBufferMemory);

    auto lMappedMemory = mLogicalDevice.MapMemory(lStagingBufferMemory, 0, sizeof(gIndices));
    std::memcpy(lMappedMemory, gIndices, sizeof(gIndices));
    mLogicalDevice.UnmapMemory(lStagingBufferMemory);

    const vkpp::BufferCreateInfo lIndexBufferCreateInfo
    {
        sizeof(gIndices), vkpp::BufferUsageFlagBits::eIndexBuffer | vkpp::BufferUsageFlagBits::eTransferDst
    };

    mIndexBuffer = mLogicalDevice.CreateBuffer(lIndexBufferCreateInfo);
    mIndexBufferMemory = AllocateBufferMemory(mIndexBuffer, vkpp::MemoryPropertyFlagBits::eDeviceLocal);
    mLogicalDevice.BindBufferMemory(mIndexBuffer, mIndexBufferMemory);

    CopyBuffer(mIndexBuffer, lStagingBuffer, sizeof(gIndices));

    mLogicalDevice.FreeMemory(lStagingBufferMemory);
    mLogicalDevice.DestroyBuffer(lStagingBuffer);
}


void UniformTriangle::CreateUniformBuffer(void)
{
    constexpr auto lUniformBufferSize = sizeof(UniformBufferObject);

    const vkpp::BufferCreateInfo lStagingBufferCreateInfo
    {
        lUniformBufferSize, vkpp::BufferUsageFlagBits::eTransferSrc
    };

    mUniformStagingBuffer = mLogicalDevice.CreateBuffer(lStagingBufferCreateInfo);
    mUniformStagingBufferMemory = AllocateBufferMemory(mUniformStagingBuffer, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);
    mLogicalDevice.BindBufferMemory(mUniformStagingBuffer, mUniformStagingBufferMemory);

    const vkpp::BufferCreateInfo lUniformBufferCreateInfo
    {
        lUniformBufferSize, vkpp::BufferUsageFlagBits::eUniformBuffer | vkpp::BufferUsageFlagBits::eTransferDst
    };

    mUniformBuffer = mLogicalDevice.CreateBuffer(lUniformBufferCreateInfo);
    mUniformBufferMemory = AllocateBufferMemory(mUniformBuffer, vkpp::MemoryPropertyFlagBits::eDeviceLocal);
    mLogicalDevice.BindBufferMemory(mUniformBuffer, mUniformBufferMemory);
}


void UniformTriangle::CreateDescriptorPool(void)
{
    const vkpp::DescriptorPoolSize lPoolSize
    {
        vkpp::DescriptorType::eUniformBuffer, 1
    };

    const vkpp::DescriptorPoolCreateInfo lPoolCreateInfo
    {
        1, lPoolSize.AddressOf(),
        1
    };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool(lPoolCreateInfo);
}


void UniformTriangle::CreateDescriptorSet(void)
{
    const vkpp::DescriptorSetAllocateInfo lSetAllocateInfo
    {
        mDescriptorPool, 1, mDescriptorSetLayout.AddressOf()
    };

    mDescriptorSet = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);

    const vkpp::DescriptorBufferInfo lBufferInfo
    {
        mUniformBuffer, 0, sizeof(UniformBufferObject)
    };

    const vkpp::WriteDescriptorSetInfo lWriteInfo
    {
        mDescriptorSet,
        0, 0,
        vkpp::DescriptorType::eUniformBuffer,
        lBufferInfo
    };

    mLogicalDevice.UpdateDescriptorSet(lWriteInfo);
}


void UniformTriangle::CopyBuffer(vkpp::Buffer& aDstBuffer, const vkpp::Buffer& aSrcBuffer, vkpp::DeviceSize aSize) const
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


vkpp::DeviceMemory UniformTriangle::AllocateBufferMemory(const vkpp::Buffer& aBuffer, const vkpp::MemoryPropertyFlags& aMemoryProperties) const
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


vkpp::Framebuffer UniformTriangle::CreateFramebuffer(const vkpp::ImageView& aImageView) const
{
    vkpp::FramebufferCreateInfo lFramebufferCreateInfo
    {
        mRenderPass, 1, aImageView.AddressOf(),
        mSwapchain.mExtent.width, mSwapchain.mExtent.height
    };

    return mLogicalDevice.CreateFramebuffer(lFramebufferCreateInfo);
}


void UniformTriangle::PrepareFrame(vkpp::Framebuffer& aFramebuffer, const vkpp::CommandBuffer& aCommandBuffer, const vkpp::ImageView& aImageView) const
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
    aCommandBuffer.BindIndexBuffer(mIndexBuffer, 0, vkpp::IndexType::eUInt16);

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
    aCommandBuffer.BindGraphicsDescriptorSet(mPipelineLayout, 0, mDescriptorSet);

    aCommandBuffer.DrawIndexed(sizeof(gIndices) / sizeof(gIndices[0]));

    aCommandBuffer.EndRenderPass();

    aCommandBuffer.End();
}


void UniformTriangle::UpdateUniformBuffer(void)
{
    static auto lStartTime = std::chrono::high_resolution_clock::now();

    auto lCurrentTime = std::chrono::high_resolution_clock::now();
    auto lTime = std::chrono::duration_cast<std::chrono::milliseconds>(lCurrentTime - lStartTime).count() / 1000.0f;

    UniformBufferObject lMVP
    {
        glm::rotate(glm::mat4(), lTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::perspective(glm::radians(45.0f), mSwapchain.mExtent.width / static_cast<float>(mSwapchain.mExtent.height), 0.1f, 10.0f)
    };

    lMVP.proj[1][1] *= -1;

    auto lMappedMemory = mLogicalDevice.MapMemory(mUniformStagingBufferMemory, 0, sizeof(UniformBufferObject));
    std::memcpy(lMappedMemory, &lMVP, sizeof(UniformBufferObject));
    mLogicalDevice.UnmapMemory(mUniformStagingBufferMemory);

    CopyBuffer(mUniformBuffer, mUniformStagingBuffer, sizeof(UniformBufferObject));
}


void UniformTriangle::DrawFrame(void)
{
    UpdateUniformBuffer();

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
