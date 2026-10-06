#include "SdlPlatform.hpp"
#include "SdlRenderer.hpp"
#include "SdlAudioDevice.hpp"
#include "SdlEventSource.hpp"

namespace
{
    Uint32 toSdlWindowFlags(WindowFlags flags)
    {
        Uint32 result = 0;
        if (flags & WindowFlags::HIGH_DPI)
            result |= SDL_WINDOW_ALLOW_HIGHDPI;
        if (flags & WindowFlags::RESIZABLE)
            result |= SDL_WINDOW_RESIZABLE;
        if (flags & WindowFlags::BORDERLESS)
            result |= SDL_WINDOW_BORDERLESS;
        if (flags & WindowFlags::FULLSCREEN_DESKTOP)
            result |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        return result;
    }
}

std::uint32_t SdlPlatform::getTicks() const
{
    return SDL_GetTicks();
}

std::string SdlPlatform::getPrefPath(std::string const &org, std::string const &app_name) const
{
    auto pref_path_ptr = SDL_GetPrefPath(org.c_str(), app_name.c_str());
    std::string pref_path_str(pref_path_ptr);
    SDL_free(pref_path_ptr);
    return pref_path_str;
}

std::pair<int, int> SdlPlatform::getDisplayResolution() const
{
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    return {display_mode.w, display_mode.h};
}

std::unique_ptr<Renderer> SdlPlatform::createWindowAndRenderer(int width, int height,
                                                                WindowFlags flags)
{
    auto [win, rend] = sdl::createWindowAndRenderer(width, height, toSdlWindowFlags(flags));
    return std::make_unique<SdlRenderer>(std::move(win), std::move(rend));
}

std::unique_ptr<Renderer> SdlPlatform::createOffscreenRenderer(int width, int height)
{
    sdl::SurfacePtr surface(SDL_CreateRGBSurfaceWithFormat(0, width, height, 8,
                                                           SDL_PIXELFORMAT_RGBA8888));
    sdl::RendererPtr renderer(SDL_CreateSoftwareRenderer(surface));
    return std::make_unique<SdlRenderer>(std::move(surface), std::move(renderer));
}

std::unique_ptr<AudioDevice> SdlPlatform::createAudioDevice()
{
    return std::make_unique<SdlAudioDevice>();
}

std::unique_ptr<EventSource> SdlPlatform::createEventSource(std::uint32_t ticks_per_frame)
{
    return std::make_unique<SdlEventSource>(ticks_per_frame);
}
