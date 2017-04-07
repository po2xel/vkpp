#ifndef __VKPP_TYPE_LOGICAL_DEVICE_H__
#define __VKPP_TYPE_LOGICAL_DEVICE_H__



#include <utility>

#include <Info/Common.h>
#include <Info/LogicalDeviceInfo.h>
#include <Info/BufferCreateInfo.h>
#include <Info/SemaphoreCreateInfo.h>
#include <Info/SwapchainCreateInfo.h>

#include <Type/VkDeleter.h>
#include <Type/Queue.h>
#include <Type/Semaphore.h>
#include <Type/Swapchain.h>



namespace vkpp
{



class LogicalDevice
{
private:
    internal::VkDeleter<VkDevice> mDevice{ vkDestroyDevice };

public:
    LogicalDevice(void) = default;

    LogicalDevice(std::nullptr_t)
    {}

    LogicalDevice(VkDevice aLogicalDevice)
    {
        mDevice = aLogicalDevice;
    }

    LogicalDevice(LogicalDevice&& aLogicalDevice) : mDevice(std::move(aLogicalDevice.mDevice))
    {}

    LogicalDevice& operator=(LogicalDevice&& aLogicalDevice)
    {
        mDevice = std::move(aLogicalDevice.mDevice);

        return *this;
    }

    Queue GetQueue(uint32_t aQueueFamilyIndex, uint32_t aQueueIndex) const
    {
        Queue lQueue;
        vkGetDeviceQueue(mDevice, aQueueFamilyIndex, aQueueIndex, &lQueue);

        return lQueue;
    }

    Semaphore CreateSemaphore(const SemaphoreCreateInfo& aSemaphoreCreateInfo)
    {
        Semaphore lSemaphore;
        ThrowIfFailed(vkCreateSemaphore(mDevice, &aSemaphoreCreateInfo, nullptr, &lSemaphore));

        return lSemaphore;
    }

    Semaphore CreateSemaphore(const SemaphoreCreateInfo& aSemaphoreCreateInfo, const AllocationCallbacks& aAllocator)
    {
        Semaphore lSemaphore;
        ThrowIfFailed(vkCreateSemaphore(mDevice, &aSemaphoreCreateInfo, &aAllocator, &lSemaphore));

        return lSemaphore;
    }

    void DestroySemaphore(const Semaphore& aSemaphore)
    {
        vkDestroySemaphore(mDevice, aSemaphore, nullptr);
    }

    void DestroySemaphore(const Semaphore& aSemaphore, const AllocationCallbacks& aAllocator)
    {
        vkDestroySemaphore(mDevice, aSemaphore, &aAllocator);
    }

    khr::Swapchain CreateSwapchain(const khr::SwapchainCreateInfo& aSwapchainCreateInfo)
    {
        khr::Swapchain lSwapchain;
        ThrowIfFailed(vkCreateSwapchainKHR(mDevice, &aSwapchainCreateInfo, nullptr, &lSwapchain));

        return lSwapchain;
    }

    khr::Swapchain CreateSwapchain(const khr::SwapchainCreateInfo& aSwapchainCreateInfo, const AllocationCallbacks& aAllocator)
    {
        khr::Swapchain lSwapchain;
        ThrowIfFailed(vkCreateSwapchainKHR(mDevice, &aSwapchainCreateInfo, &aAllocator, &lSwapchain));

        return lSwapchain;
    }

    void DestroySwapchain(const khr::Swapchain& aSwapchain)
    {
        vkDestroySwapchainKHR(mDevice, aSwapchain, nullptr);
    }

    void DestroySwapchain(const khr::Swapchain& aSwapchain, const AllocationCallbacks& aAllocator)
    {
        vkDestroySwapchainKHR(mDevice, aSwapchain, &aAllocator);
    }

    VkResult Wait(void) const
    {
        return vkDeviceWaitIdle(mDevice);
    }
};



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_LOGICAL_DEVICE_H__
