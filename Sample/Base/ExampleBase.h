#ifndef __VKPP_SAMPLE_EXAMPLE_BASE_H__
#define __VKPP_SAMPLE_EXAMPLE_BASE_H__



#include <Type/Instance.h>
#include <Type/LogicalDevice.h>

#ifdef _DEBUG
#include <Type/DebugReportCallback.h>
#endif              // End of _DEBUG



namespace vkpp::sample
{



struct DeviceQueue
{
    vkpp::Queue handle;
    uint32_t familyIndex{ UINT32_MAX };
};



struct SwapchainBuffer
{
    vkpp::Image image;
    vkpp::ImageView view;

    SwapchainBuffer(const vkpp::Image& aImage, const vkpp::ImageView& aImageView) : image(aImage), view(aImageView)
    {}
};



class DeviceResource
{
protected:
    const vkpp::LogicalDevice& device;
    const vkpp::PhysicalDeviceMemoryProperties& physicalMemProperties;

public:
    DeviceResource(const vkpp::LogicalDevice& aDevice, const vkpp::PhysicalDeviceMemoryProperties& aPhysicalDeviceMemProperties)
        : device(aDevice), physicalMemProperties(aPhysicalDeviceMemProperties)
    {}

    virtual ~DeviceResource(void)
    {}

    // Return the index of the memory type that supports all the property flags (e.g. device local, host visible) requested.
    uint32_t GetMemoryTypeIndex(uint32_t aMemoryTypeBits, const vkpp::MemoryPropertyFlags& aMemProperties) const
    {
        for (uint32_t lIndex = 0; lIndex < physicalMemProperties.memoryTypeCount; ++lIndex)
        {
            // memoryTypeBits is a bitmask and contains one bit set for every supported memory type for the resource.
            // Bit i is set iff the i-th type of the memoryTypes array in the VkPhysicalDeviceMemoryProperties structure is supported for the resource.
            if (aMemoryTypeBits & (1 << lIndex) &&
                (physicalMemProperties.memoryTypes[lIndex].propertyFlags & aMemProperties) == aMemProperties)
                return lIndex;
        }

        assert(false);
        return UINT32_MAX;
    }
};



struct BufferResource : public DeviceResource
{
    vkpp::Buffer buffer;
    vkpp::DeviceMemory memory;
    vkpp::BufferView view;

    BufferResource(const vkpp::LogicalDevice& aDevice, const vkpp::PhysicalDeviceMemoryProperties& aPhysicalDeviceMemProperties)
        : DeviceResource(aDevice, aPhysicalDeviceMemProperties)
    {}

    virtual ~BufferResource(void)
    {
        Reset();
    }

    void Reset(void)
    {
        assert(device);

        device.DestroyBufferView(view);
        view = nullptr;

        device.FreeMemory(memory);
        memory = nullptr;

        device.DestroyBuffer(buffer);
        buffer = nullptr;
    }

    void Reset(const vkpp::BufferCreateInfo& aBufferCreateInfo, const vkpp::MemoryPropertyFlags& aMemProperties)
    {
        buffer = device.CreateBuffer(aBufferCreateInfo);
        memory = AllocateBufferMemory(aMemProperties);
        device.BindBufferMemory(buffer, memory);
    }

    void Reset(const vkpp::BufferCreateInfo& aBufferCreateInfo, vkpp::BufferViewCreateInfo& aBufferViewCreateInfo, const vkpp::MemoryPropertyFlags& aMemProperties)
    {
        Reset(aBufferCreateInfo, aMemProperties);

        aBufferViewCreateInfo.buffer = buffer;
        view = device.CreateBufferView(aBufferViewCreateInfo);
    }

    vkpp::DeviceMemory AllocateBufferMemory(const vkpp::MemoryPropertyFlags& aMemProperties) const
    {
        const auto& lBufferMemRequirements = device.GetBufferMemoryRequirements(buffer);

        const MemoryAllocateInfo lMemoryAllocateInfo
        {
            lBufferMemRequirements.size,
            GetMemoryTypeIndex(lBufferMemRequirements.memoryTypeBits, aMemProperties)
        };

        return device.AllocateMemory(lMemoryAllocateInfo);
    }
};



struct ImageResource : public DeviceResource
{
    vkpp::Image image;
    vkpp::DeviceMemory memory;
    vkpp::ImageView view;

    ImageResource(const vkpp::LogicalDevice& aDevice, const vkpp::PhysicalDeviceMemoryProperties& aPhysicalDeviceMemProperties)
        : DeviceResource(aDevice, aPhysicalDeviceMemProperties)
    {}

    virtual ~ImageResource(void)
    {
        Reset();
    }

    void Reset(void)
    {
        assert(device);

        device.DestroyImageView(view);
        view = nullptr;

        device.FreeMemory(memory);
        memory = nullptr;        

        device.DestroyImage(image);
        image = nullptr;
    }

    void Reset(const vkpp::ImageCreateInfo& aImageCreateInfo, vkpp::ImageViewCreateInfo& aImageViewCreateInfo, const vkpp::MemoryPropertyFlags& aMemProperties)
    {
        image = device.CreateImage(aImageCreateInfo);
        memory = AllocateImageMemory(aMemProperties);
        device.BindImageMemory(image, memory);

        aImageViewCreateInfo.SetImage(image);
        view = device.CreateImageView(aImageViewCreateInfo);
    }

    vkpp::DeviceMemory AllocateImageMemory(const vkpp::MemoryPropertyFlags& aMemoryProperties) const
    {
        const auto &lImageRequirements = device.GetImageMemoryRequirements(image);

        const vkpp::MemoryAllocateInfo lMemAllocateInfo
        {
            lImageRequirements.size,
            GetMemoryTypeIndex(lImageRequirements.memoryTypeBits, aMemoryProperties)
        };

        return device.AllocateMemory(lMemAllocateInfo);
    }
};



struct Swapchain
{
    vkpp::LogicalDevice device;

    vkpp::khr::Swapchain handle;
    vkpp::khr::SurfaceFormat surfaceFormat;
    vkpp::Extent2D extent;

    mutable std::vector<SwapchainBuffer> buffers;

    Swapchain(void) = default;

    Swapchain(std::nullptr_t)
    {}

    void Release(void) const
    {
        if (!handle)
            return;

        assert(device);

        for (auto& lImageView : buffers)
            device.DestroyImageView(lImageView.view);

        buffers.clear();
        device.DestroySwapchain(handle);
    }
};



class CWindow;



class ExampleBase
{
protected:
    CWindow& mWindow;
    vkpp::Instance mInstance;
    vkpp::PhysicalDeviceFeatures mEnabledFeatures{};

#ifdef _DEBUG
    vkpp::ext::DebugReportCallback mDebugReportCallback;
#endif                  // End of _DEBUG

    vkpp::khr::Surface mSurface;
    vkpp::PhysicalDevice mPhysicalDevice;
    DeviceQueue mGraphicsQueue;
    DeviceQueue mPresentQueue;
    vkpp::LogicalDevice mLogicalDevice;
    Swapchain mSwapchain;

    vkpp::PhysicalDeviceProperties mPhysicalDeviceProperties;
    vkpp::PhysicalDeviceFeatures mPhysicalDeviceFeatures;
    vkpp::PhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;

    void CreateInstance(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion);
    void SetupDebugCallback(void);
    void CreateSurface(void);
    bool CheckPhysicalDeviceProperties(const vkpp::PhysicalDevice& aPhysicalDevice);
    void PickPhysicalDevice(void);
    void CreateLogicalDevice(void);
    void GetDeviceQueues(void);
    void CreateSwapchain(const Swapchain& aOldSwapchain = nullptr);
    void CreateSwapchainImageViews(const std::vector<vkpp::Image>& aImages);

    vkpp::ShaderModule CreateShaderModule(const std::string& aFilename) const;

public:
    ExampleBase(CWindow& aWindow, const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion);
    virtual ~ExampleBase(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_EXAMPLE_BASE_H__
