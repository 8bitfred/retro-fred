#pragma once

#include "platform/Platform.hpp"
#include "sdl_raii.hpp"

class SdlPlatform : public Platform
{
    // Constructed before `app` (declaration order) so the hint is set before SDL_Init runs.
    struct HintSetter
    {
        HintSetter() { SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, "0"); }
    };
    HintSetter hint_setter;
    sdl::App app;

public:
    SdlPlatform() = default;

    std::uint32_t getTicks() const override;
    std::string getPrefPath(std::string const &org, std::string const &app) const override;
    std::pair<int, int> getDisplayResolution() const override;

    std::unique_ptr<Renderer> createWindowAndRenderer(int width, int height,
                                                       WindowFlags flags) override;
    std::unique_ptr<Renderer> createOffscreenRenderer(int width, int height) override;
    std::unique_ptr<AudioDevice> createAudioDevice() override;
    std::unique_ptr<EventSource> createEventSource(std::uint32_t ticks_per_frame) override;
};
