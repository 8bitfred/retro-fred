#include "TextureManager.hpp"
#include "Config.hpp"
#include <cassert>
#include <cstdio>
#include <map>
#include <algorithm>

TextureManager::TextureManager(Config const &cfg, SDL_Renderer *renderer)
{
    static char const *sprite_list[] = {
        "sprites/splash_screen.png",
        "sprites/fred_logo.png",
        "sprites/todays_greatest.png",
        "sprites/block.png",
        "sprites/bullet.png",
        "sprites/aciddrop.png",
        "sprites/skeleton.png",
        "sprites/mummy.png",
        "sprites/vampire.png",
        "sprites/rat.png",
        "sprites/chameleon.png",
        "sprites/object.png",
        "sprites/ghost.png",
        "sprites/smoke.png",
        "sprites/fred.png",
        "sprites/tomb.png",
        "sprites/window.png",
        "sprites/zx_font.png",
        "sprites/small_digits.png",
        "sprites/power.png",
        "sprites/fred_puffing.png",
        "sprites/controller.png",
        "sprites/fred_icon.png",
        "sprites/loading1.png",
        "sprites/loading2.png",
        "sprites/loading3.png",
        "sprites/loading4.png",
    };
    static_assert(std::size(sprite_list) == static_cast<size_t>(TextureID::COUNT));
    for (auto p : sprite_list) {
        auto path = cfg.resource_path.empty() ? p : cfg.resource_path + "/" + p;
        texture_list.emplace_back(IMG_LoadTexture(renderer, path.c_str()));
    }

    auto const &fred_icon_path = sprite_list[static_cast<int>(TextureID::FRED_ICON)];
    auto fred_path = cfg.resource_path.empty() ? fred_icon_path : cfg.resource_path + "/" + fred_icon_path;
    fred_icon = sdl::SurfacePtr(IMG_Load(fred_path.c_str()));
}

void TextureManager::renderText(SDL_Renderer *renderer, std::string_view text,
                                int x, int y,
                                Uint8 red, Uint8 green, Uint8 blue) const
{
    auto texture = get(TextureID::ZX_FONT);
    if (SDL_SetTextureColorMod(texture, red, green, blue) < 0)
        throw sdl::Error();
    SDL_Rect src{0, 0, 8, 8};
    SDL_Rect dst{x, y, 8, 8};
    for (auto ch : text)
    {
        auto c = static_cast<unsigned>(ch);
        if (c > ' ' && c < 0x90)
        {
            src.x = 10 * (c % 16) + 1;
            src.y = 10 * ((c - ' ') / 16) + 1;
            if (SDL_RenderCopy(renderer, texture, &src, &dst) < 0)
                throw sdl::Error();
        }
        dst.x += 8;
    }
}

void TextureManager::renderScore(SDL_Renderer *renderer, unsigned score,
                                 int x, int y, Uint8 red, Uint8 green, Uint8 blue) const
{
    auto texture = get(TextureID::SMALL_DIGITS);
    if (SDL_SetTextureColorMod(texture, red, green, blue) < 0)
        throw sdl::Error();
    score = std::min(score, 999999u);
    SDL_Rect src{0, 0, 5, 5};
    SDL_Rect dst{x + 5 * 6, y, 5, 5};
    for (int i = 0; i < 6; ++i)
    {
        int digit = score % 10;
        score /= 10;
        src.x = 5 * digit;
        if (SDL_RenderCopy(renderer, texture, &src, &dst) < 0)
            throw sdl::Error();
        dst.x -= 6;
    }
}
