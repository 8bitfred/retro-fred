#pragma once

#include "sdl.hpp"
#include "DisplayConfig.hpp"
#include "TextureManager.hpp"
#include "SoundManager.hpp"
#include "GameEvent.hpp"
#include "Window.hpp"
#include "GameRunner.hpp"
#include <random>
#include <variant>

struct Config;
class GameBase;
class Fred;

class FredApp
{
    struct StateSplashScreen {
        static constexpr int LOADING_FRAME_TICKS = 25;
        int seq = 0;
        int counter = 5000 / LOADING_FRAME_TICKS;
        Uint32 sound_timer = 0;
        StateSplashScreen() = default;
    };
    struct StateMenu
    {
        int counter = 0;
        StateMenu() = default;
    };
    struct StateTodaysGreatest {};
    struct StatePlay {
        Window play_window;
        GameRunner game;
        int counter = 0;
        StatePlay(Config const &cfg, DisplayConfig const &display_cfg,
                  std::minstd_rand &random_engine,
                  unsigned high_score)
            : play_window(cfg,
                          display_cfg.getGameWindowWidth(),
                          display_cfg.getGameWindowHeight())
            , game(cfg, random_engine, high_score) {}
    };
    struct StateGameOver
    {
        unsigned score;
        explicit StateGameOver(unsigned score): score(score) {}
    };
    struct StateEnterHighScore {
        unsigned score;
        std::string initials = "A";
        explicit StateEnterHighScore(unsigned score): score(score) {}
    };
    using State = std::variant<StateTodaysGreatest,
                               StateSplashScreen,
                               StateMenu,
                               StatePlay,
                               StateGameOver,
                               StateEnterHighScore>;

    sdl::App app;
    Config const &cfg;
    std::minstd_rand &random_engine;
    std::pair<sdl::WindowPtr, sdl::RendererPtr> w_and_r;
    DisplayConfig display_cfg;
    TextureManager tmgr;
    SoundManager smgr;
    std::vector<std::pair<unsigned, std::string>> high_scores;
    State state = StateSplashScreen();

    static std::pair<sdl::WindowPtr, sdl::RendererPtr> initDisplay(Config const &cfg);
    void splashScreen(StateSplashScreen const &state_data);
    void menu(StateMenu &state_data);
    void todaysGreatest();
    void renderGame(StatePlay const &state_data);
    void debugMode(StatePlay &state_data, EventMask event_mask);
    void updateGame(StatePlay &state_data,
                    EventManager &event_manager, EventMask event_mask);
    void transitionToNextLevel(StatePlay &state_data, EventManager &event_manager);
    void updateGameOverSequence(StatePlay &state_data, EventManager &event_manager);
    void renderHighScoreScreen(std::string const &initials);
    void updateHighScore(std::string &initials, unsigned score,
                         EventManager &event_manager, EventMask event_mask);

public:
    explicit FredApp(Config const &cfg, std::minstd_rand &random_engine);
    SDL_Window *getWindow() const { return w_and_r.first; }
    SDL_Renderer *getRenderer() const { return w_and_r.second; }
    TextureManager const &getTextureManager() const { return tmgr; }
    SoundManager &getSoundManager() { return smgr; }
    Config const &getConfig() const { return cfg; }
    DisplayConfig const &getDisplayConfig() const { return display_cfg; }
    void mainLoop();
};
