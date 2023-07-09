#include <SDL.h>
#include <vector>
#include <iostream>

struct AudioData {
    Uint8* audio_buf = nullptr;
    Uint32 audio_len = 0;
    Uint32 ptr = 0;
    int device_id;
};

void audioCallback(void *user_data, Uint8 *stream, int len)
{
    auto audio_data = reinterpret_cast<AudioData *>(user_data);
    auto ticks = SDL_GetTicks();
    static auto prev_ticks = ticks;
    std::cout
        << "callback at " << ticks - prev_ticks
        << " len=" << len << std::endl;
    prev_ticks = ticks;
    std::fill(stream, stream + len, 0);
    return;

    auto end = audio_data->ptr + len;
    if (end > audio_data->audio_len)
        end = audio_data->audio_len;
    auto end_ptr = std::copy(audio_data->audio_buf + audio_data->ptr,
                             audio_data->audio_buf + end, stream);
    std::fill(end_ptr, stream + len, 0);
    audio_data->ptr = end;
}

int main()
{
    SDL_Init(SDL_INIT_AUDIO);

    auto num_audio_drivers = SDL_GetNumAudioDrivers();
    std::cout << "num audio drivers: " << num_audio_drivers << std::endl;
    std::cout << "num audio devices: " << SDL_GetNumAudioDevices(0) << std::endl;
    std::cout << "current driver: " << SDL_GetCurrentAudioDriver() << std::endl;

    SDL_AudioSpec spec;
    AudioData audio_data;
    SDL_LoadWAV("/usr/share/sounds/sound-icons/percussion-10.wav", &spec, &audio_data.audio_buf, &audio_data.audio_len);
    //spec.callback = audioCallback;
    //spec.userdata = &audio_data;
    std::cout << "freq=" << spec.freq
              << " format=" << spec.format
              << " channels=" << static_cast<int>(spec.channels)
              << " silence=" << static_cast<int>(spec.silence)
              << " samples=" << spec.samples
              << " size=" << spec.size
              << std::endl;
    spec.samples = 256;

    SDL_AudioSpec device_spec;
    auto device_id = SDL_OpenAudioDevice(nullptr, 0, &spec, &device_spec, 0);
    audio_data.device_id = device_id;
    auto name = SDL_GetAudioDeviceName(device_id, 0);
    if (name)
        std::cout << "audio device name: " << name << std::endl;
    std::cout << "freq=" << device_spec.freq
              << " format=" << device_spec.format
              << " channels=" << static_cast<int>(device_spec.channels)
              << " silence=" << static_cast<int>(device_spec.silence)
              << " samples=" << device_spec.samples
              << " size=" << device_spec.size
              << std::endl;
    std::vector<Uint32> tick_times(20, 0);
    for (auto &ticks : tick_times)
    {
        ticks = SDL_GetTicks();
        SDL_QueueAudio(device_id, audio_data.audio_buf, audio_data.audio_len);
        SDL_PauseAudioDevice(device_id, 0);
        SDL_Delay(200);
        SDL_PauseAudioDevice(device_id, 1);
        SDL_ClearQueuedAudio(device_id);
        //SDL_Delay(100);
    }
    for (auto ticks : tick_times)
        std::cout << ticks - tick_times.front() << std::endl;
    SDL_CloseAudioDevice(device_id);

    SDL_Quit();

    return 0;
}