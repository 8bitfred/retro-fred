#pragma once

#include "fredcore/Renderer.hpp"
#include "sdl_raii.hpp"

class SdlTexture : public Texture
{
    sdl::TexturePtr texture;

public:
    explicit SdlTexture(sdl::TexturePtr texture) : texture(std::move(texture)) {}
    SDL_Texture *get() const { return texture; }
};
