#include "MultiPipelines.h"

#include <iostream>



namespace vkpp::sample
{



decltype(auto) SetEnabledFeatures(const vkpp::PhysicalDeviceFeatures& aPhysicalDeviceFeatures)
{
    vkpp::PhysicalDeviceFeatures lEnabledFeatures{};

    // Fill mode non-solid is required for wire frame display.
    if (aPhysicalDeviceFeatures.fillModeNonSolid)
    {
        lEnabledFeatures.fillModeNonSolid = VK_TRUE;

        // Wide lines must be present for line width > 1.0f.
        if (aPhysicalDeviceFeatures.wideLines)
            lEnabledFeatures.wideLines = VK_TRUE;
    }

    return lEnabledFeatures;
}



MultiPipelines::MultiPipelines(CWindow& aWindow, const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion)
    : ExampleBase(aWindow, apApplicationName, aApplicationVersion, apEngineName, aEngineVersion),
      CWindowEvent(aWindow),
      mModel(*this, mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mDepthResource(mLogicalDevice, mPhysicalDeviceMemoryProperties),
      mUniformBufferResource(mLogicalDevice, mPhysicalDeviceMemoryProperties)
{
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
        mLogicalDevice.FreeCommandBuffers(mCmdPool, mCmdDrawBuffers);
        AllocateDrawCmdBuffers();
        BuildCommandBuffers();
    };

    CreateCommandPool();
    AllocateDrawCmdBuffers();

    CreateRenderPass();
    CreateDepthResource();
    CreateSemaphores();
    CreateFences();

    CreateFramebuffer();

    mModel.LoadMode("model/treasure_smooth.dae");

    CreateSetLayout();
    CreatePipelineLayout();

    CreateGraphicsPipelines();
    CreateUniformBuffer();
    UpdateUniformBuffer();
    CreateDescriptorPool();
    AllocateDescriptorSet();
    UpdateDescriptorSet();

    BuildCommandBuffers();

    theApp.RegisterUpdateEvent([this](void)
    {
        Update();
    });
}


MultiPipelines::~MultiPipelines(void)
{
    mLogicalDevice.Wait();

    // mLogicalDevice.FreeDescriptorSet(mDescriptorPool, mDescriptorSet);
    mLogicalDevice.DestroyDescriptorPool(mDescriptorPool);

    mLogicalDevice.DestroyPipeline(mPipelines.toon);
    mLogicalDevice.DestroyPipeline(mPipelines.wireframe);
    mLogicalDevice.DestroyPipeline(mPipelines.phong);

    mLogicalDevice.DestroyPipelineLayout(mPipelineLayout);
    mLogicalDevice.DestroyDescriptorSetLayout(mSetLayout);

    for (auto& lFramebuffer : mFramebuffers)
        mLogicalDevice.DestroyFramebuffer(lFramebuffer);

    mLogicalDevice.DestroyRenderPass(mRenderPass);

    // mDepthResource.Reset();

    for (auto& lFence : mWaitFences)
        mLogicalDevice.DestroyFence(lFence);

    mLogicalDevice.DestroySemaphore(mRenderCompleteSemaphore);
    mLogicalDevice.DestroySemaphore(mPresentCompleteSemaphore);

    mLogicalDevice.FreeCommandBuffers(mCmdPool, mCmdDrawBuffers);
    mLogicalDevice.DestroyCommandPool(mCmdPool);
}


void MultiPipelines::CreateCommandPool(void)
{
    const vkpp::CommandPoolCreateInfo lCmdCreateInfo
    {
        mGraphicsQueue.familyIndex,
        vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer
    };

    mCmdPool = mLogicalDevice.CreateCommandPool(lCmdCreateInfo);
}


void MultiPipelines::AllocateDrawCmdBuffers(void)
{
    const vkpp::CommandBufferAllocateInfo lCmdAllocateInfo
    {
        mCmdPool,
        static_cast<uint32_t>(mSwapchain.buffers.size()),
    };

    // Create one command buffer for each swapchain image and reuse for renderering.
    mCmdDrawBuffers = mLogicalDevice.AllocateCommandBuffers(lCmdAllocateInfo);
}


void MultiPipelines::CreateRenderPass(void)
{
    const std::array<vkpp::AttachementDescription, 2> lAttachementDescriptions
    { {
        // Color Attachment.
        {
            mSwapchain.surfaceFormat.format,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear,
            vkpp::AttachmentStoreOp::eStore,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::ePresentSrcKHR
        },
        // Depth Attachment.
        {
            vkpp::Format::eD32sFloatS8uInt,
            vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eDontCare,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
        }
    }};

    // Setup color attachment reference.
    constexpr vkpp::AttachmentReference lColorRef
    {
        0, vkpp::ImageLayout::eColorAttachmentOptimal
    };

    // Setup depth attachment reference.
    constexpr vkpp::AttachmentReference lDepthRef
    {
        1, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
    };

    constexpr std::array<vkpp::SubpassDescription, 1> lSubpassDesc
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
    {{
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
    }};

    const vkpp::RenderPassCreateInfo lRenderPassCreateInfo
    {
        lAttachementDescriptions,
        lSubpassDesc,
        lSubpassDependencies
    };

    mRenderPass = mLogicalDevice.CreateRenderPass(lRenderPassCreateInfo);
}


void MultiPipelines::CreateDepthResource(void)
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

    mDepthResource.Reset(lImageCreateInfo, lImageViewCreateInfo, vkpp::MemoryPropertyFlagBits::eDeviceLocal);
}


void MultiPipelines::CreateFramebuffer(void)
{
    for (auto& lSwapchainRes : mSwapchain.buffers)
    {
        const std::array<vkpp::ImageView, 2> lAttachments
        {
            lSwapchainRes.view,
            mDepthResource.view
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


void MultiPipelines::CreateUniformBuffer(void)
{
    // Prepare and initialize an uniform buffer block containing shader uniforms.
    vkpp::BufferCreateInfo lBufferCreateInfo
    {
        sizeof(UniformBufferObject),
        vkpp::BufferUsageFlagBits::eUniformBuffer
    };

    mUniformBufferResource.Reset(lBufferCreateInfo, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);
}


void MultiPipelines::UpdateUniformBuffer(void)
{
    const auto lWidth = mSwapchain.extent.width;
    const auto lHeight = mSwapchain.extent.height;
    const glm::vec3 lCameraPos;
    const glm::vec3 lRotation{ -25.0f, 15.0f, 0.0f };
    const auto lZoom = -15.5f;

    mMVPMatrix.projection = glm::perspective(glm::radians(60.0f), static_cast<float>(lWidth / 3.0f) / static_cast<float>(lHeight), 0.1f, 256.0f);

    const auto& viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, lZoom));

    mMVPMatrix.modelView = viewMatrix * glm::translate(glm::mat4(), lCameraPos);
    mMVPMatrix.modelView = glm::rotate(mMVPMatrix.modelView, glm::radians(lRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    mMVPMatrix.modelView = glm::rotate(mMVPMatrix.modelView, glm::radians(lRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    mMVPMatrix.modelView = glm::rotate(mMVPMatrix.modelView, glm::radians(lRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    auto lMappedMemory = mLogicalDevice.MapMemory(mUniformBufferResource.memory, 0, sizeof(UniformBufferObject));
    std::memcpy(lMappedMemory, &mMVPMatrix, sizeof(UniformBufferObject));

    // Note: Since we requested a host coherent memory for the uniform buffer, the write is instantly visible to the GPU.
    mLogicalDevice.UnmapMemory(mUniformBufferResource.memory);
}


void MultiPipelines::CreateSetLayout(void)
{
    constexpr vkpp::DescriptorSetLayoutBinding lSetLayoutBinding
    {
        0, vkpp::DescriptorType::eUniformBuffer,
        1, vkpp::ShaderStageFlagBits::eVertex
    };

    constexpr vkpp::DescriptorSetLayoutCreateInfo lSetLayoutCreateInfo
    {
        1, lSetLayoutBinding.AddressOf()
    };

    mSetLayout = mLogicalDevice.CreateDescriptorSetLayout(lSetLayoutCreateInfo);
}


void MultiPipelines::CreatePipelineLayout(void)
{
    const vkpp::PipelineLayoutCreateInfo lLayoutCreateInfo
    {
        1, mSetLayout.AddressOf()
    };

    mPipelineLayout = mLogicalDevice.CreatePipelineLayout(lLayoutCreateInfo);
}


void MultiPipelines::CreateGraphicsPipelines(void)
{
    std::array<vkpp::PipelineShaderStageCreateInfo, 2> lShaderStageCreateInfos
    {{
        {
            vkpp::ShaderStageFlagBits::eVertex
        },
        {
            vkpp::ShaderStageFlagBits::eFragment
        }
    } };

    constexpr std::array<vkpp::VertexInputBindingDescription, 1> lVertexInputBindingDescriptions{ VertexData::GetBindingDescription() };
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

    constexpr vkpp::PipelineViewportStateCreateInfo lViewportStateCreateInfo
    {
        1, 1
    };

    vkpp::PipelineRasterizationStateCreateInfo lRasterizationStateCreateInfo
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

    constexpr vkpp::PipelineDepthStencilStateCreateInfo lDepthStencilStateCreateInfo
    {
        DepthTest::Enable, DepthWrite::Enable,
        vkpp::CompareOp::eLessOrEqual
    };

    constexpr vkpp::PipelineColorBlendAttachmentState lColorBlendAttachmentState;
    const vkpp::PipelineColorBlendStateCreateInfo lColorBlendStateCreateInfo
    {
        1, lColorBlendAttachmentState.AddressOf()
    };

    constexpr std::array<vkpp::DynamicState, 3> lDynamicStates
    {
        vkpp::DynamicState::eViewport,
        vkpp::DynamicState::eScissor,
        vkpp::DynamicState::eLineWidth
    };

    const vkpp::PipelineDynamicStateCreateInfo lDynamicStateCreateInfo{ lDynamicStates };

    vkpp::GraphicsPipelineCreateInfo lGraphicsPipelineCreateInfo
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

    // Create the graphics pipeline state objects.

    // This pipeline is used as the base for other pipelines (derivatives).
    // Pipeline derivatives can be used for pipelines that shader most of their states.
    // Depending on the implementation, this may result in better performance for pipeline switching and faster creation time.
    lGraphicsPipelineCreateInfo.flags = vkpp::PipelineCreateFlagBits::eAllowDerivatives;

    // Textured pipeline.
    // Phong shading pipeline.
    lShaderStageCreateInfos[0].module = CreateShaderModule("Shader/SPV/phong.vert.spv");
    lShaderStageCreateInfos[1].module = CreateShaderModule("Shader/SPV/phong.frag.spv");
    mPipelines.phong = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lShaderStageCreateInfos[0].module);
    mLogicalDevice.DestroyShaderModule(lShaderStageCreateInfos[1].module);

    // All pipelines created after the base pipeline will be derivatives.
    lGraphicsPipelineCreateInfo.flags = vkpp::PipelineCreateFlagBits::eDerivative;

    // Base pipeline will be out first created pipeline.
    lGraphicsPipelineCreateInfo.basePipelineHandle = mPipelines.phong;

    // It is only allowed to either use a handle or index for the base pipeline.
    // As we use the handle, we must set the index to -1.
    lGraphicsPipelineCreateInfo.basePipelineIndex = -1;

    // Toon Shading pipeline.
    lShaderStageCreateInfos[0].module = CreateShaderModule("Shader/SPV/toon.vert.spv");
    lShaderStageCreateInfos[1].module = CreateShaderModule("Shader/SPV/toon.frag.spv");
    mPipelines.toon = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

    mLogicalDevice.DestroyShaderModule(lShaderStageCreateInfos[0].module);
    mLogicalDevice.DestroyShaderModule(lShaderStageCreateInfos[1].module);

    // Pipeline for wire frame rendering.
    // Non-solid rendering is not a mandatory Vulkan feature.
    if(mEnabledFeatures.fillModeNonSolid)
    {
        lRasterizationStateCreateInfo.polygonMode = PolygonMode::eLine;
        lShaderStageCreateInfos[0].module = CreateShaderModule("Shader/SPV/wireframe.vert.spv");
        lShaderStageCreateInfos[1].module = CreateShaderModule("Shader/SPV/wireframe.frag.spv");
        mPipelines.wireframe = mLogicalDevice.CreateGraphicsPipeline(lGraphicsPipelineCreateInfo);

        mLogicalDevice.DestroyShaderModule(lShaderStageCreateInfos[0].module);
        mLogicalDevice.DestroyShaderModule(lShaderStageCreateInfos[1].module);
    }
}


void MultiPipelines::CreateDescriptorPool(void)
{
    constexpr vkpp::DescriptorPoolSize lPoolSize
    {
        vkpp::DescriptorType::eUniformBuffer,
        1
    };

    constexpr vkpp::DescriptorPoolCreateInfo lPoolCreateInfo
    {
        1, lPoolSize.AddressOf(),
        2
    };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool(lPoolCreateInfo);
}


void MultiPipelines::AllocateDescriptorSet(void)
{
    const vkpp::DescriptorSetAllocateInfo lSetAllocateInfo
    {
        mDescriptorPool,
        1, mSetLayout.AddressOf()
    };

    mDescriptorSet = mLogicalDevice.AllocateDescriptorSet(lSetAllocateInfo);
}


void MultiPipelines::UpdateDescriptorSet(void) const
{
    const vkpp::DescriptorBufferInfo lDescriptorBufferInfo
    {
        mUniformBufferResource.buffer,
        0,
        sizeof(UniformBufferObject)
    };

    const vkpp::WriteDescriptorSetInfo lWriteDescriptorSetInfo
    {
        mDescriptorSet,
        0,
        vkpp::DescriptorType::eUniformBuffer,
        lDescriptorBufferInfo
    };

    mLogicalDevice.UpdateDescriptorSet(lWriteDescriptorSetInfo);
}


vkpp::CommandBuffer MultiPipelines::BeginOneTimeCommandBuffer(void) const
{
    const vkpp::CommandBufferAllocateInfo lCmdBufferAllocateInfo
    {
        mCmdPool,
        1               // Command buffer count.
    };

    auto lCmdBuffer = mLogicalDevice.AllocateCommandBuffer(lCmdBufferAllocateInfo);

    constexpr vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo{ vkpp::CommandBufferUsageFlagBits::eOneTimeSubmit };

    lCmdBuffer.Begin(lCmdBufferBeginInfo);

    return lCmdBuffer;
}


void MultiPipelines::EndOneTimeCommandBuffer(const vkpp::CommandBuffer& aCmdBuffer) const
{
    aCmdBuffer.End();

    const vkpp::SubmitInfo lSubmitInfo{ aCmdBuffer };

    // Create fence to ensure that the command buffer has finished executing.
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo;
    const auto& lFence = mLogicalDevice.CreateFence(lFenceCreateInfo);

    // Submit to the queue.
    mGraphicsQueue.handle.Submit(lSubmitInfo, lFence);

    // Wait for the fence to signal that command buffer has finished executing.
    mLogicalDevice.WaitForFence(lFence);

    mLogicalDevice.DestroyFence(lFence);
    mLogicalDevice.FreeCommandBuffer(mCmdPool, aCmdBuffer);
}


void MultiPipelines::CopyBuffer(vkpp::Buffer& aDstBuffer, const Buffer& aSrcBuffer, DeviceSize aSize) const
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


void MultiPipelines::BuildCommandBuffers(void)
{
    constexpr vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo;

    constexpr std::array<vkpp::ClearValue, 2> lClearValues
    { {
        { 0.129411f, 0.156862f, 0.188235f, 1.0f },
        { 1.0f, 0.0f }
    } };

    for (std::size_t lIndex = 0; lIndex < mCmdDrawBuffers.size(); ++lIndex)
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

        const auto& lDrawCmdBuffer = mCmdDrawBuffers[lIndex];

        lDrawCmdBuffer.Begin(lCmdBufferBeginInfo);

        lDrawCmdBuffer.BeginRenderPass(lRenderPassBeginInfo);
        lDrawCmdBuffer.BindVertexBuffer(mModel.vertices.buffer, 0);
        lDrawCmdBuffer.BindIndexBuffer(mModel.indices.buffer, 0, vkpp::IndexType::eUInt32);

        vkpp::Viewport lViewport
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

        lDrawCmdBuffer.BindGraphicsDescriptorSet(mPipelineLayout, 0, mDescriptorSet);

        // Left: Solid colored.
        lViewport.width /= 3.0f;
        lDrawCmdBuffer.SetViewport(lViewport);
        lDrawCmdBuffer.BindGraphicsPipeline(mPipelines.phong);
        lDrawCmdBuffer.DrawIndexed(mModel.indexCount);

        // Center: Toon.
        lViewport.x = lViewport.width;
        lDrawCmdBuffer.SetViewport(lViewport);
        lDrawCmdBuffer.BindGraphicsPipeline(mPipelines.toon);

        // Line Width > 1.0f only if wide lines feature is supported.
        if (mEnabledFeatures.wideLines)
            lDrawCmdBuffer.SetLineWidth(2.0f);

        lDrawCmdBuffer.DrawIndexed(mModel.indexCount);

        // Right: Wireframe.
        if (mEnabledFeatures.fillModeNonSolid)
        {
            lViewport.x = lViewport.width * 2;
            lDrawCmdBuffer.SetViewport(lViewport);
            lDrawCmdBuffer.BindGraphicsPipeline(mPipelines.wireframe);
            lDrawCmdBuffer.DrawIndexed(mModel.indexCount);
        }

        lDrawCmdBuffer.EndRenderPass();

        lDrawCmdBuffer.End();
    }
}


void MultiPipelines::Update()
{
    auto lIndex = mLogicalDevice.AcquireNextImage(mSwapchain.handle, mPresentCompleteSemaphore);

    mLogicalDevice.WaitForFence(mWaitFences[lIndex]);
    mLogicalDevice.ResetFence(mWaitFences[lIndex]);

    constexpr vkpp::PipelineStageFlags lWaitDstStageMask{ vkpp::PipelineStageFlagBits::eColorAttachmentOutput };

    const vkpp::SubmitInfo lSubmitInfo
    {
        1, mPresentCompleteSemaphore.AddressOf(),       // Wait Semaphores.
        &lWaitDstStageMask,
        1, mCmdDrawBuffers[lIndex].AddressOf(),
        1, mRenderCompleteSemaphore.AddressOf()         // Signal Semaphores
    };

    mPresentQueue.handle.Submit(lSubmitInfo, mWaitFences[lIndex]);

    const vkpp::khr::PresentInfo lPresentInfo
    {
        1, mRenderCompleteSemaphore.AddressOf(),
        1, mSwapchain.handle.AddressOf(), &lIndex
    };

    mPresentQueue.handle.Present(lPresentInfo);
}


void MultiPipelines::CreateSemaphores(void)
{
    constexpr vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    // Semaphore used to ensure that image presentation is complete before starting to submit again.
    mPresentCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);

    // Semaphore used to ensure that all commands submitted have been finished before submitting the image to the presentation queue.
    mRenderCompleteSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void MultiPipelines::CreateFences(void)
{
    // Create in signaled state so we don't wait on the first render of each command buffer.
    constexpr vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    for (std::size_t lIndex = 0; lIndex < mCmdDrawBuffers.size(); ++lIndex)
        mWaitFences.emplace_back(mLogicalDevice.CreateFence(lFenceCreateInfo));
}


Model::Model(const MultiPipelines& aMultiPipelineSample, const vkpp::LogicalDevice& aDevice, const vkpp::PhysicalDeviceMemoryProperties& aPhysicalDeviceMemProperties)
    : multiPipelineSample(aMultiPipelineSample), device(aDevice), memProperties(aPhysicalDeviceMemProperties), vertices(aDevice, aPhysicalDeviceMemProperties), indices(aDevice, aPhysicalDeviceMemProperties)
{}


void Model::LoadMode(const std::string& aFilename, unsigned int aImporterFlags)
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

        aiColor3D lColor;
        lpAIScene->mMaterials[lpAIMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, lColor);

        const aiVector3D lZero3D;

        for (unsigned int lVtxIndex = 0; lVtxIndex < lpAIMesh->mNumVertices; ++lVtxIndex)
        {
            // Vertex positions.
            const auto lPos = lpAIMesh->mVertices[lVtxIndex];
            lVertexBuffer.emplace_back(lPos.x);
            lVertexBuffer.emplace_back(-lPos.y);
            lVertexBuffer.emplace_back(lPos.z);

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
            lVertexBuffer.emplace_back(lColor.r);
            lVertexBuffer.emplace_back(lColor.g);
            lVertexBuffer.emplace_back(lColor.b);

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

    multiPipelineSample.CopyBuffer(vertices.buffer, lStagingVtxBufferRes.buffer, lVtxBufferSize);
    multiPipelineSample.CopyBuffer(indices.buffer, lStagingIdxBufferRes.buffer, lIdxBufferSize);
}



}                   // End of namespace vkpp::sample.