#ifndef __VKPP_SAMPLE_TRIANGLE_H__
#define __VKPP_SAMPLE_TRIANGLE_H__



#include <iostream>
#include <limits>

#include <Type/Instance.h>
#include <Type/VkDeleter.h>
#include <GLFW/glfw3.h>



namespace sample
{



class Triangle
{
private:
    GLFWwindow* mpWindow{ nullptr };

    vkpp::Instance mInstance;
    vkpp::PhysicalDevice mPhysicalDevice;
    uint32_t mGraphicsQueueFamilyIndex{ std::numeric_limits<uint32_t>::max() };
    vkpp::LogicalDevice mLogicalDevice;

    vkpp::khr::Surface mSurface;

    void CheckValidationLayerSupport(void) const;
    void CheckValidationExtensions(void) const;

    bool CheckPhysicalDeviceProperties(const vkpp::PhysicalDevice& aPhysicalDevice);

    void CreateInstance(void);
    void CreateSurface(void);

    bool PickPhysicalDevice(void);
    void CreateLogicalDevice(void);

    void InitWindow(void);
    void InitVulkan(void);

    void MainLoop(void);

    void DrawFrame(void);

public:
    ~Triangle(void)
    {
        mInstance.DestroySurface(mSurface);
    }

    void Run(void)
    {
        InitWindow();
        InitVulkan();

        MainLoop();
    }
};



}                   // End of namespace sample.



#endif              // __VKPP_SAMPLE_TRIANGLE_H__
