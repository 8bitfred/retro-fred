#include "SoundManager.hpp"
#include "Config.hpp"

std::vector<sdl::WAVData> SoundManager::loadWAVs(Config const &cfg)
{
    static char const *wav_files[] = {
        "sounds/pick_object.wav",
        "sounds/collision.wav",
        "sounds/fire.wav",
        "sounds/climbtak.wav",
        "sounds/climbkok.wav",
        "sounds/jump.wav",
        "sounds/walk.wav",
        "sounds/exit_maze.wav",
        "sounds/game_over.wav",
        "sounds/funeral_march.wav",
        "sounds/loading1.wav",
        "sounds/loading2.wav",
        "sounds/loading3.wav",
        "sounds/loading4.wav",
    };
    static_assert(std::size(wav_files) == static_cast<size_t>(SoundID::COUNT));
    std::vector<sdl::WAVData> wav_list;
    for (auto p : wav_files) {
        auto path = cfg.resource_path / p;
        wav_list.emplace_back(path.string().c_str());
    }
    return wav_list;
}

SDL_AudioSpec SoundManager::initAudioSpec(sdl::WAVData const &wav_data)
{
    SDL_AudioSpec audio_spec = *wav_data.getSpec();
    audio_spec.samples = 256;
    return audio_spec;
}

SoundManager::SoundManager(Config const &cfg)
    : wav_list(loadWAVs(cfg)), audio_spec(initAudioSpec(wav_list.front()))
    , audio_device(nullptr, false, &audio_spec, nullptr, 0)
{
    audio_device.pause(false);
}

void SoundManager::play(SoundID sound_id)
{
    audio_device.queueAudio(wav_list[static_cast<size_t>(sound_id)]);
}

sdl::WAVData const &SoundManager::get(SoundID sound_id) const
{
    return wav_list[static_cast<size_t>(sound_id)];
}