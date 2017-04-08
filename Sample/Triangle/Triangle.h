#ifndef __VKPP_SAMPLE_TRIANGLE_H__
#define __VKPP_SAMPLE_TRIANGLE_H__



#include <iostream>
#include <limits>

#include <Info/SurfaceCapabilities.h>

#include <Type/Instance.h>
#include <Type/VkDeleter.h>
#include <Type/Swapchain.h>

#include <GLFW/glfw3.h>



namespace sample
{



class Triangle
{
private:
    GLFWwindow* mpWindow{ nullptr };

    vkpp::Instance          mInstance;
    vkpp::ext::DebugReportCallback mDebugReportCallback;
    vkpp::PhysicalDevice    mPhysicalDevice;
    uint32_t                mGraphicsQueueFamilyIndex{ UINT32_MAX };
    uint32_t                mPresentQueueFamilyIndex{ std::numeric_limits<uint32_t>::max() };
    vkpp::LogicalDevice     mLogicalDevice;
    vkpp::Queue             mGraphicsQueue;
    vkpp::Queue             mPresentQueue;
    vkpp::CommandPool       mPresentQueueCmdPool;
    std::vector<vkpp::CommandBuffer> mPresentQueueCmdBuffers;

    vkpp::Semaphore         mImageAvailSemaphore;
    vkpp::Semaphore         mRenderingFinishedSemaphore;

    vkpp::khr::Surface      mSurface;
    vkpp::khr::Swapchain    mSwapchain;
    std::vector<vkpp::Image> mSwapchainImages;

    void CheckValidationLayerSupport(void) const;
    void CheckValidationExtensions(void) const;

    bool CheckPhysicalDeviceProperties(const vkpp::PhysicalDevice& aPhysicalDevice);

    void SetupDebugCallback(void);

    uint32_t GetSwapchainImageCount(void) const;
    vkpp::khr::SurfaceFormat GetSwapchainFormat(void) const;
    vkpp::Extent2D GetSwapchainExtent(void) const;
    vkpp::ImageUsageFlags GetSwapchainUsageFlags(void) const;
    vkpp::khr::SurfaceTransformFlagBits GetSwapchainTransform(void) const;
    vkpp::khr::PresentMode GetSwapchainPresentMode(void) const;

    void CreateInstance(void);
    void CreateSurface(void);

    bool PickPhysicalDevice(void);
    void CreateLogicalDevice(void);
    void GetDeviceQueue(void);
    void CreateSemaphore(void);

    void CreateSwapChain(void);
    void CreateCommandBuffers(void);
    void RecordCommandBuffers(void);

    void InitWindow(void);
    void InitVulkan(void);

    void MainLoop(void);

    void DrawFrame(void);

public:
    ~Triangle(void)
    {
        mLogicalDevice.Wait();

        mLogicalDevice.FreeCommandBuffers(mPresentQueueCmdPool, mPresentQueueCmdBuffers);
        mLogicalDevice.DestroyCommandPool(mPresentQueueCmdPool);

        mLogicalDevice.DestroySemaphore(mImageAvailSemaphore);
        mLogicalDevice.DestroySemaphore(mRenderingFinishedSemaphore);

        mLogicalDevice.DestroySwapchain(mSwapchain);

        mInstance.DestroySurface(mSurface);
        mInstance.DestroyDebugReportCallback(mDebugReportCallback);
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
