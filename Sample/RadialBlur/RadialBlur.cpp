#include "RadialBlur.h"



namespace vkpp::sample
{



RadialBlur::RadialBlur(CWindow& aWindow, const char* apAppName, uint32_t aAppVersion, const char* apEngineName, uint32_t aEngineVersion)
    : ExampleBase(aWindow, apAppName, aAppVersion, apEngineName, aEngineVersion),
      CWindowEvent(aWindow), CMouseMotionEvent(aWindow), CMouseWheelEvent(aWindow)
{}


RadialBlur::~RadialBlur(void)
{
    mLogicalDevice.Wait();
}



}                   // End of namespace vkpp::sample.