#include "Game.hpp"
#include "Config.hpp"
#include "Fred.hpp"
#include "SoundManager.hpp"
#include "Bullet.hpp"
#include <algorithm>

unsigned Game::getEventOfKey(SDL_Keycode keycode)
{
    switch (keycode)
    {
    case SDLK_UP:
        return EVENT_UP;
    case SDLK_DOWN:
        return EVENT_DOWN;
    case SDLK_LEFT:
        return EVENT_LEFT;
    case SDLK_RIGHT:
        return EVENT_RIGHT;
    case SDLK_SPACE:
        return EVENT_FIRE;
    case SDLK_LSHIFT:
        return EVENT_SHIFT;
    case SDLK_f:
        return EVENT_MOVE_FRED;
    case SDLK_r:
        return EVENT_RESET_USER_OFFSET;
    case SDLK_o:
        return EVENT_HATCH_LEFT;
    case SDLK_p:
        return EVENT_HATCH_RIGHT;
    case SDLK_h:
        return EVENT_MOVE_TO_HATCH;
    case SDLK_LCTRL:
        return EVENT_LCTRL;
    case SDLK_RCTRL:
        return EVENT_RCTRL;
    case SDLK_k:
        return EVENT_DIE;
    case SDLK_m:
        return EVENT_MAP;
    default:
        return 0;
    }
}

Game::Game(Config const &cfg, std::minstd_rand &random_engine,
    TextureManager const &tmgr, SoundManager &smgr, unsigned high_score)
    : cfg(cfg), tmgr(tmgr), smgr(smgr)
    , window(cfg), game_map(cfg, random_engine)
    , sprite_lists(static_cast<size_t>(SpriteClass::COUNT))
    , sprite_count(getSpriteCountOfLevel(cfg, level))
    , high_score(high_score)
{
}

void Game::nextLevel(std::minstd_rand &random_engine)
{
    ++level;
    game_map = GameMap(cfg, random_engine);
    for (auto &list : sprite_lists)
        list.clear();
    treasure_count = 0;
    sprite_count = getSpriteCountOfLevel(cfg, level);
    minimap_pos.reset();
}

void Game::render(SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer);
    game_map.render(renderer, tmgr,
                    window.getWindowPos().x, window.getWindowPos().y,
                    &window.rect());
    for (auto const &sprites : sprite_lists)
    {
        for (auto const &s: sprites)
            s->render(cfg, window, tmgr, renderer);
    }
    window.renderFrame(*this, renderer, tmgr);
    SDL_RenderPresent(renderer);
}

void Game::playSound(SoundID sound_id)
{
    smgr.play(sound_id);
}

void Game::addSound(SoundID sound_id)
{
    static_assert(static_cast<int>(SoundID::COUNT) < sizeof(pending_sounds) * 8);
    pending_sounds |= 1 << static_cast<int>(sound_id);
}

void Game::playPendingSounds()
{
    std::uint32_t mask = 1;
    for (int i = 0; i < static_cast<int>(SoundID::COUNT); ++i, mask <<= 1)
    {
        if ((pending_sounds & mask) != 0)
            playSound(static_cast<SoundID>(i));
    }
    pending_sounds = 0;
}

MapPos Game::getFredCellPos() const
{
    auto pos = fred_pos;
    pos.yadd(-fred_vposition);
    return pos;
}

void Game::updateFredPos(MapPos new_fred_pos, int vposition)
{
    fred_pos = new_fred_pos;
    fred_vposition = vposition;
    auto ref_pos = fred_pos;
    ref_pos.yadd(-vposition);
    window.setWindowPos(ref_pos);
}

bool Game::canShoot() const
{
    return sprite_lists[static_cast<int>(SpriteClass::BULLET)].empty() && bullet_count > 0;
}

void Game::fireGun(MapPos initial_position, int direction)
{
    auto &sprite_list = getSpriteList(SpriteClass::BULLET);
    sprite_list.emplace_back(std::make_unique<Bullet>(*this, initial_position, direction));
    addSound(SoundID::FIRE);
    --bullet_count;
    if (bullet_count == 0 && cfg.infinite_ammo)
        bullet_count = MAX_BULLETS;
}

bool Game::decPower()
{
    --power;
    if (power > 0)
        return true;
    if (cfg.infinite_power)
    {
        power = MAX_POWER;
        return true;
    }
    return false;
}

Game::SpriteCount Game::getSpriteCountOfLevel(Config const &cfg, int level)
{
    static SpriteCount level_config[] = {
    //    A   R   G   C   M   V   S   O
        {20, 40, 10,  0,  0,  0,  0, 16, false, false, false},
        {25, 25, 10, 10,  5,  0,  0, 17, false, false, false},
        {30, 30, 10, 15, 10,  5,  0, 18,  true, false, false},
        {35, 35, 15, 20, 15, 15, 10, 21,  true,  true,  true},
        {30, 30, 15, 20, 15, 10,  5, 19,  true,  true,  true},
        { 1,  1,  1,  0,  0, 30, 20, 25,  true,  true,  true},
    };

    if (cfg.debug_map)
        return {15, 15, 7, 15, 7, 7, 7, 25, true, true, true};
    else
        return level_config[std::min(static_cast<size_t>(level - 1), std::size(level_config) - 1)];
}
