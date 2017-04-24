#ifndef __VKPP_INFO_FLAGS_H__
#define __VKPP_INFO_FLAGS_H__



#include <Info/Common.h>



namespace vkpp::internal
{



struct DefaultFlags
{};



template <typename BitType, typename VkType = VkFlags>
class Flags
{
private:
    VkType mMask{ 0 };

public:
    constexpr const static auto AllFlags{ 0 };

    constexpr Flags(void) = default;

    constexpr Flags(const DefaultFlags&)
    {}

    constexpr Flags(BitType aBit) : mMask(static_cast<VkType>(aBit))
    {}

    constexpr Flags(const Flags& aRhs) : mMask(aRhs.mMask)
    {}

    Flags& operator=(const Flags& aRhs)
    {
        mMask = aRhs.mMask;

        return *this;
    }

    explicit operator bool(void) const
    {
        return !!mMask;
    }

    operator VkType(void) const
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
        auto lResult{ *this };
        lResult.mMask ^= AllFlags;

        return lResult;
    }
};



}                   // End of namespace vkpp::internal.



namespace vkpp
{



constexpr auto DefaultFlags = internal::DefaultFlags();



}                   // End of namespace vkpp.



#endif              // __VKPP_INFO_FLAGS_H__