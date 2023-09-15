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
    void playGame();

private:
    void initializeSprites(Game &game);
    void initializeFred(Game &game);
    void initializeAcidDrops(Game &game);
    void initializeRats(Game &game);
    void initializeGhosts(Game &game);
    void initializeChameleons(Game &game);
    void initializeMummies(Game &game);
    void initializeVampires(Game &game);
    void initializeSkeletons(Game &game);
    void initializeObjects(Game &game);
    void updateSprites(Game &game);
    void checkCollisionsWithEnemies(Game &game);
    void checkBulletCollisions(Game &game);
    void debugMode(Game &game, unsigned events);

    Config const &cfg;
    std::minstd_rand &random_engine;
    std::pair<sdl::WindowPtr, sdl::RendererPtr> w_and_r;
    TextureManager tmgr;
    SoundManager smgr;
};