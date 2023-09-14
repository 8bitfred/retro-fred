#pragma once

#include <vector>
#include "sdl.hpp"
#include "TextureID.hpp"

class TextureManager
{
    std::vector<sdl::TexturePtr> texture_list;

public:
    explicit TextureManager(SDL_Renderer *renderer);
    SDL_Texture *get(TextureID texture_id) const
    {
        return texture_list[static_cast<size_t>(texture_id)];
    }

    // TODO: reconsider this API: perhaps the color should be a struct, and perhaps the
    // coordinates should be a struct too.
    void renderText(SDL_Renderer *renderer, std::string_view text,
                    int x, int y, Uint8 red, Uint8 green, Uint8 blue) const;
    void renderScore(SDL_Renderer *renderer, unsigned score,
                     int x, int y, Uint8 red, Uint8 green, Uint8 blue) const;
};
