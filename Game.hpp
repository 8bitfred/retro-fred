#pragma once

#include "Frame.hpp"
#include "GameMap.hpp"
#include "Sprite.hpp"
#include "SoundID.hpp"
#include <vector>

class TextureManager;
class SoundManager;
class Fred;
class Game
{
public:
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

    static unsigned getEventOfKey(SDL_Keycode keycode);

    Game(Config const &cfg, std::minstd_rand &random_engine,
         TextureManager const &tmgr, SoundManager &smgr);
    Frame &getFrame() { return frame; }
    GameMap &getGameMap() { return game_map; }
    SpriteList &getSpriteList(SpriteClass sprite_class)
    {
        return sprite_lists[static_cast<int>(sprite_class)];
    }
    void renderSprites(SDL_Renderer *renderer) const;
    void moveFrame(int deltax, int deltay);
    void playSound(SoundID sound_id);
    void dbgResetMapBlocks();

private:
    TextureManager const &tmgr;
    SoundManager &smgr;
    Frame frame;
    GameMap game_map;
    std::vector<SpriteList> sprite_lists;
};
