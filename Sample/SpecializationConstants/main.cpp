#include <System/Application.h>
#include <Window/Window.h>

#include "SpecializationConstants.h"
#include <random>
#include <iostream>


using namespace vkpp::sample;



namespace vkpp::sample
{
    CApplication& theApp = TheApp::Instance(CApplication::eVideo);
}



int main(int /*argc*/, char* /*argv*/[])
{
    std::random_device lRndDev;
    std::mt19937 lRndGen(static_cast<uint32_t>(lRndDev()));
    std::normal_distribution<float> lRndDist{ -1.0f, 1.0f };

    std::cout << lRndDist(lRndGen) << '\t' << lRndDist(lRndGen) << '\t' << lRndDist(lRndGen) << '\t' << std::endl;

    constexpr static auto lAppName = "Specialization Constants";
    CWindow lWindow{ lAppName, CWindow::eCentered, CWindow::eCentered, 1024, 768, CWindow::eResizable };

    SpecializationConstants lSpecializationConstants{ lWindow, lAppName, 1 };

    theApp.Run();

    return 0;
}