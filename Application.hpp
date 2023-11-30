#pragma once

#include "sdl.hpp"
#include "TextureManager.hpp"
#include "SoundManager.hpp"
#include "GameEvent.hpp"
#include "Game.hpp"
#include <random>
#include <variant>

struct Config;
class Game;
class Fred;
class Bullet;

class FredApp : sdl::App
{
public:
    explicit FredApp(Config const &cfg, std::minstd_rand &random_engine);
    SDL_Window *getWindow() const { return w_and_r.first; }
    SDL_Renderer *getRenderer() const { return w_and_r.second; }
    TextureManager const &getTextureManager() const { return tmgr; }
    void mainLoop();

private:
    struct StateSplashScreen {};
    struct StateMenu { int counter = 0; };
    struct StateTodaysGreatest {};
    struct StatePlay {
        Game game;
        int counter = 0;
        StatePlay(Config const &cfg, std::minstd_rand &random_engine,
                  TextureManager const &tmgr, SoundManager &smgr,
                  unsigned high_score)
            : game(cfg, random_engine, tmgr, smgr, high_score) {}
    };
    struct StateGameOver {
        unsigned score;
        explicit StateGameOver(unsigned score): score(score) {}
    };
    struct StateEnterHighScore {
        unsigned score;
        std::string initials = "A";
        explicit StateEnterHighScore(unsigned score): score(score) {}
    };
    using State = std::variant<StateSplashScreen,
                               StateMenu,
                               StateTodaysGreatest,
                               StatePlay,
                               StateGameOver,
                               StateEnterHighScore>;

    void splashScreen();
    void menu(StateMenu &state_data);
    void todaysGreatest();
    void initializeFred(Game &game);
    void initializeSprites(Game &game);
    void updateSprites(Game &game);
    void checkBulletCollisions(Game &game);
    void checkCollisionsWithEnemies(Game &game);
    void transitionToNextLevel(Game &game, EventManager &event_manager);
    void debugMode(Game &game, EventMask event_mask);
    void updateGame(Game &game, EventManager &event_manager, EventMask event_mask);
    void updateGameOverSequence(StatePlay &state_data, EventManager &event_manager);
    void renderHighScoreScreen(std::string const &initials);
    void updateHighScore(std::string &initials, unsigned score,
                         EventManager &event_manager, EventMask event_mask);

    Config const &cfg;
    std::minstd_rand &random_engine;
    std::pair<sdl::WindowPtr, sdl::RendererPtr> w_and_r;
    TextureManager tmgr;
    SoundManager smgr;

    std::vector<std::pair<unsigned, std::string>> high_scores;
    State state = StateSplashScreen();
};
