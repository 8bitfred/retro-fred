#include "SdlRenderer.hpp"
#include "SdlTexture.hpp"
#include "SdlSurface.hpp"
#include <stdexcept>

namespace
{
    SDL_Rect toSdlRect(Rect const &r)
    {
        return SDL_Rect{r.x, r.y, r.w, r.h};
    }

    SDL_Texture *sdlTextureOf(Texture &texture)
    {
        return static_cast<SdlTexture &>(texture).get();
    }
}

std::unique_ptr<Texture> SdlRenderer::loadTexture(std::string const &path)
{
    return std::make_unique<SdlTexture>(sdl::TexturePtr(IMG_LoadTexture(renderer, path.c_str())));
}

std::unique_ptr<Surface> SdlRenderer::loadSurface(std::string const &path)
{
    return std::make_unique<SdlSurface>(sdl::SurfacePtr(IMG_Load(path.c_str())));
}

void SdlRenderer::renderClear()
{
    if (SDL_RenderClear(renderer) < 0)
        throw sdl::Error();
}

void SdlRenderer::renderCopy(Texture &texture, Rect const *src, Rect const *dst, bool hflip)
{
    SDL_Rect sdl_src, sdl_dst;
    SDL_Rect const *sdl_src_ptr = nullptr;
    SDL_Rect const *sdl_dst_ptr = nullptr;
    if (src)
    {
        sdl_src = toSdlRect(*src);
        sdl_src_ptr = &sdl_src;
    }
    if (dst)
    {
        sdl_dst = toSdlRect(*dst);
        sdl_dst_ptr = &sdl_dst;
    }
    auto status = SDL_RenderCopyEx(renderer, sdlTextureOf(texture), sdl_src_ptr, sdl_dst_ptr, 0,
                                   nullptr, hflip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    if (status != 0)
        throw sdl::Error();
}

void SdlRenderer::renderFillRect(Rect const *rect, Color const &color)
{
    auto blend_mode = color.a == 0xff ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND;
    sdl::ColorGuard color_guard(renderer, color.r, color.g, color.b, color.a, blend_mode);
    if (!rect)
    {
        if (SDL_RenderFillRect(renderer, nullptr) < 0)
            throw sdl::Error();
        return;
    }
    auto sdl_rect = toSdlRect(*rect);
    if (SDL_RenderFillRect(renderer, &sdl_rect) < 0)
        throw sdl::Error();
}

void SdlRenderer::renderDrawRect(Rect const &rect, Color const &color)
{
    auto blend_mode = color.a == 0xff ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND;
    sdl::ColorGuard color_guard(renderer, color.r, color.g, color.b, color.a, blend_mode);
    auto sdl_rect = toSdlRect(rect);
    if (SDL_RenderDrawRect(renderer, &sdl_rect) < 0)
        throw sdl::Error();
}

void SdlRenderer::setTextureColorMod(Texture &texture, Color const &color)
{
    if (SDL_SetTextureColorMod(sdlTextureOf(texture), color.r, color.g, color.b) < 0)
        throw sdl::Error();
}

void SdlRenderer::setTextureAlphaMod(Texture &texture, std::uint8_t alpha)
{
    if (SDL_SetTextureAlphaMod(sdlTextureOf(texture), alpha) < 0)
        throw sdl::Error();
}

void SdlRenderer::setViewport(Rect const *viewport)
{
    if (!viewport)
    {
        SDL_RenderSetViewport(renderer, nullptr);
        return;
    }
    auto sdl_rect = toSdlRect(*viewport);
    SDL_RenderSetViewport(renderer, &sdl_rect);
}

void SdlRenderer::setScale(float scale_x, float scale_y)
{
    SDL_RenderSetScale(renderer, scale_x, scale_y);
}

std::pair<int, int> SdlRenderer::getOutputSize() const
{
    int w, h;
    SDL_GetRendererOutputSize(renderer, &w, &h);
    return {w, h};
}

void SdlRenderer::setTitle(std::string const &title)
{
    if (window)
        SDL_SetWindowTitle(window, title.c_str());
}

void SdlRenderer::setIcon(Surface const &icon)
{
    if (window)
        SDL_SetWindowIcon(window, static_cast<SdlSurface const &>(icon).get());
}

void SdlRenderer::saveAsPNG(std::string const &path) const
{
    if (!offscreen_surface)
        throw std::logic_error("SdlRenderer::saveAsPNG: not an offscreen renderer");
    if (IMG_SavePNG(offscreen_surface, path.c_str()) < 0)
        throw sdl::Error();
}

void SdlRenderer::present()
{
    SDL_RenderPresent(renderer);
}
