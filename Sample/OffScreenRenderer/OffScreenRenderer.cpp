#include "OffScreenRenderer.h"



namespace vkpp::sample
{



OffScreenRenderer::OffScreenRenderer(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName, uint32_t aEngineVersion)
    : ExampleBase(aWindow, apAppName, aAppVersion, apEngineName, aEngineVersion),
      CWindowEvent(aWindow), CMouseMotionEvent(aWindow), CMouseWheelEvent(aWindow)
{
}


OffScreenRenderer::~OffScreenRenderer()
{
}



}                   // End of namespace vkpp::sample.