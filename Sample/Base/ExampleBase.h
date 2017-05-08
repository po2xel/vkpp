#ifndef __VKPP_SAMPLE_EXAMPLE_BASE_H__
#define __VKPP_SAMPLE_EXAMPLE_BASE_H__



#include <Type/Instance.h>
#include <Type/LogicalDevice.h>
#include <Window/Window.h>

#if _DEBUG
#include <Type/DebugReportCallback.h>
#endif              // End of _DEBUG



namespace vkpp::sample
{



struct DeviceQueue
{
    vkpp::Queue handle;
    uint32_t familyIndex{ UINT32_MAX };
};



class ExampleBase
{
private:
    CWindow mWindow;
    vkpp::Instance mInstance;

#ifdef _DEBUG
    vkpp::ext::DebugReportCallback mDebugReportCallback;
#endif                  // End of _DEBUG

    vkpp::khr::Surface mSurface;
    vkpp::PhysicalDevice mPhysicalDevice;
    DeviceQueue mGraphicsQueue;
    DeviceQueue mPresentQueue;
    vkpp::LogicalDevice mLogicalDevice;

    vkpp::PhysicalDeviceProperties mPhysicalDeviceProperties;
    vkpp::PhysicalDeviceFeatures mPhysicalDeviceFeatures;
    vkpp::PhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;

    void CreateInstance(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion);
    void SetupDebugCallback(void);
    void CreateSurface(void);
    bool CheckPhysicalDeviceProperties(const vkpp::PhysicalDevice& aPhysicalDevice);
    void PickPhysicalDevice(void);
    void CreateLogicalDevice(void);

protected:
    virtual void SetEnabledFeatures(void);

public:
    ExampleBase(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion);
    virtual ~ExampleBase(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_EXAMPLE_BASE_H__
