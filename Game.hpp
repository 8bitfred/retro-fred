#pragma once

#include "Window.hpp"
#include "GameMap.hpp"
#include "Sprite.hpp"
#include "SoundID.hpp"
#include <vector>
#include <algorithm>
#include <optional>

class TextureManager;
class SoundManager;
class Fred;

class Game
{
public:
    // TODO: processing of keys and events is very messy: this does not belong in the Game
    // class. Also EVENT_HATCH_LEFT and EVENT_HATCH_RIGHT should only be on if the SHIFT
    // key is pressed.
    static constexpr unsigned EVENT_LEFT = 0x01;
    static constexpr unsigned EVENT_RIGHT = 0x02;
    static constexpr unsigned EVENT_UP = 0x04;
    static constexpr unsigned EVENT_DOWN = 0x08;
    static constexpr unsigned EVENT_FIRE = 0x10;
    static constexpr unsigned EVENT_SHIFT = 0x20;
    static constexpr unsigned EVENT_RESET_FRED = 0x40;
    static constexpr unsigned EVENT_HATCH_LEFT = 0x80;
    static constexpr unsigned EVENT_HATCH_RIGHT = 0x100;
    static constexpr unsigned EVENT_MOVE_TO_HATCH = 0x200;
    static constexpr unsigned EVENT_LCTRL = 0x400;
    static constexpr unsigned EVENT_RCTRL = 0x800;
    static constexpr unsigned EVENT_DIE = 0x1000;
    static constexpr unsigned EVENT_MAP = 0x2000;

    static constexpr unsigned MAX_POWER = 15;
    static constexpr unsigned MAX_BULLETS = 6;

    struct SpriteCount
    {
        int acid_drops = 0;
        int rats = 0;
        int ghosts = 0;
        int chameleons = 0;
        int mummies = 0;
        int vampires = 0;
        int skeletons = 0;
        int objects = 0;
        bool has_busts = false;
        bool has_stones = false;
        bool has_masks = false;
    };

    static unsigned getEventOfKey(SDL_Keycode keycode);

    Game(Config const &cfg, std::minstd_rand &random_engine,
         TextureManager const &tmgr, SoundManager &smgr,
         unsigned high_score);
    void nextLevel(std::minstd_rand &random_engine);
    SpriteCount const &getSpriteCount() const { return sprite_count; }
    Window &getFrame() { return window; }
    GameMap &getGameMap() { return game_map; }
    SpriteList &getSpriteList(SpriteClass sprite_class)
    {
        return sprite_lists[static_cast<int>(sprite_class)];
    }
    // TODO: this function should be const
    void render(SDL_Renderer *renderer);
    void moveFrame(int deltax, int deltay);
    void playSound(SoundID sound_id);
    void addSound(SoundID sound_id);
    void playPendingSounds();
    // TODO: we should refactor this so that the Game object does not need to know about
    // the internals of the Fred class
    MapPos const &getFredPos() const;
    int getBulletCount() const { return bullet_count; }
    unsigned getScore() const { return score; }
    unsigned getHighScore() const { return high_score; }
    int getTreasureCount() const { return treasure_count; }
    int getLevel() const { return level; }
    int getPower() const { return power; }
    bool decPower();
    void incPower()
    {
        power += 2;
        power = std::min(power, MAX_POWER);
    }
    void addTreasure(int points)
    {
        score += points;
        ++treasure_count;
    }
    void addScore(int points) { score += points; }
    void rechargeBullets() { bullet_count = MAX_BULLETS; }
    std::optional<CellPos> getMinimapPos() const { return minimap_pos; }
    void setMinimapPos(CellPos const &pos) { minimap_pos = pos; }
    void dbgResetMapBlocks();

    bool canShoot() const;
    void fireGun(MapPos initial_position, int direction);

private:
    static SpriteCount getSpriteCountOfLevel(Config const &cfg, int level);
    Config const &cfg;
    TextureManager const &tmgr;
    SoundManager &smgr;    
    Window window;
    GameMap game_map;
    std::vector<SpriteList> sprite_lists;
    std::uint32_t pending_sounds = 0;
    unsigned bullet_count = MAX_BULLETS;
    unsigned level = 1;
    SpriteCount sprite_count;
    unsigned score = 0, high_score = 0;
    unsigned treasure_count = 0;
    unsigned power = MAX_POWER;
    std::optional<CellPos> minimap_pos;
};
