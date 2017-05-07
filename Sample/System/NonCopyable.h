#ifndef __VKPP_SAMPLE_SYSTEM_NONCOPYABLE_H__
#define __VKPP_SAMPLE_SYSTEM_NONCOPYABLE_H__


namespace dm
{



// Deleted copy constructor and copy assignment ensure classes derived from
// class CNonCopyale cannot be copied.
class CNonCopyable
{
protected:
    constexpr CNonCopyable(void) = default;
    ~CNonCopyable(void) = default;

    CNonCopyable(const CNonCopyable&) = delete;
    CNonCopyable& operator=(const CNonCopyable&) = delete;
};



}               // End of namespace vkpp::sample.



#endif          // __VKPP_SAMPLE_SYSTEM_NONCOPYABLE_H__