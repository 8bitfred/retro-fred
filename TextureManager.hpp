#pragma once

#include <vector>
#include "sdl.hpp"
#include "TextureID.hpp"

class TextureManager
{
    std::vector<sdl::TexturePtr> texture_list;
    sdl::TexturePtr createTextureAndChangeColor(SDL_Renderer *renderer, SDL_Surface *src,
                                                Uint32 in_color, Uint32 out_color);

public:
    explicit TextureManager(SDL_Renderer *renderer);
    SDL_Texture *get(TextureID texture_id) const
    {
        return texture_list[static_cast<size_t>(texture_id)];
    }
};
