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
    enum class State : uint8_t
    {
        REST_ON_FOOT,
        WALK,
        VERTICAL_JUMP,
        SIDE_JUMP,
        REST_ON_THE_ROPE,
        ROPE_CLIMB,
        EXIT_MAZE,
    };
    enum class FrameDir : std::int8_t
    {
        LEFT = -1,
        RIGHT = 1,
    };
    enum class FrameType : std::uint8_t
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
        return sprite_pos.cellPos().hmove(static_cast<int>(frame_dir));
    }
    std::pair<int, int> stateRestOnFoot(GameMap const& game_map, unsigned action);
    std::pair<int, int> stateWalk(GameMap const& game_map, unsigned action);
    std::pair<int, int> checkWalkActions(GameMap const& game_map, unsigned action);
    std::pair<int, int> stateVerticalJump(GameMap const &game_map, unsigned action);
    std::pair<int, int> stateSideJump(GameMap const &game_map, unsigned action);
    std::pair<int, int> stateRestOnTheRope(GameMap const &game_map, unsigned action);
    std::pair<int, int> stateRopeClimb(GameMap const &game_map, unsigned action);
    std::pair<int, int> checkRopeActions(GameMap const &game_map, unsigned action);

    FrameDir frame_dir = FrameDir::LEFT;
    FrameType frame_type = FrameType::STANDING;
    int jump_stage = 0;
    State state = State::REST_ON_FOOT;
};
