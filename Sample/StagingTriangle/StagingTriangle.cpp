#include "StagingTriangle.h"



namespace sample
{



StagingTriangle::StagingTriangle(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName, uint32_t aEngineVersion)
    : Application(apApplicationName, aApplicationVersion, apEngineName, aEngineVersion), mRenderingResources(DefaultResourceSize)
{
    CreateCommandPool();
    CreateRenderingResources();
}


StagingTriangle::~StagingTriangle(void)
{
    mLogicalDevice.Wait();

    for (auto& lRenderingResource : mRenderingResources)
    {
        mLogicalDevice.DestroyFence(lRenderingResource.mFence);
        mLogicalDevice.DestroySemaphore(lRenderingResource.mFinishedRenderingSemaphore);
        mLogicalDevice.DestroySemaphore(lRenderingResource.mImageAvailSemaphore);
        mLogicalDevice.FreeCommandBuffer(mCommandPool, lRenderingResource.mCommandBuffer);
        mLogicalDevice.DestroyFrameBuffer(lRenderingResource.mFrameBuffer);
    }

    mLogicalDevice.DestroyCommandPool(mCommandPool);
}


void StagingTriangle::CreateCommandPool(void)
{
    const vkpp::CommandPoolCreateInfo lCommandPoolCreateInfo
    {
        mGraphicsQueue.mFamilyIndex, vkpp::CommandPoolCreateFlagBits::eResetCommandBuffer
    };

    mCommandPool = mLogicalDevice.CreateCommandPool(lCommandPoolCreateInfo);
}


void StagingTriangle::CreateRenderingResources(void)
{
    for(auto& lRenderingResource : mRenderingResources)
    {
        AllocateCommandBuffers(lRenderingResource.mCommandBuffer);
        CreateSemaphore(lRenderingResource.mImageAvailSemaphore); 
        CreateSemaphore(lRenderingResource.mFinishedRenderingSemaphore);
        CreateFence(lRenderingResource.mFence);
    }
}


void StagingTriangle::AllocateCommandBuffers(vkpp::CommandBuffer& aCommandBuffer) const
{
    const vkpp::CommandBufferAllocateInfo lCommandBufferAllocateInfo
    {
        mCommandPool, 1
    };

    aCommandBuffer = mLogicalDevice.AllocateCommandBuffer(lCommandBufferAllocateInfo);
}


void StagingTriangle::CreateSemaphore(vkpp::Semaphore& aSemaphore) const
{
    vkpp::SemaphoreCreateInfo lSemaphoreCreateInfo;

    aSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);
}


void StagingTriangle::CreateFence(vkpp::Fence& aFence) const
{
    vkpp::FenceCreateInfo lFenceCreateInfo{ vkpp::FenceCreateFlagBits::eSignaled };

    aFence = mLogicalDevice.CreateFence(lFenceCreateInfo);
}


void StagingTriangle::CreateRenderPass(void)
{

}


}                   // End of namespace sample.
