#include "SoundManager.hpp"
#include <cassert>

std::vector<sdl::WAVData> SoundManager::loadWAVs()
{
    std::vector<sdl::WAVData> wav_list;
    wav_list.emplace_back("sounds/pick_object.wav");
    wav_list.emplace_back("sounds/collision.wav");
    wav_list.emplace_back("sounds/fire.wav");
    wav_list.emplace_back("sounds/climb1.wav");
    wav_list.emplace_back("sounds/climb2.wav");
    wav_list.emplace_back("sounds/jump.wav");
    wav_list.emplace_back("sounds/walk.wav");
    assert(wav_list.size() == static_cast<size_t>(SoundID::COUNT));
    return wav_list;
}

SDL_AudioSpec SoundManager::initAudioSpec(sdl::WAVData const &wav_data)
{
    SDL_AudioSpec audio_spec = *wav_data.getSpec();
    audio_spec.samples = 256;
    return audio_spec;
}

SoundManager::SoundManager()
    : wav_list(loadWAVs()), audio_spec(initAudioSpec(wav_list.front()))
    , audio_device(nullptr, false, &audio_spec, nullptr, 0)
{
    audio_device.pause(false);
}

void SoundManager::play(SoundID sound_id)
{
    audio_device.queueAudio(wav_list[static_cast<size_t>(sound_id)]);
}
