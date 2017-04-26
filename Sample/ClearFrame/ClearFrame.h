#ifndef __VKPP_SAMPLE_CLEAR_FRAME_H__
#define __VKPP_SAMPLE_CLEAR_FRAME_H__



#include <Application/Application.h>



namespace sample
{



class ClearFrame : public Application
{
private:
    vkpp::CommandPool mCommandPool;
    std::vector<vkpp::CommandBuffer> mCommandBuffers;

    void CreateCommandPool(void);
    void AllocateCommandBuffers(void);
    void RecordCommandBuffers(void);
    void CreateSemaphores(void);

    virtual void DrawFrame(void) override;

public:
    ClearFrame(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~ClearFrame(void);
};



}                   // End of namespace sample.



#endif              // __VKPP_SAMPLE_CLEAR_FRAME_H__
