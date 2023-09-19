#include "TextureManager.hpp"
#include <SDL_image.h>
#include <cassert>
#include <cstdio>
#include <map>
#include <algorithm>

TextureManager::TextureManager(SDL_Renderer *renderer)
{
    // This list must match the order of elements in the TextureID enum
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/block.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/bullet.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/skeleton.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/mummy.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/vampire.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/rat.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/chameleon.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/object.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/ghost.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/smoke.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/game_over.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/window.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/zx_font.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/small_digits.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/power.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_puffing.png"));
    assert(texture_list.size() == static_cast<size_t>(TextureID::COUNT));
}

void TextureManager::renderText(SDL_Renderer *renderer, std::string_view text,
                                int x, int y,
                                Uint8 red, Uint8 green, Uint8 blue) const
{
    auto texture = get(TextureID::ZX_FONT);
    if (SDL_SetTextureColorMod(texture, red, green, blue) < 0)
        throw sdl::Error();
    SDL_Rect src(0, 0, 8, 8);
    SDL_Rect dst(x, y, 8, 8);
    for (auto ch : text)
    {
        auto c = static_cast<unsigned>(ch);
        if (c > ' ' && c < 0x90)
        {
            src.x = 8 * (c % 16);
            src.y = 8 * ((c - ' ') / 16);
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
    SDL_Rect src(0, 0, 5, 5);
    SDL_Rect dst(x + 5 * 6 + 1, y + 2, 5, 5);
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
