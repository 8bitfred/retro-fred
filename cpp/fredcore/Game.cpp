#include "Game.hpp"
#include "Config.hpp"
#include "SoundManager.hpp"
#include "DisplayConfig.hpp"
#include "Controller.hpp"
#include <algorithm>

GameBase::GameBase(Config const &cfg,
                   std::minstd_rand &random_engine,
                   unsigned high_score)
    : cfg(cfg)
    , game_map(cfg, random_engine)
    , sprite_lists(static_cast<size_t>(SpriteClass::COUNT))
    , high_score(high_score)
    , level(cfg.level)
    , sprite_count(getSpriteCountOfLevel(cfg, level))
    , bullet_count(sprite_count.charge_bullets)
{
}

void GameBase::nextLevel(std::minstd_rand &random_engine)
{
    ++level;
    level_status = LevelStatus::PLAY;
    game_map = GameMap(cfg, random_engine);
    for (auto &list : sprite_lists)
        list.clear();
    treasure_count = 0;
    sprite_count = getSpriteCountOfLevel(cfg, level);
    minimap_pos.reset();
    if (cfg.replenish_power)
        power = MAX_POWER;
    else
        power = std::min(power + 2, MAX_POWER);
    if (cfg.replenish_bullets)
        bullet_count = sprite_count.charge_bullets;
}

void GameBase::render(TextureManager const &tmgr,
                      SDL_Renderer *renderer,
                      GameWindow const &game_window) const
{
    game_map.render(renderer, tmgr, game_window);
    for (auto const &sprites : sprite_lists)
    {
        for (auto const &s: sprites)
            s->render(cfg, game_window, tmgr, renderer);
    }
}

void GameBase::setLabels(LabelTable &label_table, GameWindow const &game_window) const
{
    label_table.reset();
    game_map.setLabels(label_table, game_window);
    for (auto const &sprites : sprite_lists)
    {
        for (auto const &s: sprites)
            s->setLabel(game_window, label_table);
    }
}

void GameBase::addSound(SoundID sound_id)
{
    static_assert(static_cast<int>(SoundID::COUNT) < sizeof(pending_sounds) * 8);
    pending_sounds |= 1 << static_cast<int>(sound_id);
}

void GameBase::playPendingSounds(SoundManager &smgr)
{
    smgr.clearQueuedAudio();
    std::uint32_t mask = 1;
    for (int i = 0; i < static_cast<int>(SoundID::COUNT); ++i, mask <<= 1)
    {
        if ((pending_sounds & mask) != 0)
            smgr.play(static_cast<SoundID>(i));
    }
    pending_sounds = 0;
}

MapPos GameBase::getFredCellPos() const
{
    auto pos = fred_pos;
    pos.yadd(-fred_vposition);
    return pos;
}

void GameBase::updateFredPos(MapPos new_fred_pos, int vposition)
{
    fred_pos = new_fred_pos;
    fred_vposition = vposition;
}

bool GameBase::canShoot() const
{
    return sprite_lists[static_cast<int>(SpriteClass::BULLET)].empty() && bullet_count > 0;
}

void GameBase::fireGun(MapPos initial_position, int direction)
{
    auto &sprite_list = getSpriteList(SpriteClass::BULLET);
    sprite_list.emplace_back(makeBullet(initial_position, direction));
    addSound(SoundID::FIRE);
    --bullet_count;
    if (bullet_count == 0 && cfg.infinite_ammo)
        bullet_count = MAX_BULLETS;
}

bool GameBase::decPower()
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

void GameBase::incPower()
{
    power += sprite_count.charge_power;
    power = std::min(power, MAX_POWER);
}

GameBase::SpriteCount GameBase::getSpriteCountOfLevel(Config const &cfg, int level)
{
    static SpriteCount level_config[] = {
    //    A   R   G   C   M   V   S   O  busts stones  masks
        {20, 40, 10,  0,  0,  0,  0, 16, false, false, false, 2, 6},
        {25, 25, 10, 10,  5,  0,  0, 17, false, false, false, 2, 6},
        {30, 30, 10, 15, 10,  5,  0, 18,  true, false, false, 3, 9},
        {35, 35, 15, 20, 15, 15, 10, 21,  true,  true, false, 3, 10},
        {30, 30, 15, 20, 15, 10,  5, 19,  true,  true,  true, 4, 11},
        { 1,  1,  1,  0,  0, 30, 20, 25,  true,  true,  true, 4, 12},
    };

    if (cfg.debug_map)
        return {15, 15, 7, 15, 7, 7, 7, 25, true, true, true, 2, 6};
    else {
        auto lvl = std::min(static_cast<size_t>(level), std::size(level_config)) - 1;
        auto sprite_count = level_config[lvl];
        if (!cfg.set_power_with_level)
            sprite_count.charge_power = 2;
        if (!cfg.set_bullets_with_level)
            sprite_count.charge_bullets = 6;
        return sprite_count;
    }
}
