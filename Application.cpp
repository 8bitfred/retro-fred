#include "Application.hpp"
#include "Config.hpp"
#include "Game.hpp"

FredApp::FredApp(Config const &cfg, std::minstd_rand &random_engine)
    : cfg(cfg)
    , random_engine(random_engine)
    , w_and_r(sdl::createWindowAndRenderer(cfg.scale_x * cfg.window_width,
                                           cfg.scale_y * cfg.window_height)),
      tmgr(getRenderer())
{
    SDL_RenderSetScale(getRenderer(), cfg.scale_x, cfg.scale_y);
}

void FredApp::playGame() const
{
    Game game(cfg, random_engine, tmgr);
    game.mainLoop(getRenderer());
}