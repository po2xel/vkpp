#include <iostream>
#include <exception>
#include <thread>

#include <Triangle/Triangle.h>

#include <Memory/AutoPtr.h>


using namespace vkpp;


void Destroy(VkInstance, VkAllocationCallbacks*)
{
    std::cout << "Hello\n";
}


int main(int /*argc*/, char* /*argv*/[])
{
    {
        AutoPtr<VkInstance> lInstance{ nullptr, vkDestroyInstance };
        lInstance;
    }

    /*const std::array<const char*, 1> lValidationLayers{
        "VK_LAYER_LUNARG_standard_validation"
    };

    const std::array<const char*, 1> lInstanceExtensions{
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };

    vkpp::ApplicationInfo lApp{ "Hello World", 1 };
    vkpp::InstanceInfo lInstanceInfo{ lApp, lValidationLayers, lInstanceExtensions };*/

    try
    {
        sample::Triangle lTriangle;
        lTriangle.Run();
        /*vkpp::Instance lInstance{ lInstanceInfo };

        auto lPhysicalDevices = lInstance.EnumeratePhysicalDevices();

        auto lPhysicalDeviceProperties = lPhysicalDevices[0].GetProperties();
        lPhysicalDeviceProperties;

        auto lMemoryProperties = lPhysicalDevices[0].GetMemoryProperties();
        lMemoryProperties;

        auto lQueueFamilyProperties = lPhysicalDevices[0].GetQueueFamilyProperties();
        lQueueFamilyProperties;*/
    }
    catch (const std::exception&)
    {}

    return 0;
}