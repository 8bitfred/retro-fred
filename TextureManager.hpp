#pragma once

#include <vector>
#include "sdl.hpp"
#include "TextureID.hpp"

class Config;

class TextureManager
{
    std::vector<sdl::TexturePtr> texture_list;

    static sdl::TexturePtr createFrameTexture(Config const &cfg,
                                              SDL_Renderer *renderer,
                                              SDL_Texture *base_frame);

public:
    TextureManager(Config const& cfg, SDL_Renderer *renderer);
    SDL_Texture *get(TextureID texture_id) const
    {
        return texture_list[static_cast<size_t>(texture_id)];
    }
};
