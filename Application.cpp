#include "Application.hpp"
#include "Config.hpp"

FredApp::FredApp(Config const &cfg)
    : w_and_r(sdl::createWindowAndRenderer(cfg.scale_x * cfg.window_width,
                                           cfg.scale_y * cfg.window_height)),
      tmgr(getRenderer())
{
    SDL_RenderSetScale(getRenderer(), cfg.scale_x, cfg.scale_y);
}
