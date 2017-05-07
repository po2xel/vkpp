#ifndef __VKPP_SAMPLE_SYSTEM_SINGLETON_H__
#define __VKPP_SAMPLE_SYSTEM_SINGLETON_H__



#include <utility>



namespace vkpp::sample
{



template <typename T>
class CSingleton
{
public:
    template <typename... Args>
    static T& Instance(Args&&... aArgs)
    {
        static T lInstance(std::forward<Args>(aArgs)...);

        return lInstance;
    }
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_SYSTEM_SINGLETON_H__
