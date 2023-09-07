#include "TextureManager.hpp"
#include <SDL_image.h>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <map>

sdl::TexturePtr TextureManager::createTextureAndChangeColor(SDL_Renderer *renderer,
                                                            SDL_Surface *src,
                                                            Uint32 in_color,
                                                            Uint32 out_color)
{
    assert(src->format->format == SDL_PIXELFORMAT_ABGR8888);
    sdl::SurfacePtr dst(SDL_CreateRGBSurfaceWithFormat(0, src->w, src->h,
                        32, src->format->format));
    sdl::LockedSurfacePtr locked_src(src);
    sdl::LockedSurfacePtr locked_dst(dst);

    auto src_ptr = reinterpret_cast<Uint32 *>(locked_src->pixels);
    auto dst_ptr = reinterpret_cast<Uint32 *>(locked_dst->pixels);
    int count = src->w * src->h;
    for (int i = 0; i < count; ++i, ++src_ptr, ++dst_ptr)
        *dst_ptr = *src_ptr == in_color ? out_color : *src_ptr;
    return sdl::TexturePtr(SDL_CreateTextureFromSurface(renderer, dst));
}

TextureManager::TextureManager(SDL_Renderer *renderer)
{
    // This list must match the order of elements in the TextureID enum
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/window.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/stone_1x_1.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/stone_1x_2.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/stone_1x_3.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/ropestartsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/ropemainsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/ropeendsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/skysprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/sandsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/trapdoorsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/rat.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/ghost.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/chameleon.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/mummy.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/vampire.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/skeleton.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/bullet.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/smoke.png"));
    //texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred.png"));

    sdl::SurfacePtr fred_surface(IMG_Load("sprites/fred.png"));
    texture_list.emplace_back(SDL_CreateTextureFromSurface(renderer, fred_surface));
    // red
    texture_list.emplace_back(createTextureAndChangeColor(renderer, fred_surface,
                                                          0xff00ffff, 0xff0000ff));
    // cyan
    texture_list.emplace_back(createTextureAndChangeColor(renderer, fred_surface,
                                                          0xff00ffff, 0xffffff00));
    // green
    texture_list.emplace_back(createTextureAndChangeColor(renderer, fred_surface,
                                                          0xff00ffff, 0xff00ff00));
    // magenta
    texture_list.emplace_back(createTextureAndChangeColor(renderer, fred_surface,
                                                          0xff00ffff, 0xffff00ff));
    // blue
    texture_list.emplace_back(createTextureAndChangeColor(renderer, fred_surface,
                                                          0xff00ffff, 0xffff0000));
    assert(texture_list.size() == static_cast<size_t>(TextureID::COUNT));
}
