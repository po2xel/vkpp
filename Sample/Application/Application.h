#ifndef __VKPP_SAMPLE_APPLICATION_H__
#define __VKPP_SAMPLE_APPLICATION_H__



#include <Type/Instance.h>
#include <Type/Surface.h>
#include <Type/Queue.h>
#include <Type/PhysicalDevice.h>
#include <Type/LogicalDevice.h>

#ifdef _DEBUG
#include <Type/DebugReportCallback.h>
#endif              // End of _DEBUG

#include <GLFW/glfw3.h>



namespace sample
{



struct QueueParams
{
    vkpp::Queue mQueue;
    uint32_t    mFamilyIndex{ UINT32_MAX };
};


struct SwapchainParams
{
    vkpp::khr::Swapchain Handle;
    vkpp::khr::SurfaceFormat mSurfaceFormat;
    vkpp::Extent2D mExtent;

    std::vector<vkpp::Image> mSwapchainImages;
    std::vector<vkpp::ImageView> mSwapchainImageViews;
};


class Application
{
protected:
    GLFWwindow* mpWindow{ nullptr };

    vkpp::Instance mInstance;

#ifdef _DEBUG
    vkpp::ext::DebugReportCallback mDebugReportCallback;
#endif              // End of _DEBUG

    vkpp::khr::Surface mSurface;
    vkpp::PhysicalDevice mPhysicalDevice;
    QueueParams mPresentQueue;
    QueueParams mGraphicsQueue;
    vkpp::LogicalDevice mLogicalDevice;
    SwapchainParams mSwapchain;    

    vkpp::Semaphore mImageAvailSemaphore;
    vkpp::Semaphore mRenderingFinishedSemaphore;

    void CreateInstance(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    void CreateNativeWindow(const char* apTitle);

    void SetupDebugCallback(void);
    void CreateSurface(void);
    void PickPhysicalDevice(void);
    bool CheckPhysicalDeviceProperties(const vkpp::PhysicalDevice& aPhysicalDevice);
    void CreateLogicalDevice(void);
    void GetDeviceQueues(void);
    void CreateSwapchain(void);
    void CreateSwapchainImageViews(void);

    static uint32_t GetSwapchainImageCount(const vkpp::khr::SurfaceCapabilities& aSurfaceCapabilities);
    static vkpp::khr::SurfaceFormat GetSwapchainFormat(const std::vector<vkpp::khr::SurfaceFormat>& aSurfaceFormats);
    static vkpp::Extent2D GetSwapchainExtent(const vkpp::khr::SurfaceCapabilities& aSurfaceCapabilities);
    static vkpp::ImageUsageFlags GetSwapchainUsageFlags(const vkpp::khr::SurfaceCapabilities& aSurfaceCapabilities);
    static vkpp::khr::SurfaceTransformFlagBits GetSwapchainTransform(const vkpp::khr::SurfaceCapabilities& aSurfaceCapabilities);
    static vkpp::khr::PresentMode GetSwapchainPresentMode(const std::vector<vkpp::khr::PresentMode>& aPresentModes);
    vkpp::ShaderModule CreateShaderModule(const std::string& aFilename) const;
    void DestroyShaderModule(const vkpp::ShaderModule& aShaderModule) const;

    virtual void DrawFrame(void) = 0;

public:
    Application(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);

    virtual ~Application(void);

    void MainLoop(void);
};



}                   // End of namespace sample.



#endif              // __VKPP_SAMPLE_APPLICATION_H__
