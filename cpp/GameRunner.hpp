#pragma once

#include "Game.hpp"
#include "GameEvent.hpp"

class GameRunner: public GameBase
{
    void initializeFred(std::minstd_rand &random_engine);
    void updateSprites();
    void checkBulletCollisions();
    void checkCollisionsWithEnemies();
    void debugMode(EventMask event_mask);

public:
    using GameBase::GameBase;
    void initializeSprites(std::minstd_rand &random_engine);
    LevelStatus update(EventMask event_mask);
};
