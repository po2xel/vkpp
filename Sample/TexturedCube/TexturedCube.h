#ifndef __VKPP_SAMPLE_TEXTURED_CUBE_H__
#define __VKPP_SAMPLE_TEXTURED_CUBE_H__



#include <Base/ExampleBase.h>
#include <Window/WindowEvent.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gli/gli.hpp>



namespace vkpp::sample
{



class TexturedCube : public ExampleBase, private CWindowEvent
{
private:
    vkpp::CommandPool mCmdPool;
    std::vector<vkpp::CommandBuffer> mDrawCmdBuffers;

    ImageResource mDepthRes;
    vkpp::RenderPass mRenderPass;
    std::vector<vkpp::Framebuffer> mFramebuffers;

    void CreateCommandPool(void);
    void AllocateCommandBuffers(void);

    void CreateDepthResource(void);
    void CreateRenderPass(void);
    void CreateFramebuffers(void);

public:
    TexturedCube(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~TexturedCube(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_TEXTURED_CUBE_H__
