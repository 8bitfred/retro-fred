#pragma once

#include "Coord.hpp"
#include <vector>
#include <SDL.h>
#include <memory>


class Frame;

class Sprite
{
public:
    struct CenterPos {
        int x = 0, y = 0;
    };
    bool isVisible(Frame const &frame) const;
    void render(Frame const &frame, SDL_Renderer *renderer) const;

protected:
    Sprite(Frame const &frame, MapPos const &pos, int char_width, int char_height);
    virtual std::pair<SDL_Texture *, CenterPos> getTexture() const = 0;

    // sSprite
    MapPos sSpritePos;
private:
    // Width and height, in characters
    int sCharWidth, sCharHeight;
    // Visibility min and max positions
    ScreenPos sMinPos, sMaxPos;
};

enum class SpriteClass
{
    BLOCK,
    COUNT
};

using SpriteList = std::vector<std::unique_ptr<Sprite>>;
