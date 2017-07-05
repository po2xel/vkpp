#include <iostream>
#include "TextRendering.h"



using namespace vkpp::sample;



namespace vkpp::sample
{
    CApplication& theApp = TheApp::Instance(CApplication::eVideo);
}



int main(int /*argc*/, char* /*argv*/[])
{
    constexpr static auto lAppName = "Text Rendering";
    CWindow lWindow{ lAppName, CWindow::eCentered, CWindow::eCentered, 1024, 768, CWindow::eShown };

    SceneRenderer lTextRenderer{ lWindow, lAppName, 1 };

    theApp.Run();

    return 0;
}