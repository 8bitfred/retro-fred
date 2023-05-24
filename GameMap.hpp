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
    bool isEmpty(CellPos const &pos) const { return getCell(pos) == Cell::empty; }
    bool isStone(CellPos const &pos) const {
        auto c = getCell(pos);
        return static_cast<int>(c) >= static_cast<int>(Cell::stone1) &&
               static_cast<int>(c) <= static_cast<int>(Cell::stone3);
    }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    void render(int x, int y, TextureManager const &tmgr,
                SDL_Renderer *renderer, SDL_Rect const *dest);

private:
    void initializeMap(std::minstd_rand &random_engine);
    void createMaze(std::minstd_rand &random_engine);
    void setUpTheRopes();
    void setHatchPosition(std::minstd_rand &random_engine);

    void setCell(CellPos const &pos, Cell c);
    SDL_Texture *getTextureOf(TextureManager const& tmgr, CellPos const &pos) const;

    int width, height;
    std::vector<Cell> contents;
};
