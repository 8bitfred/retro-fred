#pragma once

#include "fredcore/Renderer.hpp"
#include "sdl_raii.hpp"

class SdlSurface : public Surface
{
    sdl::SurfacePtr surface;

public:
    explicit SdlSurface(sdl::SurfacePtr surface) : surface(std::move(surface)) {}
    SDL_Surface *get() const { return surface; }
    void savePNG(std::string const &path) const override
    {
        if (IMG_SavePNG(surface, path.c_str()) < 0)
            throw sdl::Error();
    }
};
