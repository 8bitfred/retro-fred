#pragma once

#include "Coord.hpp"
#include "TextureID.hpp"
#include "Label.hpp"
#include "LabelTable.hpp"
#include <vector>
#include <memory>


class GameWindow;
class TextureManager;
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
    void render(Config const &cfg,
                GameWindow const &game_window, TextureManager const &tmgr,
                SDL_Renderer *renderer) const;
    void setLabel(GameWindow const &game_window, LabelTable &label_table) const;
    // TODO: update() could be made =0
    virtual void update(unsigned) {}
    //TODO: this may not need to be exposed
    MapPos const &getPos() const { return sprite_pos; }
    bool checkCollision(Sprite const &other);

    enum class BulletEffect
    {
        IGNORE,
        HIT,
        DISAPPEAR,
        DIE
    };
    virtual BulletEffect bulletHit() { return BulletEffect::IGNORE; }

protected:
    explicit Sprite(MapPos const &pos): sprite_pos(pos) { }
    virtual BoxParams const &getBoxParams() const = 0;
    virtual RenderParams getRenderParams() const = 0;
    virtual Label getLabel() const = 0;

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
    BULLET,
    RAT,
    ACID_DROP,
    GHOST,
    MUMMY,
    VAMPIRE,
    CHAMELEON,
    SKELETON,
    SMOKE,
    FRED,
    OBJECT,
    TOMB,
    COUNT
};

using SpriteList = std::vector<std::unique_ptr<Sprite>>;
