#include "sdl.hpp"
#include "Config.hpp"
#include "GameMap.hpp"
#include "Game.hpp"
#include "DisplayConfig.hpp"
#include "TextureManager.hpp"
#include "SoundManager.hpp"
#include "GameEvent.hpp"
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
#include "Tomb.hpp"
#include "Controller.hpp"
#include <random>
#include <iostream>
#include <algorithm>


class AutoFred
{
    sdl::App app;
    Config const &cfg;
    std::minstd_rand &random_engine;
    std::pair<sdl::WindowPtr, sdl::RendererPtr> w_and_r;
    DisplayConfig display_cfg;
    TextureManager tmgr;
    SoundManager smgr;

    void updateGame(Game &game, EventManager &event_manager, EventMask event_mask);
    static std::pair<sdl::WindowPtr, sdl::RendererPtr> initDisplay(Config const &cfg);

public:
    explicit AutoFred(Config const &cfg, std::minstd_rand &random_engine);
    SDL_Window *getWindow() const { return w_and_r.first; }
    SDL_Renderer *getRenderer() const { return w_and_r.second; }
    TextureManager const &getTextureManager() const { return tmgr; }
    void initializeFred(Game &game);
    void initializeSprites(Game &game);
    void updateSprites(Game &game);
    void checkBulletCollisions(Game &game);
    void checkCollisionsWithEnemies(Game &game);
    void mainLoop();
};


class Player
{
    Game const &game;
    std::vector<CellPos> stack;
    std::vector<bool> visited;
    MapPos next_pos;
    bool visit(CellPos const &pos);
    CellPos getNextCell();

public:
    explicit Player(Game const &game);
    GameEvent getMove();
};

bool Player::visit(CellPos const &pos)
{
    auto index = pos.y * game.getGameMap().getWidth() + pos.x;
    bool result = visited[index];
    visited[index] = true;
    return result;
}

CellPos Player::getNextCell()
{
    std::pair<int, int> moves[] = {
        {  0, -1 },
        { -1,  0 },
        {  1,  0 },
        {  0,  1 },
    };
    auto pos = stack.back();
    for (auto [delta_x, delta_y] : moves)
    {
        CellPos next_pos{pos.x + delta_x, pos.y + delta_y};
        if (game.getGameMap().isStone(next_pos))
            continue;
        if (!visit(next_pos))
        {
            stack.emplace_back(next_pos);
            return next_pos;
        }
    }
    if (stack.size() == 1)
    {
        std::cerr << "Back to starting point\n";
        std::fill(visited.begin(), visited.end(), false);
        visit(stack.back());
    }
    else
        stack.pop_back();
    return stack.back();
}

Player::Player(Game const &game)
    : game(game)
    , visited(game.getGameMap().getWidth() * game.getGameMap().getHeight(), false)
    , next_pos(game.getFredPos())
{
    stack.emplace_back(next_pos.cellPos());
    visit(stack.back());
}

GameEvent Player::getMove()
{
    auto const &fred_pos = game.getFredPos();
    if (fred_pos == next_pos)
    {
        auto next_cell = getNextCell();
        if (next_cell.y < fred_pos.y())
            next_pos = MapPos(next_cell.x, next_cell.y, 0, 0);
        else if (next_cell.y > fred_pos.y())
            next_pos = MapPos(next_cell.x, next_cell.y, 0, 0);
        else if (game.getGameMap().isStone(next_cell, 0, 1))
            next_pos = MapPos(next_cell.x, next_cell.y, 0, 1);
        else
            next_pos = MapPos(next_cell.x, next_cell.y, 0, 0);
    }
    if (next_pos.getCharX() < fred_pos.getCharX())
        return GameEvent::LEFT;
    else if (next_pos.getCharX() > fred_pos.getCharX())
        return GameEvent::RIGHT;
    else if (next_pos.getCharY() < fred_pos.getCharY())
        return GameEvent::UP;
    else
        return GameEvent::DOWN;
}

std::pair<sdl::WindowPtr, sdl::RendererPtr>
AutoFred::initDisplay(Config const &cfg)
{
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    Uint32 window_flags = 0;
    int width = display_mode.w, height = display_mode.h;
    if (cfg.full_screen)
        window_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    else if (cfg.max_resolution)
    {
        auto max_w = (cfg.map_width * MapPos::CELL_WIDTH + 20) * MapPos::PIXELS_PER_CHAR;
        auto max_h = (cfg.map_height * MapPos::CELL_HEIGHT + 15) * MapPos::PIXELS_PER_CHAR;
        width = std::min(max_w, display_mode.w * 19 / 20);
        height = std::min(max_h, display_mode.h * 19 / 20);
        window_flags = SDL_WINDOW_RESIZABLE;
    }
    else
    {
        auto scale_w = static_cast<int>(static_cast<double>(display_mode.w * .8 / cfg.logical_width));
        auto scale_h = static_cast<int>(static_cast<double>(display_mode.h * .8 / cfg.logical_height));
        auto scale = std::max(std::min(scale_w, scale_h), 1);
        width = cfg.logical_width * scale;
        height = cfg.logical_height * scale;
        window_flags = SDL_WINDOW_RESIZABLE;
    }
    return sdl::createWindowAndRenderer(width, height, window_flags);
}


void AutoFred::updateGame(Game &game, EventManager &event_manager, EventMask event_mask)
{
    auto fred = dynamic_cast<Fred *>(game.getSpriteList(SpriteClass::FRED).front().get());

    updateSprites(game);

    fred->updateFred(event_mask);

    checkBulletCollisions(game);
    checkCollisionsWithEnemies(game);
    if (game.getLevelStatus() == Game::LevelStatus::GAME_OVER)
    {
        game.playSound(SoundID::GAME_OVER);
        event_manager.setTimer(500);
        game.render(getWindow(), getRenderer());
        return;
    }
    else if (game.getLevelStatus() == Game::LevelStatus::NEXT_LEVEL)
        return;
    fred->checkCollisionWithObject();
    game.render(getWindow(), getRenderer());
    //game.playPendingSounds();
}


AutoFred::AutoFred(Config const &cfg, std::minstd_rand &random_engine)
    : cfg(cfg)
    , random_engine(random_engine)
    , w_and_r(initDisplay(cfg))
    , display_cfg(cfg, w_and_r.first, w_and_r.second)
    , tmgr(cfg, getRenderer())
    , smgr(cfg)
{
    SDL_SetWindowTitle(getWindow(), "Fred2023");
    SDL_SetWindowIcon(getWindow(), tmgr.getFredIcon());
}


void AutoFred::initializeFred(Game &game)
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
    auto fred_unique_ptr = std::make_unique<Fred>(game, fred_initial_position);
    game.updateFredPos(fred_initial_position, 1);
    game.getSpriteList(SpriteClass::FRED).emplace_back(std::move(fred_unique_ptr));
}


void AutoFred::initializeSprites(Game &game)
{
    Rat::initialize(random_engine, game);
    AcidDrop::initialize(random_engine, game);
    Ghost::initialize(random_engine, game);
    Mummy::initialize(random_engine, game);
    Vampire::initialize(random_engine, game);
    Chameleon::initialize(random_engine, game);
    Skeleton::initialize(random_engine, game);
    Object::initialize(random_engine, game);
    initializeFred(game);
}


void AutoFred::updateSprites(Game &game)
{
    for (auto const &sprite : game.getSpriteList(SpriteClass::BULLET))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::ACID_DROP))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::RAT))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::GHOST))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::CHAMELEON))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::MUMMY))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::VAMPIRE))
        sprite->update(0);
    for (auto const &sprite : game.getSpriteList(SpriteClass::SKELETON))
        sprite->update(0);
    auto &smoke_list = game.getSpriteList(SpriteClass::SMOKE);
    for (size_t i = 0; i < smoke_list.size();)
    {
        auto &smoke = dynamic_cast<Smoke &>(*game.getSpriteList(SpriteClass::SMOKE)[i]);
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

void AutoFred::checkBulletCollisions(Game &game)
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
    if (!bullet.isAlive())
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


void AutoFred::checkCollisionsWithEnemies(Game &game)
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
            fred->checkCollisionWithEnemy(*sprite);
        }
    }
}


void AutoFred::mainLoop()
{
    EventManager event_manager(cfg.ticks_per_frame, cfg.virtual_controller);
    Game game(cfg, display_cfg, random_engine, tmgr, smgr, 0);
    initializeSprites(game);
    Player player(game);
    game.render(getWindow(), getRenderer());
    while (true)
    {
        EventMask event_mask;
        event_mask = event_manager.collectEvents(getWindow());
        if (event_mask.check(GameEvent::QUIT))
            break;
        else if (event_mask.check(GameEvent::BACK))
            break;
        event_mask = EventMask();
        event_mask.set(player.getMove());
        updateGame(game, event_manager, event_mask);
        if (game.getLevelStatus() == Game::LevelStatus::NEXT_LEVEL)
            break;
        else if (game.getLevelStatus() == Game::LevelStatus::GAME_OVER)
            break;
    }
}


int main(int argc, char* argv[])
{
    const Config cfg(argc, argv);

    std::minstd_rand random_engine;
    if (cfg.debug_map)
        random_engine.seed(1);
    else
    {
        std::random_device random_dev;
        std::uniform_int_distribution<std::uint32_t> distrib;
        random_engine.seed(distrib(random_dev));
    }
    AutoFred fred_app(cfg, random_engine);
    fred_app.mainLoop();
    return 0;
}
