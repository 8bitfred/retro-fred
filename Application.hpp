#pragma once

#include "sdl.hpp"
#include "TextureManager.hpp"

class FredApp : sdl::App
{
public:
    FredApp();
    SDL_Window *getWindow() const { return w_and_r.first; }
    SDL_Renderer *getRenderer() const { return w_and_r.second; }
    TextureManager const &getTextureManager() const { return tmgr; }

private:
    std::pair<sdl::WindowPtr, sdl::RendererPtr> w_and_r;
    TextureManager tmgr;
};