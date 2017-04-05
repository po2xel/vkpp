#ifndef __VKPP_TYPE_VKDELETER_H__
#define __VKPP_TYPE_VKDELETER_H__


#include <functional>



namespace vkpp::internal
{



template <typename T>
class VkDeleter
{
private:
    T mObject{ VK_NULL_HANDLE };
    std::function<void(T)> mDeleter;

    void Reset(void)
    {
        if (mObject != VK_NULL_HANDLE)
            mDeleter(mObject);

        mObject = VK_NULL_HANDLE;
    }

public:
    VkDeleter(void) : VkDeleter([](T, VkAllocationCallbacks*) {})
    {}

    explicit VkDeleter(std::function<void(T, VkAllocationCallbacks*)> aDeleter)
        : mDeleter{ [aDeleter](T aObj) { aDeleter(aObj, nullptr); } }
    {}

    VkDeleter(const VkDeleter<VkInstance>& aInstance, std::function<void(VkInstance, T, VkAllocationCallbacks*)> aDeleter)
        : mDeleter{ [&aInstance, aDeleter](T aObj) { aDeleter(aInstance, aObj, nullptr); } }
    {}

    VkDeleter(const VkDeleter<VkDevice>& aLogicalDevice, std::function<void(VkDevice, T, VkAllocationCallbacks*)> aDeleter)
        : aDeleter{ [&aLogicalDevice, aDeleter](T aObj) { aDeleter(aLogicalDevice, aObj, nullptr); } }
    {}

    ~VkDeleter(void)
    {
        Reset();
    }

    const T* operator&(void) const
    {
        return &mObject;
    }

    T* operator&(void)
    {
        return &mObject;
    }

    T* Replace(void)
    {
        Reset();

        return &mObject;
    }

    operator T(void) const
    {
        return mObject;
    }

    VkDeleter& operator=(T aRhs)
    {
        if (aRhs != mObject)
        {
            Reset();
            mObject = aRhs;
        }
    }

    template <typename U>
    bool operator==(U aRhs)
    {
        return mObject == static_cast<T>(aRhs);
    }
};



}                    // End of namespace vkpp::internal.



#endif              // __VKPP_TYPE_VKDELETER_H__
