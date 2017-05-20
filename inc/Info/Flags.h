#ifndef __VKPP_INFO_FLAGS_H__
#define __VKPP_INFO_FLAGS_H__



#include <type_traits>



namespace vkpp::internal
{



struct DefaultFlags
{};



template <typename B, typename V = VkFlags>
class Flags
{
public:
    using BitType = B;
    using VkType = V;

private:
    VkType mMask{ 0 };

public:
    constexpr Flags(void) noexcept = default;

    constexpr Flags(const DefaultFlags&) noexcept
    {}

    constexpr Flags(BitType aBit) noexcept : mMask(static_cast<VkType>(aBit))
    {}

    constexpr Flags(const Flags& aRhs) noexcept : mMask(aRhs.mMask)
    {}

    constexpr Flags& operator=(const Flags& aRhs) noexcept
    {
        mMask = aRhs.mMask;

        return *this;
    }

    constexpr Flags& operator=(BitType aBit) noexcept
    {
        mMask = static_cast<VkType>(aBit);

        return *this;
    }

    explicit operator bool(void) const
    {
        return !!mMask;
    }

    constexpr operator VkType(void) const
    {
        return mMask;
    }

    bool operator!(void) const
    {
        return !mMask;
    }

    bool operator==(const Flags& aRhs) const
    {
        return mMask == aRhs.mMask;
    }

    bool operator!=(const Flags& aRhs) const
    {
        return !(mMask == aRhs.mMask);
    }

    Flags& operator|=(const Flags& aRhs)
    {
        mMask |= aRhs.mMask;

        return *this;
    }

    Flags& operator&=(const Flags& aRhs)
    {
        mMask &= aRhs.mMask;

        return *this;
    }

    Flags& operator^=(const Flags& aRhs)
    {
        mMask ^= aRhs.mMask;

        return *this;
    }

    Flags operator|(const Flags& aRhs) const
    {
        auto lResult{ *this };
        lResult |= aRhs;

        return lResult;
    }

    Flags operator&(const Flags& aRhs) const
    {
        auto lResult{ *this };
        lResult &= aRhs;

        return lResult;
    }

    Flags operator^(const Flags& aRhs) const
    {
        auto lResult{ *this };
        lResult ^= aRhs;

        return lResult;
    }

    Flags operator~(void) const
    {
        constexpr const static auto AllFlags{ 0 };

        auto lResult{ *this };
        lResult.mMask ^= AllFlags;

        return lResult;
    }
};



#define VKPP_ENUM_BIT_MASK_FLAG_BITS_NAME(Name) Name##FlagBits
#define VKPP_ENUM_BIT_MASK_FLAGS_NAME(Name) Name##Flags

#define VKPP_VK_ENUM_FLAGS_NAME(Name) Vk##Name##Flags
#define VKPP_VK_ENUM_FLAGS_NAME_KHR(Name) VKPP_VK_ENUM_FLAGS_NAME(Name)##KHR
#define VKPP_VK_ENUM_FLAGS_NAME_EXT(Name) VKPP_VK_ENUM_FLAGS_NAME(Name)##EXT


#ifdef _WIN32
#define VKPP_VK_ENUM_FLAGS_NAME_OS_KHR(Name) VKPP_VK_ENUM_FLAGS_NAME(Win32 ## Name)##KHR
#endif



#define VKPP_ENUM_BIT_MASK_FLAGS_INTERNAL(FlagsType, BitsType, VkType) using FlagsType = internal::Flags<BitsType, VkType>; \
inline constexpr auto operator|(BitsType aLhs, BitsType aRhs) \
{\
    using Underlying = std::underlying_type_t<BitsType>; \
    return static_cast<BitsType>(static_cast<Underlying>(aLhs) | static_cast<Underlying>(aRhs)); \
}



#define VKPP_ENUM_BIT_MASK_FLAGS(Name) VKPP_ENUM_BIT_MASK_FLAGS_INTERNAL(VKPP_ENUM_BIT_MASK_FLAGS_NAME(Name), VKPP_ENUM_BIT_MASK_FLAG_BITS_NAME(Name), VKPP_VK_ENUM_FLAGS_NAME(Name))
#define VKPP_ENUM_BIT_MASK_FLAGS_KHR(Name) VKPP_ENUM_BIT_MASK_FLAGS_INTERNAL(VKPP_ENUM_BIT_MASK_FLAGS_NAME(Name), VKPP_ENUM_BIT_MASK_FLAG_BITS_NAME(Name), VKPP_VK_ENUM_FLAGS_NAME_KHR(Name))
#define VKPP_ENUM_BIT_MASK_FLAGS_OS_KHR(Name) VKPP_ENUM_BIT_MASK_FLAGS_INTERNAL(VKPP_ENUM_BIT_MASK_FLAGS_NAME(Name), VKPP_ENUM_BIT_MASK_FLAG_BITS_NAME(Name), VKPP_VK_ENUM_FLAGS_NAME_OS_KHR(Name))
#define VKPP_ENUM_BIT_MASK_FLAGS_EXT(Name) VKPP_ENUM_BIT_MASK_FLAGS_INTERNAL(VKPP_ENUM_BIT_MASK_FLAGS_NAME(Name), VKPP_ENUM_BIT_MASK_FLAG_BITS_NAME(Name), VKPP_VK_ENUM_FLAGS_NAME_EXT(Name))



}                   // End of namespace vkpp::internal.



namespace vkpp
{



static constexpr internal::DefaultFlags DefaultFlags;



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_FLAGS_H__