#pragma once

#include "platform/Renderer.hpp"
#include "sdl_raii.hpp"

// A Renderer backed by an SDL window + renderer (on-screen), or by an SDL software
// renderer bound to an in-memory surface (offscreen, for frame capture).
class SdlRenderer : public Renderer
{
    sdl::WindowPtr window;
    sdl::RendererPtr renderer;
    sdl::SurfacePtr offscreen_surface;

public:
    SdlRenderer(sdl::WindowPtr window, sdl::RendererPtr renderer)
        : window(std::move(window)), renderer(std::move(renderer)) {}
    SdlRenderer(sdl::SurfacePtr surface, sdl::RendererPtr renderer)
        : renderer(std::move(renderer)), offscreen_surface(std::move(surface)) {}

    std::unique_ptr<Texture> loadTexture(std::string const &path) override;
    std::unique_ptr<Surface> loadSurface(std::string const &path) override;

    void renderClear() override;
    void renderCopy(Texture &texture, Rect const *src, Rect const *dst,
                    bool hflip) override;
    void renderFillRect(Rect const *rect, Color const &color) override;
    void renderDrawRect(Rect const &rect, Color const &color) override;
    void setTextureColorMod(Texture &texture, Color const &color) override;
    void setTextureAlphaMod(Texture &texture, std::uint8_t alpha) override;

    void setViewport(Rect const *viewport) override;
    void setScale(float scale_x, float scale_y) override;
    std::pair<int, int> getOutputSize() const override;

    void setTitle(std::string const &title) override;
    void setIcon(Surface const &icon) override;

    void saveAsPNG(std::string const &path) const override;

    void present() override;
};
