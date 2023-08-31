#include "Chameleon.hpp"
#include "Game.hpp"

Chameleon::Chameleon(Window const &window, MapPos const &pos, 
                     std::minstd_rand &random_engine)
    : Sprite::Sprite(window, pos, 1, 2), random_engine(random_engine)
{

}

void Chameleon::update(Game &game, unsigned)
{
    alternate_frame ^= 1;
    if (sprite_pos.cx == 1 || sprite_pos.cx == 2)
    {
        sprite_pos.xadd(direction_x);
        return;
    }
    std::uniform_int_distribution<> distrib(0, 63);
    if (distrib(random_engine) == 3)
    {
        direction_x = -direction_x;
        sprite_pos.xadd(direction_x);
        return;
    }
    int limit = direction_y < 0 ? 0 : 3;
    if (sprite_pos.cy == limit && 
        !isValidCell(game.getGameMap(), sprite_pos.cellPos(0, direction_y)))
    {
        direction_y = -direction_y;
        return;
    }
    sprite_pos.yadd(direction_y);
}

bool Chameleon::isValidCell(GameMap const &game_map, CellPos const &pos)
{
    return !game_map.isStone(pos) &&
        game_map.isStone(pos, -1) &&
        game_map.isStone(pos, 1);
}

Sprite::RenderInfo Chameleon::getTexture() const
{
    // Offset of cells 0, 1, 2 and 3:
    static constexpr int c0 = 1;
    static constexpr int c1 = c0 + 18;
    static constexpr int c2 = c1 + 18;
    static constexpr int c3 = c2 + 18;
    static constexpr int c4 = c3 + 18;
    static constexpr int c5 = c4 + 18;
    static constexpr int c6 = c5 + 18;
    static constexpr int c7 = c6 + 18;
    static RenderInfo textures[2][2][2] =
        {
            {
                {
                    {TextureID::CHAMELEON, {c0, 1, 8, 16}, 0, 0},  // frame id 1
                    {TextureID::CHAMELEON, {c1, 1, 8, 16}, 0, 0},  // frame id 2
                },
                {
                    {TextureID::CHAMELEON, {c2, 1, 8, 16}, 0, 0},  // frame id 3
                    {TextureID::CHAMELEON, {c3, 1, 8, 16}, 0, 0},  // frame id 4
                },
            },
            {
                {
                    {TextureID::CHAMELEON, {c4, 1, 8, 16}, 0, 0},  // frame id 5
                    {TextureID::CHAMELEON, {c5, 1, 8, 16}, 0, 0},  // frame id 6
                },
                {
                    {TextureID::CHAMELEON, {c6, 1, 8, 16}, 0, 0},  // frame id 7
                    {TextureID::CHAMELEON, {c7, 1, 8, 16}, 0, 0},  // frame id 8
                },
            },
        };
    // side is 0 if cx is 0 or 2, and 1 if cx is 1 or 3
    int side = sprite_pos.cx % 2;
    // vdir is 0 if direction_y is -1 and 1 if direction_y is +1
    int vdir = (direction_y + 1) >> 1;
    return textures[side][vdir][alternate_frame];
}
