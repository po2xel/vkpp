#ifndef __VKPP_INFO_WINDOWS_SURFACE_CREATE_INFO_H__
#define __VKPP_INFO_WINDOWS_SURFACE_CREATE_INFO_H__



#include <Info/Common.h>
#include <Info/Flags.h>



#define vkCreateSurfaceKHR vkCreateWin32SurfaceKHR



namespace vkpp::khr
{



enum class SurfaceCreateFlagBits
{};

using SurfaceCreateFlags = internal::Flags<SurfaceCreateFlagBits, VkWin32SurfaceCreateFlagsKHR>;



class SurfaceCreateInfo : public internal::VkTrait<SurfaceCreateInfo, VkWin32SurfaceCreateInfoKHR>
{
private:
    const internal::Structure sType = internal::Structure::eWin32Surface;

public:
    const void*         pNext{ nullptr };
    SurfaceCreateFlags  flags;
    HINSTANCE           hInstance{ nullptr };
    HWND                hWnd{ nullptr };

    DEFINE_CLASS_MEMBER(SurfaceCreateInfo)

    SurfaceCreateInfo(HINSTANCE ahInstance, HWND ahWnd) : hInstance(ahInstance), hWnd(ahWnd)
    {}

    SurfaceCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    SurfaceCreateInfo& SetFlags(SurfaceCreateFlags aFlags)
    {
        flags = aFlags;

        return *this;
    }

    SurfaceCreateInfo& SetWindowHandle(HINSTANCE ahInstance, HWND ahWnd)
    {
        hInstance = ahInstance;
        hWnd = ahWnd;

        return *this;
    }
};

StaticSizeCheck(SurfaceCreateInfo);



}                   // End of namespace vkpp::khr.



#endif              // __VKPP_INFO_WINDOWS_SURFACE_CREATE_INFO_H__
