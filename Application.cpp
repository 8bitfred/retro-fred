#include "Application.hpp"
#include "Config.hpp"
#include "Game.hpp"
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
#include <iostream>


class KeyState
{
    unsigned state = 0;
public:
    std::pair<bool, unsigned> updateKeyState()
    {
        SDL_Event event;
        unsigned key_presses = 0;
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
                return {true, 0};
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.repeat == 0) {
                    auto events_of_key = Game::getEventOfKey(event.key.keysym.sym);
                    state |= events_of_key;
                    key_presses |= events_of_key;
                }
            }
            else if (event.type == SDL_KEYUP) {
                auto events_of_key = Game::getEventOfKey(event.key.keysym.sym);
                state &= ~events_of_key;
            }
        }
        return {false, state | key_presses};
    }
};

FredApp::FredApp(Config const &cfg, std::minstd_rand &random_engine)
    : cfg(cfg)
    , random_engine(random_engine)
    , w_and_r(sdl::createWindowAndRenderer(static_cast<int>(cfg.scale_x * cfg.window_width),
                                           static_cast<int>(cfg.scale_y * cfg.window_height))),
      tmgr(getRenderer())
{
    SDL_RenderSetScale(getRenderer(), cfg.scale_x, cfg.scale_y);
}

void FredApp::playGame()
{
    static constexpr std::uint32_t FRAMES_PER_SECOND = 6;
    static constexpr std::uint32_t TICKS_PER_FRAME = 1000 / FRAMES_PER_SECOND;
    Game game(cfg, random_engine, tmgr, smgr);
    initializeSprites(game);
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());
    game.getGameMap().initializeMapBlocks(game.getFrame(),
                                          game.getSpriteList(SpriteClass::BLOCK));

    std::uint32_t frame_count = 0;
    KeyState key_state;
    while (true)
    {
        Uint32 const start_ticks = SDL_GetTicks();
        auto [quit, events] = key_state.updateKeyState();
        if (quit)
            break;
        updateSprites(game);

        if ((events & Game::EVENT_SHIFT) != 0)
            debugMode(game, events);
        else
            fred->updateFred(game, events);

        checkBulletCollisions(game);
        checkCollisionsWithEnemies(game);
        fred->checkCollisionWithObject(game);

        SDL_RenderClear(getRenderer());
        game.renderSprites(getRenderer());
        game.getFrame().renderFrame(cfg, game, getRenderer(), tmgr);
        SDL_RenderPresent(getRenderer());
        game.playPendingSounds();

        ++frame_count;
        Uint32 const ticks = SDL_GetTicks() - start_ticks;
        if (ticks < TICKS_PER_FRAME)
            SDL_Delay(TICKS_PER_FRAME - ticks);
    }
}

void FredApp::initializeSprites(Game &game)
{
    initializeFred(game);
    initializeAcidDrops(game);
    initializeRats(game);
    initializeGhosts(game);
    initializeChameleons(game);
    initializeMummies(game);
    initializeVampires(game);
    initializeSkeletons(game);
    initializeObjects(game);
}

void FredApp::initializeFred(Game &game)
{
    MapPos fred_initial_position;
    if (cfg.debug_map)
        fred_initial_position = {23, 31, 0, 1};
    else
    {
        CellPos fred_cell_position = {0, game.getGameMap().getHeight() - 2};
        std::uniform_int_distribution<> distrib(1, game.getGameMap().getWidth() - 2);
        while (true)
        {
            fred_cell_position.x = distrib(random_engine);
            if (game.getGameMap().getBlock(fred_cell_position) == GameMap::Cell::EMPTY)
                break;
        }
        fred_initial_position = {fred_cell_position.x, fred_cell_position.y, 0, 1};
    }
    auto fred_unique_ptr = std::make_unique<Fred>(fred_initial_position);
    game.getFrame().adjustFramePos(fred_initial_position);
    game.getSpriteList(SpriteClass::FRED).emplace_back(std::move(fred_unique_ptr));
}


void FredApp::initializeAcidDrops(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::ACID_DROP);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    std::uniform_int_distribution<> distrib_frame(0, 3);
    for (int i = 0; i < 20; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 1, 0};
            if (!game.getGameMap().isStone(pos.cellPos(), 0, -1) &&
                game.getGameMap().getBlock(pos.cellPos(), 0, -1) != GameMap::Cell::TRAPDOOR)
                continue;
            if (game.getGameMap().isStone(pos.cellPos()))
                continue;
            if (!game.getGameMap().isStone(pos.cellPos(), 0, 1))
                continue;
            if (auto cell = game.getGameMap().getBlock(pos.cellPos(), -1);
                cell == GameMap::Cell::ROPE_END ||
                cell == GameMap::Cell::ROPE_MAIN ||
                cell == GameMap::Cell::ROPE_START)
                continue;
            if (auto cell = game.getGameMap().getBlock(pos.cellPos(), 1);
                cell == GameMap::Cell::ROPE_END ||
                cell == GameMap::Cell::ROPE_MAIN ||
                cell == GameMap::Cell::ROPE_START)
                continue;
            auto initial_state = distrib_frame(random_engine);
            sprite_list.emplace_back(std::make_unique<AcidDrop>(pos, initial_state));
            break;
        }
    }
}

void FredApp::initializeRats(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::RAT);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < 40; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 4};
            if (game.getGameMap().isStone(pos.cellPos()))
                continue;
            if (game.getGameMap().isStone(pos.cellPos(), -1))
                continue;
            if (game.getGameMap().isStone(pos.cellPos(), 1))
                continue;
            if (!game.getGameMap().isStone(pos.cellPos(), 0, 1))
                continue;
            if (!game.getGameMap().isStone(pos.cellPos(), -1, 1))
                continue;
            if (!game.getGameMap().isStone(pos.cellPos(), 1, 1))
                continue;
            sprite_list.emplace_back(std::make_unique<Rat>(pos));
            break;
        }
    }
}

void FredApp::initializeGhosts(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::GHOST);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < 10; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 1};
            if (game.getGameMap().isStone(pos.cellPos()))
                continue;
            sprite_list.emplace_back(std::make_unique<Ghost>(pos, random_engine));
            break;
        }
    }
}

void FredApp::initializeChameleons(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::CHAMELEON);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < 10; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 0};
            if (!Chameleon::isValidCell(game.getGameMap(), pos.cellPos()))
                continue;
            sprite_list.emplace_back(std::make_unique<Chameleon>(pos, random_engine));
            break;
        }
    }
}

void FredApp::initializeMummies(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::MUMMY);
    for (int i = 0; i < 5; ++i) {
        sprite_list.emplace_back(std::make_unique<Mummy>(game, random_engine));
    }
}

void FredApp::initializeVampires(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::VAMPIRE);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < 10; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 0};
            if (game.getGameMap().getBlock(pos.cellPos()) != GameMap::Cell::EMPTY)
                continue;
            sprite_list.emplace_back(std::make_unique<Vampire>(pos, random_engine));
            break;
        }
    }
}

void FredApp::initializeSkeletons(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::SKELETON);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < 10; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 1};
            if (game.getGameMap().getBlock(pos.cellPos()) != GameMap::Cell::EMPTY)
                continue;
            sprite_list.emplace_back(std::make_unique<Skeleton>(pos, random_engine));
            break;
        }
    }
}

void FredApp::initializeObjects(Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::OBJECT);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    std::uniform_int_distribution<> distrib_t(0, static_cast<int>(Object::Type::COUNT));
    // TODO: the number of objects depends on the game level. Also not all objects are
    // allowed in all levels.
    for (int counter = 0; counter < 25;)
    {
        MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 1, 3};
        if (game.getGameMap().getBlock(pos.cellPos()) != GameMap::Cell::EMPTY)
            continue;
        Object::Type object_type = {};
        if (counter >= 16)
            object_type = Object::Type::BULLETS;
        else if (counter < static_cast<int>(Object::Type::COUNT))
            object_type = static_cast<Object::Type>(counter);
        else
            object_type = static_cast<Object::Type>(distrib_t(random_engine));
        sprite_list.emplace_back(std::make_unique<Object>(pos, object_type));
        ++counter;
    }
}


void FredApp::updateSprites(Game &game)
{
    for (auto const &sprite : game.getSpriteList(SpriteClass::BULLET))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::ACID_DROP))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::RAT))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::GHOST))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::CHAMELEON))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::MUMMY))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::VAMPIRE))
        sprite->update(game, 0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::SKELETON))
        sprite->update(game, 0);
    auto &smoke_list = game.getSpriteList(SpriteClass::SMOKE);
    for (size_t i = 0; i < smoke_list.size();)
    {
        auto &smoke = dynamic_cast<Smoke &>(*game.getSpriteList(SpriteClass::SMOKE)[i]);
        smoke.update(game, 0);
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

void FredApp::checkCollisionsWithEnemies(Game &game)
{
    static SpriteClass enemies[] = { SpriteClass::ACID_DROP,
                                     SpriteClass::RAT,
                                     SpriteClass::GHOST,
                                     SpriteClass::CHAMELEON,
                                     SpriteClass::MUMMY,
                                     SpriteClass::VAMPIRE,
                                     SpriteClass::SKELETON };
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());
    for (auto enemy_class : enemies)
    {
        for (auto const &sprite : game.getSpriteList(enemy_class))
        {
            if (fred->collisionInProgress())
                return;
            fred->checkCollisionWithEnemy(game, *sprite);
        }
    }
}

void FredApp::checkBulletCollisions(Game &game)
{
    static SpriteClass bullet_enemies[] = {
        SpriteClass::GHOST,
        SpriteClass::MUMMY,
        SpriteClass::VAMPIRE,
        SpriteClass::SKELETON,
    };

    auto &bullet_list = game.getSpriteList(SpriteClass::BULLET);
    if (bullet_list.empty())
        return;
    auto &bullet = dynamic_cast<Bullet &>(*bullet_list.back());
    if (!bullet.isAlive(game))
    {
        bullet_list.pop_back();
        return;
    }
    for (auto enemy_class : bullet_enemies)
    {
        auto &sprite_list = game.getSpriteList(enemy_class);
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
                auto &smoke_list = game.getSpriteList(SpriteClass::SMOKE);
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

void FredApp::debugMode(Game &game, unsigned events)
{
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());
    if ((events & Game::EVENT_LEFT) != 0)
        game.moveFrame(-1, 0);
    else if ((events & Game::EVENT_RIGHT) != 0)
        game.moveFrame(1, 0);
    else if ((events & Game::EVENT_UP) != 0)
        game.moveFrame(0, -1);
    else if ((events & Game::EVENT_DOWN) != 0)
        game.moveFrame(0, 1);
    else if ((events & Game::EVENT_RESET_FRED) != 0)
        fred->dbgResetPosition(game);
    else if ((events & Game::EVENT_HATCH_LEFT) != 0)
    {
        if (game.getGameMap().dbgMoveHatch(-1))
            game.dbgResetMapBlocks();
    }
    else if ((events & Game::EVENT_HATCH_RIGHT) != 0)
    {
        if (game.getGameMap().dbgMoveHatch(1))
            game.dbgResetMapBlocks();
    }
    else if ((events & Game::EVENT_MOVE_TO_HATCH) != 0)
        fred->dbgMoveToHatch(game);
}
