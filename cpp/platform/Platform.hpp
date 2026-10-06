#pragma once

#include "Renderer.hpp"
#include "AudioDevice.hpp"
#include "GameEvent.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

// Facade for platform/backend services that don't belong to a particular renderer or
// audio device: ticks, preferences path, display resolution, and construction of the
// other abstract interfaces. One Platform is constructed at application startup.
class Platform
{
public:
    virtual ~Platform() = default;

    virtual std::uint32_t getTicks() const = 0;
    virtual std::string getPrefPath(std::string const &org, std::string const &app) const = 0;
    virtual std::pair<int, int> getDisplayResolution() const = 0;

    virtual std::unique_ptr<Renderer> createWindowAndRenderer(int width, int height,
                                                               WindowFlags flags) = 0;
    virtual std::unique_ptr<Renderer> createOffscreenRenderer(int width, int height) = 0;
    virtual std::unique_ptr<AudioDevice> createAudioDevice() = 0;
    virtual std::unique_ptr<EventSource> createEventSource(std::uint32_t ticks_per_frame) = 0;
};
