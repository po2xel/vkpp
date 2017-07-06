#include <System/Application.h>
#include <Window/Window.h>

#include "ParticleFire.h"


using namespace vkpp::sample;



namespace vkpp::sample
{
    CApplication& theApp = TheApp::Instance(CApplication::eVideo);
}



int main(int /*argc*/, char* /*argv*/[])
{
    constexpr auto lAppName = "Particle Fire";
    CWindow lWindow{ lAppName, CWindow::eCentered, CWindow::eCentered, 1024, 768, CWindow::eShown };

    ParticleFire lParticleFire{ lWindow, lAppName, 1 };

    theApp.Run();

    return EXIT_SUCCESS;
}