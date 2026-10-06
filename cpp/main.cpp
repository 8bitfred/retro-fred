#include <iostream>
#include "sdlbackend/SdlMain.hpp"
#include "sdlbackend/SdlPlatform.hpp"
#include "fredcore/Config.hpp"
#include "fredcore/GameMap.hpp"
#include "fredgame/Application.hpp"
#include "fredcore/Game.hpp"

int main(int argc, char* argv[])
{
    const Config cfg(argc, argv);

    std::minstd_rand random_engine;
    if (cfg.debug_map)
        random_engine.seed(1);
    else
    {
        std::random_device random_dev;
        std::uniform_int_distribution<std::uint32_t> distrib;
        random_engine.seed(distrib(random_dev));
    }
    SdlPlatform platform;
    FredApp fred_app(cfg, random_engine, platform);
    fred_app.mainLoop();
    return 0;
}
