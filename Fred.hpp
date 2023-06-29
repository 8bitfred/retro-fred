#pragma once

#include "Sprite.hpp"

class GameMap;

class Fred : public Sprite
{
public:
    Fred(Frame const &frame, MapPos initial_position);
    std::pair<int, int> updateFred(GameMap const &game_map, unsigned action);

protected:
    std::pair<TextureID, CenterPos> getTexture() const override;

private:
    enum class State
    {
        REST_ON_FOOT,
        WALK,
        VERTICAL_JUMP,
        SIDE_JUMP,
        REST_ON_THE_ROPE,
        ROPE_CLIMB,
        EXIT_MAZE,
    };
    enum class Direction : std::uint8_t
    {
        LEFT = 0,
        RIGHT = 1,
        COUNT
    };
    enum class Type : std::uint8_t
    {
        STANDING,
        BIG_STEP,
        SMALL_STEP,
        CLIMBING1,
        CLIMBING2,
        SHOOTING,
        JUMP_SHOOTING,
        COUNT
    };
    CellPos nextCellPos() const {
        return sSpritePos.cellPos().hmove(2 * static_cast<int>(direction) - 1);
    }
    CellPos prevCellPos() const {
        return sSpritePos.cellPos().hmove(1 - 2 * static_cast<int>(direction));
    }
    std::pair<int, int> updateRestOnFoot(GameMap const& game_map, unsigned action);
    std::pair<int, int> updateWalk(GameMap const& game_map, unsigned action);
    std::pair<int, int> updateVerticalJump(GameMap const &game_map, unsigned action);
    std::pair<int, int> updateSideJump(GameMap const &game_map, unsigned action);
    std::pair<int, int> updateRestOnTheRope(GameMap const &game_map, unsigned action);
    std::pair<int, int> updateRopeClimb(GameMap const &game_map, unsigned action);

    Direction direction = Direction::LEFT;
    Type type = Type::STANDING;
    int sStage = 0;
    State sState = State::REST_ON_FOOT;
};
