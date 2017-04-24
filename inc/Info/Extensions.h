#ifndef __VKPP_INFO_EXTENSIONS_H__
#define __VKPP_INFO_EXTENSIONS_H__



#include <Info/Common.h>


constexpr const char* KHR_SURFACE_EXT_NAME      = VK_KHR_SURFACE_EXTENSION_NAME;
constexpr const char* KHR_SWAPCHAIN_EXT_NAME    = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

#ifdef VK_USE_PLATFORM_WIN32_KHR
constexpr const char* KHR_OS_SURFACE_EXT_NAME   = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#endif

constexpr const char* EXT_DEBUG_REPORT_EXT_NAME = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;


namespace vkpp
{



struct ExtensionProperty : public internal::VkTrait<ExtensionProperty, VkExtensionProperties>
{
    char        extensionName[VK_MAX_EXTENSION_NAME_SIZE]{};
    uint32_t    specVersion{ 0 };

    DEFINE_CLASS_MEMBER(ExtensionProperty)
};

ConsistencyCheck(ExtensionProperty, extensionName, specVersion)



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_EXTENSIONS_H__
