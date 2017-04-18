#include "Triangle/Triangle.h"

#include <algorithm>
#include <iterator>
#include <cassert>
#include <fstream>
#include <cstddef>

#include <Info/Common.h>
#include <Info/Layers.h>
#include <Info/Extensions.h>
#include <Info/RenderPassBeginInfo.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>



namespace sample
{




constexpr std::array<const char*, 1> gRequiredLayers{
    LUNARG_STANDARD_VALIDATION_NAME
};


constexpr std::array<const char*, 3> gRequiredInstanceExtensions{
    KHR_SURFACE_EXT_NAME,
    KHR_OS_SURFACE_EXT_NAME,
    EXT_DEBUG_REPORT_EXT_NAME
};


constexpr std::array<const char*, 1> gRequiredDeviceExtensions{
    KHR_SWAPCHAIN_EXT_NAME
};



void PrintApiVersion(uint32_t aApiVersion)
{
    auto lMajorVersion = VK_VERSION_MAJOR(aApiVersion);
    auto lMinorVersion = VK_VERSION_MINOR(aApiVersion);
    auto lPatchVersion = VK_VERSION_PATCH(aApiVersion);

    std::cout << "Vulkan API Version: " << lMajorVersion << '.' << lMinorVersion << '.' << lPatchVersion << std::endl;
}


VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT /*aFlags*/, VkDebugReportObjectTypeEXT /*aObjType*/,
    uint64_t /*aObj*/, std::size_t /*aLocation*/, int32_t /*aCode*/, const char* /*apLayerPrefix*/, const char* apMsg, void* /*apUserData*/)
{
    std::cerr << "Validation Layer: " << apMsg << std::endl;

    return VK_FALSE;
}


void Triangle::CheckValidationLayerSupport(void) const
{
    auto lLayers = vkpp::Instance::GetLayers();

    std::cout << "Instance/Device Layers:\n";

    for (const auto& lLayer : lLayers)
        std::cout << '\t' << lLayer.layerName << '\t' << lLayer.specVersion << '\t' << lLayer.implementationVersion << '\t' << lLayer.description << std::endl;

    std::cout << std::endl;
}


void Triangle::CheckValidationExtensions(void) const
{
    auto lExtensions = vkpp::Instance::GetExtensions(/*LUNARG_STANDARD_VALIDATION_NAME*/);

    std::cout << "Instance Extensions: \n";

    for (const auto& lExtension : lExtensions)
        std::cout << '\t' << lExtension.extensionName << '\t' << lExtension.specVersion << std::endl;

    std::cout << std::endl;
}


bool Triangle::CheckPhysicalDeviceProperties(const vkpp::PhysicalDevice& aPhysicalDevice)
{
    auto lPhysicalLayers = aPhysicalDevice.GetLayers();

    std::cout << "Device Layers:\n";
    for(const auto& lLayer : lPhysicalLayers)
        std::cout << '\t' << lLayer.layerName << '\t' << lLayer.specVersion << '\t' << lLayer.implementationVersion << '\t' << lLayer.description << std::endl;

    std::cout << std::endl;

    auto lPhysicalExtensions = aPhysicalDevice.GetExtensions();

    std::cout << "Device Extensions:\n";
    for (const auto& lExtension : lPhysicalExtensions)
        std::cout << '\t' << lExtension.extensionName << '\t' << lExtension.specVersion << std::endl;

    std::cout << std::endl;

    auto lPhysicalDeviceProperties = aPhysicalDevice.GetProperties();
    auto lPhysicalDeviceFeatures = aPhysicalDevice.GetFeatures();

    PrintApiVersion(lPhysicalDeviceProperties.apiVersion);

    auto lQueueFamilyProperties = aPhysicalDevice.GetQueueFamilyProperties();
    auto lFamilyIter = std::find_if(lQueueFamilyProperties.cbegin(), lQueueFamilyProperties.cend(),
        [](auto&& aProperty) {
            return aProperty.queueFlags & vkpp::QueueFlagBits::eGraphics;
    });

    if (lFamilyIter != lQueueFamilyProperties.cend())
    {
        mGraphicsQueueFamilyIndex = static_cast<uint32_t>(std::distance(lQueueFamilyProperties.cbegin(), lFamilyIter));

        if (aPhysicalDevice.IsSurfaceSupported(mGraphicsQueueFamilyIndex, mSurface))
        {
            mPresentQueueFamilyIndex = mGraphicsQueueFamilyIndex;
            return true;
        }
    }

    return false;
}


void Triangle::SetupDebugCallback(void)
{
    vkpp::ext::DebugReportCallbackCreateInfo lDebugReportCallbackInfo{
        vkpp::ext::DebugReportFlags(vkpp::ext::DebugReportFlagBits::eError) | vkpp::ext::DebugReportFlagBits::eWarning,
        DebugCallback
    };

    mDebugReportCallback = mInstance.CreateDebugReportCallback(lDebugReportCallbackInfo);
}


uint32_t Triangle::GetSwapchainImageCount(void) const
{
    auto lSurfaceCapabilities = mPhysicalDevice.GetSurfaceCapabilities(mSurface);

    uint32_t lImageCount = lSurfaceCapabilities.minImageCount + 1;

    if (lSurfaceCapabilities.maxImageCount > 0 && lImageCount > lSurfaceCapabilities.maxImageCount)
        lImageCount = lSurfaceCapabilities.maxImageCount;

    return lImageCount;
}


vkpp::khr::SurfaceFormat Triangle::GetSwapchainFormat(void) const
{
    auto lSurfaceFormats = mPhysicalDevice.GetSurfaceFormats(mSurface);

    if (lSurfaceFormats.size() == 1 && lSurfaceFormats[0].format == vkpp::Format::eUndefined)
        return { vkpp::Format::eR8G8B8A8Unorm, vkpp::khr::ColorSpace::esRGBNonLinear };

    for (vkpp::khr::SurfaceFormat& lSurfaceFormat : lSurfaceFormats)
    {
        if (lSurfaceFormat.format == vkpp::Format::eR8G8B8A8Unorm)
            return lSurfaceFormat;
    }

    return lSurfaceFormats[0];
}


vkpp::Extent2D Triangle::GetSwapchainExtent(void) const
{
    // TODO:
    auto lSurfaceCapabilities = mPhysicalDevice.GetSurfaceCapabilities(mSurface);

    return lSurfaceCapabilities.currentExtent;
}


vkpp::ImageUsageFlags Triangle::GetSwapchainUsageFlags(void) const
{
    // TODO:
    auto lSurfaceCapabilities = mPhysicalDevice.GetSurfaceCapabilities(mSurface);

    if (lSurfaceCapabilities.supportedUsageFlags & vkpp::ImageUsageFlagBits::eTransferDst)
        return vkpp::ImageUsageFlags(vkpp::ImageUsageFlagBits::eColorAttachment) | vkpp::ImageUsageFlagBits::eTransferDst;

    return vkpp::DefaultFlags;
}


vkpp::khr::SurfaceTransformFlagBits Triangle::GetSwapchainTransform(void) const
{
    // TODO:
    auto lSurfaceCapabilities = mPhysicalDevice.GetSurfaceCapabilities(mSurface);

    return lSurfaceCapabilities.currentTransform;
}


vkpp::khr::PresentMode Triangle::GetSwapchainPresentMode(void) const
{
    auto lPresentModes = mPhysicalDevice.GetSurfacePresentModes(mSurface);

    for (auto& lPresentMode : lPresentModes)
    {
        if (lPresentMode == vkpp::khr::PresentMode::eMailBox)
            return lPresentMode;
    }

    return vkpp::khr::PresentMode::eFIFO;
}


void Triangle::CreateInstance(void)
{
    CheckValidationLayerSupport();
    CheckValidationExtensions();

    vkpp::ApplicationInfo lAppInfo{ "Hello Vulkan", VK_MAKE_VERSION(1, 0, 0) };
    vkpp::InstanceInfo lInstanceInfo{ lAppInfo, gRequiredLayers, gRequiredInstanceExtensions };

    mInstance.Reset(lInstanceInfo);
}


void Triangle::CreateSurface(void)
{
    vkpp::khr::SurfaceCreateInfo lSurfaceCreateInfo{
        glfwGetWin32Window(mpWindow)
    };

    mSurface = mInstance.CreateSurface(lSurfaceCreateInfo);         // TODO: Release previous surface.
}


bool Triangle::PickPhysicalDevice(void)
{
    auto lPhysicalDevices = mInstance.GetPhysicalDevices();
    auto lDeviceIter = std::find_if(lPhysicalDevices.cbegin(), lPhysicalDevices.cend(),
        [this](auto&& aDevice) {
        return CheckPhysicalDeviceProperties(aDevice);
    });

    if (lDeviceIter != lPhysicalDevices.cend())
    {
        mPhysicalDevice = *lDeviceIter;
        return true;
    }

    return false;
}


void Triangle::CreateLogicalDevice(void)
{
    assert(mGraphicsQueueFamilyIndex == mPresentQueueFamilyIndex);

    const std::array<float, 1> lPriorities{ 1.0f };

    vkpp::QueueCreateInfo lQueueCreateInfo{
        mGraphicsQueueFamilyIndex,
        lPriorities
    };

    vkpp::LogicalDeviceCreateInfo lLogicalDeiveInfo (
        1, lQueueCreateInfo.AddressOf(),
        static_cast<uint32_t>(gRequiredDeviceExtensions.size()), gRequiredDeviceExtensions.data(),
        nullptr
    );

    mLogicalDevice = mPhysicalDevice.CreateLogicalDevice(lLogicalDeiveInfo);
}


void Triangle::GetDeviceQueue(void)
{
    mGraphicsQueue = mLogicalDevice.GetQueue(mGraphicsQueueFamilyIndex, 0);
    mPresentQueue = mLogicalDevice.GetQueue(mPresentQueueFamilyIndex, 0);
}


void Triangle::CreateSemaphore(void)
{
    vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    mImageAvailSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mRenderingFinishedSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void Triangle::CreateSwapChain(void)
{
    auto lDesiredImageCount = GetSwapchainImageCount();
    auto lDesiredFormat     = GetSwapchainFormat();
    auto lDesiredExtent     = GetSwapchainExtent();
    auto lDesiredUsage      = GetSwapchainUsageFlags();
    auto lDesiredTransform  = GetSwapchainTransform();
    auto lDesiredPresentMode = GetSwapchainPresentMode();
    auto lOldSwapchain      = mSwapchain;

    vkpp::khr::SwapchainCreateInfo lSwapchainCreateInfo{
        mSurface,
        lDesiredImageCount, lDesiredFormat.format, lDesiredFormat.colorSpace, lDesiredExtent, lDesiredUsage,
        lDesiredTransform, vkpp::khr::CompositeAlphaFlagBits::eOpaque,
        lDesiredPresentMode,
        lOldSwapchain
    };

    mSwapchain = mLogicalDevice.CreateSwapchain(lSwapchainCreateInfo);
    mSwapchainImages = mLogicalDevice.GetSwapchainImages(mSwapchain);
}


void Triangle::CreateSwapchainImageViews(void)
{
    for (const auto& lImage : mSwapchainImages)
    {
        vkpp::ImageViewCreateInfo lImageViewCreateInfo
        {
            lImage,
            vkpp::ImageViewType::e2D, 
            GetSwapchainFormat().format,
            {
                vkpp::ComponentSwizzle::eIdentity,
                vkpp::ComponentSwizzle::eIdentity,
                vkpp::ComponentSwizzle::eIdentity,
                vkpp::ComponentSwizzle::eIdentity
            },
            {
                vkpp::ImageAspectFlagBits::eColor,
                0, 1,
                0, 1
            }
        };

        mSwapchainImageViews.emplace_back(mLogicalDevice.CreateImageView(lImageViewCreateInfo));
    }
}


void Triangle::CreateCommandBuffers(void)
{
    vkpp::CommandPoolCreateInfo lCmdPoolCreateInfo{
        mPresentQueueFamilyIndex
    };

    mPresentQueueCmdPool = mLogicalDevice.CreateCommandPool(lCmdPoolCreateInfo);

    auto lImageCount = static_cast<uint32_t>(mSwapchainImages.size());

    mPresentQueueCmdBuffers.resize(lImageCount);

    vkpp::CommandBufferAllocateInfo lCmdBufferAllocateInfo{
        mPresentQueueCmdPool, static_cast<uint32_t>(lImageCount)
    };

    mPresentQueueCmdBuffers = mLogicalDevice.AllocateCommandBuffers(lCmdBufferAllocateInfo);
}


void Triangle::RecordCommandBuffers(void)
{
    vkpp::CommandBufferBeginInfo lCmdBufferBeginInfo{
        vkpp::CommandBufferUsageFlagBits::eSimultaneousUse
    };

    vkpp::ClearValue lClearColor{ 1.0f, 0.8f, 0.4f, 0.0f };

    vkpp::ImageSubresourceRange lImageSubRange{
        vkpp::ImageAspectFlagBits::eColor,
        0, 1,
        0, 1
    };

    for (std::size_t i = 0; i < mSwapchainImages.size(); ++i)
    {
        /*vkpp::ImageMemoryBarrier lBarrierFromPresentToClear{
            vkpp::AccessFlagBits::eMemoryRead, vkpp::AccessFlagBits::eTransferWrite,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eTransferDstOptimal,
            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
            mSwapchainImages[i], lImageSubRange
        };

        vkpp::ImageMemoryBarrier lBarrierFromClearToPresent{
            vkpp::AccessFlagBits::eTransferWrite, vkpp::AccessFlagBits::eMemoryRead,
            vkpp::ImageLayout::eTransferDstOptimal, vkpp::ImageLayout::ePresentSrcKHR,
            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
            mSwapchainImages[i], lImageSubRange
        };

        vkpp::CommandPipelineBarrier lCmdPipelineBarrier{
            vkpp::PipelineStageFlagBits::eTransfer, vkpp::PipelineStageFlagBits::eTransfer, vkpp::DependencyFlags(),
            0, nullptr,
            0, nullptr,
            1, lBarrierFromPresentToClear.AddressOf()
        };*/

        const auto lCmdBuffer = mPresentQueueCmdBuffers[i];

        lCmdBuffer.Begin(lCmdBufferBeginInfo);
        // lCmdBuffer.PipelineBarrier(lCmdPipelineBarrier);

        vkpp::RenderPassBeginInfo lRenderPassBeginInfo
        {
            mRenderPass, mFramebuffers[i],
            {
                {0, 0},
                {300, 300}
            },
            1, &lClearColor
        };

        mPresentQueueCmdBuffers[i].BeginRenderPass(lRenderPassBeginInfo, vkpp::SubpassContents::eInline);

        mPresentQueueCmdBuffers[i].BindGraphicsPipeline(mGraphicsPipeline);
        mPresentQueueCmdBuffers[i].Draw(3, 1);

        mPresentQueueCmdBuffers[i].EndRenderPass();

        // lCmdBuffer.ClearColorImage(mSwapchainImages[i], vkpp::ImageLayout::eTransferDstOptimal, lClearColor, { lImageSubRange });

        /*vkpp::CommandPipelineBarrier lCmdPipelineBarrier2{
            vkpp::PipelineStageFlagBits::eTransfer, vkpp::PipelineStageFlagBits::eBottomOfPipe, vkpp::DependencyFlags(),
            0, nullptr,
            0, nullptr,
            1, lBarrierFromClearToPresent.AddressOf()
        };

        lCmdPipelineBarrier.dstStageMask = vkpp::PipelineStageFlagBits::eBottomOfPipe;
        lCmdPipelineBarrier.pImageMemoryBarriers = lBarrierFromClearToPresent.AddressOf();
        lCmdBuffer.PipelineBarrier(lCmdPipelineBarrier);*/

        lCmdBuffer.End();
    }
}


void Triangle::CreateRenderPass(void)
{
    vkpp::AttachementDescription lAttachmentDescriptions[]
    {
        {
            GetSwapchainFormat().format,  vkpp::SampleCountFlagBits::e1, vkpp::AttachmentLoadOp::eClear, vkpp::AttachmentStoreOp::eStore,
            vkpp::AttachmentLoadOp::eDontCare, vkpp::AttachmentStoreOp::eDontCare, vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::ePresentSrcKHR
        }
    };

    vkpp::AttachmentReference lColorAttachmentReferences[]
    {
        {0, vkpp::ImageLayout::eColorAttachmentOptimal},
    };

    vkpp::SubpassDescription lSubpassDescriptions[]
    {
        {
            vkpp::PipelineBindPoint::eGraphics,
            0, nullptr,
            1, lColorAttachmentReferences,
        }
    };

    vkpp::RenderPassCreateInfo lRenderPassCreateInfo
    {
        1, lAttachmentDescriptions,
        1, lSubpassDescriptions
    };

    mRenderPass = mLogicalDevice.CreateRenderPass(lRenderPassCreateInfo);
}


void Triangle::CreateFrameBuffers(void)
{
    for (const auto& lImageView : mSwapchainImageViews)
    {
        vkpp::FrameBufferCreateInfo lFramebufferCreateInfo
        {
            mRenderPass,
            1, lImageView.AddressOf(),
            300, 300, 1
        };

        mFramebuffers.emplace_back(mLogicalDevice.CreateFrameBuffer(lFramebufferCreateInfo));
    }
}


vkpp::ShaderModule Triangle::CreateShaderModule(const std::string& aFilename) const
{
    std::ifstream lFin(aFilename, std::ios::binary);

    std::vector<char> lShaderContent((std::istreambuf_iterator<char>(lFin)), std::istreambuf_iterator<char>());
    assert(!lShaderContent.empty());

    /*vkpp::ShaderModuleCreateInfo lShaderModuleCreateInfo
    {
        lShaderContent
    };*/

    return mLogicalDevice.CreateShaderModule({ lShaderContent });
}


vkpp::PipelineLayout Triangle::CreatePipelineLayout(void) const
{
    vkpp::PipelineLayoutCreateInfo lLayoutCreateInfo
    {
        0, nullptr,
        0, nullptr
    };

    return mLogicalDevice.CreatePipelineLayout(lLayoutCreateInfo);
}


void Triangle::CreatePipeline(void)
{
    auto lVertexShaderModule = CreateShaderModule("Shader/vert.spv");
    auto lFragmentShaderModule = CreateShaderModule("Shader/frag.spv");

    std::vector<vkpp::PipelineShaderStageCreateInfo> lShaderStageCreateInfos
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

    vkpp::PipelineVertexInputStateCreateInfo lVertexInputStateCreateInfo
    {
        0, nullptr,
        0, nullptr
    };

    vkpp::PipelineInputAssemblyStateCreateInfo lInputAssemblyStateCreateInfo
    {
        vkpp::PrimitiveTopology::eTriangleList
    };

    vkpp::Viewport lViewport
    {
        0.0f, 0.0f,
        300.0f, 300.0f,
        0.0f, 1.0f
    };

    vkpp::Rect2D lScissor
    {
        {0, 0} ,
        {300, 300}
    };

    vkpp::PipelineViewportStateCreateInfo lViewportStateCreatInfo
    {
        1, lViewport.AddressOf(),
        1, lScissor.AddressOf()
    };

    vkpp::PipelineRasterizationStateCreateInfo lRasterizationCreateInfo
    {
        VK_FALSE, VK_FALSE,
        vkpp::PolygonMode::eFill, vkpp::CullModeFlagBits::eBack, vkpp::FrontFace::eCounterClockwise,
        VK_FALSE, 0, 0, 0, 1.0f
    };

    vkpp::PipelineMultisampleStateCreateInfo lMultisampleStateCreatInfo
    {
        vkpp::SampleCountFlagBits::e1, VK_FALSE,
        1.0f, nullptr,
        VK_FALSE, VK_FALSE
    };

    vkpp::PipelineColorBlendAttachmentState lColorBlendAttachmentState
    {
        VK_FALSE,
        vkpp::BlendFactor::eOne, vkpp::BlendFactor::eZero, vkpp::BlendOp::eAdd,
        vkpp::BlendFactor::eOne, vkpp::BlendFactor::eZero, vkpp::BlendOp::eAdd,
        vkpp::ColorComponentFlags(vkpp::ColorComponentFlagBits::eR) | vkpp::ColorComponentFlagBits::eG |
        vkpp::ColorComponentFlags(vkpp::ColorComponentFlagBits::eB) | vkpp::ColorComponentFlagBits::eA
    };

    vkpp::PipelineColorBlendStateCreateInfo lColorBlendStateCreateInfo
    {
        VK_FALSE,
        vkpp::LogicalOp::eCopy,
        1, lColorBlendAttachmentState.AddressOf(),
        {0, 0, 0, 0}
    };

    auto lPipelineLayout = CreatePipelineLayout();

    vkpp::GraphicsPipelineCreateInfo lPipelineCreateInfo
    {
        static_cast<uint32_t>(lShaderStageCreateInfos.size()), lShaderStageCreateInfos.data(),
        lVertexInputStateCreateInfo.AddressOf(), lInputAssemblyStateCreateInfo.AddressOf(), nullptr,
        lViewportStateCreatInfo.AddressOf(), lRasterizationCreateInfo.AddressOf(), lMultisampleStateCreatInfo.AddressOf(),
        nullptr, lColorBlendStateCreateInfo.AddressOf(), nullptr, lPipelineLayout, mRenderPass, 0
    };

    mGraphicsPipeline = mLogicalDevice.CreateGraphicsPipeline(nullptr, 1, lPipelineCreateInfo.AddressOf());

    mLogicalDevice.DestroyPipelineLayout(lPipelineLayout);
    mLogicalDevice.DestroyShaderModule(lFragmentShaderModule);
    mLogicalDevice.DestroyShaderModule(lVertexShaderModule);
}


void Triangle::InitWindow(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mpWindow = glfwCreateWindow(1024, 768, "Hello Vulkan", nullptr, nullptr);
}


void Triangle::InitVulkan(void)
{
    CreateInstance();
    SetupDebugCallback();
    CreateSurface();

    PickPhysicalDevice();
    CreateLogicalDevice();
    GetDeviceQueue();

    CreateSwapChain();
    CreateSwapchainImageViews();

    CreateRenderPass();
    CreateFrameBuffers();

    CreatePipeline();

    CreateSemaphore();

    CreateCommandBuffers();
    RecordCommandBuffers();
}


void Triangle::MainLoop(void)
{
    while (!glfwWindowShouldClose(mpWindow))
    {
        glfwPollEvents();
        DrawFrame();
    }

    mLogicalDevice.Wait();
    glfwDestroyWindow(mpWindow);
    mpWindow = nullptr;

    glfwTerminate();
}


void Triangle::DrawFrame(void)
{
    auto lImageIndex = mLogicalDevice.AcquireNextImage(mSwapchain, std::numeric_limits<uint32_t>::max(), mImageAvailSemaphore, nullptr);

    vkpp::PipelineStageFlags lWaitDstStageMask{ vkpp::PipelineStageFlagBits::eTransfer };

    vkpp::SubmitInfo lSubmitInfo{
        1, mImageAvailSemaphore.AddressOf(),
        &lWaitDstStageMask,
        1, mPresentQueueCmdBuffers[lImageIndex].AddressOf(),
        1, mRenderingFinishedSemaphore.AddressOf()
    };

    mPresentQueue.Submit(1, lSubmitInfo.AddressOf());

    vkpp::khr::PresentInfo lPresentInfo{
        1, mRenderingFinishedSemaphore.AddressOf(),
        1, mSwapchain.AddressOf(), &lImageIndex,
        nullptr
    };

    mPresentQueue.Present(lPresentInfo);
}



}                   // End of namespace sample.