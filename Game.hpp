#pragma once

#include "Frame.hpp"
#include "GameMap.hpp"
#include "Sprite.hpp"
#include <vector>

class TextureManager;
class Fred;
class Game
{
public:
    static constexpr unsigned EVENT_LEFT = 0x01;
    static constexpr unsigned EVENT_RIGHT = 0x02;
    static constexpr unsigned EVENT_UP = 0x04;
    static constexpr unsigned EVENT_DOWN = 0x08;
    static constexpr unsigned EVENT_FIRE = 0x10;

    static unsigned getEventOfKey(SDL_Keycode keycode);

    Game(Config const &cfg, std::minstd_rand &random_engine, TextureManager const &tmgr);
    Frame &getFrame() { return frame; }
    GameMap &getGameMap() { return game_map; }
    SpriteList &getSpriteList(SpriteClass sprite_class)
    {
        return sprite_lists[static_cast<int>(sprite_class)];
    }
    void renderSprites(SDL_Renderer *renderer) const;
    void moveFrame(int deltax, int deltay);

private:
    TextureManager const &tmgr;
    Frame frame;
    GameMap game_map;
    std::vector<SpriteList> sprite_lists;
};
