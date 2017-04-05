#ifndef __VKPP_INFO_APPLICATION_INFO_H__
#define __VKPP_INFO_APPLICATION_INFO_H__



#include <Info/Common.h>



namespace vkpp
{



class ApplicationInfo : public internal::VkTrait<ApplicationInfo, VkApplicationInfo>
{
private:
    const internal::Structure sType = internal::Structure::eApplication;

public:
    const void*             pNext{ nullptr };
    const char*             pApplicationName{ nullptr };
    uint32_t                applicationVersion{ 0 };
    const char*             pEngineName{ nullptr };
    uint32_t                engineVersion{ 0 };
    uint32_t                apiVersion{ DEFAULT_VK_API_VERSION };

    DEFINE_CLASS_MEMBER(ApplicationInfo)

    ApplicationInfo(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0, uint32_t aApiVersion = DEFAULT_VK_API_VERSION)
        : pApplicationName(apApplicationName), applicationVersion(aApplicationVersion), pEngineName(apEngineName), engineVersion(aEngineVersion), apiVersion(aApiVersion)
    {}

    ApplicationInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    ApplicationInfo& SetApplication(const char* apAppName, uint32_t aMajor = 0, uint32_t aMinor = 0, uint32_t aPatch = 0)
    {
        pApplicationName = apAppName;
        applicationVersion = VK_MAKE_VERSION(aMajor, aMinor, aPatch);

        return *this;
    }

    ApplicationInfo& SetEngine(const char* apEngineName, uint32_t aMajor = 0, uint32_t aMinor = 0, uint32_t aPatch = 0)
    {
        pEngineName = apEngineName;
        engineVersion = VK_MAKE_VERSION(aMajor, aMinor, aPatch);

        return *this;
    }

    ApplicationInfo& SetApiVersion(uint32_t aApiVersion = DEFAULT_VK_API_VERSION)
    {
        apiVersion = aApiVersion;

        return *this;
    }
};

StaticSizeCheck(ApplicationInfo);



}                    // End of namespace vkpp.



#endif             // __VKPP_INFO_APPLICATION_INFO_H__
