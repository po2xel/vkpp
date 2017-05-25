#include <System/Application.h>
#include <Window/Window.h>

#include "TexturedCube.h"



using namespace vkpp::sample;



namespace vkpp::sample
{
    CApplication& theApp = TheApp::Instance(CApplication::eVideo);
}



int main(int /*argc*/, char* /*argv*/[])
{
    constexpr static auto lAppName = "Textured Cube";
    CWindow lWindow{ lAppName, CWindow::eCentered, CWindow::eCentered, 1024, 768, CWindow::eResizable };

    TexturedCube lTexturedCube{ lWindow, lAppName, 1 };

    theApp.Run();

    return 0;
}