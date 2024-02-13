#pragma once

#include "Window.hpp"
#include "GameMap.hpp"
#include "Sprite.hpp"
#include "SoundID.hpp"
#include "LabelTable.hpp"
#include <vector>
#include <algorithm>
#include <optional>

class TextureManager;
class SoundManager;

class Game
{
public:
    static constexpr unsigned MAX_POWER = 15;
    static constexpr unsigned MAX_BULLETS = 6;

    enum class LevelStatus
    {
        PLAY,
        NEXT_LEVEL,
        GAME_OVER,
    };

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
        int charge_power = 2;
        int charge_bullets = 6;
    };

    Game(Config const &cfg, int total_width, int total_height,
         std::minstd_rand &random_engine,
         unsigned high_score);
    void nextLevel(std::minstd_rand &random_engine);
    SpriteCount const &getSpriteCount() const { return sprite_count; }
    Window &getFrame() { return window; }
    GameMap const &getGameMap() const { return game_map; }
    GameMap &dbgModifyGameMap() { return game_map; }
    SpriteList &getSpriteList(SpriteClass sprite_class)
    {
        return sprite_lists[static_cast<int>(sprite_class)];
    }
    void renderGameWindow(TextureManager const &tmgr,
                          SDL_Renderer *renderer,
                          GameWindow const &game_window) const;
    void render(TextureManager const &tmgr,
                SDL_Window *sdl_window, SDL_Renderer *renderer) const;
    void setLabels(LabelTable &label_table, GameWindow const &game_window) const;
    void addSound(SoundID sound_id);
    void playPendingSounds(SoundManager &smgr);
    void setLevelStatus(LevelStatus new_level_status)
    {
        level_status = new_level_status;
    }
    LevelStatus getLevelStatus() const { return level_status; }
    // TODO: we should refactor this so that the Game object does not need to know about
    // the internals of the Fred class
    MapPos const &getFredPos() const { return fred_pos; }
    MapPos getFredCellPos() const;
    void updateFredPos(MapPos fred_pos, int vposition);
    Config const &getConfig() const { return cfg; }
    int getBulletCount() const { return bullet_count; }
    unsigned getScore() const { return score; }
    unsigned getHighScore() const { return high_score; }
    int getTreasureCount() const { return treasure_count; }
    int getLevel() const { return level; }
    int getPower() const { return power; }
    bool decPower();
    void incPower();
    void addTreasure(int points)
    {
        score += points;
        ++treasure_count;
    }
    void addScore(int points) { score += points; }
    void rechargeBullets() { bullet_count = sprite_count.charge_bullets; }
    std::optional<CellPos> getMinimapPos() const { return minimap_pos; }
    void setMinimapPos(CellPos const &pos) { minimap_pos = pos; }

    bool canShoot() const;
    void fireGun(MapPos initial_position, int direction);
    GameWindow const &getGameWindow() const { return window.getGameWindow(); }

private:
    static SpriteCount getSpriteCountOfLevel(Config const &cfg, int level);
    Config const &cfg;
    Window window;
    GameMap game_map;
    MapPos fred_pos;
    int fred_vposition = 0;
    std::vector<SpriteList> sprite_lists;
    std::uint32_t pending_sounds = 0;
    LevelStatus level_status = LevelStatus::PLAY;
    unsigned score = 0, high_score = 0;
    unsigned level = 1;
    SpriteCount sprite_count;
    unsigned treasure_count = 0;
    unsigned power = MAX_POWER;
    unsigned bullet_count = MAX_BULLETS;
    std::optional<CellPos> minimap_pos;
};
