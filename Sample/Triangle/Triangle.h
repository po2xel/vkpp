#ifndef __VKPP_SAMPLE_TRIANGLE_H__
#define __VKPP_SAMPLE_TRIANGLE_H__



#include <iostream>

#include <Type/Instance.h>



namespace sample
{



const std::array<const char*, 1> gRequiredLayers{
    "VK_LAYER_LUNARG_standard_validation"
};


const std::array<const char*, 2> gRequiredExtensions{
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME
};



class Triangle
{
private:
    vkpp::Instance mInstance;

    void CheckValidationLayerSupport(void)
    {
        auto lLayers = vkpp::Instance::GetLayers();

        std::cout << "Available Layers:\n";

        for (const auto& lLayer : lLayers)
            std::cout << '\t' << lLayer.layerName << '\t' << lLayer.specVersion << '\t' << lLayer.implementationVersion << '\t' << lLayer.description << std::endl;

        std::cout << std::endl;
    }

    void CreateInstance(void)
    {
        CheckValidationLayerSupport();

        auto lExtensions = vkpp::Instance::GetExtensions(/*"VK_LAYER_LUNARG_standard_validation"*/);

        std::cout << "Instance Extension:\n";

        for (const auto& lExtension : lExtensions)
            std::cout << '\t' << lExtension.extensionName << '\t' << lExtension.specVersion << std::endl;

        std::cout << std::endl;

        vkpp::ApplicationInfo lAppInfo{ "Hello Vulkan", VK_MAKE_VERSION(1, 0, 0) };
        vkpp::InstanceInfo lInstanceInfo{ lAppInfo, gRequiredLayers, gRequiredExtensions };

        mInstance.Reset(lInstanceInfo);
    }

    void InitWindow(void)
    {}

    void InitVulkan(void)
    {
        CreateInstance();
    }

    void MainLoop(void)
    {}

public:
    void Run(void)
    {
        InitWindow();
        InitVulkan();

        MainLoop();
    }
};



}                   // End of namespace sample.



#endif              // __VKPP_SAMPLE_TRIANGLE_H__
