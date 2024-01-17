#pragma once

#include <vector>
#include "sdl.hpp"
#include "SoundID.hpp"

struct Config;

class SoundManager
{
    std::vector<sdl::WAVData> wav_list;
    SDL_AudioSpec audio_spec;
    sdl::AudioDevice audio_device;

    static std::vector<sdl::WAVData> loadWAVs(Config const &cfg);
    static SDL_AudioSpec initAudioSpec(sdl::WAVData const &wav_data);

public:
    explicit SoundManager(Config const &cfg);
    void play(SoundID sound_id);
};
