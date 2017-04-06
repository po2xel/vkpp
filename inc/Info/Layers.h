#ifndef __VKPP_INFO_LAYERS_H__
#define __VKPP_INFO_LAYERS_H__



#include <Info/Common.h>



constexpr const char* LUNARG_API_DUMP_NAME              = "VK_LAYER_LUNARG_api_dump";
constexpr const char* LUNARG_CORE_VALIDATION_NAME       = "VK_LAYER_LUNARG_core_validation";
constexpr const char* LUNARG_MONITOR_NAME               = "VK_LAYER_LUNARG_monitor";
constexpr const char* LUNARG_OBJECT_TRACKER_NAME        = "VK_LAYER_LUNARG_object_tracker";
constexpr const char* LUNARG_PARAMETER_VALIDATION_NAME  = "VK_LAYER_LUNARG_parameter_validation";
constexpr const char* LUNARG_SCREENSHOT_NAME            = "VK_LAYER_LUNARG_screenshot";
constexpr const char* LUNARG_SWAPCHAIN_NAME             = "VK_LAYER_LUNARG_swapchain";
constexpr const char* LUNARG_VKTRACE_NAME               = "VK_LAYER_LUNARG_vktrace";
constexpr const char* LUNARG_STANDARD_VALIDATION_NAME   = "VK_LAYER_LUNARG_standard_validation";

constexpr const char* GOOGLE_THREADING_NAME             = "VK_LAYER_GOOGLE_threading";
constexpr const char* GOOGLE_UNIQUE_OBJECTS_NAME        = "VK_LAYER_GOOGLE_unique_objects";

constexpr const char* RENDERDOC_CAPTURE_NAME            = "VK_LAYER_RENDERDOC_Capture";

constexpr const char* NV_OPTIMUS_NAME                   = "VK_LAYER_NV_optimus";



namespace vkpp
{



struct LayerProperty : public internal::VkTrait<LayerProperty, VkLayerProperties>
{
    char        layerName[VK_MAX_EXTENSION_NAME_SIZE]{};
    uint32_t    specVersion{ 0 };
    uint32_t    implementationVersion{ 0 };
    char        description[VK_MAX_DESCRIPTION_SIZE]{};

    DEFINE_CLASS_MEMBER(LayerProperty)
};

StaticSizeCheck(LayerProperty);



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_LAYERS_H__
