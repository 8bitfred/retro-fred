#pragma once

#include "Coord.hpp"
#include "TextureID.hpp"
#include <vector>
#include <SDL.h>
#include <memory>


class Frame;
class TextureManager;

class Sprite
{
public:
    struct CenterPos {
        int x = 0, y = 0;
    };
    bool isVisible(Frame const &frame) const;
    void render(Frame const &frame, TextureManager const &tmgr,
                SDL_Renderer *renderer) const;

protected:
    Sprite(Frame const &frame, MapPos const &pos, int char_width, int char_height);
    virtual std::pair<TextureID, CenterPos> getTexture() const = 0;

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
    FRED,
    COUNT
};

using SpriteList = std::vector<std::unique_ptr<Sprite>>;
