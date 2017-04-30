#include <iostream>

#include "UniformTriangle/UniformTriangle.h"




using namespace vkpp;



int main(int /*argc*/, char* /*argv*/[])
{
    try
    {
        sample::UniformTriangle lUniformTriangle("Uniform Triangle", 1);
        lUniformTriangle.MainLoop();
    }
    catch (const std::exception&)
    {}

    return 0;
}