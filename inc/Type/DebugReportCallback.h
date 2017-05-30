#ifndef __VKPP_TYPE_DEBUG_REPORT_CALLBACK_H__
#define __VKPP_TYPE_DEBUG_REPORT_CALLBACK_H__



#include <Info/Common.h>
#include <Info/Flags.h>



namespace vkpp::ext
{



enum class DebugReportFlagBits
{
    eInformation        = VK_DEBUG_REPORT_INFORMATION_BIT_EXT,
    eWarning            = VK_DEBUG_REPORT_WARNING_BIT_EXT,
    ePerformanceWarning = VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
    eError              = VK_DEBUG_REPORT_ERROR_BIT_EXT,
    eDebug              = VK_DEBUG_REPORT_DEBUG_BIT_EXT
};

VKPP_ENUM_BIT_MASK_FLAGS_EXT(DebugReport)



class DebugReportCallbackCreateInfo : public internal::VkTrait<DebugReportCallbackCreateInfo, VkDebugReportCallbackCreateInfoEXT>
{
private:
    const internal::Structure sType = internal::Structure::eDebugReportCallback;

public:
    const void*                     pNext{ nullptr };
    DebugReportFlags                flags;
    PFN_vkDebugReportCallbackEXT    pfnCallback{ nullptr };
    void*                           pUserData{ nullptr };

    DEFINE_CLASS_MEMBER(DebugReportCallbackCreateInfo)

    DebugReportCallbackCreateInfo(const DebugReportFlags& aFlags, PFN_vkDebugReportCallbackEXT apfnCallback, void* apUserData = nullptr) noexcept
        : flags(aFlags), pfnCallback(apfnCallback), pUserData(apUserData)
    {
        assert(flags != DefaultFlags);              // flags must not be 0.
    }

    DebugReportCallbackCreateInfo& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    DebugReportCallbackCreateInfo& SetFlags(const DebugReportFlags& aFlags) noexcept
    {
        assert(flags != DefaultFlags);

        flags = aFlags;

        return *this;
    }

    DebugReportCallbackCreateInfo& SetCallback(PFN_vkDebugReportCallbackEXT apfnCallback, void* apUserData = nullptr) noexcept
    {
        pfnCallback = apfnCallback;
        pUserData   = apUserData;

        return *this;
    }
};

ConsistencyCheck(DebugReportCallbackCreateInfo, pNext, flags, pfnCallback, pUserData)



class DebugReportCallback : public internal::VkTrait<DebugReportCallback, VkDebugReportCallbackEXT>
{
private:
    VkDebugReportCallbackEXT mDebugReportCallback{ VK_NULL_HANDLE };

public:
    DebugReportCallback(void) noexcept = default;

    DebugReportCallback(std::nullptr_t) noexcept
    {}

    explicit DebugReportCallback(VkDebugReportCallbackEXT aDebugReportCallback) noexcept : mDebugReportCallback(aDebugReportCallback)
    {}
};

StaticSizeCheck(DebugReportCallback)



}                   // End of namespace vkpp::ext.



#endif              // __VKPP_TYPE_DEBUG_REPORT_CALLBACK_H__
