#pragma once

#include "sdl.hpp"
#include "TextureManager.hpp"
#include "SoundManager.hpp"
#include <random>

struct Config;
class Game;
class Fred;

class FredApp : sdl::App
{
public:
    explicit FredApp(Config const &cfg, std::minstd_rand &random_engine);
    SDL_Window *getWindow() const { return w_and_r.first; }
    SDL_Renderer *getRenderer() const { return w_and_r.second; }
    TextureManager const &getTextureManager() const { return tmgr; }
    void playGame();

private:
    Fred* initializeFred(Game &game);
    void initializeAcidDrops(Game &game);
    void debugMode(Game &game, Fred *fred, unsigned events);

    Config const &cfg;
    std::minstd_rand &random_engine;
    std::pair<sdl::WindowPtr, sdl::RendererPtr> w_and_r;
    TextureManager tmgr;
    SoundManager smgr;
};