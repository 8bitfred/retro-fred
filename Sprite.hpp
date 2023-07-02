#pragma once

#include "Coord.hpp"
#include "TextureID.hpp"
#include <vector>
#include <SDL.h>
#include <memory>


class Frame;
class TextureManager;
class Game;

class Sprite
{
public:
    struct CenterPos {
        int x = 0, y = 0;
    };
    bool isVisible(Frame const &frame) const;
    void render(Frame const &frame, TextureManager const &tmgr,
                SDL_Renderer *renderer) const;
    virtual void update(Game &game, unsigned events);

protected:
    Sprite(Frame const &frame, MapPos const &pos, int char_width, int char_height);
    virtual std::pair<TextureID, CenterPos> getTexture() const = 0;

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
    FRED,
    COUNT
};

using SpriteList = std::vector<std::unique_ptr<Sprite>>;
