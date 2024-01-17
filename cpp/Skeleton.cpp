#include "Skeleton.hpp"
#include "Game.hpp"

int Skeleton::climbing_frame = 0;

void Skeleton::initialize(std::minstd_rand &random_engine, Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::SKELETON);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < game.getSpriteCount().skeletons; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 1};
            if (game.getGameMap().getBlock(pos.cellPos()) != GameMap::Cell::EMPTY)
                continue;
            sprite_list.emplace_back(std::make_unique<Skeleton>(game, pos, random_engine));
            break;
        }
    }
}

void Skeleton::update(unsigned)
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

Sprite::BoxParams const &Skeleton::getBoxParams() const
{
    static BoxParams box_params[] = {
        {      9, 9, { -1, -1, 34, 34 }, {{11, 0, 12, 32}, {0, 11, 29, 12}} },
        {   58+9, 9, { -1, -1, 34, 34 }, {{12, 1, 17, 31}, {0, 13, 12, 16}} },
        { 2*58+9, 9, { -1, -1, 34, 34 }, {{11, 1, 13, 31}, {0, 13, 28, 13}} },
        { 3*58+9, 9, { -1, -1, 34, 34 }, {{ 5, 0, 22, 29}} },
        { 4*58+9, 9, { -1, -1, 34, 34 }, {{ 5, 0, 22, 31}} },
    };
    static_assert(std::size(box_params) == static_cast<size_t>(Frame::COUNT));
    return box_params[static_cast<size_t>(frame)];
}

Sprite::RenderParams Skeleton::getRenderParams() const
{
    // dir_x is -1 when moving left, +1 when moving right, and 0 if moving vertically
    auto [dir_x, dir_y] = getDirDelta();
    return {TextureID::SKELETON, dir_x == 1, {}};
}
