#ifndef __VKPP_TYPE_ALLOCATIONCALLBACKS_H__
#define __VKPP_TYPE_ALLOCATIONCALLBACKS_H__



#include <Info/Common.h>



namespace vkpp
{



struct DefaultAllocationCallbacks
{
    constexpr const VkAllocationCallbacks* operator&(void) const noexcept
    {
        return nullptr;
    }
};

static constexpr DefaultAllocationCallbacks DefaultAllocator;



class AllocationCallbacks : public internal::VkTrait<AllocationCallbacks, VkAllocationCallbacks>
{
public:
    void*                       pUserData{ nullptr };
    PFN_vkAllocationFunction    pfnAllocation{ nullptr };
    PFN_vkReallocationFunction  pfnReallocation{ nullptr };
    PFN_vkFreeFunction          pfnFree{ nullptr };
    PFN_vkInternalAllocationNotification pfnInternalAllocation{ nullptr };
    PFN_vkInternalFreeNotification       pfnInternalFree{ nullptr };

    DEFINE_CLASS_MEMBER(AllocationCallbacks)

    AllocationCallbacks(void* apUserData, PFN_vkAllocationFunction apfnAllocation, PFN_vkReallocationFunction apfnReallocation, PFN_vkFreeFunction apfnFree,
        PFN_vkInternalAllocationNotification apfnInternalAllocation = nullptr, PFN_vkInternalFreeNotification apfnInternalFree = nullptr)
        : pUserData(apUserData), pfnAllocation(apfnAllocation), pfnReallocation(apfnReallocation), pfnFree(apfnFree),
          pfnInternalAllocation(apfnInternalAllocation), pfnInternalFree(apfnInternalFree)
    {}

    AllocationCallbacks& SetUserData(void* apUserData)
    {
        pUserData = apUserData;

        return *this;
    }

    AllocationCallbacks& SetAlloc(PFN_vkAllocationFunction apfnAllocation, PFN_vkFreeFunction apfnFree, PFN_vkReallocationFunction apfnReallocation)
    {
        pfnAllocation = apfnAllocation;
        pfnReallocation = apfnReallocation;
        pfnFree = apfnFree;

        return *this;
    }

    AllocationCallbacks& SetInternalAlloc(PFN_vkInternalAllocationNotification apfnInternalAllocation, PFN_vkInternalFreeNotification apfnInternalFree)
    {
        pfnInternalAllocation = apfnInternalAllocation;
        pfnInternalFree = apfnInternalFree;

        return *this;
    }
};

ConsistencyCheck(AllocationCallbacks, pUserData, pfnAllocation, pfnReallocation, pfnFree, pfnInternalAllocation, pfnInternalFree)



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_ALLOCATIONCALLBACKS_H__