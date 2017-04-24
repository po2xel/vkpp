#ifndef __VKPP_TYPE_SWAPCHAIN_H__
#define __VKPP_TYPE_SWAPCHAIN_H__



#include <Info/Common.h>
#include <Info/Flags.h>
#include <Info/Format.h>
#include <Info/SurfaceCapabilities.h>

#include <Type/Surface.h>


namespace vkpp::khr
{



class Swapchain : public internal::VkTrait<Swapchain, VkSwapchainKHR>
{
private:
    VkSwapchainKHR mSwpchain{ VK_NULL_HANDLE };

public:
    Swapchain(void) = default;

    Swapchain(std::nullptr_t)
    {}

    explicit Swapchain(VkSwapchainKHR aSwapchain) : mSwpchain(aSwapchain)
    {}
};



enum class ColorSpace
{
    esRGBNonLinear = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
};



enum class PresentMode
{
    eImmediate          = VK_PRESENT_MODE_IMMEDIATE_KHR,
    eMailBox            = VK_PRESENT_MODE_MAILBOX_KHR,
    eFIFO               = VK_PRESENT_MODE_FIFO_KHR,
    eFIFORelaxed        = VK_PRESENT_MODE_FIFO_RELAXED_KHR
};



struct SurfaceFormat : public internal::VkTrait<SurfaceFormat, VkSurfaceFormatKHR>
{
    Format      format;
    ColorSpace  colorSpace;

    DEFINE_CLASS_MEMBER(SurfaceFormat)

    SurfaceFormat(Format aFormat, ColorSpace aColorSpace) : format(aFormat), colorSpace(aColorSpace)
    {}

    SurfaceFormat& SetFormat(Format aFormat, ColorSpace aColorSpace)
    {
        format = aFormat;
        colorSpace = aColorSpace;

        return *this;
    }
};

StaticSizeCheck(SurfaceFormat);



enum class SwapchainCreateFlagBits
{
    eBindSFR    = VK_SWAPCHAIN_CREATE_BIND_SFR_BIT_KHX
};

using SwapchainCreateFlags = internal::Flags<SwapchainCreateFlagBits, VkSwapchainCreateFlagsKHR>;



class SwapchainCreateInfo : public internal::VkTrait<SwapchainCreateInfo, VkSwapchainCreateInfoKHR>
{
private:
    const internal::Structure sType = internal::Structure::eSwapchain;

public:
    const void*                 pNext{ nullptr };
    SwapchainCreateFlags        flags;
    Surface                     surface;
    uint32_t                    minImageCount{ 0 };
    Format                      imageFormat;
    ColorSpace                  imageColorSpace;
    Extent2D                    imageExtent;
    uint32_t                    imageArrayLayers{ 0 };
    ImageUsageFlags             imageUsage;
    SharingMode                 imageSharingMode{ SharingMode::eExclusive };
    uint32_t                    queueFamilyIndexCount{ 0 };
    const uint32_t*             pQueueFamilyIndices{ nullptr };
    SurfaceTransformFlagBits    preTransform;
    CompositeAlphaFlagBits      compositeAlpha;
    PresentMode                 presentMode;
    Bool32                      clipped{ VK_TRUE };
    Swapchain                   oldSwapchain;

    DEFINE_CLASS_MEMBER(SwapchainCreateInfo)

    SwapchainCreateInfo(const Surface& aSurface, uint32_t aMinImageCount, Format aImageFormat, ColorSpace aImageColorSpace,
        const Extent2D& aImageExtent, const ImageUsageFlags& aImageUsage,
        SurfaceTransformFlagBits aPreTransform, CompositeAlphaFlagBits aCompositeAlpha,
        PresentMode aPresentMode, const Swapchain& aOldSwapChain,
        uint32_t aImageArrayLayers = 1, Bool32 aClipped = VK_TRUE, const SwapchainCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), surface(aSurface),
        minImageCount(aMinImageCount), imageFormat(aImageFormat), imageColorSpace(aImageColorSpace),
        imageExtent(aImageExtent), imageArrayLayers(aImageArrayLayers), imageUsage(aImageUsage),
        preTransform(aPreTransform), compositeAlpha(aCompositeAlpha),
        presentMode(aPresentMode), clipped(aClipped), oldSwapchain(aOldSwapChain)
    {}

    SwapchainCreateInfo(const Surface& aSurface, uint32_t aMinImageCount, Format aImageFormat, ColorSpace aImageColorSpace,
        const Extent2D& aImageExtent, const ImageUsageFlags& aImageUsage,
        uint32_t aQueueFamilyIndexCount, const uint32_t* apQueueFamilyIndices, SurfaceTransformFlagBits aPreTransform, CompositeAlphaFlagBits aCompositeAlpha,
        PresentMode aPresentMode, const Swapchain& aOldSwapChain,
        uint32_t aImageArrayLayers = 1, Bool32 aClipped = VK_TRUE, const SwapchainCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), surface(aSurface),
        minImageCount(aMinImageCount), imageFormat(aImageFormat), imageColorSpace(aImageColorSpace),
        imageExtent(aImageExtent), imageArrayLayers(aImageArrayLayers), imageUsage(aImageUsage), imageSharingMode(SharingMode::eConcurrent),
        queueFamilyIndexCount(aQueueFamilyIndexCount), pQueueFamilyIndices(apQueueFamilyIndices), preTransform(aPreTransform), compositeAlpha(aCompositeAlpha),
        presentMode(aPresentMode), clipped(aClipped), oldSwapchain(aOldSwapChain)
    {}

    SwapchainCreateInfo(const Surface& aSurface, uint32_t aMinImageCount, const SurfaceFormat& aSurfaceFormat,
        const Extent2D& aImageExtent, const ImageUsageFlags& aImageUsage,
        const std::vector<uint32_t>& aQueueFamilyIndices, SurfaceTransformFlagBits aPreTransform, CompositeAlphaFlagBits aCompositeAlpha,
        PresentMode aPresentMode, const Swapchain& aOldSwapChain,
        uint32_t aImageArrayLayers = 1, Bool32 aClipped = VK_TRUE, const SwapchainCreateFlags& aFlags = DefaultFlags)
        : SwapchainCreateInfo(aSurface,
            aMinImageCount, aSurfaceFormat.format, aSurfaceFormat.colorSpace,
            aImageExtent, aImageUsage,
            static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data(), aPreTransform, aCompositeAlpha,
            aPresentMode, aOldSwapChain,
            aImageArrayLayers, aClipped, aFlags)
    {}

    template <std::size_t Q>
    SwapchainCreateInfo(const Surface& aSurface, uint32_t aMinImageCount, const SurfaceFormat& aSurfaceFormat,
        const Extent2D& aImageExtent, const ImageUsageFlags& aImageUsage, SharingMode aSharingMode,
        const std::array<uint32_t, Q>& aQueueFamilyIndices, SurfaceTransformFlagBits aPreTransform, CompositeAlphaFlagBits aCompositeAlpha,
        PresentMode aPresentMode, const Swapchain& aOldSwapChain,
        uint32_t aImageArrayLayers = 1, Bool32 aClipped = VK_TRUE, const SwapchainCreateFlags& aFlags = DefaultFlags)
        : SwapchainCreateInfo(aSurface,
            aMinImageCount, aSurfaceFormat.format, aSurfaceFormat.colorSpace,
            aImageExtent, aImageUsage, aSharingMode,
            static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data(), aPreTransform, aCompositeAlpha,
            aPresentMode, aOldSwapChain,
            aImageArrayLayers, aClipped, aFlags)
    {}

    SwapchainCreateInfo& SetNext(const void* apNext)
    {
        pNext = apNext;

        return *this;
    }

    SwapchainCreateInfo& SetFlags(const SwapchainCreateFlags& aFlags)
    {
        flags = aFlags;

        return *this;
    }

    SwapchainCreateInfo& SetSurface(const Surface& aSurface)
    {
        surface = aSurface;

        return *this;
    }

    SwapchainCreateInfo& SetImageProperties(uint32_t aMinImageCount, Format aImageFormat, ColorSpace aImageColorSpace, const Extent2D& aImageExtent,
        const ImageUsageFlags& aImageUsage, uint32_t aImageArrayLayers = 1)
    {
        minImageCount       = aMinImageCount;
        imageFormat         = aImageFormat;
        imageColorSpace     = aImageColorSpace;
        imageExtent         = aImageExtent;
        imageArrayLayers    = aImageArrayLayers;
        imageUsage          = aImageUsage;

        return *this;
    }

    SwapchainCreateInfo& SetImageProperties(uint32_t aMinImageCount, const SurfaceFormat& aSurfaceFormat, const Extent2D& aImageExtent,
        const ImageUsageFlags& aImageUsage, uint32_t aImageArrayLayers = 1)
    {
        return SetImageProperties(aMinImageCount, aSurfaceFormat.format, aSurfaceFormat.colorSpace, aImageExtent, aImageUsage, aImageArrayLayers);
    }

    SwapchainCreateInfo& SetExclusiveMode(void)
    {
        imageSharingMode             = SharingMode::eExclusive;
        queueFamilyIndexCount   = 0;
        pQueueFamilyIndices     = nullptr;

        return *this;
    }

    SwapchainCreateInfo& SetConcurrentMode(uint32_t aQueueFamilyIndexCount, const uint32_t* apQueueFamilyIndices)
    {
        imageSharingMode             = SharingMode::eConcurrent;
        queueFamilyIndexCount   = aQueueFamilyIndexCount;
        pQueueFamilyIndices     = apQueueFamilyIndices;

        return *this;
    }

    SwapchainCreateInfo& SetConcurrentMode(const std::vector<uint32_t>& aQueueFamilyIndices)
    {
        return SetConcurrentMode(static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data());
    }

    template <std::size_t Q>
    SwapchainCreateInfo& SetConcurrentMode(const std::array<uint32_t, Q>& aQueueFamilyIndices)
    {
        return SetConcurrentMode(static_cast<uint32_t>(aQueueFamilyIndices.size()), aQueueFamilyIndices.data());
    }

    SwapchainCreateInfo& SetPreTransform(SurfaceTransformFlagBits aPreTransform)
    {
        preTransform = aPreTransform;

        return *this;
    }

    SwapchainCreateInfo& SetCompositeAlpha(CompositeAlphaFlagBits aCompositeAlpha)
    {
        compositeAlpha = aCompositeAlpha;

        return *this;
    }

    SwapchainCreateInfo& SetPresentMode(PresentMode aPresentMode)
    {
        presentMode = aPresentMode;

        return *this;
    }

    SwapchainCreateInfo& SetClipped(Bool32 aClipped = VK_TRUE)
    {
        clipped = aClipped;

        return *this;
    }

    SwapchainCreateInfo& SetOldSwapchain(const Swapchain& aOldSwapchain)
    {
        oldSwapchain = aOldSwapchain;

        return *this;
    }
};

ConsistencyCheck(SwapchainCreateInfo, pNext, flags, surface, minImageCount, imageFormat, imageColorSpace, imageExtent, imageArrayLayers, imageUsage, imageSharingMode,
    queueFamilyIndexCount, pQueueFamilyIndices, preTransform, compositeAlpha, presentMode, clipped, oldSwapchain)



}                   // End of namespace vkpp::khr.



#endif              // __VKPP_TYPE_SWAPCHAIN_H__
