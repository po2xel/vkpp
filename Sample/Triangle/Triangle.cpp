#include "Triangle/Triangle.h"

#include <algorithm>
#include <iterator>

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
        return true;
    }

    return false;
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
    const std::array<float, 1> lPriorities{ 1.0f };

    vkpp::QueueCreateInfo lQueueCreateInfo{
        mGraphicsQueueFamilyIndex,
        lPriorities
    };

    vkpp::LogicalDeviceCreateInfo lLogicalDeiveInfo (
        1, lQueueCreateInfo.AddressOf(),
        0, nullptr,
        nullptr
    );

    mLogicalDevice = mPhysicalDevice.CreateLogicalDevice(lLogicalDeiveInfo);
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
    CreateSurface();

    PickPhysicalDevice();
    CreateLogicalDevice();
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