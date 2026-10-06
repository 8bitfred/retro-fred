#pragma once

#include "fredcore/AudioDevice.hpp"
#include "sdl_raii.hpp"

class SdlSound : public Sound
{
    sdl::MixChunkPtr chunk;

public:
    explicit SdlSound(sdl::MixChunkPtr chunk) : chunk(std::move(chunk)) {}
    Mix_Chunk *get() const { return chunk; }
};

class SdlAudioDevice : public AudioDevice
{
    sdl::AudioDevice audio_device;

public:
    SdlAudioDevice() : audio_device(256) { audio_device.pause(false); }

    std::unique_ptr<Sound> loadSound(std::string const &path) override
    {
        return std::make_unique<SdlSound>(sdl::MixChunkPtr(Mix_LoadWAV(path.c_str())));
    }
    int play(Sound const &sound, int loops) override
    {
        return audio_device.playChannel(static_cast<SdlSound const &>(sound).get(), -1, loops);
    }
    std::uint32_t getDuration(Sound const &sound) const override
    {
        return audio_device.getDuration(static_cast<SdlSound const &>(sound).get());
    }
    void pause(bool pause_on) override { audio_device.pause(pause_on); }
    void clearQueuedAudio() override { audio_device.clearQueuedAudio(); }
};
