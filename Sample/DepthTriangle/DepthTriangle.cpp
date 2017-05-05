
#include "DepthTriangle.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <memory>



namespace sample
{



const VertexData gVertexData[]
{
    { { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 2.0f} },
    { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 2.0f, 2.0f} },
    { { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 2.0f, 0.0f} },
    { { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },

    { { -0.5f, -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 2.0f } },
    { { 0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 2.0f, 2.0f } },
    { { 0.5f, 0.5f, -0.5f },{ 0.0f, 0.0f, 1.0f },{ 2.0f, 0.0f } },
    { { -0.5f, 0.5f, -0.5f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } }
};

const uint16_t gIndices[]
{
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
};



DepthTriangle::DepthTriangle(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion)
    : Application(apApplicationName, aApplicationVersion, apEngineName, aEngineVersion), mRenderingResources(DefaultResourceSize)
{
    CreateCommandPool();
    CreateDepthResources();
    CreateRenderingResources();
    CreateRenderPass();
    CreateGraphicsPipeline();

    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateUniformBuffer();
    CreateTextureImage();
    CreateTextureImageView();
    CreateTextureSampler();

    CreateDescriptorPool();
    CreateDescriptorSet();
}


DepthTriangle::~DepthTriangle(void)
{
    mLogicalDevice.Wait();

    // mLogicalDevice.FreeDescriptorSet(mDescriptorPool, mDescriptorSet);
    mLogicalDevice.DestroyDescriptorPool(mDescriptorPool);

    mLogicalDevice.DestroySampler(mTextureSampler);
    mLogicalDevice.DestroyImageView(mTextureImageView);

    mLogicalDevice.FreeMemory(mTextureImageMemory);
    mLogicalDevice.DestroyImage(mTextureImage);

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

    mLogicalDevice.FreeMemory(mDepthImageMemory);
    mLogicalDevice.DestroyImageView(mDepthImageView);
    mLogicalDevice.DestroyImage(mDepthImage);

    mLogicalDevice.DestroyCommandPool(mCommandPool);
}


void DepthTriangle::CreateCommandPool(void)
{
    const vkpp::CommandPoolCreateInfo lCommandPoolCreateInfo
    {
        mGraphicsQueue.mFamilyIndex, vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer
    };

    mCommandPool = mLogicalDevice.CreateCommandPool(lCommandPoolCreateInfo);
}


void DepthTriangle::CreateDepthResources(void)
{
    const vkpp::ImageCreateInfo lImageCreateInfo
    {
        vkpp::ImageType::e2D, vkpp::Format::eD32sFloat,
        { mSwapchain.mExtent.width, mSwapchain.mExtent.height, 1 },
        1, 1,
        vkpp::SampleCountFlagBits::e1,
        vkpp::ImageTiling::eOptimal,
        vkpp::ImageUsageFlagBits::eDepthStencilAttachment,
        vkpp::ImageLayout::eUndefined
    };

    mDepthImage = mLogicalDevice.CreateImage(lImageCreateInfo);
    mDepthImageMemory = AllocateImageMemory(mDepthImage, vkpp::MemoryPropertyFlagBits::eDeviceLocal);
    mLogicalDevice.BindImageMemory(mDepthImage, mDepthImageMemory);

    const vkpp::ImageViewCreateInfo lImageViewCreateInfo
    {
        mDepthImage,
        vkpp::ImageViewType::e2D, vkpp::Format::eD32sFloat,
        {vkpp::ComponentSwizzle::eIdentity, vkpp::ComponentSwizzle::eIdentity, vkpp::ComponentSwizzle::eIdentity, vkpp::ComponentSwizzle::eIdentity},
        {vkpp::ImageAspectFlagBits::eDepth, 0, 1, 0, 1}
    };

    mDepthImageView = mLogicalDevice.CreateImageView(lImageViewCreateInfo);
}


void DepthTriangle::CreateRenderingResources(void)
{
    for(auto& lRenderingResource : mRenderingResources)
    {
        AllocateCommandBuffers(lRenderingResource.mCommandBuffer);
        CreateSemaphore(lRenderingResource.mImageAvailSemaphore); 
        CreateSemaphore(lRenderingResource.mFinishedRenderingSemaphore);
        CreateFence(lRenderingResource.mFence);
    }
}


void DepthTriangle::AllocateCommandBuffers(vkpp::CommandBuffer& aCommandBuffer) const
{
    const vkpp::CommandBufferAllocateInfo lCommandBufferAllocateInfo
    {
        mCommandPool, 1
    };

    aCommandBuffer = mLogicalDevice.AllocateCommandBuffer(lCommandBufferAllocateInfo);
}


void DepthTriangle::CreateSemaphore(vkpp::Semaphore& aSemaphore) const
{
    vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    aSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void DepthTriangle::CreateFence(vkpp::Fence& aFence) const
{
    vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    aFence = mLogicalDevice.CreateFence(lFenceCreateInfo);
}


void DepthTriangle::CreateRenderPass(void)
{
    const vkpp::AttachementDescription lAttachementDescriptions[]
    {
        {
            mSwapchain.mSurfaceFormat.format, vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eStore,
            vkpp::AttachmentLoadOp::eDontCare, vkpp::AttachmentStoreOp::eDontCare,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::ePresentSrcKHR
        },
        {
            vkpp::Format::eD32sFloat, vkpp::SampleCountFlagBits::e1,
            vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eDontCare,
            vkpp::AttachmentLoadOp::eDontCare, vkpp::AttachmentStoreOp::eDontCare,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
        }
    };

    const vkpp::AttachmentReference lColorAttachments[]
    {
        {0, vkpp::ImageLayout::eColorAttachmentOptimal}
    };

    const vkpp::AttachmentReference lDepthAttachment
    {
        1, vkpp::ImageLayout::eDepthStencilAttachmentOptimal
    };

    const vkpp::SubpassDescription lSubpassDescriptions[]
    {
        {
            vkpp::PipelineBindPoint::eGraphics,
            0, nullptr,
            1, lColorAttachments,
            nullptr,
            lDepthAttachment.AddressOf()
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
        2, lAttachementDescriptions,
        1, lSubpassDescriptions,
        2, lSubpassDependencies
    };

    mRenderPass = mLogicalDevice.CreateRenderPass(lRenderPassCreateInfo);
}


vkpp::DescriptorSetLayout DepthTriangle::CreateDescriptorSetLayout(void) const
{
    const vkpp::DescriptorSetLayoutBinding lSetLayoutBinding[]
    {
        {
            0, vkpp::DescriptorType::eUniformBuffer,
            1, vkpp::ShaderStageFlagBits::eVertex
        },
        {
            1, vkpp::DescriptorType::eCombinedImageSampler,
            1, vkpp::ShaderStageFlagBits::eFragment
        }
    };

    const vkpp::DescriptorSetLayoutCreateInfo lSetLayoutCreateInfo{ 2, lSetLayoutBinding };

    return mLogicalDevice.CreateDescriptorSetLayout(lSetLayoutCreateInfo);
}


void DepthTriangle::CreateGraphicsPipeline(void)
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
            0, lVertexInputBindingDescriptions[0].binding, vkpp::Format::eRGB32sFloat, offsetof(VertexData, inPosition)
        },
        {
            1, lVertexInputBindingDescriptions[0].binding, vkpp::Format::eRGB32sFloat, offsetof(VertexData, inColor)
        },
        {
            2, lVertexInputBindingDescriptions[0].binding, vkpp::Format::eRG32sFloat, offsetof(VertexData, texCoord)
        }
    };

    const vkpp::PipelineVertexInputStateCreateInfo lVertexInputStateCreateInfo
    {
        1, lVertexInputBindingDescriptions,
        3, lVertexInputAttributeDescriptions
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

    const vkpp::PipelineDepthStencilStateCreateInfo lDepthStencilStateCreateInfo
    {
        VK_TRUE, VK_TRUE, vkpp::CompareOp::eLess
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
        lDepthStencilStateCreateInfo.AddressOf(),
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


void DepthTriangle::CreateVertexBuffer(void)
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


void DepthTriangle::CreateIndexBuffer(void)
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


void DepthTriangle::CreateUniformBuffer(void)
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


void DepthTriangle::CreateTextureImage(void)
{
    int lTexWidth{ 0 }, lTexHeight{ 0 }, lTexChannels{ 0 };
    std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> lpPixels{ stbi_load("Texture/statue.jpg", &lTexWidth, &lTexHeight, &lTexChannels, STBI_rgb_alpha), stbi_image_free };
    assert(lpPixels);

    const auto lImageSize = lTexWidth * lTexHeight * 4;

    const vkpp::ImageCreateInfo lStagingImageCreateInfo
    {
        vkpp::ImageType::e2D, vkpp::Format::eRGBA8uNorm,
        {static_cast<uint32_t>(lTexWidth), static_cast<uint32_t>(lTexHeight), 1},
        1, 1,
        vkpp::SampleCountFlagBits::e1, vkpp::ImageTiling::eLinear,
        vkpp::ImageUsageFlagBits::eTransferSrc, vkpp::ImageLayout::ePreinitialized
    };

    auto lStagingImage = mLogicalDevice.CreateImage(lStagingImageCreateInfo);
    auto lStagingImageMemory = AllocateImageMemory(lStagingImage, vkpp::MemoryPropertyFlagBits::eHostVisible | vkpp::MemoryPropertyFlagBits::eHostCoherent);
    mLogicalDevice.BindImageMemory(lStagingImage, lStagingImageMemory);

    auto lpMappedData = mLogicalDevice.MapMemory(lStagingImageMemory, 0, lImageSize);

    const vkpp::ImageSubresource lImageSubresource
    {
        vkpp::ImageAspectFlagBits::eColor,
        0, 0
    };

    const auto& lImageSubresourceLayout = mLogicalDevice.GetImageSubresourceLayout(lStagingImage, lImageSubresource);

    if (lImageSubresourceLayout.rowPitch == lTexWidth * 4)
    {
        std::memcpy(lpMappedData, lpPixels.get(), lImageSize);
    }
    else
    {
        // TODO
        assert(false);
    }

    mLogicalDevice.UnmapMemory(lStagingImageMemory);

    const vkpp::ImageCreateInfo lImageCreateInfo
    {
        vkpp::ImageType::e2D, vkpp::Format::eRGBA8uNorm,
        {static_cast<uint32_t>(lTexWidth), static_cast<uint32_t>(lTexHeight), 1},
        1, 1,
        vkpp::SampleCountFlagBits::e1, vkpp::ImageTiling::eOptimal,
        vkpp::ImageUsageFlagBits::eTransferDst | vkpp::ImageUsageFlagBits::eSampled,
        vkpp::ImageLayout::eUndefined
    };

    mTextureImage = mLogicalDevice.CreateImage(lImageCreateInfo);
    mTextureImageMemory = AllocateImageMemory(mTextureImage, vkpp::MemoryPropertyFlagBits::eDeviceLocal);
    mLogicalDevice.BindImageMemory(mTextureImage, mTextureImageMemory);

    TransitionImageLayout<vkpp::ImageLayout::ePreinitialized, vkpp::ImageLayout::eTransferSrcOptimal>(lStagingImage, vkpp::AccessFlagBits::eHostWrite, vkpp::AccessFlagBits::eTransferRead);
    TransitionImageLayout<vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eTransferDstOptimal>(mTextureImage, vkpp::DefaultFlags, vkpp::AccessFlagBits::eTransferWrite);

    CopyImage(mTextureImage, lStagingImage, static_cast<uint32_t>(lTexWidth), static_cast<uint32_t>(lTexHeight));

    TransitionImageLayout<vkpp::ImageLayout::eTransferDstOptimal, vkpp::ImageLayout::eShaderReadOnlyOptimal>(mTextureImage, vkpp::AccessFlagBits::eTransferWrite, vkpp::AccessFlagBits::eShaderRead);

    mLogicalDevice.FreeMemory(lStagingImageMemory);
    mLogicalDevice.DestroyImage(lStagingImage);
}


void DepthTriangle::CreateTextureImageView(void)
{
    const vkpp::ImageViewCreateInfo lImageViewCreateInfo
    {
        mTextureImage, 
        vkpp::ImageViewType::e2D, vkpp::Format::eRGBA8uNorm,
        {vkpp::ComponentSwizzle::eIdentity, vkpp::ComponentSwizzle::eIdentity, vkpp::ComponentSwizzle::eIdentity, vkpp::ComponentSwizzle::eIdentity },
        {vkpp::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
    };

    mTextureImageView = mLogicalDevice.CreateImageView(lImageViewCreateInfo);
}


void DepthTriangle::CreateTextureSampler(void)
{
    const vkpp::SamplerCreateInfo lSamplerCreateInfo
    {
        vkpp::Filter::eLinear, vkpp::Filter::eLinear,
        vkpp::SamplerMipmapMode::eLinear,
        vkpp::SamplerAddressMode::eRepeat, vkpp::SamplerAddressMode::eMirroredRepeat, vkpp::SamplerAddressMode::eRepeat,
        0.0f, VK_FALSE, 1.0f,
        VK_FALSE, vkpp::CompareOp::eAlways,
        0.0f, 0.0f, vkpp::BorderColor::eIntOpaqueBlack
    };

    mTextureSampler = mLogicalDevice.CreateSampler(lSamplerCreateInfo);
}


void DepthTriangle::CreateDescriptorPool(void)
{
    const vkpp::DescriptorPoolSize lPoolSizes[]
    {
        { vkpp::DescriptorType::eUniformBuffer, 1 },
        { vkpp::DescriptorType::eCombinedImageSampler, 1 }
    };

    const vkpp::DescriptorPoolCreateInfo lPoolCreateInfo{ 2, lPoolSizes, 1 };

    mDescriptorPool = mLogicalDevice.CreateDescriptorPool(lPoolCreateInfo);
}


void DepthTriangle::CreateDescriptorSet(void)
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

    const vkpp::DescriptorImageInfo lImageInfo
    {
        mTextureSampler, mTextureImageView, vkpp::ImageLayout::eShaderReadOnlyOptimal
    };

    const vkpp::WriteDescriptorSetInfo lWriteInfos[]
    {
        {
            mDescriptorSet,
            0, 0,
            vkpp::DescriptorType::eUniformBuffer,
            lBufferInfo
        },
        {
            mDescriptorSet,
            1, 0,
            vkpp::DescriptorType::eCombinedImageSampler,
            lImageInfo
        }
    };

    mLogicalDevice.UpdateDescriptorSets(2, lWriteInfos);
}


void DepthTriangle::CopyBuffer(vkpp::Buffer& aDstBuffer, const vkpp::Buffer& aSrcBuffer, vkpp::DeviceSize aSize) const
{
    const auto& lCommandBuffer = BeginOneTimeCommandBuffer();

    const vkpp::BufferCopy lBufferCopy
    {
        0, 0, aSize
    };

    lCommandBuffer.Copy(aDstBuffer, aSrcBuffer, lBufferCopy);

    EndOneTimeCommandBuffer(lCommandBuffer);
}


void DepthTriangle::CopyImage(vkpp::Image& aDstImage, const vkpp::Image& aSrcImage, uint32_t aWidth, uint32_t aHeight) const
{
    const auto& lCommandBuffer = BeginOneTimeCommandBuffer();

    const vkpp::ImageSubresourceLayers lSubresourceLayers
    {
        vkpp::ImageAspectFlagBits::eColor, 0, 0, 1
    };

    const vkpp::ImageCopy lImageCopy
    {
        lSubresourceLayers, {0, 0, 0},
        lSubresourceLayers, {0, 0, 0},
        {aWidth, aHeight, 1}
    };

    lCommandBuffer.Copy(aDstImage, vkpp::ImageLayout::eTransferDstOptimal, aSrcImage, vkpp::ImageLayout::eTransferSrcOptimal, lImageCopy);

    EndOneTimeCommandBuffer(lCommandBuffer);
}


vkpp::DeviceMemory DepthTriangle::AllocateBufferMemory(const vkpp::Buffer& aBuffer, const vkpp::MemoryPropertyFlags& aMemoryProperties) const
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


vkpp::DeviceMemory DepthTriangle::AllocateImageMemory(const vkpp::Image& aImage, const vkpp::MemoryPropertyFlags& aMemoryProperties) const
{
    const auto& lImageMemRequirements = mLogicalDevice.GetImageMemoryRequirements(aImage);
    const auto& lDeviceMemoryProperties = mPhysicalDevice.GetMemoryProperties();

    for(uint32_t i = 0; i < lDeviceMemoryProperties.memoryTypeCount; ++i)
    {
        if(lImageMemRequirements.memoryTypeBits & (1 << i) && (lDeviceMemoryProperties.memoryTypes[i].propertyFlags & aMemoryProperties) == aMemoryProperties)
        {
            vkpp::MemoryAllocateInfo lMemoryAllocateInfo
            {
                lImageMemRequirements.size, i
            };

            return mLogicalDevice.AllocateMemory(lMemoryAllocateInfo);
        }
    }

    assert(false);
    return nullptr;
}


vkpp::Framebuffer DepthTriangle::CreateFramebuffer(const vkpp::ImageView& aImageView) const
{
    const std::array<vkpp::ImageView, 2> lAttachments{ aImageView, mDepthImageView };

    vkpp::FramebufferCreateInfo lFramebufferCreateInfo
    (
        mRenderPass,
        lAttachments,
        mSwapchain.mExtent.width, mSwapchain.mExtent.height
    );

    return mLogicalDevice.CreateFramebuffer(lFramebufferCreateInfo);
}


void DepthTriangle::PrepareFrame(vkpp::Framebuffer& aFramebuffer, const vkpp::CommandBuffer& aCommandBuffer, const vkpp::ImageView& aImageView) const
{
    if (aFramebuffer)
        mLogicalDevice.DestroyFramebuffer(aFramebuffer);

    aFramebuffer = CreateFramebuffer(aImageView);

    const vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo
    {
        vkpp::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    aCommandBuffer.Begin(lCmdBufferBeginInfo);

    const vkpp::ClearValue lClearValue[]
    {
        { 0.129411f, 0.156862f, 0.188235f, 1.0f },
        { 1.0f, 0.0f }
    };

    const vkpp::RenderPassBeginInfo lRenderPassBeginInfo
    {
        mRenderPass,
        aFramebuffer,
        {
            {0, 0},
            mSwapchain.mExtent
        },
        2, lClearValue
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


vkpp::CommandBuffer DepthTriangle::BeginOneTimeCommandBuffer(void) const
{
    const vkpp::CommandBufferAllocateInfo lCommandBufferAllocateInfo
    {
        mCommandPool, 1
    };

    auto lCommandBuffer = mLogicalDevice.AllocateCommandBuffer(lCommandBufferAllocateInfo);

    const vkpp::CommandBufferBeginInfo lCommandBufferBeginInfo
    {
        vkpp::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    lCommandBuffer.Begin(lCommandBufferBeginInfo);

    return lCommandBuffer;
}


void DepthTriangle::EndOneTimeCommandBuffer(const vkpp::CommandBuffer& aCommandBuffer) const
{
    aCommandBuffer.End();

    const vkpp::SubmitInfo lSubmitInfo
    {
        0, nullptr, nullptr,
        1, aCommandBuffer.AddressOf()
    };

    const vkpp::FenceCreateInfo lFenceCreateInfo;

    auto lFence = mLogicalDevice.CreateFence(lFenceCreateInfo);

    mGraphicsQueue.mQueue.Submit(lSubmitInfo, lFence);

    mLogicalDevice.WaitForFence(lFence);
    mLogicalDevice.DestroyFence(lFence);
    mLogicalDevice.FreeCommandBuffer(mCommandPool, aCommandBuffer);
}


template <vkpp::ImageLayout OldLayout, vkpp::ImageLayout NewLayout>
void DepthTriangle::TransitionImageLayout(const vkpp::Image& aImage, const vkpp::AccessFlags& aSrcAccessMask, const vkpp::AccessFlags& aDstAccessMask) const
{
    const auto& lCommandBuffer = BeginOneTimeCommandBuffer();

    const vkpp::ImageMemoryBarrier lImageMemoryBarrier
    {
        aSrcAccessMask, aDstAccessMask,
        OldLayout, NewLayout,
        aImage,
        {
            vkpp::ImageAspectFlagBits::eColor,
            0, 1, 0, 1
        }
    };

    const vkpp::CommandPipelineBarrier lCommandPipelineBarrier
    {
        vkpp::PipelineStageFlagBits::eTopOfPipe, vkpp::PipelineStageFlagBits::eTopOfPipe,
        vkpp::DependencyFlagBits::eByRegion,
        0, nullptr,
        0, nullptr,
        1, lImageMemoryBarrier.AddressOf()
    };

    lCommandBuffer.PipelineBarrier(lCommandPipelineBarrier);

    EndOneTimeCommandBuffer(lCommandBuffer);
}


void DepthTriangle::UpdateUniformBuffer(void)
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


void DepthTriangle::DrawFrame(void)
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
