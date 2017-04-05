#ifndef __VKPP_TYPE_ALLOCATIONCALLBACKS_H__
#define __VKPP_TYPE_ALLOCATIONCALLBACKS_H__



#include <Info/Common.h>



namespace vkpp
{



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

    explicit AllocationCallbacks(void* apUserData = nullptr, PFN_vkAllocationFunction apfnAllocation = nullptr, PFN_vkReallocationFunction apfnReallocation = nullptr,
        PFN_vkFreeFunction apfnFree = nullptr, PFN_vkInternalAllocationNotification apfnInternalAllocation = nullptr, PFN_vkInternalFreeNotification apfnInternalFree = nullptr)
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

StaticSizeCheck(AllocationCallbacks);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_ALLOCATIONCALLBACKS_H__