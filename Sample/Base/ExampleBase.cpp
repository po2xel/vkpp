#include "Base/ExampleBase.h"

#include <cassert>
#include <iostream>
#include <fstream>

#include <Window/Window.h>



namespace
{



VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT /*aFlags*/, VkDebugReportObjectTypeEXT /*aObjType*/,
    uint64_t /*aObj*/, std::size_t /*aLocation*/, int32_t aMsgCode, const char* apLayerPrefix, const char* apMsg, void* /*apUserData*/)
{
    std::cerr << "Validation Layer: [" << apLayerPrefix << "] Code " << aMsgCode << " : " <<  apMsg << std::endl;

    return VK_FALSE;                // Applications should always return VK_FALSE so that they see the same behavior with and without validation layers enabled.
}


const char* PhysicalDeviceTypeString(vkpp::PhysicalDeviceType aType)
{
    switch (aType)
    {
#define CASE_(Type) case vkpp::PhysicalDeviceType::e##Type: return #Type;
        CASE_(Other)
        CASE_(IntegrateGPU)
        CASE_(DiscreteGPU)
        CASE_(VirtualGPU)
        CASE_(CPU)
#undef CASE_
    }

    return "Unknown device type.";
}


vkpp::khr::PresentMode GetSwapchainPresentMode(const std::vector<vkpp::khr::PresentMode>& aPresentModes)
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


vkpp::khr::SurfaceFormat GetSwapchainFormat(const std::vector<vkpp::khr::SurfaceFormat>& aSurfaceFormats)
{
    if (aSurfaceFormats.size() == 1 && aSurfaceFormats[0].format == vkpp::Format::eUndefined)
        return { vkpp::Format::eRGBA8uNorm, vkpp::khr::ColorSpace::esRGBNonLinear };

    for (auto& lSurfaceFormat : aSurfaceFormats)
    {
        if (lSurfaceFormat.format == vkpp::Format::eRGBA8uNorm)
            return lSurfaceFormat;
    }

    // In case Format::eRGBA8uNorm is not available, select the first available color format.
    return aSurfaceFormats[0];
}


uint32_t GetSwapchainImageCount(const vkpp::khr::SurfaceCapabilities& aSurfaceCapabilities)
{
    auto lImageCount = aSurfaceCapabilities.minImageCount + 1;

    if (aSurfaceCapabilities.maxImageCount > 0 && aSurfaceCapabilities.maxImageCount < lImageCount)
        lImageCount = aSurfaceCapabilities.maxImageCount;

    return lImageCount;
}


// TODO
vkpp::Extent2D GetSwapchainExtent(const vkpp::khr::SurfaceCapabilities& aSurfaceCapabilities)
{
    return aSurfaceCapabilities.currentExtent;
}


// TODO
vkpp::ImageUsageFlags GetSwapchainUsageFlags(const vkpp::khr::SurfaceCapabilities& aSurfaceCapabilities, const vkpp::FormatProperties& aFormatProperties)
{
    vkpp::ImageUsageFlags lImageUsageFlags;

    if (aSurfaceCapabilities.supportedUsageFlags & vkpp::ImageUsageFlagBits::eColorAttachment)
        lImageUsageFlags = vkpp::ImageUsageFlagBits::eColorAttachment; // | vkpp::ImageUsageFlagBits::eTransferDst;

    // Set additional usage flag for blitting from swapchain images if supported.
    if (aFormatProperties.optimalTilingFeatures & vkpp::FormatFeatureFlagBits::eBlitDst)
        lImageUsageFlags |= vkpp::ImageUsageFlagBits::eTransferSrc;

    return lImageUsageFlags;
}


vkpp::khr::SurfaceTransformFlagBits GetSwapchainTransform(const vkpp::khr::SurfaceCapabilities& aSurfaceCapabilities)
{
    if (aSurfaceCapabilities.supportedTransforms & vkpp::khr::SurfaceTransformFlagBits::eIdentity)
        return vkpp::khr::SurfaceTransformFlagBits::eIdentity;

    return aSurfaceCapabilities.currentTransform;
}


// TODO
vkpp::khr::CompositeAlphaFlagBits GetSwapchainCompositeAlphaFlags(const vkpp::khr::SurfaceCapabilities& /*aSurfaceCapabilities*/)
{
    return vkpp::khr::CompositeAlphaFlagBits::eOpaque;
}


}




namespace vkpp::sample
{



ExampleBase::ExampleBase(CWindow& aWindow, const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion)
    : mWindow(aWindow)
{
    assert(apApplicationName != nullptr);

    CreateInstance(apApplicationName, aApplicationVersion, apEngineName, aEngineVersion);

#ifdef _DEBUG
    SetupDebugCallback();
#endif              // End of _DEBUG

    CreateSurface();
    PickPhysicalDevice();

    CreateLogicalDevice();
    GetDeviceQueues();

    CreateSwapchain();
}


ExampleBase::~ExampleBase(void)
{
    mSwapchain.Release();

    mLogicalDevice.Reset();
    mInstance.DestroySurface(mSurface);

#ifdef _DEBUG
    mInstance.DestroyDebugReportCallback(mDebugReportCallback);
#endif              // End of _DEBUG
}


void ExampleBase::CreateInstance(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion)
{
    const auto& lLayers = vkpp::Instance::GetLayers();

    std::cout << "Instance Layers:\n";
    for (auto& lLayer : lLayers)
        std::cout << '\t' << lLayer.layerName << '\t' << lLayer.specVersion << '\t' << lLayer.implementationVersion << '\t' << lLayer.description << std::endl;

    std::cout << std::endl;

    const auto& lExtensions = vkpp::Instance::GetExtensions();

    std::cout << "Instance Extensions:\n";
    for (auto& lExtension : lExtensions)
        std::cout << '\t' << lExtension.extensionName << '\t' << lExtension.specVersion << std::endl;

    std::cout << std::endl;

    const vkpp::ApplicationInfo lApplicationInfo
    {
        apApplicationName, aApplicationVersion,
        apEngineName, aEngineVersion
    };

    constexpr std::array<const char*, 1> lInstanceLayers
    {
        LUNARG_STANDARD_VALIDATION_NAME
    };

    constexpr std::array<const char*, 3> lInstanceExtensions
    {
        KHR_SURFACE_EXT_NAME,
        KHR_OS_SURFACE_EXT_NAME,
        EXT_DEBUG_REPORT_EXT_NAME
    };

    const vkpp::InstanceInfo lInstanceInfo
    {
        lApplicationInfo,
        lInstanceLayers,
        lInstanceExtensions
    };

    mInstance.Reset(lInstanceInfo);
}


void ExampleBase::SetupDebugCallback(void)
{
#ifdef _DEBUG
    const vkpp::ext::DebugReportCallbackCreateInfo lDebugReportCallbackCreateInfo
    {
        vkpp::ext::DebugReportFlagBits::eError | vkpp::ext::DebugReportFlagBits::eWarning,
        DebugCallback
    };

    mDebugReportCallback = mInstance.CreateDebugReportCallback(lDebugReportCallbackCreateInfo);
#endif          // _DEBUG
}


void ExampleBase::CreateSurface(void)
{
    const vkpp::khr::SurfaceCreateInfo lSurfaceCreateInfo
    {
        mWindow.GetNativeHandle()
    };

    mSurface = mInstance.CreateSurface(lSurfaceCreateInfo);
}


void ExampleBase::PickPhysicalDevice(void)
{
    std::cout << "Available Vulkan Devices:\n";

    const auto& lPhysicalDevices = mInstance.GetPhysicalDevices();

    auto lDeviceIter = std::find_if(lPhysicalDevices.cbegin(), lPhysicalDevices.cend(),
        [this](auto&& aDevice) {
        return CheckPhysicalDeviceProperties(aDevice);
    });

    mPhysicalDevice = *lDeviceIter;
    mPhysicalDeviceProperties = mPhysicalDevice.GetProperties();
    mPhysicalDeviceFeatures = mPhysicalDevice.GetFeatures();
    mPhysicalDeviceMemoryProperties = mPhysicalDevice.GetMemoryProperties();
}


bool ExampleBase::CheckPhysicalDeviceProperties(const vkpp::PhysicalDevice& aPhysicalDevice)
{
    const auto& lProperties = aPhysicalDevice.GetProperties();
    auto lApiVersion = lProperties.apiVersion;

    std::cout << "\tName:\t" << lProperties.deviceName << std::endl
        << "\tType:\t" << PhysicalDeviceTypeString(lProperties.deviceType) << std::endl
        << "\tAPI:\t" << VK_VERSION_MAJOR(lApiVersion) << "." << VK_VERSION_MINOR(lApiVersion) << "." << VK_VERSION_PATCH(lApiVersion) << std::endl;

    std::cout << std::endl
        << "\tPhysical Device Layers:\n";

    const auto& lPhysicalLayers = aPhysicalDevice.GetLayers();
    for (auto& lLayer : lPhysicalLayers)
        std::cout << "\t\t" << lLayer.layerName << '\t' << lLayer.specVersion << '\t' << lLayer.implementationVersion << '\t' << lLayer.description << std::endl;

    std::cout << std::endl
        << "\tPhysical Device Extensions:\n";

    const auto& lPhysicalExts = aPhysicalDevice.GetExtensions();
    for (auto& lExt : lPhysicalExts)
        std::cout << "\t\t" << lExt.extensionName << '\t' << lExt.specVersion << std::endl;

    const auto& lQueueFamilyProperties = aPhysicalDevice.GetQueueFamilyProperties();
    for(uint32_t lIndex = 0; lIndex < lQueueFamilyProperties.size(); ++lIndex)
    {
        // Search for a graphics and a present queue in the array of queue families.
        // Try to find one that supports both.
        if ((lQueueFamilyProperties[lIndex].queueFlags & vkpp::QueueFlagBits::eGraphics) &&
            aPhysicalDevice.IsSurfaceSupported(lIndex, mSurface))
        {
            mGraphicsQueue.familyIndex = lIndex;
            mPresentQueue.familyIndex = lIndex;

            return true;
        }
    }

    // TODO: Add support for seperate graphics and present queue.
    return false;
}


void ExampleBase::CreateLogicalDevice(void)
{
    constexpr std::array<float, 1> lProprities{ 1.0f };

    std::vector<vkpp::QueueCreateInfo> lQueueCreateInfos;
    lQueueCreateInfos.emplace_back(mGraphicsQueue.familyIndex, lProprities);

    constexpr std::array<const char*, 1> lDeviceExts
    {
        KHR_SWAPCHAIN_EXT_NAME
    };

    mEnabledFeatures.fillModeNonSolid = mPhysicalDeviceFeatures.fillModeNonSolid;

    // Wide lines must be present for line width > 1.0f.
    mEnabledFeatures.wideLines = mPhysicalDeviceFeatures.wideLines;

    mEnabledFeatures.textureCompressionBC = mPhysicalDeviceFeatures.textureCompressionBC;

    const vkpp::LogicalDeviceCreateInfo lLogicalDeviceCreateInfo
    {
        lQueueCreateInfos,
        lDeviceExts,
        &mEnabledFeatures
    };

    mLogicalDevice.Reset(mPhysicalDevice, lLogicalDeviceCreateInfo);
}


void ExampleBase::GetDeviceQueues(void)
{
    mGraphicsQueue.handle = mLogicalDevice.GetQueue(mGraphicsQueue.familyIndex, 0);
    mPresentQueue.handle = mLogicalDevice.GetQueue(mPresentQueue.familyIndex, 0);
}


void ExampleBase::CreateSwapchain(const Swapchain& aOldSwapchain)
{
    const auto& lSurfacePresentModes = mPhysicalDevice.GetSurfacePresentModes(mSurface);
    auto lDesiredPresentMode = GetSwapchainPresentMode(lSurfacePresentModes);

    const auto& lSurfaceFormats = mPhysicalDevice.GetSurfaceFormats(mSurface);
    auto lDesiredSurfaceFormat = GetSwapchainFormat(lSurfaceFormats);

    const auto& lSurfaceCapabilities = mPhysicalDevice.GetSurfaceCapabilities(mSurface);
    auto lDesiredImageCount = GetSwapchainImageCount(lSurfaceCapabilities);
    auto lDesiredExtent = GetSwapchainExtent(lSurfaceCapabilities);
    auto lDesiredTransform = GetSwapchainTransform(lSurfaceCapabilities);
    auto lDesiredCompositeAlpha = GetSwapchainCompositeAlphaFlags(lSurfaceCapabilities);

    const auto& lFormatProperties = mPhysicalDevice.GetFormatProperties(lDesiredSurfaceFormat.format);
    auto lDesiredUsage = GetSwapchainUsageFlags(lSurfaceCapabilities, lFormatProperties);

    const vkpp::khr::SwapchainCreateInfo lSwapchainCreateInfo
    {
        mSurface,
        lDesiredImageCount, lDesiredSurfaceFormat.format, lDesiredSurfaceFormat.colorSpace,
        lDesiredExtent, lDesiredUsage,
        lDesiredTransform,
        lDesiredCompositeAlpha,
        lDesiredPresentMode,
        aOldSwapchain.handle
    };

    auto lSwapchain = mLogicalDevice.CreateSwapchain(lSwapchainCreateInfo);

    // If an existing swapchain is re-created, destroy the old swapchain.
    // This also cleans up all the presentable images.
    aOldSwapchain.Release();

    mSwapchain.handle = lSwapchain;
    mSwapchain.device = mLogicalDevice;
    mSwapchain.surfaceFormat = lDesiredSurfaceFormat;
    mSwapchain.extent = lDesiredExtent;

    const auto& lImages = mLogicalDevice.GetSwapchainImages(mSwapchain.handle);
    CreateSwapchainImageViews(lImages);
}


void ExampleBase::CreateSwapchainImageViews(const std::vector<vkpp::Image>& aImages) const
{
    for(auto& lImage : aImages)
    {
        vkpp::ImageViewCreateInfo lImageViewCreateInfo
        {
            lImage, vkpp::ImageViewType::e2D,
            mSwapchain.surfaceFormat.format,
            {
                vkpp::ImageAspectFlagBits::eColor,
                0, 1,
                0, 1
            }
        };

        mSwapchain.buffers.emplace_back(lImage, mLogicalDevice.CreateImageView(lImageViewCreateInfo));
    }
}


vkpp::ShaderModule ExampleBase::CreateShaderModule(const std::string& aFilename) const
{
    std::ifstream lFin(aFilename, std::ios::binary);
    assert(lFin);

    std::vector<char> lShaderContent((std::istreambuf_iterator<char>(lFin)), std::istreambuf_iterator<char>());
    assert(!lShaderContent.empty());

    return mLogicalDevice.CreateShaderModule(lShaderContent);
}


}               // End of namespace vkpp::sample.