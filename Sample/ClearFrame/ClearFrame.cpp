#include "ClearFrame.h"




namespace sample
{



ClearFrame::ClearFrame(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion)
    : Application(apApplicationName, aApplicationVersion, apEngineName, aEngineVersion)
{
    CreateCommandPool();
    AllocateCommandBuffers();
    RecordCommandBuffers();

    CreateSemaphores();
}


ClearFrame::~ClearFrame(void)
{
    mLogicalDevice.Wait();

    mLogicalDevice.DestroySemaphore(mRenderingFinishedSemaphore);
    mLogicalDevice.DestroySemaphore(mImageAvailSemaphore);

    mLogicalDevice.FreeCommandBuffers(mCommandPool, mCommandBuffers);
    mLogicalDevice.DestroyCommandPool(mCommandPool);
}


void ClearFrame::CreateCommandPool(void)
{
    vkpp::CommandPoolCreateInfo lCommandPoolCreateInfo{ mPresentQueue.mFamilyIndex };
    mCommandPool = mLogicalDevice.CreateCommandPool(lCommandPoolCreateInfo);
}


void ClearFrame::AllocateCommandBuffers(void)
{
    auto lSwapchainImageCount = static_cast<uint32_t>(mSwapchain.mSwapchainImages.size());

    vkpp::CommandBufferAllocateInfo lCommandBufferAllocateInfo
    {
        mCommandPool, lSwapchainImageCount
    };

    mCommandBuffers = mLogicalDevice.AllocateCommandBuffers(lCommandBufferAllocateInfo);
}


void ClearFrame::RecordCommandBuffers(void)
{
    vkpp::CommandBufferBeginInfo lCommandBufferBeginInfo{ vkpp::CommandBufferUsageFlagBits::eSimultaneousUse };
    vkpp::ClearColorValue lClearColorValue
    {
        1.0f, 0.8f, 0.4f, 0.0f
    };

    vkpp::ImageSubresourceRange lImageSubresourceRange
    {
        vkpp::ImageAspectFlagBits::eColor,
        0, 1,
        0, 1
    };

    for (std::size_t lIndex = 0; lIndex < mSwapchain.mSwapchainImages.size(); ++lIndex)
    {
        vkpp::ImageMemoryBarrier lBarrierFromPresentToClear
        {
            vkpp::AccessFlagBits::eMemoryRead, vkpp::AccessFlagBits::eTransferWrite,
            vkpp::ImageLayout::eUndefined, vkpp::ImageLayout::eTransferDstOptimal,
            mSwapchain.mSwapchainImages[lIndex],
            lImageSubresourceRange
        },

        lBarrierFromClearToPresent
        {
            vkpp::AccessFlagBits::eTransferWrite, vkpp::AccessFlagBits::eMemoryRead,
            vkpp::ImageLayout::eTransferDstOptimal, vkpp::ImageLayout::ePresentSrcKHR,
            mSwapchain.mSwapchainImages[lIndex],
            lImageSubresourceRange
        };

        vkpp::CommandPipelineBarrier lCommandPipelineBarrier
        {
            vkpp::PipelineStageFlagBits::eTransfer, vkpp::PipelineStageFlagBits::eTransfer,
            vkpp::DefaultFlags,
            0, nullptr,
            0, nullptr,
            1, lBarrierFromPresentToClear.AddressOf()
        };

        mCommandBuffers[lIndex].Begin(lCommandBufferBeginInfo);

        mCommandBuffers[lIndex].PipelineBarrier(lCommandPipelineBarrier);
        mCommandBuffers[lIndex].ClearColorImage(mSwapchain.mSwapchainImages[lIndex], vkpp::ImageLayout::eTransferDstOptimal, lClearColorValue, { lImageSubresourceRange });

        lCommandPipelineBarrier.SetImageMemoryBarriers(1, lBarrierFromClearToPresent.AddressOf());
        mCommandBuffers[lIndex].PipelineBarrier(lCommandPipelineBarrier);

        mCommandBuffers[lIndex].End();
    }
}


void ClearFrame::CreateSemaphores(void)
{
    vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    mImageAvailSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
    mRenderingFinishedSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void ClearFrame::DrawFrame(void)
{
    auto lImageIndex = mLogicalDevice.AcquireNextImage(mSwapchain.Handle, UINT64_MAX, mImageAvailSemaphore, nullptr);
    vkpp::PipelineStageFlags lWaitDstStageMask = vkpp::PipelineStageFlagBits::eTransfer;

    vkpp::SubmitInfo lSubmitInfo
    {
        1, mImageAvailSemaphore.AddressOf(),
        &lWaitDstStageMask,
        1, mCommandBuffers[lImageIndex].AddressOf(),
        1, mRenderingFinishedSemaphore.AddressOf()
    };

    mPresentQueue.mQueue.Submit(lSubmitInfo);

    vkpp::khr::PresentInfo lPresentInfo
    {
        1, mRenderingFinishedSemaphore.AddressOf(),
        1, mSwapchain.Handle.AddressOf(),
        &lImageIndex
    };

    mPresentQueue.mQueue.Present(lPresentInfo);
}



}                   // End of namespace sample.