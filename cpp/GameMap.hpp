#pragma once

#include "Coord.hpp"
#include "Sprite.hpp"
#include <vector>
#include <random>

struct SDL_Rect;
struct SDL_Renderer;
struct SDL_Texture;
struct Config;
class GameWindow;

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

    GameMap(Config const& cfg, std::minstd_rand &random_engine);
    Cell getBlock(CellPos const &pos, int offset_x = 0, int offset_y = 0) const;
    bool isStone(CellPos const &pos, int offset_x = 0, int offset_y = 0) const
    {
        auto c = getBlock(pos, offset_x, offset_y);
        return static_cast<int>(c) >= static_cast<int>(Cell::STONE1) &&
               static_cast<int>(c) <= static_cast<int>(Cell::STONE3);
    }
    int getWidth() const { return width_minus_one + 1; }
    int getHeight() const { return height; }
    static void renderCell(SDL_Renderer *renderer, TextureManager const &tmgr,
                           int x, int y, Cell cell);
    void render(SDL_Renderer *renderer, TextureManager const &tmgr,
                GameWindow const &game_window) const;

    CellPos dbgGetHatchPos() const { return CellPos{hatch_x, 0}; }
    bool dbgMoveHatch(int deltax);

private:
    void initializeMap(std::minstd_rand &random_engine);
    void createMaze(std::minstd_rand &random_engine);
    void setUpTheRopes();
    void setHatchPosition(std::minstd_rand &random_engine);
    bool tryHatchPosition(int x);

    void setCell(CellPos const &pos, Cell c);

    int width_minus_one, height;
    std::vector<Cell> cell_list;
    int hatch_x; // for debugging
    static std::uint8_t debug_map[];
};
