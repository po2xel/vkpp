#ifndef __VKPP_TYPE_DEBUG_REPORT_CALLBACK_H__
#define __VKPP_TYPE_DEBUG_REPORT_CALLBACK_H__



#include <Info/Common.h>


namespace vkpp::ext
{



class DebugReportCallback : public internal::VkTrait<DebugReportCallback, VkDebugReportCallbackEXT>
{
private:
    VkDebugReportCallbackEXT mDebugReportCallback{ VK_NULL_HANDLE };

public:
    DebugReportCallback(void) = default;

    DebugReportCallback(std::nullptr_t)
    {}

    explicit DebugReportCallback(VkDebugReportCallbackEXT aDebugReportCallback) : mDebugReportCallback(aDebugReportCallback)
    {}
};

StaticSizeCheck(DebugReportCallback);



}                   // End of namespace vkpp::ext.



#endif              // __VKPP_TYPE_DEBUG_REPORT_CALLBACK_H__
