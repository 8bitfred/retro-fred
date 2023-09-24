#pragma once

#include "Sprite.hpp"
#include "SoundID.hpp"

class Game;

class Fred : public Sprite
{
public:
    explicit Fred(MapPos pos): Sprite(pos) {}
    void updateFred(Game& game, unsigned events);
    bool collisionInProgress() const { return collision_timer != 0; }
    void checkCollisionWithEnemy(Game &game, Sprite const &other);
    void checkCollisionWithObject(Game &game);
    bool exiting() const { return state == State::EXIT_MAZE; }
    bool gameOver() const { return state == State::GAME_OVER; }

    void dbgResetPosition(Game& game);
    void dbgMoveToHatch(Game &game);
    void dbgDie();

protected:
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;

private:
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
        CLIMBING1,
        CLIMBING2,
        SHOOTING_STANDING,
        SHOOTING_BIG_STEP,
        SHOOTING_SMALL_STEP,
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
    void checkFire(Game &game, bool fire);
    void stateWalk(Game &game, int input_x, int input_y);
    void walkOneStep(Game &game);
    void startSideJump(Game &game);
    void startVerticalJump(Game &game);
    void stateVerticalJump(Game &game, int input_x, int input_y);
    void stateSideJump(Game& game, int input_x, int input_y);
    void stateRopeClimb(Game& game, int input_x, int input_y);
    void stateExitMaze(Game &game);
    void stateGameOver(Game &game);

    int vposition = 1;
    int direction = -1;
    Frame frame = Frame::STANDING;
    Color color = Color::YELLOW;
    bool shooting = false;
    int jump_stage = 0;
    State state = State::WALK;
    SoundID climbing_sound = SoundID::CLIMB1;
    int collision_timer = 0;
};
