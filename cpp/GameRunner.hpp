#pragma once

#include "fredcore/Game.hpp"
#include "fredcore/GameEvent.hpp"

class GameRunner: public GameBase
{
    void initializeFred(std::minstd_rand &random_engine);
    void updateSprites();
    void checkBulletCollisions();
    void checkCollisionsWithEnemies();
    void debugMode(EventMask event_mask);
    std::unique_ptr<Sprite> makeBullet(MapPos initial_position, int direction) override;


public:
    using GameBase::GameBase;
    void initializeSprites(std::minstd_rand &random_engine);
    LevelStatus update(EventMask event_mask);
};
