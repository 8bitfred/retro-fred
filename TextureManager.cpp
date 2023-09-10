#include "TextureManager.hpp"
#include <SDL_image.h>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <map>

TextureManager::TextureManager(SDL_Renderer *renderer)
{
    // This list must match the order of elements in the TextureID enum
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/window.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/block.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/rat.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/ghost.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/chameleon.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/mummy.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/vampire.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/skeleton.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/bullet.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/smoke.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred.png"));
    assert(texture_list.size() == static_cast<size_t>(TextureID::COUNT));
}
