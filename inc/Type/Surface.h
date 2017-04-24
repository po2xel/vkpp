#ifndef __VKPP_TYPE_SURFACE_H__
#define __VKPP_TYPE_SURFACE_H__



#include <Info/Common.h>

#ifdef VK_USE_PLATFORM_WIN32_KHR
#include <Type/Windows/SurfaceCreateInfo.h>
#endif



namespace vkpp::khr
{



class Surface : public internal::VkTrait<Surface, VkSurfaceKHR>
{
private:
    VkSurfaceKHR mSurface{ VK_NULL_HANDLE };

public:
    DEFINE_CLASS_MEMBER(Surface)

    Surface(std::nullptr_t)
    {}

    Surface(VkSurfaceKHR aSurface) : mSurface(aSurface)
    {}
};



}                   // End of namespace vkpp::khr.



#endif              // __VKPP_TYPE_SURFACE_H__
