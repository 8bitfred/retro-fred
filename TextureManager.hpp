#pragma once

#include <vector>
#include "sdl.hpp"

class TextureManager
{
public:
    enum class TextureID
    {
        stone1,
        stone2,
        stone3,
        fred,
        count
    };
    explicit TextureManager(SDL_Renderer *renderer);
    SDL_Texture *get(TextureID texture_id) const
    {
        return texture_list[static_cast<size_t>(texture_id)];
    }

private:
    std::vector<sdl::TexturePtr> texture_list;
};
