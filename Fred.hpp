#pragma once

#include "Sprite.hpp"
#include "SoundID.hpp"

class Game;

class Fred : public Sprite
{
public:
    Fred(Window const &window, MapPos initial_position);
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
        CLIMB,
        EXIT_MAZE,
    };
    enum class Frame : std::uint8_t
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
    void checkFire(Game &game, bool fire);
    void stateWalk(Game &game, int input_x, int input_y);
    void walkOneStep(Game &game);
    void startSideJump(Game &game);
    void startVerticalJump(Game &game);
    void stateVerticalJump(Game &game, int input_x, int input_y);
    void stateSideJump(Game& game, int input_x, int input_y);
    void stateRopeClimb(Game& game, int input_x, int input_y);

    int direction = -1;
    Frame frame = Frame::STANDING;
    bool shooting = false;
    int jump_stage = 0;
    State state = State::WALK;
    SoundID climbing_sound = SoundID::CLIMB1;
};
