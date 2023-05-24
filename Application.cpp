#include "Application.hpp"
#include "Config.hpp"

FredApp::FredApp(Config const& cfg)
//    : w_and_r(sdl::createWindowAndRenderer(5 * 256, 5 * 192)), tmgr(getRenderer())
    : w_and_r(sdl::createWindowAndRenderer(cfg.window_width, cfg.window_height)), tmgr(getRenderer())
{
    SDL_RenderSetScale(getRenderer(), cfg.scale_x, cfg.scale_y);
}