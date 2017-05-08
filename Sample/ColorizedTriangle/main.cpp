#include <System/Application.h>
#include <Window/Window.h>

#include "ColorizedTriangle/ColorizedTriangle.h"



using namespace vkpp::sample;



namespace vkpp::sample
{
    CApplication& theApp = TheApp::Instance(CApplication::VIDEO);
}



int main(int /*argc*/, char* /*argv*/[])
{
    ColorizedTriangle lColorizedTriangle("Colorized Triangle", 1);

    theApp.Run();

    return 0;
}
