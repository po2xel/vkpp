#include <iostream>
#include <exception>

#include <Triangle/Triangle.h>



int main(int /*argc*/, char* /*argv*/[])
{
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