#include <iostream>
#include "sdl.hpp"
#include "Config.hpp"
#include "GameMap.hpp"
#include "Application.hpp"
#include "Game.hpp"

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
    FredApp fred_app(cfg, random_engine);
    fred_app.playGame();
    return 0;
}
