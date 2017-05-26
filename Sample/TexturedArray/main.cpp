#include <System/Application.h>
#include <Window/Window.h>

#include "TexturedArray.h"



using namespace vkpp::sample;



namespace vkpp::sample
{
    CApplication& theApp = TheApp::Instance(CApplication::eVideo);
}



int main(int /*argc*/, char* /*argv*/[])
{
    constexpr static auto lAppName = "Textured Array";
    CWindow lWindow{ lAppName, CWindow::eCentered, CWindow::eCentered, 1024, 768, CWindow::eResizable };

    TexturedArray lTexturedArray{ lWindow, lAppName, 1 };

    theApp.Run();

    return 0;
}