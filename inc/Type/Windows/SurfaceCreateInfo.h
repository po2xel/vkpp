#ifndef __VKPP_TYPE_WINDOWS_SURFACE_CREATE_INFO_H__
#define __VKPP_TYPE_WINDOWS_SURFACE_CREATE_INFO_H__



#include <Info/Common.h>
#include <Info/Flags.h>



#define vkCreateSurfaceKHR vkCreateWin32SurfaceKHR



namespace vkpp::khr
{



enum class SurfaceCreateFlagBits
{};

VKPP_ENUM_BIT_MASK_FLAGS_OS_KHR(SurfaceCreate)



class SurfaceCreateInfo : public internal::VkTrait<SurfaceCreateInfo, VkWin32SurfaceCreateInfoKHR>
{
private:
    const internal::Structure sType = internal::Structure::eWin32Surface;

public:
    const void*         pNext{ nullptr };
    SurfaceCreateFlags  flags;
    HINSTANCE           hinstance{ nullptr };
    HWND                hwnd{ nullptr };

    DEFINE_CLASS_MEMBER(SurfaceCreateInfo)

    SurfaceCreateInfo(HWND ahWnd, HINSTANCE ahInstance = nullptr, const SurfaceCreateFlags& aFlags = DefaultFlags) : flags(aFlags), hinstance(ahInstance), hwnd(ahWnd)
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
        hinstance = ahInstance;
        hwnd = ahWnd;

        return *this;
    }
};

ConsistencyCheck(SurfaceCreateInfo, pNext, flags, hinstance, hwnd)



}                   // End of namespace vkpp::khr.



#endif              // __VKPP_TYPE_WINDOWS_SURFACE_CREATE_INFO_H__
