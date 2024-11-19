#pragma once

#include "fredcore/sdl.hpp"
#include "fredcore/Config.hpp"
#include "fredcore/DisplayConfig.hpp"
#include "fredcore/TextureManager.hpp"
#include "fredcore/SoundManager.hpp"
#include "fredcore/GameEvent.hpp"
#include "fredcore/Window.hpp"
#include "fredcore/Menu.hpp"
#include "fredcore/Controller.hpp"
#include "GameRunner.hpp"
#include <random>
#include <variant>
#include <filesystem>
#include <optional>

struct Config;
class GameBase;
class Fred;

class FredApp
{
    sdl::App app;
    Config cfg;
    std::minstd_rand &random_engine;
    DisplayConfig display_cfg;
    std::optional<Controller> controller;
    TextureManager tmgr;
    SoundManager smgr;
    std::vector<std::pair<unsigned, std::string>> high_scores;
    std::filesystem::path high_scores_path, config_path;

    static std::filesystem::path getPrefPath();
    void saveHighScores() const;
    void loadHighScores();

public:
    explicit FredApp(Config const &cfg, std::minstd_rand &random_engine);
    std::minstd_rand &getRandomEngine() { return random_engine; }
    SDL_Window *getWindow() const { return display_cfg.getWindow(); }
    SDL_Renderer *getRenderer() const { return display_cfg.getRenderer(); }
    TextureManager const &getTextureManager() const { return tmgr; }
    SoundManager &getSoundManager() { return smgr; }
    Config const &getConfig() const { return cfg; }
    DisplayConfig const &getDisplayConfig() const { return display_cfg; }
    std::vector<std::pair<unsigned, std::string>> const &getHighScores() const
    {
        return high_scores;
    }
    void addHighScore(unsigned score, std::string const &initials);
    void saveConfig() const;
    void renderController() const;
    void mainLoop();
};
