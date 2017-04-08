#ifndef __VKPP_TYPE_IMAGE_H__
#define __VKPP_TYPE_IMAGE_H__



#include <Info/Common.h>



namespace vkpp
{



class Image : public internal::VkTrait<Image, VkImage>
{
private:
    VkImage mImage{ VK_NULL_HANDLE };

public:
    Image(void) = default;

    Image(std::nullptr_t)
    {}

    explicit Image(VkImage aImage) : mImage(aImage)
    {}
};

StaticSizeCheck(Image);



}                   // End of namespace vkpp.



#endif              // __VKPP_TYPE_IMAGE_H__
