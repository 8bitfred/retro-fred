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
        SHOOTING_STANDING,
        SHOOTING_BIG_STEP,
        SHOOTING_SMALL_STEP,
        COUNT
    };
    CellPos nextCellPos() const {
        return sprite_pos.cellPos().hmove(static_cast<int>(frame_dir));
    }
    void checkFire(Game &game, bool fire);
    void stateWalk(Game &game, int hmove, int vmove);
    void walkOneStep(Game &game);
    void startSideJump(Game &game);
    void startVerticalJump(Game &game);
    void stateVerticalJump(Game &game, int hmove, int vmove);
    void stateSideJump(Game& game, int hmove, int vmove);
    void stateRopeClimb(Game& game, int hmove, int vmove);

    int frame_dir = -1;
    FrameType frame_type = FrameType::STANDING;
    bool frame_shooting = false;
    int jump_stage = 0;
    State state = State::WALK;
    SoundID climbing_sound = SoundID::CLIMB1;
};
