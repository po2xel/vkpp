#ifndef __VKPP__SAMPLE_RADIAL_BLUR_H__
#define __VKPP__SAMPLE_RADIAL_BLUR_H__



#include <Base/ExampleBase.h>
#include <Window/WindowEvent.h>
#include <window/MouseEvent.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <gli/gli.hpp>



namespace vkpp::sample
{



class RadialBlur : public ExampleBase, private CWindowEvent, private CMouseMotionEvent, private CMouseWheelEvent
{
public:
    RadialBlur(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
    virtual ~RadialBlur(void);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP__SAMPLE_RADIAL_BLUR_H__
