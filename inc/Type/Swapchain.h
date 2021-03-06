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
    Swapchain(void) noexcept = default;

    Swapchain(std::nullptr_t) noexcept
    {}

    explicit Swapchain(VkSwapchainKHR aSwapchain) noexcept : mSwpchain(aSwapchain)
    {}

    operator bool(void) const noexcept
    {
        return mSwpchain != VK_NULL_HANDLE;
    }
};

StaticSizeCheck(Swapchain)



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

    constexpr SurfaceFormat(Format aFormat, ColorSpace aColorSpace) noexcept : format(aFormat), colorSpace(aColorSpace)
    {}

    SurfaceFormat& SetFormat(Format aFormat, ColorSpace aColorSpace) noexcept
    {
        format      = aFormat;
        colorSpace  = aColorSpace;

        return *this;
    }
};

ConsistencyCheck(SurfaceFormat, format, colorSpace)



enum class SwapchainCreateFlagBits
{
    eBindSFR    = VK_SWAPCHAIN_CREATE_BIND_SFR_BIT_KHX
};

VKPP_ENUM_BIT_MASK_FLAGS_KHR(SwapchainCreate)



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
    Clipped                     clipped{ Clipped::Disable };
    Swapchain                   oldSwapchain;

    DEFINE_CLASS_MEMBER(SwapchainCreateInfo)

    SwapchainCreateInfo(const Surface& aSurface, uint32_t aMinImageCount, Format aImageFormat, ColorSpace aImageColorSpace,
        const Extent2D& aImageExtent, const ImageUsageFlags& aImageUsage,
        SurfaceTransformFlagBits aPreTransform, CompositeAlphaFlagBits aCompositeAlpha,
        PresentMode aPresentMode, const Swapchain& aOldSwapChain,
        uint32_t aImageArrayLayers = 1, Clipped aClipped = Clipped::Enable, const SwapchainCreateFlags& aFlags = DefaultFlags) noexcept
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
        uint32_t aImageArrayLayers = 1, Clipped aClipped = Clipped::Enable, const SwapchainCreateFlags& aFlags = DefaultFlags) noexcept
        : flags(aFlags), surface(aSurface),
        minImageCount(aMinImageCount), imageFormat(aImageFormat), imageColorSpace(aImageColorSpace),
        imageExtent(aImageExtent), imageArrayLayers(aImageArrayLayers), imageUsage(aImageUsage), imageSharingMode(SharingMode::eConcurrent),
        queueFamilyIndexCount(aQueueFamilyIndexCount), pQueueFamilyIndices(apQueueFamilyIndices), preTransform(aPreTransform), compositeAlpha(aCompositeAlpha),
        presentMode(aPresentMode), clipped(aClipped), oldSwapchain(aOldSwapChain)
    {}

    template <typename Q, typename = EnableIfValueType<ValueType<Q>, uint32_t>>
    SwapchainCreateInfo(const Surface& aSurface, uint32_t aMinImageCount, const SurfaceFormat& aSurfaceFormat,
        const Extent2D& aImageExtent, const ImageUsageFlags& aImageUsage,
        Q&& aQueueFamilyIndices, SurfaceTransformFlagBits aPreTransform, CompositeAlphaFlagBits aCompositeAlpha,
        PresentMode aPresentMode, const Swapchain& aOldSwapChain,
        uint32_t aImageArrayLayers = 1, Clipped aClipped = Clipped::Enable, const SwapchainCreateFlags& aFlags = DefaultFlags) noexcept
        : SwapchainCreateInfo(aSurface,
        aMinImageCount, aSurfaceFormat.format, aSurfaceFormat.colorSpace,
        aImageExtent, aImageUsage,
        SizeOf<uint32_t>(aQueueFamilyIndices), DataOf(aQueueFamilyIndices), aPreTransform, aCompositeAlpha,
        aPresentMode, aOldSwapChain,
        aImageArrayLayers, aClipped, aFlags)
    {
        StaticLValueRefAssert(Q, aQueueFamilyIndices);
    }

    SwapchainCreateInfo& SetNext(const void* apNext) noexcept
    {
        pNext = apNext;

        return *this;
    }

    SwapchainCreateInfo& SetFlags(const SwapchainCreateFlags& aFlags) noexcept
    {
        flags = aFlags;

        return *this;
    }

    SwapchainCreateInfo& SetSurface(const Surface& aSurface) noexcept
    {
        surface = aSurface;

        return *this;
    }

    SwapchainCreateInfo& SetImageProperties(uint32_t aMinImageCount, Format aImageFormat, ColorSpace aImageColorSpace, const Extent2D& aImageExtent,
        const ImageUsageFlags& aImageUsage, uint32_t aImageArrayLayers = 1) noexcept
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
        const ImageUsageFlags& aImageUsage, uint32_t aImageArrayLayers = 1) noexcept
    {
        return SetImageProperties(aMinImageCount, aSurfaceFormat.format, aSurfaceFormat.colorSpace, aImageExtent, aImageUsage, aImageArrayLayers);
    }

    SwapchainCreateInfo& SetExclusiveMode(void) noexcept
    {
        imageSharingMode            = SharingMode::eExclusive;
        queueFamilyIndexCount       = 0;
        pQueueFamilyIndices         = nullptr;

        return *this;
    }

    SwapchainCreateInfo& SetConcurrentMode(uint32_t aQueueFamilyIndexCount, const uint32_t* apQueueFamilyIndices) noexcept
    {
        imageSharingMode            = SharingMode::eConcurrent;
        queueFamilyIndexCount       = aQueueFamilyIndexCount;
        pQueueFamilyIndices         = apQueueFamilyIndices;

        return *this;
    }

    template <typename Q, typename = EnableIfValueType<ValueType<Q>, uint32_t>>
    SwapchainCreateInfo& SetConcurrentMode(Q&& aQueueFamilyIndices) noexcept
    {
        StaticLValueRefAssert(Q, aQueueFamilyIndices);

        return SetConcurrentMode(SizeOf<uint32_t>(aQueueFamilyIndices), DataOf(aQueueFamilyIndices));
    }

    SwapchainCreateInfo& SetPreTransform(SurfaceTransformFlagBits aPreTransform) noexcept
    {
        preTransform = aPreTransform;

        return *this;
    }

    SwapchainCreateInfo& SetCompositeAlpha(CompositeAlphaFlagBits aCompositeAlpha) noexcept
    {
        compositeAlpha = aCompositeAlpha;

        return *this;
    }

    SwapchainCreateInfo& SetPresentMode(PresentMode aPresentMode) noexcept
    {
        presentMode = aPresentMode;

        return *this;
    }

    SwapchainCreateInfo& EnableClipped(void) noexcept
    {
        clipped = Clipped::Enable;

        return *this;
    }

    SwapchainCreateInfo& DisableClipped(void) noexcept
    {
        clipped = Clipped::Disable;

        return *this;
    }

    SwapchainCreateInfo& SetOldSwapchain(const Swapchain& aOldSwapchain) noexcept
    {
        oldSwapchain = aOldSwapchain;

        return *this;
    }
};

ConsistencyCheck(SwapchainCreateInfo, pNext, flags, surface, minImageCount, imageFormat, imageColorSpace, imageExtent, imageArrayLayers, imageUsage, imageSharingMode,
    queueFamilyIndexCount, pQueueFamilyIndices, preTransform, compositeAlpha, presentMode, clipped, oldSwapchain)



}                   // End of namespace vkpp::khr.



#endif              // __VKPP_TYPE_SWAPCHAIN_H__
