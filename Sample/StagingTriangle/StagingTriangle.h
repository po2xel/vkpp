#ifndef __VKPP_SAMPLE_STAGING_TRIANGLE_H__
#define __VKPP_SAMPLE_STAGING_TRIANGLE_H__



#include "Application/Application.h"



namespace sample
{



struct RenderingResource
{
    vkpp::FrameBuffer mFrameBuffer;
    vkpp::CommandBuffer mCommandBuffer;
    vkpp::Semaphore mImageAvailSemaphore;
    vkpp::Semaphore mFinishedRenderingSemaphore;
    vkpp::Fence mFence;
};



class StagingTriangle : public Application
{
private:
    constexpr static std::size_t DefaultResourceSize = 3;

    vkpp::CommandPool mCommandPool;
    std::vector<RenderingResource> mRenderingResources;
    vkpp::RenderPass mRenderPass;

    void CreateCommandPool(void);
    void CreateRenderingResources(void);
    void AllocateCommandBuffers(vkpp::CommandBuffer& aCommandBuffer) const;
    void CreateSemaphore(vkpp::Semaphore& aSemaphore) const;
    void CreateFence(vkpp::Fence& aFence) const;

    void CreateRenderPass(void);

    virtual void DrawFrame(void) override
    {}

public:
    StagingTriangle(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~StagingTriangle(void);
};



}                   // End of namespace sample.



#endif              // __VKPP_SAMPLE_STAGING_TRIANGLE_H__
