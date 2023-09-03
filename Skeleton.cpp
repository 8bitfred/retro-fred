#include "Skeleton.hpp"
#include "Game.hpp"

int Skeleton::climbing_frame = 0;

void Skeleton::update(Game &game, unsigned)
{
    if (sprite_pos.cx() == 0 && sprite_pos.cy() == 1)
    {
        auto [delta_x, delta_y] = getDirDelta();
        auto const &fred_pos = game.getFredPos();
        int distance = delta_x != 0 ?
            (sprite_pos.y() - fred_pos.y()) * delta_x :
            (sprite_pos.x() - fred_pos.x()) * -delta_y;
        if (distance == 0)
        {
            auto next_pos = sprite_pos.cellPos(delta_x, delta_y);
            if (game.getGameMap().isStone(next_pos) ||
                game.getGameMap().getBlock(next_pos) == GameMap::Cell::TRAPDOOR)
                selectDirection(game.getGameMap(), getRandomSense());
        }
        else
            selectDirection(game.getGameMap(), distance > 0);
    }
    auto [delta_x, delta_y] = getDirDelta();
    if (delta_x != 0)
    {
        if (frame != Frame::STANDING)
            frame = Frame::STANDING;
        else if (sprite_pos.cx() < 2)
            frame = Frame::BIG_STEP;
        else
            frame = Frame::SMALL_STEP;
        sprite_pos.xadd(delta_x);
    }
    else
    {
        frame = climbing_frame == 0 ? Frame::CLIMBING1 : Frame::CLIMBING2;
        sprite_pos.yadd(delta_y);
    }
}

Sprite::RenderInfo Skeleton::getTexture() const
{
    static constexpr int c0 = 8;
    static constexpr int c1 = c0 + 58;
    static constexpr int c2 = c1 + 58;
    static constexpr int c3 = c2 + 58;
    static constexpr int c4 = c3 + 58;
    static constexpr int c5 = c4 + 58;
    static constexpr int c6 = c5 + 58;
    static constexpr int c7 = c6 + 58;

    static RenderInfo textures[2][5] =
        {
            {
                {TextureID::SKELETON, {c0, 8, 34, 34}, 1, 1},  // frame id 1
                {TextureID::SKELETON, {c1, 8, 34, 34}, 1, 1},  // frame id 2
                {TextureID::SKELETON, {c2, 8, 34, 34}, 1, 1},  // frame id 3
                {TextureID::SKELETON, {c6, 8, 34, 34}, 1, 1},  // frame id 7
                {TextureID::SKELETON, {c7, 8, 34, 34}, 1, 1},  // frame id 8
            },
            {
                {TextureID::SKELETON, {c3, 8, 34, 34}, 1, 1},  // frame id 4
                {TextureID::SKELETON, {c4, 8, 34, 34}, 1, 1},  // frame id 5
                {TextureID::SKELETON, {c5, 8, 34, 34}, 1, 1},  // frame id 6
                {TextureID::SKELETON, {c6, 8, 34, 34}, 1, 1},  // frame id 7
                {TextureID::SKELETON, {c7, 8, 34, 34}, 1, 1},  // frame id 8
            },
        };
    // dir_x is -1 when moving left, +1 when moving right, and 0 if moving vertically
    auto [dir_x, dir_y] = getDirDelta();
    // dir_x is 0 if moving left or vertically, and 1 if moving right
    int dir_index = (dir_x + 1) >> 1;
    return textures[dir_index][static_cast<int>(frame)];
}
