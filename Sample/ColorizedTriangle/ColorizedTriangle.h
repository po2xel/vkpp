#ifndef __VKPP_SAMPLE_COLORIZED_TRIANGLE_H__
#define __VKPP_SAMPLE_COLORIZED_TRIANGLE_H__



#include <Info/PipelineStage.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm//glm.hpp>

#include "Base/ExampleBase.h"



namespace vkpp::sample
{



struct VertexData
{
    glm::vec3 inPosition;
    glm::vec3 inColor;

    static const vkpp::VertexInputBindingDescription& GetBindingDescription(void)
    {
        static const vkpp::VertexInputBindingDescription lInputBindingDescription
        {
            0,                                  // binding
            sizeof(VertexData),                 // stride
            vkpp::VertexInputRate::eVertex      // inputRate
        };

        return lInputBindingDescription;
    }

    static decltype(auto) GetAttributeDescriptions(void)
    {
        static const std::array<vkpp::VertexInputAttributeDescription, 2> lInputAttributeDescriptions
        { {
            {
                0,                                  // location
                0,                                  // binding
                vkpp::Format::eRGB32sFloat,         // format
                offsetof(VertexData, inPosition)    // offset
            },
            {
                1,                                  // location
                0,                                  // binding
                vkpp::Format::eRGB32sFloat,          // format
                offsetof(VertexData, inColor)       // offset
            }
        } };

        return lInputAttributeDescriptions;
    }
};



class ColorizedTriangle : ExampleBase
{
public:
    ColorizedTriangle(const char* apApplicationName, uint32_t aApplicationVersion, const char* apEngineName = nullptr, uint32_t aEngineVersion = 0);
};



}                   // End of namespace vkpp::sample.



#endif              // __VKPP_SAMPLE_COLORIZED_TRIANGLE_H__
