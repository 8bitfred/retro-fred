#pragma once

#include "platform/Geometry.hpp"

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
    // Space at the edges of the window that the game should not use, so that the controller
    // does not cover it.
    struct Margins
    {
        // On each side of the window
        int side = 0;
        // At the bottom of the window
        int bottom = 0;
    };
    // In portrait orientation the buttons are placed at the bottom of the window, and in
    // landscape orientation on the sides. We only reserve half of the size of the buttons,
    // so the other half covers part of the game.
    static Margins getMargins(int window_w, int window_h);

    Controller(DisplayConfig const &display_cfg, bool back_button);
    void resetPosition(DisplayConfig const &display_cfg);
    bool hasBackButton() const { return back_button; }
    FRect const &getArrowRect() const { return farrows; }
    FRect const &getFireRect() const { return ffire; }
    FRect const &getBackRect() const { return fback; }
    void render(Renderer &renderer, TextureManager const &tmgr) const;
};
