#include "GameRunner.hpp"
#include "fredcore/Config.hpp"
#include "Fred.hpp"
#include "AcidDrop.hpp"
#include "Rat.hpp"
#include "Ghost.hpp"
#include "Chameleon.hpp"
#include "Mummy.hpp"
#include "Vampire.hpp"
#include "Skeleton.hpp"
#include "Bullet.hpp"
#include "Smoke.hpp"
#include "Object.hpp"
#include "Tomb.hpp"

void GameRunner::initializeFred(std::minstd_rand &random_engine)
{
    MapPos fred_initial_position;
    if (getConfig().debug_map)
        fred_initial_position = {23, 31, 0, 1};
    else
    {
        CellPos fred_cell_position = {0, getGameMap().getHeight() - 2};
        std::uniform_int_distribution<> distrib(1, getGameMap().getWidth() - 2);
        while (true)
        {
            fred_cell_position.x = distrib(random_engine);
            if (getGameMap().getBlock(fred_cell_position) == GameMap::Cell::EMPTY)
                break;
        }
        fred_initial_position = {fred_cell_position.x, fred_cell_position.y, 0, 1};
    }
    auto fred_unique_ptr = std::make_unique<Fred>(*this, fred_initial_position);
    updateFredPos(fred_initial_position, 1);
    getSpriteList(SpriteClass::FRED).emplace_back(std::move(fred_unique_ptr));
}

void GameRunner::updateSprites()
{
    for (auto const &sprite : getSpriteList(SpriteClass::BULLET))
        sprite->update(0);
    for (auto const &sprite : getSpriteList(SpriteClass::ACID_DROP))
        sprite->update(0);
    for (auto const &sprite : getSpriteList(SpriteClass::RAT))
        sprite->update(0);
    for (auto const &sprite : getSpriteList(SpriteClass::GHOST))
        sprite->update(0);
    for (auto const &sprite : getSpriteList(SpriteClass::CHAMELEON))
        sprite->update(0);
    for (auto const &sprite : getSpriteList(SpriteClass::MUMMY))
        sprite->update(0);
    for (auto const &sprite : getSpriteList(SpriteClass::VAMPIRE))
        sprite->update(0);
    for (auto const &sprite : getSpriteList(SpriteClass::SKELETON))
        sprite->update(0);
    auto &smoke_list = getSpriteList(SpriteClass::SMOKE);
    for (size_t i = 0; i < smoke_list.size();)
    {
        auto &smoke = dynamic_cast<Smoke &>(*getSpriteList(SpriteClass::SMOKE)[i]);
        smoke.update(0);
        if (!smoke.isAlive())
            smoke_list.erase(smoke_list.begin() + i);
        else
            ++i;
    }
    // TODO: I would like to avoid exposing the toggleClimbingFrame() API by using
    // some signal or callback
    Skeleton::toggleClimbingFrame();
    Mummy::toggleMummyTimer();
}

void GameRunner::checkBulletCollisions()
{
    static SpriteClass bullet_enemies[] = {
        SpriteClass::GHOST,
        SpriteClass::MUMMY,
        SpriteClass::VAMPIRE,
        SpriteClass::SKELETON,
    };

    auto &bullet_list = getSpriteList(SpriteClass::BULLET);
    if (bullet_list.empty())
        return;
    auto &bullet = dynamic_cast<Bullet &>(*bullet_list.back());
    if (!bullet.isAlive())
    {
        bullet_list.pop_back();
        return;
    }
    for (auto enemy_class : bullet_enemies)
    {
        auto &sprite_list = getSpriteList(enemy_class);
        for (size_t i = 0; i < sprite_list.size(); ++i)
        {
            auto &sprite = *sprite_list[i];
            if (!sprite.checkCollision(bullet))
                continue;
            auto effect = sprite.bulletHit();
            if (effect == Sprite::BulletEffect::HIT)
            {
                bullet_list.pop_back();
                return;
            }
            else if (effect == Sprite::BulletEffect::DIE)
            {
                auto &smoke_list = getSpriteList(SpriteClass::SMOKE);
                auto smoke_pos = sprite.getPos();
                smoke_pos.yadd(1);
                smoke_list.emplace_back(std::make_unique<Smoke>(smoke_pos));
                sprite_list.erase(sprite_list.begin() + i);
                bullet_list.pop_back();
                return;
            }
        }
    }
}

void GameRunner::checkCollisionsWithEnemies()
{
    static SpriteClass enemies[] = { SpriteClass::ACID_DROP,
                                     SpriteClass::RAT,
                                     SpriteClass::GHOST,
                                     SpriteClass::CHAMELEON,
                                     SpriteClass::MUMMY,
                                     SpriteClass::VAMPIRE,
                                     SpriteClass::SKELETON };
    auto fred = dynamic_cast<Fred *>(getSpriteList(SpriteClass::FRED).front().get());
    for (auto enemy_class : enemies)
    {
        for (auto const &sprite : getSpriteList(enemy_class))
        {
            if (fred->collisionInProgress())
                return;
            fred->checkCollisionWithEnemy(*sprite);
        }
    }
}

void GameRunner::debugMode(EventMask event_mask)
{
    if (event_mask.check(GameEvent::DBG_EXIT_LEFT))
        dbgModifyGameMap().dbgMoveHatch(-1);
    else if (event_mask.check(GameEvent::DBG_EXIT_RIGHT))
        dbgModifyGameMap().dbgMoveHatch(1);
    else if (event_mask.check(GameEvent::DBG_MAP))
        setMinimapPos(getFredPos().cellPos());
}


std::unique_ptr<Sprite> GameRunner::makeBullet(MapPos initial_position, int direction)
{
    return std::make_unique<Bullet>(*this, initial_position, direction);
}

void GameRunner::initializeSprites(std::minstd_rand &random_engine)
{
    Rat::initialize(random_engine, *this);
    AcidDrop::initialize(random_engine, *this);
    Ghost::initialize(random_engine, *this);
    Mummy::initialize(random_engine, *this);
    Vampire::initialize(random_engine, *this);
    Chameleon::initialize(random_engine, *this);
    Skeleton::initialize(random_engine, *this);
    Object::initialize(random_engine, *this);
    initializeFred(random_engine);
}

GameBase::LevelStatus GameRunner::update(EventMask event_mask)
{
    auto fred = dynamic_cast<Fred *>(getSpriteList(SpriteClass::FRED).front().get());

    updateSprites();

    if (getConfig().debug_keys)
        debugMode(event_mask);
    fred->updateFred(event_mask);

    checkBulletCollisions();
    checkCollisionsWithEnemies();
    if (getLevelStatus() == GameBase::LevelStatus::PLAY)
        fred->checkCollisionWithObject();
    return getLevelStatus();
}
