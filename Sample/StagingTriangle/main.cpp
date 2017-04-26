#include <iostream>

#include "StagingTriangle/StagingTriangle.h"




using namespace vkpp;



int main(int /*argc*/, char* /*argv*/[])
{
    try
    {
        sample::StagingTriangle lStagingTriangle("Staging Triangle", 1);
        lStagingTriangle.MainLoop();
    }
    catch (const std::exception&)
    {}

    return 0;
}