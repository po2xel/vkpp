#include <iostream>

#include "TexturedTriangle/TexturedTriangle.h"




using namespace vkpp;



int main(int /*argc*/, char* /*argv*/[])
{
    try
    {
        sample::TexturedTriangle lTexturedTriangle("Textured Triangle", 1);
        lTexturedTriangle.MainLoop();
    }
    catch (const std::exception&)
    {}

    return 0;
}