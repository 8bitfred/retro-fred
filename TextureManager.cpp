#include "TextureManager.hpp"
#include <SDL_image.h>
#include <cassert>

TextureManager::TextureManager(SDL_Renderer *renderer)
{
    // This list must match the order of elements in the TextureID enum
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/frame.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/stone_1x_1.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/stone_1x_2.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/stone_1x_3.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/ropestartsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/ropemainsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/ropeendsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/skysprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/sandsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/trapdoorsprite_1x.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_1.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_2.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_3.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_4.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_5.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_6.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_7.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_8.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_9.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_10.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/aciddrop_seq_11.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_01.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_02.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_03.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_04.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_05.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_06.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_07.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_08.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_09.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_10.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_11.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_12.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_13.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_14.png"));
    assert(texture_list.size() == static_cast<size_t>(TextureID::COUNT));
}
