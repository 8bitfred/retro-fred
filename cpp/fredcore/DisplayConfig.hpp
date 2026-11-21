#pragma once

#include "platform/Renderer.hpp"
#include "Config.hpp"
#include <memory>

class Platform;

class DisplayConfig
{
    std::unique_ptr<Renderer> renderer;
    // Size of the display, to choose the size of the window
    std::pair<int, int> display_resolution;
    // The size of the window is determined by the platform (see Config::fixed_window_size)
    bool fixed_window;
    // The window can be resized when the full map mode changes (it is neither full screen
    // nor has a size selected by the user)
    bool resizable_window;
    // Mode used for the last layout of the screen
    MapMode mode;
    int window_frame_scale = 1;
    int game_window_w = 0, game_window_h = 0;
    // Space at the sides and the bottom of the window that is not used by the game (to make
    // room for the virtual controller)
    int reserved_side = 0, reserved_bottom = 0;
    int intro_window_w = 0, intro_window_h = 0;
    // The game is drawn to this texture, which has the size of the game window, and then
    // the texture is scaled to the window (see beginGameFrame())
    std::unique_ptr<Texture> frame_texture;
    int frame_texture_w = 0, frame_texture_h = 0;

    void updateFrameTexture();
    std::pair<int, int> getFullMapSize(Config const &cfg) const;
    int getFullMapFrameScale(Config const &cfg) const;
    std::pair<int, int> getFullMapWindowSize(Config const &cfg) const;
    // Integer scale used for the window in normal mode
    int getNormalScale(Config const &cfg) const;
    std::pair<int, int> getNormalWindowSize(Config const &cfg) const;
    // Area of the window available to the game
    Rect getGameArea() const;
    void setFullMapLayout(Config const &cfg);
    void setNormalLayout(Config const &cfg);
    void setFullSizeLayout(Config const &cfg);
    void setFixedWindowLayout(Config const &cfg);
    void initWindowAndRenderer(Platform &platform, int width, int height, WindowFlags flags);
    void initFullMapMode(Platform &platform, Config const &cfg);
    void initNormalMode(Platform &platform, Config const &cfg);

public:
    DisplayConfig(Config const &cfg, Platform &platform);
    Renderer &getRenderer() const { return *renderer; }
    std::pair<int, int> getWindowSize() const;
    // Update the layout of the screen (and the size of the window, if possible) after a
    // change in the mode (see Config::getMapMode()), or in the size of the window (e.g. when
    // it is resized in the full size mode, or when the device is rotated). Call it regularly,
    // it does nothing if nothing has changed.
    void updateLayout(Config const &cfg);
    Rect getGameWindowRect() const;
    void setWindowTitleAndIcon(std::string const &title, Surface const &icon) const;
    void setIntroViewport() const;
    // The game is not drawn directly to the window, because scaling each sprite separately
    // causes artifacts (for example, when the scale is not an integer the size of the pixels
    // of a sprite can differ from the neighboring sprites, and gaps appear between them).
    // Instead, a frame of the game is drawn to a texture of the size of the game window, in
    // game pixels (scale 1), and then the texture is scaled to the window as a whole:
    //   beginGameFrame(), draw the game and the window frame, endGameFrame().
    // The coordinates are the coordinates of the game window (see getGameWindowRect()).
    void beginGameFrame() const;
    void endGameFrame() const;
    // Draw the original 256x192 screen (e.g. text) in the center of the frame
    void setGameFrameIntroViewport() const;
    // Set the viewport and scale to draw the window frame and the scoreboard, during a frame
    // of the game. Return the size of the viewport (in units of the scale).
    std::pair<int, int> setWindowFrameViewport() const;
    std::pair<int, int> setScoreboardViewport() const;
};
