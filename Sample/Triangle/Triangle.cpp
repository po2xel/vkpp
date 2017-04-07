#include "Triangle/Triangle.h"

#include <algorithm>
#include <iterator>
#include <cassert>

#include <Info/Common.h>
#include <Info/Layers.h>
#include <Info/Extensions.h>

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

    return vkpp::ImageUsageFlags();
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
        nullptr, glfwGetWin32Window(mpWindow)
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
    CreateSemaphore();

    CreateSwapChain();
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

}



}                   // End of namespace sample.