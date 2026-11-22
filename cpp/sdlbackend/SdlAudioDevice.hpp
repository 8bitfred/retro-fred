#pragma once

#include "platform/AudioDevice.hpp"
#include "sdl_raii.hpp"

class SdlSound : public Sound
{
    sdl::MixAudioPtr audio;

public:
    explicit SdlSound(sdl::MixAudioPtr audio) : audio(std::move(audio)) {}
    MIX_Audio *get() const { return audio; }
};

class SdlAudioDevice : public AudioDevice
{
    sdl::AudioDevice audio_device;

public:
    SdlAudioDevice() = default;

    std::unique_ptr<Sound> loadSound(std::string const &path) override
    {
        return std::make_unique<SdlSound>(audio_device.loadAudio(path.c_str()));
    }
    // loops is unused: nothing in this game ever plays a sound on a loop.
    int play(Sound const &sound, int /*loops*/) override
    {
        return audio_device.play(static_cast<SdlSound const &>(sound).get()) ? 0 : -1;
    }
    std::uint32_t getDuration(Sound const &sound) const override
    {
        return audio_device.getDurationMS(static_cast<SdlSound const &>(sound).get());
    }
    void pause(bool pause_on) override { audio_device.pause(pause_on); }
    void clearQueuedAudio() override { audio_device.clearQueuedAudio(); }
};
