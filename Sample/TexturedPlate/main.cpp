#include <System/Application.h>
#include <Window/Window.h>

#include "TexturedPlate.h"



using namespace vkpp::sample;



namespace vkpp::sample
{
    CApplication& theApp = TheApp::Instance(CApplication::eVideo);
}



int main(int /*argc*/, char* /*argv*/[])
{
    constexpr static auto lAppName = "Textured Plate";
    CWindow lWindow(lAppName, CWindow::eCentered, CWindow::eCentered, 1024, 768, CWindow::eResizable);

    TexturedPlate lTexturedPlate(lWindow, lAppName, 1);

    theApp.Run();

    return 0;
}
