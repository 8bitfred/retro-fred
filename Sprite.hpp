#pragma once

#include "Coord.hpp"
#include "TextureID.hpp"
#include <vector>
#include <SDL.h>
#include <memory>


class Window;
class TextureManager;
class Game;

class Sprite
{
public:
    struct RenderInfo {
        TextureID texture_id;
        SDL_Rect src_rect;
        int center_x, center_y;
    };
    virtual ~Sprite() = default;
    bool isVisible(Window const &window) const;
    void render(Window const &window, TextureManager const &tmgr,
                SDL_Renderer *renderer) const;
    virtual void update(Game &game, unsigned events);
    //TODO: this may not need to be exposed
    MapPos const &getPos() const { return sprite_pos; }
    bool checkCollision(Sprite const &other);

    enum class BulletEffect
    {
        IGNORE,
        HIT,
        DIE
    };
    virtual BulletEffect bulletHit() { return BulletEffect::IGNORE; }

protected:
    Sprite(Window const &window, MapPos const &pos, int char_width, int char_height);
    virtual RenderInfo const &getTexture() const = 0;

    // sSprite
    MapPos sprite_pos;
private:
    // Width and height, in characters
    int char_width, char_height;
    // Visibility min and max positions
    ScreenPos min_visible_pos, max_visible_pos;
};

enum class SpriteClass
{
    BLOCK,
    ACID_DROP,
    RAT,
    GHOST,
    CHAMELEON,
    MUMMY,
    VAMPIRE,
    SKELETON,
    FRED,
    BULLET,
    SMOKE,
    COUNT
};

using SpriteList = std::vector<std::unique_ptr<Sprite>>;
