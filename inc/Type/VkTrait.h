#ifndef __VKPP_TYPE_VKTRAIT_H__
#define __VKPP_TYPE_VKTRAIT_H__



namespace vkpp::internal
{



template <typename T, typename VT>
struct VkTrait
{
    using Type = T;
    using VkType = VT;

    constexpr VkTrait(void) noexcept = default;

    constexpr const Type* AddressOf(void) const noexcept
    {
        return static_cast<const Type*>(this);
    }

    constexpr const VkType* operator&(void) const noexcept
    {
        return reinterpret_cast<const VkType*>(this);
    }

    constexpr VkType* operator&(void) noexcept
    {
        return reinterpret_cast<VkType*>(this);
    }

    constexpr operator const VkType&(void) const noexcept
    {
        return *reinterpret_cast<const VkType*>(this);
    }
};



}                    // End of namespace vkpp::internal.



#endif             // __VKPP_TYPE_VKTRAIT_H__
