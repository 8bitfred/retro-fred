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
    static constexpr std::uint32_t FRAMES_PER_SECOND = 5;

    Game(Config const &cfg, std::minstd_rand &random_engine, TextureManager const &tmgr);
    Frame &getFrame() { return frame; }
    GameMap &getGameMap() { return game_map; }
    SpriteList &getSpriteList(SpriteClass sprite_class)
    {
        return sprite_lists[static_cast<int>(sprite_class)];
    }
    void renderSprites(SDL_Renderer *renderer) const;

private:
    TextureManager const &tmgr;
    Frame frame;
    GameMap game_map;
    std::vector<SpriteList> sprite_lists;
};
