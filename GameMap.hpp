#pragma once

#include <stdexcept>
#include <cstdint>
#include <vector>
#include <random>

struct SDL_Rect;
struct SDL_Renderer;
struct SDL_Texture;
class TextureManager;

class GameMap
{
public:
    static constexpr int cell_width = 32;
    static constexpr int cell_height = 40;
    enum class Cell : std::uint8_t
    {
        empty,
        stone1,
        stone2,
        stone3,
        rope_start,
        rope_middle,
        rope_end,
        sky,
        sand,
        trapdoor
    };

    struct CellPos
    {
        int x = 0, y = 0;
        CellPos hmove(int delta) const { return {x + delta, y}; }
        CellPos vmove(int delta) const { return {x, y + delta}; }
    };
    GameMap(std::minstd_rand &random_engine, int size_x, int size_y);
    Cell getCell(CellPos const &pos) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    void render(int x, int y, TextureManager const &tmgr,
                SDL_Renderer *renderer, SDL_Rect const *dest);

private:
    int width, height;
    std::vector<Cell> contents;
    void setCell(CellPos const &pos, Cell c);
    SDL_Texture *getTextureOf(TextureManager const& tmgr, CellPos const &pos) const;
};
