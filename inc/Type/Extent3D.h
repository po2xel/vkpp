#ifndef __VKPP_TYPE_EXTENT3D_H__
#define __VKPP_TYPE_EXTENT3D_H__



#include <Info/Common.h>



namespace vkpp
{



struct Extent3D : public internal::VkTrait<Extent3D, VkExtent3D>
{
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    uint32_t depth{ 0 };

    DEFINE_CLASS_MEMBER(Extent3D)

    Extent3D(uint32_t aWidth, uint32_t aHeight, uint32_t aDepth) : width(aWidth), height(aHeight), depth(aDepth)
    {}

    Extent3D& SetWidth(uint32_t aWidth)
    {
        width = aWidth;

        return *this;
    }

    Extent3D& SetHeight(uint32_t aHeight)
    {
        height = aHeight;

        return *this;
    }

    Extent3D& SetDepth(uint32_t aDepth)
    {
        depth = aDepth;

        return *this;
    }

    bool operator==(const Extent3D& aRhs) const
    {
        return width == aRhs.width &&
               height == aRhs.height &&
               depth == aRhs.depth;
    }

    bool operator!=(const Extent3D& aRhs) const
    {
        return !(*this == aRhs);
    }
};

StaticSizeCheck(Extent3D);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_EXTENT3D_H__
