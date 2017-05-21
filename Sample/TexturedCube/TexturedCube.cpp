#include "TexturedCube.h"



namespace vkpp::sample
{



TexturedCube::TexturedCube(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName, uint32_t aEngineVersion)
    : ExampleBase(aWindow, apAppName, aAppVersion, apEngineName, aEngineVersion),
      CWindowEvent(aWindow),
      mDepthRes(mLogicalDevice, mPhysicalDeviceMemoryProperties)
{
    CreateCommandPool();
    AllocateCommandBuffers();

    CreateDepthResource();
    CreateRenderPass();
    CreateFramebuffers();
}


TexturedCube::~TexturedCube(void)
{
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
            vkpp::AttachmentLoadOp::eDontCare, vkpp::AttachmentStoreOp::eDontCare,
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
    std::vector<vkpp::FramebufferCreateInfo> lFramebufferCreateInfos;

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

        lFramebufferCreateInfos.emplace_back(lFramebufferCreateInfo);
    }

    mFramebuffers = mLogicalDevice.CreateFramebuffers(lFramebufferCreateInfos);
}


}                   // End of namespace vkpp::sample.