#pragma once

#include "Geometry.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

class Texture
{
public:
    virtual ~Texture() = default;
};

class Surface
{
public:
    virtual ~Surface() = default;
    virtual void savePNG(std::string const &path) const = 0;
};

enum class WindowFlags : unsigned
{
    NONE = 0,
    HIGH_DPI = 1u << 0,
    RESIZABLE = 1u << 1,
    BORDERLESS = 1u << 2,
    FULLSCREEN_DESKTOP = 1u << 3,
};

inline WindowFlags operator|(WindowFlags a, WindowFlags b)
{
    return static_cast<WindowFlags>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
}

inline bool operator&(WindowFlags a, WindowFlags b)
{
    return (static_cast<unsigned>(a) & static_cast<unsigned>(b)) != 0;
}

// A Renderer represents a window and its associated 2D drawing surface.
class Renderer
{
public:
    virtual ~Renderer() = default;

    virtual std::unique_ptr<Texture> loadTexture(std::string const &path) = 0;
    virtual std::unique_ptr<Surface> loadSurface(std::string const &path) = 0;

    virtual void renderClear() = 0;
    // src == nullptr means the whole texture; dst == nullptr means the whole render target.
    virtual void renderCopy(Texture &texture, Rect const *src, Rect const *dst,
                            bool hflip = false) = 0;
    // rect == nullptr means the whole render target.
    virtual void renderFillRect(Rect const *rect, Color const &color) = 0;
    virtual void renderDrawRect(Rect const &rect, Color const &color) = 0;
    virtual void setTextureColorMod(Texture &texture, Color const &color) = 0;
    virtual void setTextureAlphaMod(Texture &texture, std::uint8_t alpha) = 0;

    virtual void setViewport(Rect const *viewport) = 0;
    virtual void setScale(float scale_x, float scale_y) = 0;
    virtual std::pair<int, int> getOutputSize() const = 0;
    // Change the size of the window (and center it in the screen). The size is in the same
    // units as the size passed to Platform::createWindowAndRenderer(). Does nothing for
    // renderers without a window.
    virtual void setWindowSize(int width, int height) = 0;

    virtual void setTitle(std::string const &title) = 0;
    virtual void setIcon(Surface const &icon) = 0;

    // Only valid for offscreen (software) renderers created via
    // Platform::createOffscreenRenderer().
    virtual void saveAsPNG(std::string const &path) const = 0;

    virtual void present() = 0;
};
