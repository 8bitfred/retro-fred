#include "TextureManager.hpp"
#include <SDL2/SDL_image.h>

TextureManager::TextureManager(SDL_Renderer *renderer)
{
    // This list must match the order of elements in the TextureID enum
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/stone_1x_1.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/stone_1x_2.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/stone_1x_3.png"));
    texture_list.emplace_back(IMG_LoadTexture(renderer, "sprites/fred_1x_01.png"));
}
