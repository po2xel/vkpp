#include "Base/ExampleBase.h"

#include <cassert>
#include <iostream>



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



}




namespace vkpp::sample
{



ExampleBase::ExampleBase(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion)
    : mWindow(apApplicationName, CWindow::CENTERED, CWindow::CENTERED, 1024, 768, CWindow::RESIZABLE)
{
    assert(apApplicationName != nullptr);

    CreateInstance(apApplicationName, aApplicationVersion, apEngineName, aEngineVersion);

#ifdef _DEBUG
    SetupDebugCallback();
#endif              // End of _DEBUG

    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
}


ExampleBase::~ExampleBase(void)
{
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


void ExampleBase::SetEnabledFeatures(void)
{}


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
        if ((lQueueFamilyProperties[lIndex].queueFlags & vkpp::QueueFlagBits::eGraphics) &&
            aPhysicalDevice.IsSurfaceSupported(lIndex, mSurface))
        {
            mGraphicsQueue.familyIndex = lIndex;
            mPresentQueue.familyIndex = lIndex;

            return true;
        }
    }

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

    const vkpp::LogicalDeviceCreateInfo lLogicalDeviceCreateInfo
    {
        lQueueCreateInfos,
        lDeviceExts
    };

    mLogicalDevice.Reset(mPhysicalDevice, lLogicalDeviceCreateInfo);
}



}               // End of namespace vkpp::sample.