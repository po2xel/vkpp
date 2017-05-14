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
      mDepthResource(mLogicalDevice, mPhysicalDeviceMemoryProperties)
{
    mResizedFunc = [this](Sint32 aWidth, Sint32 aHeight)
    {
        std::cout << "Resized to " << aWidth << "x" << aHeight << std::endl;

        CreateSwapchain(mSwapchain);
    };

    CreateCommandPool();
    AllocateDrawCmdBuffers();

    CreateRenderPass();
    CreateDepthResource();
    CreateFramebuffer();

    CreateSetLayout();
    CreatePipelineLayout();

    CreateGraphicsPipelines();
    CreateDescriptorPool();
    AllocateDescriptorSet();
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
        VK_FALSE, VK_FALSE,
        vkpp::PolygonMode::eFill,
        vkpp::CullModeFlagBits::eBack,
        vkpp::FrontFace::eClockwise,
        VK_FALSE,
        0.0f, 0.0f, 0.0f,
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


}                   // End of namespace vkpp::sample.