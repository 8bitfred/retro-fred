#pragma once

#include "sdl.hpp"
#include "TextureManager.hpp"
#include "SoundManager.hpp"
#include "GameEvent.hpp"
#include "Sprite.hpp"  // TODO: this is only necessary for checkBullet()
#include <random>

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
    enum class State
    {
        SPLASH_SCREEN,
        MENU,
        HIGH_SCORES,
        PLAY_GAME,
        GAME_OVER,
        ENTER_HIGH_SCORE,
    };

    void splashScreen();
    void menu();
    void todaysGreatest();
    void initializeFred(Game &game);
    void initializeSprites(Game &game);
    void updateSprites(Game &game);
    void checkBulletCollisions(Game &game);
    void checkCollisionsWithEnemies(Game &game);
    void transitionToNextLevel(Game &game, EventManager &event_manager);
    void debugMode(Game &game, EventMask event_mask);
    void updateGame(Game &game, EventManager &event_manager, EventMask event_mask);
    void updateGameOverSequence(Game &game, EventManager &event_manager);
    void renderHighScoreScreen(std::string const &initials);
    void updateHighScore(std::string &initials, unsigned score,
                         EventManager &event_manager, EventMask event_mask);

    Config const &cfg;
    std::minstd_rand &random_engine;
    std::pair<sdl::WindowPtr, sdl::RendererPtr> w_and_r;
    TextureManager tmgr;
    SoundManager smgr;

    std::vector<std::pair<unsigned, std::string>> high_scores;
    State state = State::SPLASH_SCREEN;
    int state_timer = 0;
};
