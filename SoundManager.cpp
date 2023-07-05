#include "SoundManager.hpp"
#include <cassert>

std::vector<sdl::WAVData> SoundManager::loadWAVs()
{
    std::vector<sdl::WAVData> wav_list;
    wav_list.emplace_back("sounds/walk.wav");
    wav_list.emplace_back("sounds/jump.wav");
    wav_list.emplace_back("sounds/climb_1.wav");
    wav_list.emplace_back("sounds/climb_2.wav");
    assert(wav_list.size() == static_cast<size_t>(SoundID::COUNT));
    return wav_list;
}

SoundManager::SoundManager()
    : wav_list(loadWAVs()), audio_device(nullptr, false, wav_list.front().getSpec(), nullptr, 0)
{
    audio_device.pause(false);
}

void SoundManager::play(SoundID sound_id)
{
    audio_device.queueAudio(wav_list[static_cast<size_t>(sound_id)]);
}
