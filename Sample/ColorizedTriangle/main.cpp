#include <System/Application.h>
#include <Window/Window.h>

#include "ColorizedTriangle/ColorizedTriangle.h"



using namespace vkpp::sample;



namespace vkpp::sample
{
    CApplication& theApp = TheApp::Instance(CApplication::eVideo);
}



int main(int /*argc*/, char* /*argv*/[])
{
    static constexpr auto ApplicationName = "Colorized Triangle";

    CWindow lWindow(ApplicationName, CWindow::eCentered, CWindow::eCentered, 1024, 768, CWindow::eResizable);

    ColorizedTriangle lColorizedTriangle(lWindow, ApplicationName, 1);

    theApp.Run();

    return 0;
}
