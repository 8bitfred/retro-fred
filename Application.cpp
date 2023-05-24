#include "Application.hpp"

FredApp::FredApp()
//    : w_and_r(sdl::createWindowAndRenderer(5 * 256, 5 * 192)), tmgr(getRenderer())
    : w_and_r(sdl::createWindowAndRenderer(35*32, 35*40)), tmgr(getRenderer())
{
    //SDL_RenderSetScale(getRenderer(), 5, 5);
}