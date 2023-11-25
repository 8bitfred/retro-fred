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
        QUIT,
        SPLASH_SCREEN,
        MENU,
        HIGH_SCORES,
    };

    void playLevel(Game &game);
    void initializeSprites(Game &game);
    void initializeFred(Game &game);
    void updateSprites(Game &game);
    void checkCollisionsWithEnemies(Game &game);
    void checkBulletCollisions(Game &game);
    void debugMode(Game &game, EventMask event_mask);
    void gameOverSequence(Game &game);
    void transitionToNextLevel(Game &game);

    void playGame();
    void splashScreen(EventManager &event_manager);
    void menu(EventManager &event_manager, bool flash);
    void todaysGreatest(EventManager &event_manager);
    void enterHighScore(unsigned score);

    Config const &cfg;
    std::minstd_rand &random_engine;
    std::pair<sdl::WindowPtr, sdl::RendererPtr> w_and_r;
    TextureManager tmgr;
    SoundManager smgr;

    State state;
    std::vector<std::pair<unsigned, std::string>> high_scores;
};
