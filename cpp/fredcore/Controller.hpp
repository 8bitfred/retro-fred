#pragma once

#include "Geometry.hpp"

class Renderer;
class TextureManager;
class DisplayConfig;

class Controller
{
    bool back_button = false;
    // Coordinates and size in pixels in the window
    Rect arrows, fire, back;
    // Relative position as a fraction of the window width and height
    FRect farrows, ffire, fback;

public:
    Controller(DisplayConfig const &display_cfg, bool back_button);
    void resetPosition(DisplayConfig const &display_cfg);
    bool hasBackButton() const { return back_button; }
    FRect const &getArrowRect() const { return farrows; }
    FRect const &getFireRect() const { return ffire; }
    FRect const &getBackRect() const { return fback; }
    void render(Renderer &renderer, TextureManager const &tmgr) const;
};
