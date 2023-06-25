#pragma once

#include "Frame.hpp"
#include "GameMap.hpp"
#include "Sprite.hpp"
#include <vector>

class TextureManager;
class Game
{
public:
    static constexpr std::uint32_t FRAMES_PER_SECOND = 2;

    Game(Config const &cfg, std::minstd_rand &random_engine, TextureManager const &tmgr);
    SpriteList &getSpriteList(SpriteClass sprite_class)
    {
        return sprite_lists[static_cast<int>(sprite_class)];
    }
    void renderSprites(SDL_Renderer *renderer) const;
    void mainLoop(SDL_Renderer *renderer);

private:
    TextureManager const &tmgr;
    Frame frame;
    GameMap game_map;
    std::vector<SpriteList> sprite_lists;
};