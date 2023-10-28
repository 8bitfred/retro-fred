#pragma once

#include "sdl.hpp"
#include "TextureManager.hpp"
#include "SoundManager.hpp"
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
    enum class LevelStatus
    {
        QUIT,
        NEXT_LEVEL,
        GAME_OVER,
    };

    LevelStatus playLevel(Game &game);
    void initializeSprites(Game &game);
    void initializeFred(Game &game);
    void updateSprites(Game &game);
    void checkCollisionsWithEnemies(Game &game);
    void checkBulletCollisions(Game &game);
    void debugMode(Game &game, unsigned events);
    void endOfLevelSequence(Game &game);
    void gameOverSequence(Game &game);
    void showLevelSummary(Game &game);

    void playGame();
    void splashScreen();
    void menu();
    void todaysGreatest();
    void enterHighScore(unsigned score);

    Config const &cfg;
    std::minstd_rand &random_engine;
    std::pair<sdl::WindowPtr, sdl::RendererPtr> w_and_r;
    TextureManager tmgr;
    SoundManager smgr;

    State state;
    int timer = 0;
    std::vector<std::pair<unsigned, std::string>> high_scores;
};
