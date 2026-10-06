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

    SDL_FRect toSdlFRect(Rect const &r)
    {
        return SDL_FRect{static_cast<float>(r.x), static_cast<float>(r.y),
                         static_cast<float>(r.w), static_cast<float>(r.h)};
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
    if (!SDL_RenderClear(renderer))
        throw sdl::Error();
}

void SdlRenderer::renderCopy(Texture &texture, Rect const *src, Rect const *dst, bool hflip)
{
    SDL_FRect sdl_src, sdl_dst;
    SDL_FRect const *sdl_src_ptr = nullptr;
    SDL_FRect const *sdl_dst_ptr = nullptr;
    if (src)
    {
        sdl_src = toSdlFRect(*src);
        sdl_src_ptr = &sdl_src;
    }
    if (dst)
    {
        sdl_dst = toSdlFRect(*dst);
        sdl_dst_ptr = &sdl_dst;
    }
    if (!SDL_RenderTextureRotated(renderer, sdlTextureOf(texture), sdl_src_ptr, sdl_dst_ptr, 0,
                                  nullptr, hflip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE))
        throw sdl::Error();
}

void SdlRenderer::renderFillRect(Rect const *rect, Color const &color)
{
    auto blend_mode = color.a == 0xff ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND;
    sdl::ColorGuard color_guard(renderer, color.r, color.g, color.b, color.a, blend_mode);
    if (!rect)
    {
        if (!SDL_RenderFillRect(renderer, nullptr))
            throw sdl::Error();
        return;
    }
    auto sdl_rect = toSdlFRect(*rect);
    if (!SDL_RenderFillRect(renderer, &sdl_rect))
        throw sdl::Error();
}

void SdlRenderer::renderDrawRect(Rect const &rect, Color const &color)
{
    auto blend_mode = color.a == 0xff ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND;
    sdl::ColorGuard color_guard(renderer, color.r, color.g, color.b, color.a, blend_mode);
    auto sdl_rect = toSdlFRect(rect);
    if (!SDL_RenderRect(renderer, &sdl_rect))
        throw sdl::Error();
}

void SdlRenderer::setTextureColorMod(Texture &texture, Color const &color)
{
    if (!SDL_SetTextureColorMod(sdlTextureOf(texture), color.r, color.g, color.b))
        throw sdl::Error();
}

void SdlRenderer::setTextureAlphaMod(Texture &texture, std::uint8_t alpha)
{
    if (!SDL_SetTextureAlphaMod(sdlTextureOf(texture), alpha))
        throw sdl::Error();
}

void SdlRenderer::setViewport(Rect const *viewport)
{
    if (!viewport)
    {
        SDL_SetRenderViewport(renderer, nullptr);
        return;
    }
    auto sdl_rect = toSdlRect(*viewport);
    SDL_SetRenderViewport(renderer, &sdl_rect);
}

void SdlRenderer::setScale(float scale_x, float scale_y)
{
    SDL_SetRenderScale(renderer, scale_x, scale_y);
}

std::pair<int, int> SdlRenderer::getOutputSize() const
{
    int w, h;
    if (!SDL_GetRenderOutputSize(renderer, &w, &h))
        throw sdl::Error();
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
    // Unlike SDL2, SDL3's software renderer buffers draws and only writes them through to
    // the backing surface on present -- flush before reading the surface back out.
    if (!SDL_RenderPresent(renderer))
        throw sdl::Error();
    if (!IMG_SavePNG(offscreen_surface, path.c_str()))
        throw sdl::Error();
}

void SdlRenderer::present()
{
    SDL_RenderPresent(renderer);
}
