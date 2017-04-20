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



struct VertexData
{
    float x, y, z, w;
    float r, g, b, a;
};



struct RenderingResourceData
{
    vkpp::FrameBuffer   mFrameBuffer;
    vkpp::CommandBuffer mCommandBuffer;
    vkpp::Semaphore     mImageAvailableSemaphore;
    vkpp::Semaphore     mFinishedRenderingSemaphore;
    vkpp::Fence         mFence;
};



class Triangle
{
private:
    constexpr static auto RenderingResourceCount{ 3 };

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

    vkpp::RenderPass        mRenderPass;
    vkpp::Pipeline          mGraphicsPipeline;

    vkpp::khr::Surface      mSurface;
    vkpp::khr::Swapchain    mSwapchain;
    std::vector<vkpp::Image> mSwapchainImages;
    std::vector<vkpp::ImageView> mSwapchainImageViews;
    std::vector<vkpp::FrameBuffer> mFramebuffers;
    vkpp::DeviceSize mVertexBufferSize{ 0 };
    vkpp::Buffer mVertexBuffer;
    vkpp::DeviceMemory mVertexBufferMemory;
    std::array<RenderingResourceData, RenderingResourceCount> mRenderingResources;

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
    void CreateSemaphores(void);
    void CreateFences(void);

    void CreateSwapChain(void);
    void CreateSwapchainImageViews(void);
    void CreateCommandBuffers(void);
    void RecordCommandBuffers(void);
    void CreateRenderPass(void);
    void CreateFrameBuffers(void);
    vkpp::FrameBuffer CreateFrameBuffer(const vkpp::ImageView& aImageView) const;
    void CreateVertexBuffer(void);
    void CreateRenderingResources(void);
    vkpp::DeviceMemory AllocateBufferMemory(const vkpp::Buffer& aBuffer) const;
    vkpp::ShaderModule CreateShaderModule(const std::string& aFilename) const;
    vkpp::PipelineLayout CreatePipelineLayout(void) const;
    void CreatePipeline(void);

    void InitWindow(void);
    void InitVulkan(void);

    void MainLoop(void);

    void DrawFrame(void);
    void PrepareFrame(vkpp::FrameBuffer& aFrameBuffer, const vkpp::CommandBuffer& aCommandBuffer, const vkpp::ImageView& aImgeView) const;

public:
    ~Triangle(void)
    {
        mLogicalDevice.Wait();

        for(auto& lRenderingResource : mRenderingResources)
        {
            mLogicalDevice.DestroyFrameBuffer(lRenderingResource.mFrameBuffer);
            mLogicalDevice.FreeCommandBuffer(mPresentQueueCmdPool, lRenderingResource.mCommandBuffer);
            mLogicalDevice.DestroySemaphore(lRenderingResource.mImageAvailableSemaphore);
            mLogicalDevice.DestroySemaphore(lRenderingResource.mFinishedRenderingSemaphore);
            mLogicalDevice.DestroyFence(lRenderingResource.mFence);
        }

        mLogicalDevice.DestroyPipeline(mGraphicsPipeline);
        mLogicalDevice.DestroyRenderPass(mRenderPass);

        for (const auto& lFrameBuffer : mFramebuffers)
            mLogicalDevice.DestroyFrameBuffer(lFrameBuffer);

        for (const auto& lImageView : mSwapchainImageViews)
            mLogicalDevice.DestroyImageView(lImageView);

        mLogicalDevice.DestroySwapchain(mSwapchain);

        // mLogicalDevice.FreeCommandBuffers(mPresentQueueCmdPool, mPresentQueueCmdBuffers);
        mLogicalDevice.DestroyCommandPool(mPresentQueueCmdPool);

        mLogicalDevice.DestroySemaphore(mImageAvailSemaphore);
        mLogicalDevice.DestroySemaphore(mRenderingFinishedSemaphore);

        mLogicalDevice.DestroyBuffer(mVertexBuffer);
        mLogicalDevice.FreeMemory(mVertexBufferMemory);

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
