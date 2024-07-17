#include "SoundManager.hpp"
#include "Config.hpp"

void SoundManager::loadWAVs(Config const &cfg)
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
    for (auto p : wav_files) {
        auto path = cfg.resource_path / p;
        wav_list.emplace_back(Mix_LoadWAV(path.string().c_str()));
    }
}

SoundManager::SoundManager(Config const &cfg)
    : audio_device(256)
{
    audio_device.pause(false);
    loadWAVs(cfg);
}

void SoundManager::play(SoundID sound_id)
{
    audio_device.playChannel(wav_list[static_cast<size_t>(sound_id)]);
}
