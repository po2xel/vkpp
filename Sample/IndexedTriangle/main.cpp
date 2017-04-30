#include <iostream>

#include "IndexedTriangle/IndexedTriangle.h"




using namespace vkpp;



int main(int /*argc*/, char* /*argv*/[])
{
    try
    {
        sample::IndexedTriangle lIndexedTriangle("Indexed Triangle", 1);
        lIndexedTriangle.MainLoop();
    }
    catch (const std::exception&)
    {}

    return 0;
}