#pragma once

#include <cstdint>
#include <memory>
#include <string>

class Sound
{
public:
    virtual ~Sound() = default;
};

class AudioDevice
{
public:
    virtual ~AudioDevice() = default;

    virtual std::unique_ptr<Sound> loadSound(std::string const &path) = 0;
    virtual int play(Sound const &sound, int loops = 0) = 0;
    virtual std::uint32_t getDuration(Sound const &sound) const = 0;
    virtual void pause(bool pause_on) = 0;
    virtual void clearQueuedAudio() = 0;
};
