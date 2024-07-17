#pragma once

#include <vector>
#include "sdl.hpp"
#include "SoundID.hpp"

struct Config;

class SoundManager
{
    std::vector<sdl::MixChunkPtr> wav_list;
    sdl::AudioDevice audio_device;

    void loadWAVs(Config const &cfg);

public:
    explicit SoundManager(Config const &cfg);
    void play(SoundID sound_id);
    Uint32 getDuration(SoundID sound_id) const
    {
        auto chunk = wav_list[static_cast<size_t>(sound_id)].get();
        return audio_device.getDuration(chunk);
    }
    void clearQueuedAudio() { audio_device.clearQueuedAudio(); }
};
