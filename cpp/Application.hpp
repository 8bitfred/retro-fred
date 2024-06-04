#pragma once

#include "fredcore/sdl.hpp"
#include "fredcore/DisplayConfig.hpp"
#include "fredcore/TextureManager.hpp"
#include "fredcore/SoundManager.hpp"
#include "fredcore/GameEvent.hpp"
#include "fredcore/Window.hpp"
#include "GameRunner.hpp"
#include "Menu.hpp"
#include <random>
#include <variant>

struct Config;
class GameBase;
class Fred;

class FredApp
{
    sdl::App app;
    Config cfg;
    std::minstd_rand &random_engine;
    std::pair<sdl::WindowPtr, sdl::RendererPtr> w_and_r;
    DisplayConfig display_cfg;
    TextureManager tmgr;
    SoundManager smgr;
    std::vector<std::pair<unsigned, std::string>> high_scores;
    std::string high_scores_path;

    static std::pair<sdl::WindowPtr, sdl::RendererPtr> initDisplay(Config const &cfg);
    static std::string getPrefPath();
    void saveHighScores() const;
    void loadHighScores();

public:
    explicit FredApp(Config const &cfg, std::minstd_rand &random_engine);
    std::minstd_rand &getRandomEngine() { return random_engine; }
    SDL_Window *getWindow() const { return w_and_r.first; }
    SDL_Renderer *getRenderer() const { return w_and_r.second; }
    TextureManager const &getTextureManager() const { return tmgr; }
    SoundManager &getSoundManager() { return smgr; }
    Config const &getConfig() const { return cfg; }
    DisplayConfig const &getDisplayConfig() const { return display_cfg; }
    std::vector<std::pair<unsigned, std::string>> const &getHighScores() const
    {
        return high_scores;
    }
    void addHighScore(unsigned score, std::string const &initials);
    void mainLoop();
};
