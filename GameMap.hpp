#pragma once

#include "Coord.hpp"
#include "Sprite.hpp"
#include <vector>
#include <random>

struct SDL_Rect;
struct SDL_Renderer;
struct SDL_Texture;
class Frame;

class GameMap
{
public:
    enum class Cell : std::uint8_t
    {
        EMPTY,
        STONE1,
        STONE2,
        STONE3,
        ROPE_START,
        ROPE_MAIN,
        ROPE_END,
        SKY,
        SAND,
        TRAPDOOR
    };

    GameMap(std::minstd_rand &random_engine, int width, int height);
    Cell getCell(CellPos const &pos) const;
    bool isStone(CellPos const &pos) const {
        auto c = getCell(pos);
        return static_cast<int>(c) >= static_cast<int>(Cell::STONE1) &&
               static_cast<int>(c) <= static_cast<int>(Cell::STONE3);
    }
    int getWidth() const { return width_minus_one + 1; }
    int getHeight() const { return height; }
    void render(int x, int y, TextureManager const &tmgr,
                SDL_Renderer *renderer, SDL_Rect const *dest);
    void initializeMapBlocks(TextureManager const &tmgr,
                             Frame const &frame, SpriteList &block_list) const;
    void updateMapBlocksLeft(TextureManager const &tmgr,
                             Frame const &frame, SpriteList &block_list) const;
    void updateMapBlocksRight(TextureManager const &tmgr,
                              Frame const &frame, SpriteList &block_list) const;
   void updateMapBlocksUp(TextureManager const &tmgr,
                          Frame const &frame, SpriteList &block_list) const;
   void updateMapBlocksDown(TextureManager const &tmgr,
                            Frame const &frame, SpriteList &block_list) const;

private:
    void initializeMap(std::minstd_rand &random_engine);
    void createMaze(std::minstd_rand &random_engine);
    void setUpTheRopes();
    void setHatchPosition(std::minstd_rand &random_engine);

    void setCell(CellPos const &pos, Cell c);
    TextureID getTextureIDOf(CellPos const &pos) const;

    bool addMapBlock(TextureManager const &tmgr,
                     Frame const &frame, SpriteList &block_list,
                     int offset_x, int offset_y) const;
    void removeNonVisibleBlocks(Frame const &frame, SpriteList &block_list) const;

    int width_minus_one, height;
    std::vector<Cell> cell_list;
};
