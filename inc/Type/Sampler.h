#ifndef __VKPP_TYPE_SAMPLER_H__
#define __VKPP_TYPE_SAMPLER_H__



#include <Info/Common.h>

#include <Type/Image.h>



namespace vkpp
{



enum class SamplerCreateFlagBits
{};

VKPP_ENUM_BIT_MASK_FLAGS(SamplerCreate)



enum class SamplerMipmapMode
{
    eNearest    = VK_SAMPLER_MIPMAP_MODE_NEAREST,
    eLinear     = VK_SAMPLER_MIPMAP_MODE_LINEAR
};



enum class SamplerAddressMode
{
    eRepeat             = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    eMirroredRepeat     = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
    eClampToEdge        = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
    eClampToBorder      = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
    eMirrorClampToEdge  = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
};



enum class BorderColor
{
    eFloatTransparentBlack      = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
    eIntTransparentBlack        = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK,
    eFloatOpaqueBlack           = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
    eIntOpaqueBlack             = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
    eFloatOpaqueWhite           = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
    eIntOpaqueWhite             = VK_BORDER_COLOR_INT_OPAQUE_WHITE
};



class SamplerCreateInfo : public internal::VkTrait<SamplerCreateInfo, VkSamplerCreateInfo>
{
private:
    const internal::Structure sType = internal::Structure::eSampler;

public:
    const void*         pNext{ nullptr };
    SamplerCreateFlags  flags;
    Filter              magFilter{ Filter::eNearest };
    Filter              minFilter{ Filter::eNearest };
    SamplerMipmapMode   mipmapMode{ SamplerMipmapMode::eNearest };
    SamplerAddressMode  addressModeU{ SamplerAddressMode::eRepeat };
    SamplerAddressMode  addressModeV{ SamplerAddressMode::eRepeat };
    SamplerAddressMode  addressModeW{ SamplerAddressMode::eRepeat };
    float               mipLodBias{ 0.0f };
    Bool32              anisotropyEnable{ VK_FALSE };
    float               maxAnisotropy{ 1.0f };
    Bool32              compareEnable{ VK_FALSE };
    CompareOp           compareOp{ CompareOp::eNever };
    float               minLod{ 0.0f };
    float               maxLod{ 0.0f };
    BorderColor         borderColor{ BorderColor::eFloatTransparentBlack };
    Bool32              unnormalizedCoordinates{ VK_FALSE }; 

    DEFINE_CLASS_MEMBER(SamplerCreateInfo)

    SamplerCreateInfo(Filter aMagFilter, Filter aMinFilter, SamplerMipmapMode aMipmapMode,
        SamplerAddressMode aAddressModeU, SamplerAddressMode aAddressModeV, SamplerAddressMode aAddressModeW,
        float aMipLodBias, Bool32 aAnisotropyEnble, float aMaxAnisotropy, Bool32 aCompareEnable, CompareOp aCompareOp,
        float aMinLod, float aMaxLod, BorderColor aBorderColor, Bool32 aUnnormalizedCoordinates = VK_FALSE, const SamplerCreateFlags& aFlags = DefaultFlags)
        : flags(aFlags), magFilter(aMagFilter), minFilter(aMinFilter), mipmapMode(aMipmapMode), addressModeU(aAddressModeU), addressModeV(aAddressModeV), addressModeW(aAddressModeW),
          mipLodBias(aMipLodBias), anisotropyEnable(aAnisotropyEnble), maxAnisotropy(aMaxAnisotropy), compareEnable(aCompareEnable), compareOp(aCompareOp),
          minLod(aMinLod), maxLod(aMaxLod), borderColor(aBorderColor), unnormalizedCoordinates(aUnnormalizedCoordinates)
    {}

    SamplerCreateInfo& SetNext(const void* apNext)
    {
        pNext   = apNext;

        return *this;
    }

    SamplerCreateInfo& SetFlags(const SamplerCreateFlags& aFlags)
    {
        flags   = aFlags;

        return *this;
    }

    SamplerCreateInfo& SetFilter(Filter aMagFilter, Filter aMinFilter = Filter::eNearest)
    {
        magFilter   = aMagFilter;
        minFilter   = aMinFilter;

        return *this;
    }

    SamplerCreateInfo& SetMipmapMode(SamplerMipmapMode aMipmapMode)
    {
        mipmapMode  = aMipmapMode;

        return *this;
    }

    SamplerCreateInfo& SetAddressMode(SamplerAddressMode aAddressModeU, SamplerAddressMode aAddressModeV = SamplerAddressMode::eRepeat, SamplerAddressMode aAddressModeW = SamplerAddressMode::eRepeat)
    {
        addressModeU    = aAddressModeU;
        addressModeV    = aAddressModeV;
        addressModeW    = aAddressModeW;

        return *this;
    }

    SamplerCreateInfo& SetMipLodBias(float aMipLodBias)
    {
        mipLodBias  = aMipLodBias;

        return *this;
    }

    SamplerCreateInfo& SetAnisotropy(Bool32 aAnisotropyEnable, float aMaxAnisotropy = 0.0f)
    {
        anisotropyEnable    = aAnisotropyEnable;
        maxAnisotropy       = aMaxAnisotropy;

        return *this;
    }

    SamplerCreateInfo& SetCompareEnable(Bool32 aCompareEnable, CompareOp aCompareOp = CompareOp::eAlways)
    {
        compareEnable   = aCompareEnable;
        compareOp       = aCompareOp;

        return *this;
    }

    SamplerCreateInfo& SetLod(float aMinLod, float aMaxLod)
    {
        minLod  = aMinLod;
        maxLod  = aMaxLod;

        return *this;
    }

    SamplerCreateInfo& SetBorderColor(BorderColor aBorderColor)
    {
        borderColor = aBorderColor;

        return *this;
    }

    SamplerCreateInfo& SetUnnormalizedCoordinates(Bool32 aUnnormalizedCoordinates)
    {
        unnormalizedCoordinates = aUnnormalizedCoordinates;

        return *this;
    }
};

ConsistencyCheck(SamplerCreateInfo, pNext, flags, magFilter, minFilter, mipmapMode, addressModeU, addressModeV, addressModeW, mipLodBias, anisotropyEnable, maxAnisotropy,
    compareEnable, compareOp, minLod, maxLod, borderColor, unnormalizedCoordinates)



class Sampler : public internal::VkTrait<Sampler, VkSampler>
{
private:
    VkSampler mSampler{ VK_NULL_HANDLE };

public:
    Sampler(void) = default;

    Sampler(std::nullptr_t)
    {}

    explicit Sampler(VkSampler aSampler) : mSampler(aSampler)
    {}
};

StaticSizeCheck(Sampler)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_SAMPLER_H__
