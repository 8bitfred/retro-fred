#pragma once

#include "Coord.hpp"
#include "platform/Geometry.hpp"
#include "TextureID.hpp"
#include "Label.hpp"
#include "LabelTable.hpp"
#include <vector>
#include <memory>
#include <cstdint>


class GameWindow;
class TextureManager;
class Renderer;
struct Config;

class Sprite
{
public:
    struct BoxParams {
        // Position of the "center" (reference point) of the sprite in the texture
        int pos_x = 0, pos_y = 0;
        // Bounding box, with respect to the "center" (reference point) of the sprite
        Rect bounding_box;
        // Hit boxes, defined with respect to the "center" of the sprite
        std::vector<Rect> hit_boxes;
    };
    struct ColorModulation {
        std::uint8_t r = 0xff, g = 0xff, b = 0xff;
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
                Renderer &renderer) const;
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
    static Rect getHitboxPos(int px, int py,
                             Rect const &bounding_box,
                             Rect const &hitbox, bool hflip);
    bool checkHitBoxes(Rect const &rect2, Rect const &bounding_box, bool hflip) const;
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
