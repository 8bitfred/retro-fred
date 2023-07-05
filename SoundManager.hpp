#pragma once

#include <vector>
#include "sdl.hpp"
#include "SoundID.hpp"

class SoundManager
{
    std::vector<sdl::WAVData> wav_list;
    sdl::AudioDevice audio_device;

    static std::vector<sdl::WAVData> loadWAVs();

public:
    SoundManager();
    void play(SoundID sound_id);
};
