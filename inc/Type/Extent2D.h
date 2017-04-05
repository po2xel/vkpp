#ifndef __VKPP_TYPE_EXTENT2D_H__
#define __VKPP_TYPE_EXTENT2D_H__



#include <Info/Common.h>



namespace vkpp
{



struct Extent2D : public internal::VkTrait<Extent2D, VkExtent2D>
{
    uint32_t width{ 0 };
    uint32_t height{ 0 };

    DEFINE_CLASS_MEMBER(Extent2D)

    Extent2D(uint32_t aWidth, uint32_t aHeight) : width(aWidth), height(aHeight)
    {}

    Extent2D& SetWidth(uint32_t aWidth)
    {
        width = aWidth;

        return *this;
    }

    Extent2D& SetHeight(uint32_t aHeight)
    {
        height = aHeight;

        return *this;
    }

    bool operator==(const Extent2D& aRhs) const
    {
        return width == aRhs.width &&
               height == aRhs.height;
    }

    bool operator!=(const Extent2D& aRhs) const
    {
        return !(*this == aRhs);
    }
};

StaticSizeCheck(Extent2D);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_EXTENT3D_H__