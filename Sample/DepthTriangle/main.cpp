#include <iostream>

#include "DepthTriangle/DepthTriangle.h"




using namespace vkpp;



int main(int /*argc*/, char* /*argv*/[])
{
    try
    {
        sample::DepthTriangle lDepthTriangle("Depth Triangle", 1);
        lDepthTriangle.MainLoop();
    }
    catch (const std::exception&)
    {}

    return 0;
}