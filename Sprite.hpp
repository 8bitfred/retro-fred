#pragma once

#include "Coord.hpp"
#include "TextureID.hpp"
#include <vector>
#include <SDL.h>
#include <memory>


class Window;
class TextureManager;
class Game;
struct Config;

class Sprite
{
public:
    struct BoxParams {
        // Position of the "center" (reference point) of the sprite in the texture
        int pos_x = 0, pos_y = 0;
        // Bounding box, with respect to the "center" (reference point) of the sprite
        SDL_Rect bounding_box;
        // Hit boxes, defined with respect to the "center" of the sprite
        std::vector<SDL_Rect> hit_boxes;
    };
    struct ColorModulation {
        Uint8 r = 0xff, g = 0xff, b = 0xff;
    };
    struct RenderParams
    {
        TextureID texture_id = TextureID::COUNT;
        bool hflip = false;
        ColorModulation color_mod;
    };
    virtual ~Sprite() = default;
    bool isVisible(Window const &window) const;
    void render(Config const &cfg,
                Window const &window, TextureManager const &tmgr,
                SDL_Renderer *renderer) const;
    // TODO: update() could be made =0
    virtual void update(Game &, unsigned) {}
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
    explicit Sprite(MapPos const &pos): sprite_pos(pos) { }
    virtual BoxParams const &getBoxParams() const = 0;
    virtual RenderParams getRenderParams() const = 0;

    // sSprite
    MapPos sprite_pos;
private:
    static SDL_Rect getHitboxPos(int px, int py,
                                 SDL_Rect const &bounding_box,
                                 SDL_Rect const &hitbox, bool hflip);
    bool checkHitBoxes(SDL_Rect const &rect2, SDL_Rect const &bounding_box, bool hflip) const;
};

enum class SpriteClass
{
    BLOCK,
    BULLET,
    ACID_DROP,
    SKELETON,
    MUMMY,
    VAMPIRE,
    RAT,
    CHAMELEON,
    OBJECT,
    GHOST,
    SMOKE,
    FRED,
    COUNT
};

using SpriteList = std::vector<std::unique_ptr<Sprite>>;
