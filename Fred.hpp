#pragma once

#include "Sprite.hpp"

class Game;

class Fred : public Sprite
{
public:
    Fred(Frame const &frame, MapPos initial_position);
    void updateFred(Game& game, unsigned events);

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
    void stateRestOnFoot(Game& game, unsigned events);
    void stateWalk(Game& game, unsigned events);
    void checkWalkActions(Game& game, unsigned events);
    void stateVerticalJump(Game& game, unsigned events);
    void stateSideJump(Game& game, unsigned events);
    void stateRestOnTheRope(Game& game, unsigned events);
    void stateRopeClimb(Game& game, unsigned events);
    void checkRopeActions(Game& game, unsigned events);

    FrameDir frame_dir = FrameDir::LEFT;
    FrameType frame_type = FrameType::STANDING;
    int jump_stage = 0;
    State state = State::REST_ON_FOOT;
};
