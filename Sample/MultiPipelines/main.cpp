#include <System/Application.h>
#include <Window/Window.h>

#include "MultiPipelines.h"



using namespace vkpp::sample;



namespace vkpp::sample
{
    CApplication& theApp = TheApp::Instance(CApplication::eVideo);
}



int main(int /*argc*/, char* /*argv*/[])
{
    Model lModel("../model/treasure_smooth.dae");
    lModel;

    constexpr static auto lApplicationName = "Multiple Pipelines";
    CWindow lWindow{ lApplicationName, CWindow::eCentered, CWindow::eCentered, 1024, 768, CWindow::eResizable };

    MultiPipelines lMultiPipelines{ lWindow, lApplicationName, 1 };

    theApp.Run();

    return 0;
}
