#pragma once

#include <vector>
#include <memory>
#include <string_view>
#include "platform/Renderer.hpp"
#include "TextureID.hpp"

struct Config;

class TextureManager
{
    std::vector<std::unique_ptr<Texture>> texture_list;
    std::unique_ptr<Surface> fred_icon;

public:
    TextureManager(Config const &cfg, Renderer &renderer);
    Texture &get(TextureID texture_id) const
    {
        return *texture_list[static_cast<size_t>(texture_id)];
    }

    Surface const &getFredIcon() const { return *fred_icon; }

    void renderText(Renderer &renderer, std::string_view text,
                    int x, int y, Color const &color) const;
    void renderScore(Renderer &renderer, unsigned score,
                     int x, int y, Color const &color) const;
};
