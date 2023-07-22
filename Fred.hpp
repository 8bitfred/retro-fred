#pragma once

#include "Sprite.hpp"
#include "SoundID.hpp"

class Game;

class Fred : public Sprite
{
public:
    Fred(Frame const &frame, MapPos initial_position);
    void updateFred(Game& game, unsigned events);

    void dbgResetPosition(Game& game);
    void dbgMoveToHatch(Game &game);

protected:
    RenderInfo getTexture() const override;

private:
    enum class State : uint8_t
    {
        WALK,
        VERTICAL_JUMP,
        SIDE_JUMP,
        ROPE_CLIMB,
        EXIT_MAZE,
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
    void stateWalk(Game& game, int hmove, int vmove, bool fire);
    void walkOneStep(Game &game, bool fire);
    void startSideJump(Game &game, bool fire);
    void startVerticalJump(Game &game, bool fire);
    void stateVerticalJump(Game &game, int hmove, int vmove, bool fire);
    void stateSideJump(Game& game, int hmove, int vmove, bool fire);
    void stateRopeClimb(Game& game, int hmove, int vmove, bool fire);

    int frame_dir = -1;
    FrameType frame_type = FrameType::STANDING;
    int jump_stage = 0;
    State state = State::WALK;
    SoundID climbing_sound = SoundID::CLIMB1;
};
