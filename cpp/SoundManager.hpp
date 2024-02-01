#pragma once

#include <vector>
#include <array>
#include <optional>
#include "sdl.hpp"
#include "SoundID.hpp"

struct Config;
class SoundManager;

class AudioCallbackData
{
    static constexpr size_t SIZE = 16;
    SDL_mutex *mutex;
    size_t head = 0, tail = 0;
    struct SoundRequest {
        SoundID sound_id;
        Uint32 time;
    };
    std::array<SoundRequest, SIZE> data;
    SoundManager const &smgr;
    SDL_AudioSpec device_spec;
    Uint8 const *audio_data = nullptr;
    Uint32 audio_len = 0;
    Uint32 audio_pos = 0;
    Uint32 timestamp = 0;

public:
    explicit AudioCallbackData(SoundManager const &smgr)
    : mutex(SDL_CreateMutex()), smgr(smgr) {}
    ~AudioCallbackData() { SDL_DestroyMutex(mutex); }
    AudioCallbackData(AudioCallbackData const &) = delete;
    AudioCallbackData &operator=(AudioCallbackData const &) = delete;
    SDL_AudioSpec *getDeviceSpec() { return &device_spec; }
    bool add(SoundID element);
    std::optional<SoundID> get();
    void getAudioData();
    void clearQueue();
    static void audio_callback(void *userdata, Uint8 *stream, int stream_len);
};

class SoundManager
{
    std::vector<sdl::WAVData> wav_list;
    AudioCallbackData callback_data;
    sdl::AudioDevice audio_device;

    static std::vector<sdl::WAVData> loadWAVs(Config const &cfg);
    static sdl::AudioDevice initAudioDevice(sdl::WAVData const &wav_data,
                                            AudioCallbackData *callback_data);

public:
    explicit SoundManager(Config const &cfg);
    void play(SoundID sound_id);
    sdl::WAVData const &get(SoundID sound_id) const;
    void clearQueuedAudio() { callback_data.clearQueue(); }
};
