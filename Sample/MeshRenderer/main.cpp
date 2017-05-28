#include <System/Application.h>
#include <Window/Window.h>

#include "MeshRenderer.h"


using namespace vkpp::sample;



namespace vkpp::sample
{
    CApplication& theApp = TheApp::Instance(CApplication::eVideo);
}



int main(int /*argc*/, char* /*argv*/[])
{
    constexpr static auto lAppName = "Mesh Renderer";
    CWindow lWindow{ lAppName, CWindow::eCentered, CWindow::eCentered, 1024, 768, CWindow::eResizable };

    MeshRenderer lMeshRenderer{ lWindow, lAppName, 1 };

    theApp.Run();

    return 0;
}