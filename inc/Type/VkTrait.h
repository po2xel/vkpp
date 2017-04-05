#ifndef __VKPP_TYPE_VKTRAIT_H__
#define __VKPP_TYPE_VKTRAIT_H__



namespace vkpp::internal
{



template <typename T, typename VT>
struct VkTrait
{
    using Type = T;
    using VkType = VT;

    VkTrait(void) = default;

    const Type* AddressOf(void) const
    {
        return static_cast<const Type*>(this);
    }

    const VkType* operator&(void) const
    {
        return reinterpret_cast<const VkType*>(this);
    }

    VkType* operator&(void)
    {
        return reinterpret_cast<VkType*>(this);
    }

    operator const VkType&(void) const
    {
        return *reinterpret_cast<const VkType*>(this);
    }
};



}                    // End of namespace vkpp::internal.



#endif             // __VKPP_TYPE_VKTRAIT_H__
