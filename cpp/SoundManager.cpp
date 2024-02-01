#include "SoundManager.hpp"
#include "Config.hpp"

bool AudioCallbackData::add(SoundID element)
{
    SDL_LockMutex(mutex);
    auto next = (tail + 1) % SIZE;
    bool avail = next != head;
    if (avail)
    {
        data[tail] = SoundRequest{element, SDL_GetTicks()};
        tail = next;
    }
    SDL_UnlockMutex(mutex);
    return avail;
}

std::optional<SoundID> AudioCallbackData::get()
{
    static Uint32 max_delay = 0;
    std::optional<SoundID> result;
    SDL_LockMutex(mutex);
    if (head != tail)
    {
        result = data[head].sound_id;
        auto delay = SDL_GetTicks() - data[head].time;
        max_delay = std::max(max_delay, delay);
        SDL_Log("delay=%u max=%u", delay, max_delay);
        head = (head + 1) % SIZE;
    }
    SDL_UnlockMutex(mutex);
    return result;
}

void AudioCallbackData::getAudioData()
{
    auto sound_id = get();
    if (sound_id)
    {
        auto const &wav_data = smgr.get(*sound_id);
        audio_data = wav_data.getBuf();
        audio_len = wav_data.getLen();
    }
}

void AudioCallbackData::clearQueue()
{
    // TODO: we need to lock the mutex before we clear the queue, and set audio_buf and
    // audio_len. We also need to expand the code covered by the lock in the callback
    // function
}


void AudioCallbackData::audio_callback(void *userdata, Uint8 *stream, int stream_len)
{
    auto cd = reinterpret_cast<AudioCallbackData *>(userdata);
    auto now = SDL_GetTicks();
    //SDL_Log("delta_t=%u", now - cd->timestamp);
    cd->timestamp = now;
    while (stream_len > 0)
    {
        if (!cd->audio_data || cd->audio_len == 0)
        {
            cd->getAudioData();
            if (!cd->audio_data)
            {
                SDL_memset(stream, cd->device_spec.silence, stream_len);
                stream += stream_len;
                stream_len -= stream_len;
                break;
            }
            //SDL_Log("delta_t=%u", now - cd->timestamp);
            cd->timestamp = now;
        }
        auto remaining = cd->audio_len - cd->audio_pos;
        auto len = std::min(static_cast<Uint32>(stream_len), remaining);
        SDL_memcpy(stream, cd->audio_data + cd->audio_pos, len);
        //SDL_Log("audio_len=%u audio_pos=%u stream_len=%d len=%u",
        //        cd->audio_len, cd->audio_pos, stream_len, len);
        stream += len;
        stream_len -= len;
        cd->audio_pos += len;
        if (cd->audio_pos >= cd->audio_len)
        {
            cd->audio_data = nullptr;
            cd->audio_len = 0;
            cd->audio_pos = 0;
        }
    }
}

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

sdl::AudioDevice SoundManager::initAudioDevice(sdl::WAVData const &wav_data,
                                               AudioCallbackData *callback_data)
{
    SDL_AudioSpec audio_spec = *wav_data.getSpec();
    audio_spec.samples = 64;
    audio_spec.callback = AudioCallbackData::audio_callback;
    audio_spec.userdata = callback_data;
    return sdl::AudioDevice(nullptr, false, &audio_spec,
                            callback_data->getDeviceSpec(), 0);
}

SoundManager::SoundManager(Config const &cfg)
    : wav_list(loadWAVs(cfg))
    , callback_data(*this)
    , audio_device(initAudioDevice(wav_list.front(), &callback_data))
{
    audio_device.pause(false);
}

void SoundManager::play(SoundID sound_id)
{
    SDL_Log("play: %d", static_cast<int>(sound_id));
    callback_data.add(sound_id);
    //audio_device.queueAudio(wav_list[static_cast<size_t>(sound_id)]);
}

sdl::WAVData const &SoundManager::get(SoundID sound_id) const
{
    return wav_list[static_cast<size_t>(sound_id)];
}
