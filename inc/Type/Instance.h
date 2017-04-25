#ifndef __VKPP_TYPE_INSTANCE_H__
#define __VKPP_TYPE_INSTANCE_H__



#include <vector>

#include <Info/Common.h>
#include <Info/Layers.h>
#include <Info/Extensions.h>

#include <Type/VkDeleter.h>
#include <Type/AllocationCallbacks.h>
#include <Type/PhysicalDevice.h>
#include <Type/Surface.h>
#include <Type/DebugReportCallback.h>



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

    constexpr ApplicationInfo(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0, uint32_t aApiVersion = DEFAULT_VK_API_VERSION)
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

ConsistencyCheck(ApplicationInfo, pNext, pApplicationName, applicationVersion, pEngineName, engineVersion, apiVersion)



class InstanceInfo : public internal::VkTrait<InstanceInfo, VkInstanceCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eInstance;

public:
    const void*                 pNext{ nullptr };
    VkInstanceCreateFlags       flags{ 0 };
    const ApplicationInfo*      pApplicationInfo{ nullptr };
    uint32_t                    enabledLayerCount{ 0 };
    const char* const*          ppEnabledLayerNames{ nullptr };
    uint32_t                    enabledExtensionCount{ 0 };
    const char* const*          ppEnabledExtensionNames{ nullptr };

    DEFINE_CLASS_MEMBER(InstanceInfo)

    explicit InstanceInfo(const ApplicationInfo& aAppInfo, uint32_t aEnabledLayerCount = 0, const char* const* appEnabledLayerNames = nullptr,
        uint32_t aEnabledExtensionCount = 0, const char* const* appEnabledExtensionNames = nullptr)
        : pApplicationInfo(aAppInfo.AddressOf()), enabledLayerCount(aEnabledLayerCount), ppEnabledLayerNames(appEnabledLayerNames),
        enabledExtensionCount(aEnabledExtensionCount), ppEnabledExtensionNames(appEnabledExtensionNames)
    {}

    InstanceInfo(const ApplicationInfo& aAppInfo, const std::vector<const char*>& aEnabledLayers, const std::vector<const char*>& aEnabledExtensions)
        : InstanceInfo(aAppInfo, static_cast<uint32_t>(aEnabledLayers.size()), aEnabledLayers.data(), static_cast<uint32_t>(aEnabledExtensions.size()), aEnabledExtensions.data())
    {}

    template <std::size_t L, std::size_t E>
    InstanceInfo(const ApplicationInfo& aAppInfo, const std::array<const char*, L>& aEnabledLayers, const std::array<const char*, E>& aEnabledExtensions)
        : InstanceInfo(aAppInfo, static_cast<uint32_t>(aEnabledLayers.size()), aEnabledLayers.data(), static_cast<uint32_t>(aEnabledExtensions.size()), aEnabledExtensions.data())
    {}

    InstanceInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    InstanceInfo& SetAppInfo(const ApplicationInfo& aAppInfo)
    {
        pApplicationInfo = aAppInfo.AddressOf();

        return *this;
    }

    InstanceInfo& SetEnabledLayers(uint32_t aEnabledLayerCount, const char* const* appEnabledLayerNames)
    {
        enabledLayerCount   = aEnabledLayerCount;
        ppEnabledLayerNames = appEnabledLayerNames;

        return *this;
    }

    InstanceInfo& SetEnabledLayers(const std::vector<const char*>& aEnabledLayers)
    {
        return SetEnabledLayers(static_cast<uint32_t>(aEnabledLayers.size()), aEnabledLayers.data());
    }

    template <std::size_t L>
    InstanceInfo& SetEnabledLayers(const std::array<const char*, L>& aEnabledLayers)
    {
        return SetEnabledLayers(static_cast<uint32_t>(L), aEnabledLayers.data());
    }

    InstanceInfo& SetEnabledExtensions(uint32_t aEnabledExtensionCount, const char* const* appEnalbedExtensionNames)
    {
        enabledExtensionCount = aEnabledExtensionCount;
        ppEnabledExtensionNames = appEnalbedExtensionNames;

        return *this;
    }

    InstanceInfo& SetEnalbedExtension(const std::vector<const char*>& aEnabledExtensions)
    {
        return SetEnabledExtensions(static_cast<uint32_t>(aEnabledExtensions.size()), aEnabledExtensions.data());
    }

    template <std::size_t E>
    InstanceInfo& SetEnabledExtensions(const std::array<const char*, E>& aEnabledExtensions)
    {
        return SetEnabledExtensions(static_cast<uint32_t>(E), aEnabledExtensions.data());
    }
};

ConsistencyCheck(InstanceInfo, pNext, flags, pApplicationInfo, enabledLayerCount, ppEnabledLayerNames, enabledExtensionCount, ppEnabledExtensionNames)



class Instance : public internal::VkTrait<Instance, VkInstance>
{
private:
    VkInstance mInstance;

public:
    DEFINE_CLASS_MEMBER(Instance)

    Instance(std::nullptr_t)
    {}

    explicit Instance(const InstanceInfo& aInstanceInfo)
    {
        ThrowIfFailed(vkCreateInstance(&aInstanceInfo, nullptr, &mInstance));
    }

    Instance(const InstanceInfo& aInstanceInfo, const AllocationCallbacks& aAllocator)
    {
        ThrowIfFailed(vkCreateInstance(&aInstanceInfo, &aAllocator, &mInstance));
    }

    ~Instance(void)
    {
        vkDestroyInstance(mInstance, nullptr);          // TODO: Support allocator.
    }

    void Reset(const InstanceInfo& aInstanceInfo)
    {
        ThrowIfFailed(vkCreateInstance(&aInstanceInfo, nullptr, &mInstance));
    }

    void Reset(const InstanceInfo& aInstanceInfo, const AllocationCallbacks& aAllocator)
    {
        ThrowIfFailed(vkCreateInstance(&aInstanceInfo, &aAllocator, &mInstance));
    }

    Instance& operator=(VkInstance aInstance)
    {
        mInstance = aInstance;

        return *this;
    }

    Instance& operator=(std::nullptr_t)
    {
        mInstance = VK_NULL_HANDLE;

        return *this;
    }

    bool operator==(const Instance& aRhs) const
    {
        return mInstance == aRhs.mInstance;
    }

    bool operator!=(const Instance& aRhs) const
    {
        return !(*this == aRhs);
    }

    std::vector<PhysicalDevice> GetPhysicalDevices(void) const
    {
        uint32_t lPhysicalDeviceCount{ 0 };
        ThrowIfFailed(vkEnumeratePhysicalDevices(mInstance, &lPhysicalDeviceCount, nullptr));

        std::vector<PhysicalDevice> lPhysicalDevices(lPhysicalDeviceCount);
        ThrowIfFailed(vkEnumeratePhysicalDevices(mInstance, &lPhysicalDeviceCount, &lPhysicalDevices[0]));

        return lPhysicalDevices;
    }

    static std::vector<LayerProperty> GetLayers(void)
    {
        uint32_t lLayerCount{ 0 };
        ThrowIfFailed(vkEnumerateInstanceLayerProperties(&lLayerCount, nullptr));

        std::vector<LayerProperty> lLayers(lLayerCount);
        ThrowIfFailed(vkEnumerateInstanceLayerProperties(&lLayerCount, &lLayers[0]));

        return lLayers;
    }

    static std::vector<ExtensionProperty> GetExtensions(const char* apLayerName = nullptr)
    {
        uint32_t lExtensionCount{ 0 };
        vkEnumerateInstanceExtensionProperties(apLayerName, &lExtensionCount, nullptr);

        std::vector<ExtensionProperty> lExtensions(lExtensionCount);
        ThrowIfFailed(vkEnumerateInstanceExtensionProperties(apLayerName, &lExtensionCount, &lExtensions[0]));

        return lExtensions;
    }

    khr::Surface CreateSurface(const khr::SurfaceCreateInfo& aCreateInfo) const
    {
        khr::Surface lSurface;
        ThrowIfFailed(vkCreateSurfaceKHR(mInstance, &aCreateInfo, nullptr, &lSurface));

        return lSurface;
    }

    khr::Surface CreateSurface(const khr::SurfaceCreateInfo& aCreateInfo, const AllocationCallbacks& aAllocator) const
    {
        khr::Surface lSurface;
        ThrowIfFailed(vkCreateSurfaceKHR(mInstance, &aCreateInfo, &aAllocator, &lSurface));

        return lSurface;
    }

    void DestroySurface(khr::Surface aSurface) const
    {
        vkDestroySurfaceKHR(mInstance, aSurface, nullptr);
    }

    void DestroySurface(khr::Surface aSurface, const AllocationCallbacks& aAllocator) const
    {
        vkDestroySurfaceKHR(mInstance, aSurface, &aAllocator);
    }

#ifdef _DEBUG

    ext::DebugReportCallback CreateDebugReportCallback(const ext::DebugReportCallbackCreateInfo& aDebugReportCallbackInfo) const
    {
        auto lpFunc = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(mInstance, "vkCreateDebugReportCallbackEXT"));

        ext::DebugReportCallback lDebugReportCallback;
        ThrowIfFailed(lpFunc(mInstance, &aDebugReportCallbackInfo, nullptr, &lDebugReportCallback));

        return lDebugReportCallback;
    }

    void DestroyDebugReportCallback(const ext::DebugReportCallback& aDebugReportCallback) const
    {
        auto lpFunc = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(mInstance, "vkDestroyDebugReportCallbackEXT"));
        lpFunc(mInstance, aDebugReportCallback, nullptr);
    }

#endif              // End of _DEBUG
};

StaticSizeCheck(Instance)



}                    // End of namespace vkpp.



#endif               // __VKPP_TYPE_INSTANCE_H__
