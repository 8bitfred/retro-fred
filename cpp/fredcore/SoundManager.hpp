#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include "AudioDevice.hpp"
#include "SoundID.hpp"

struct Config;

class SoundManager
{
    std::vector<std::unique_ptr<Sound>> wav_list;
    std::unique_ptr<AudioDevice> audio_device;

    void loadWAVs(Config const &cfg);

public:
    SoundManager(Config const &cfg, std::unique_ptr<AudioDevice> audio_device);
    void play(SoundID sound_id);
    std::uint32_t getDuration(SoundID sound_id) const
    {
        return audio_device->getDuration(*wav_list[static_cast<size_t>(sound_id)]);
    }
    void clearQueuedAudio() { audio_device->clearQueuedAudio(); }
};
