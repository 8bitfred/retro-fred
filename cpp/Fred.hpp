#pragma once

#include "Sprite.hpp"
#include "SoundID.hpp"
#include "GameEvent.hpp"

class Game;

class Fred : public Sprite
{
    enum class State : uint8_t
    {
        WALK,
        VERTICAL_JUMP,
        SIDE_JUMP,
        CLIMB,
        EXIT_MAZE,
        GAME_OVER,
    };
    enum class Frame : std::uint8_t
    {
        STANDING,
        BIG_STEP,
        SMALL_STEP,
        CLIMBING_CLAMPING,
        CLIMBING_EXTENDING,
        COUNT
    };
    enum class Color : std::uint8_t
    {
        YELLOW,
        CYAN,
        GREEN,
        MAGENTA,
        RED,
        BLUE,
        BLACK,
        COUNT
    };
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
    Label getLabel() const override;
    void checkFire(bool fire);
    void stateWalk(int input_x, int input_y);
    void walkOneStep();
    void startSideJump();
    void startVerticalJump();
    void stateVerticalJump(int input_x, int input_y);
    void stateSideJump(int input_x, int input_y);
    void stateRopeClimb(int input_x, int input_y);
    void stateExitMaze();
    void stateGameOver();

    Game &game;
    int vposition = 1;
    int direction = -1;
    Frame frame = Frame::STANDING;
    Color color = Color::YELLOW;
    bool shooting = false;
    int jump_stage = 0;
    State state = State::WALK;
    SoundID climbing_sound = SoundID::CLIMBTAK;
    int collision_timer = 0;

public:
    Fred(Game &game, MapPos pos): Sprite(pos), game(game) {}
    void updateFred(EventMask event_mask);
    bool collisionInProgress() const { return collision_timer != 0; }
    void checkCollisionWithEnemy(Sprite const &other);
    void checkCollisionWithObject();

    void dbgResetPosition();
    void dbgMoveToHatch();
    void dbgDie();
};
