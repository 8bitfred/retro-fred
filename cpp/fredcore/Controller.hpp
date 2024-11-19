#pragma once

#include "sdl.hpp"

class TextureManager;

class Controller
{
    bool back_button = false;
    // Coordinates and size in pixels in the window
    SDL_Rect arrows, fire, back;
    // Relative position as a fraction of the window width and height
    SDL_FRect farrows, ffire, fback;

public:
    Controller(SDL_Window *window, bool back_button);
    void resetPosition(SDL_Window *window);
    bool hasBackButton() const { return back_button; }
    SDL_FRect const &getArrowRect() const { return farrows; }
    SDL_FRect const &getFireRect() const { return ffire; }
    SDL_FRect const &getBackRect() const { return fback; }
    void render(SDL_Renderer *renderer, TextureManager const &tmgr) const;
};
