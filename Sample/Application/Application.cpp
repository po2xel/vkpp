#include "Application/Application.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <cassert>
#include <fstream>

#include <GLFW/glfw3native.h>



namespace
{



VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT /*aFlags*/, VkDebugReportObjectTypeEXT /*aObjType*/,
    uint64_t /*aObj*/, std::size_t /*aLocation*/, int32_t /*aCode*/, const char* /*apLayerPrefix*/, const char* apMsg, void* /*apUserData*/)
{
    std::cerr << "Validation Layer: " << apMsg << std::endl;

    return VK_FALSE;
}


void PrintApiVersion(uint32_t aApiVersion)
{
    auto lMajorVersion = VK_VERSION_MAJOR(aApiVersion);
    auto lMinorVersion = VK_VERSION_MINOR(aApiVersion);
    auto lPatchVersion = VK_VERSION_PATCH(aApiVersion);

    std::cout << "Vulkan API Version: " << lMajorVersion << '.' << lMinorVersion << '.' << lPatchVersion << std::endl;
}



}



namespace sample
{



Application::Application(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion)
{
    assert(apApplicationName != nullptr);

    CreateInstance(apApplicationName, aApplicationVersion, apEngineName, aEngineVersion);

    SetupDebugCallback();

    CreateNativeWindow(apApplicationName);
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    GetDeviceQueues();

    CreateSwapchain();
    CreateSwapchainImageViews();
}


Application::~Application(void)
{
#ifdef _DEBUG
    mInstance.DestroyDebugReportCallback(mDebugReportCallback);
#endif              // End of _DEBUG

    for (auto& lSwapchainImageView : mSwapchain.mSwapchainImageViews)
        mLogicalDevice.DestroyImageView(lSwapchainImageView);

    mLogicalDevice.DestroySwapchain(mSwapchain.Handle);
    mLogicalDevice.Reset();
    mInstance.DestroySurface(mSurface);

    if (mpWindow != nullptr)
    {
        glfwDestroyWindow(mpWindow);
        mpWindow = nullptr;
    }
}


void Application::CreateNativeWindow(const char* apTitle)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mpWindow = glfwCreateWindow(1024, 768, apTitle, nullptr, nullptr);
}


void Application::CreateInstance(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion)
{
    const auto& lLayers = vkpp::Instance::GetLayers();

    std::cout << "Instance Layers:\n";
    for (auto& lLayer : lLayers)
        std::cout << '\t' << lLayer.layerName << '\t' << lLayer.specVersion << '\t' << lLayer.implementationVersion << '\t' << lLayer.description << std::endl;

    std::cout << std::endl;

    const auto& lExtensions = vkpp::Instance::GetExtensions();

    std::cout << "Instance Extensions:\n";
    for (auto& lExt : lExtensions)
        std::cout << '\t' << lExt.extensionName << '\t' << lExt.specVersion << std::endl;

    std::cout << std::endl;

    const vkpp::ApplicationInfo lApplicationInfo
    {
        apApplicationName, aApplicationVersion, apEngineName, aEngineVersion
    };

    constexpr std::array<const char*, 1> lInstanceLayers
    {
        LUNARG_STANDARD_VALIDATION_NAME
    };

    constexpr std::array<const char*, 3> lInstanceExts
    {
        KHR_SURFACE_EXT_NAME,
        KHR_OS_SURFACE_EXT_NAME,
        EXT_DEBUG_REPORT_EXT_NAME
    };

    const vkpp::InstanceInfo lInstanceInfo
    {
        lApplicationInfo,
        lInstanceLayers,
        lInstanceExts
    };

    mInstance.Reset(lInstanceInfo);
}


void Application::SetupDebugCallback(void)
{
#ifdef _DEBUG
    const vkpp::ext::DebugReportCallbackCreateInfo lDebugReportCallbackCreateInfo
    {
        vkpp::ext::DebugReportFlags(vkpp::ext::DebugReportFlagBits::eError) | vkpp::ext::DebugReportFlagBits::eWarning,
        DebugCallback
    };

    mDebugReportCallback = mInstance.CreateDebugReportCallback(lDebugReportCallbackCreateInfo);
#endif              // End of _DEBUG
}


void Application::CreateSurface(void)
{
    const vkpp::khr::SurfaceCreateInfo lSurfaceCreateInfo
    {
        glfwGetWin32Window(mpWindow)
    };

    mSurface = mInstance.CreateSurface(lSurfaceCreateInfo);
}


void Application::PickPhysicalDevice(void)
{
    const auto& lPhysicalDevices = mInstance.GetPhysicalDevices();

    auto lDeviceIter = std::find_if(lPhysicalDevices.cbegin(), lPhysicalDevices.cend(),
        [this](auto&& aDevice) {
        return CheckPhysicalDeviceProperties(aDevice);
    });

    assert(lDeviceIter != lPhysicalDevices.cend());
    mPhysicalDevice = *lDeviceIter;
}


bool Application::CheckPhysicalDeviceProperties(const vkpp::PhysicalDevice& aPhysicalDevice)
{
    const auto& lPhysicalLayers = aPhysicalDevice.GetLayers();

    std::cout << "Physical Device Layers:\n";
    for (auto& lLayer : lPhysicalLayers)
        std::cout << '\t' << lLayer.layerName << '\t' << lLayer.specVersion << '\t' << lLayer.implementationVersion << '\t' << lLayer.description << std::endl;

    std::cout << std::endl;

    const auto& lPhysicalExts = aPhysicalDevice.GetExtensions();
    std::cout << "Physical Device Extensions:\n";
    for (auto& lPhysicalExt : lPhysicalExts)
        std::cout << '\t' << lPhysicalExt.extensionName << '\t' << lPhysicalExt.specVersion << std::endl;

    std::cout << std::endl;

    const auto& lPhysicalDeviceProperties = aPhysicalDevice.GetProperties();
    PrintApiVersion(lPhysicalDeviceProperties.apiVersion);

    // const auto& lPhysicalDeviceFeatures = aPhysicalDevice.GetFeatures();

    const auto& lQueueFamilyProperties = aPhysicalDevice.GetQueueFamilyProperties();

    for(uint32_t lIndex = 0; lIndex < lQueueFamilyProperties.size(); ++lIndex)
    {
        if (lQueueFamilyProperties[lIndex].queueFlags & vkpp::QueueFlagBits::eGraphics)
        {
            mGraphicsQueue.mFamilyIndex = lIndex;

            if (aPhysicalDevice.IsSurfaceSupported(lIndex, mSurface))
            {
                mPresentQueue.mFamilyIndex = lIndex;

                return true;
            }
        }
    }

    return false;
}


void Application::CreateLogicalDevice(void)
{
    constexpr std::array<float, 1> lPriorities{ 1.0f };

    std::vector<vkpp::QueueCreateInfo> lQueueCreateInfos;
    lQueueCreateInfos.emplace_back(mGraphicsQueue.mFamilyIndex, lPriorities);

    constexpr std::array<const char*, 1> lDevcieExts
    {
        KHR_SWAPCHAIN_EXT_NAME
    };

    const vkpp::LogicalDeviceCreateInfo lLogicalDeviceInfo
    (
        lQueueCreateInfos,
        lDevcieExts
    );

    mLogicalDevice.Reset(mPhysicalDevice, lLogicalDeviceInfo);
}


void Application::GetDeviceQueues(void)
{
    mGraphicsQueue.mQueue = mLogicalDevice.GetQueue(mGraphicsQueue.mFamilyIndex, 0);
    mPresentQueue.mQueue = mLogicalDevice.GetQueue(mPresentQueue.mFamilyIndex, 0);
}


void Application::CreateSwapchain(void)
{
    if (mSwapchain.Handle)
        mLogicalDevice.Wait();

    const auto lSurfaceFormats = mPhysicalDevice.GetSurfaceFormats(mSurface);
    const auto lDesiredFormat = GetSwapchainFormat(lSurfaceFormats);

    const auto lSurfaceCapabilities = mPhysicalDevice.GetSurfaceCapabilities(mSurface);
    const auto lDesiredImageCount = GetSwapchainImageCount(lSurfaceCapabilities);
    const auto lDesiredExtent = GetSwapchainExtent(lSurfaceCapabilities);
    const auto lDesiredUsage = GetSwapchainUsageFlags(lSurfaceCapabilities);
    const auto lDesiredTransform = GetSwapchainTransform(lSurfaceCapabilities);

    const auto lSurfacePresentModes = mPhysicalDevice.GetSurfacePresentModes(mSurface);
    const auto lDesiredPresentMode = GetSwapchainPresentMode(lSurfacePresentModes);

    auto lOldSwapchain = mSwapchain.Handle;

    const vkpp::khr::SwapchainCreateInfo lSwapchainCreateInfo
    {
        mSurface,
        lDesiredImageCount, lDesiredFormat.format, lDesiredFormat.colorSpace, lDesiredExtent, lDesiredUsage,
        lDesiredTransform, vkpp::khr::CompositeAlphaFlagBits::eOpaque,
        lDesiredPresentMode, lOldSwapchain
    };

    mSwapchain.Handle = mLogicalDevice.CreateSwapchain(lSwapchainCreateInfo);
    mSwapchain.mSurfaceFormat = lDesiredFormat;

    if (lOldSwapchain)
        mLogicalDevice.DestroySwapchain(lOldSwapchain);

    mSwapchain.mSwapchainImages = mLogicalDevice.GetSwapchainImages(mSwapchain.Handle);
    mSwapchain.mExtent = lDesiredExtent;
}


void Application::CreateSwapchainImageViews(void)
{
    for (auto& lSwapchainImage : mSwapchain.mSwapchainImages)
    {
        vkpp::ImageViewCreateInfo lImageViewCreateInfo
        {
            lSwapchainImage, vkpp::ImageViewType::e2D, mSwapchain.mSurfaceFormat.format,
            {
                vkpp::ComponentSwizzle::eIdentity, vkpp::ComponentSwizzle::eIdentity,
                vkpp::ComponentSwizzle::eIdentity, vkpp::ComponentSwizzle::eIdentity
            },
            {
                vkpp::ImageAspectFlagBits::eColor, 
                0, 1,
                0, 1
            }
        };

        mSwapchain.mSwapchainImageViews.emplace_back(mLogicalDevice.CreateImageView(lImageViewCreateInfo));
    }
}


void Application::MainLoop(void)
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


uint32_t Application::GetSwapchainImageCount(const vkpp::khr::SurfaceCapabilities& aSurfaceCapabilities)
{
    auto lImageCount = aSurfaceCapabilities.minImageCount + 1;

    if (aSurfaceCapabilities.maxImageCount > 0 && aSurfaceCapabilities.maxImageCount < lImageCount)
        lImageCount = aSurfaceCapabilities.maxImageCount;

    return lImageCount;
}


vkpp::khr::SurfaceFormat Application::GetSwapchainFormat(const std::vector<vkpp::khr::SurfaceFormat>& aSurfaceFormats)
{
    if (aSurfaceFormats.size() == 1 && aSurfaceFormats[0].format == vkpp::Format::eUndefined)
        return { vkpp::Format::eRGBA8uNorm, vkpp::khr::ColorSpace::esRGBNonLinear };

    for (auto& lSurfaceFormat : aSurfaceFormats)
    {
        if (lSurfaceFormat.format == vkpp::Format::eRGBA8uNorm)
            return lSurfaceFormat;
    }

    return aSurfaceFormats[0];
}


// TODO
vkpp::Extent2D Application::GetSwapchainExtent(const vkpp::khr::SurfaceCapabilities& aSurfaceCapabilities)
{
    return aSurfaceCapabilities.currentExtent;
}


// TODO
vkpp::ImageUsageFlags Application::GetSwapchainUsageFlags(const vkpp::khr::SurfaceCapabilities& aSurfaceCapabilities)
{
    if (aSurfaceCapabilities.supportedUsageFlags & vkpp::ImageUsageFlagBits::eColorAttachment)
        return vkpp::ImageUsageFlags(vkpp::ImageUsageFlagBits::eColorAttachment) | vkpp::ImageUsageFlagBits::eTransferDst;

    assert(false);
    return vkpp::DefaultFlags;
}


vkpp::khr::SurfaceTransformFlagBits Application::GetSwapchainTransform(const vkpp::khr::SurfaceCapabilities& aSurfaceCapabilities)
{
    if (aSurfaceCapabilities.supportedTransforms & vkpp::khr::SurfaceTransformFlagBits::eIdentity)
        return vkpp::khr::SurfaceTransformFlagBits::eIdentity;

    return aSurfaceCapabilities.currentTransform;
}


vkpp::khr::PresentMode Application::GetSwapchainPresentMode(const std::vector<vkpp::khr::PresentMode>& aPresentModes)
{
    for (auto& lPresentMode : aPresentModes)
    {
        if (lPresentMode == vkpp::khr::PresentMode::eMailBox)
            return lPresentMode;
    }

    for (auto& lPresentMode : aPresentModes)
    {
        if (lPresentMode == vkpp::khr::PresentMode::eFIFO)
            return lPresentMode;
    }

    assert(false);
    return static_cast<vkpp::khr::PresentMode>(-1);
}


vkpp::ShaderModule Application::CreateShaderModule(const std::string& aFilename) const
{
    std::ifstream lFin(aFilename, std::ios::binary);
    assert(lFin);

    std::vector<char> lShaderContent((std::istreambuf_iterator<char>(lFin)), std::istreambuf_iterator<char>());
    assert(!lShaderContent.empty());

    return mLogicalDevice.CreateShaderModule({ lShaderContent });
}


void Application::DestroyShaderModule(const vkpp::ShaderModule& aShaderModule) const
{
    mLogicalDevice.DestroyShaderModule(aShaderModule);
}



}                   // End of namespace sample.