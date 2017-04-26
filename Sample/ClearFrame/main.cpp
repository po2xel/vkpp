#include <iostream>
#include <exception>
#include <thread>

#include "ClearFrame.h"



using namespace vkpp;



int main(int /*argc*/, char* /*argv*/[])
{
    try
    {
        sample::ClearFrame lClearFrame{ "Clear Screen", 1 };
        lClearFrame.MainLoop();
    }
    catch (const std::exception&)
    {}

    return 0;
}