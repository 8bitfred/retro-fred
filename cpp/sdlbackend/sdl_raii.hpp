#pragma once

// Internal RAII helpers over the SDL3 C API. Not part of the public interface of
// sdlbackend: only .cpp files inside this library should include this header.

#include <stdexcept>
#include <memory>
#include <functional>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

namespace sdl
{

    class Error : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
        Error() : std::runtime_error(SDL_GetError())
        {
            SDL_ClearError();
        }
    };

    class App
    {
    public:
        App(SDL_InitFlags flags = SDL_INIT_AUDIO | SDL_INIT_VIDEO |
            SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD)
        {
            if (!SDL_Init(flags))
                throw Error();
        }
        ~App() { SDL_Quit(); }
        App(App const &) = delete;
        App &operator=(App const &) = delete;
    };

    template <typename Type, void (*DestroyFn)(Type *)>
    class ObjectPtr
    {
        struct Deleter
        {
            void operator()(Type *p)
            {
                if (p)
                    DestroyFn(p);
            }
        };
        using ptr_type = std::unique_ptr<Type, Deleter>;
        ptr_type ptr;

    public:
        ObjectPtr() = default;
        explicit ObjectPtr(Type *p)
        {
            if (!p)
                throw Error();
            ptr = ptr_type(p);
        }
        operator Type *() const { return ptr.get(); }
        operator bool() const { return static_cast<bool>(ptr); }
        Type *operator->() const { return ptr.get(); }
        Type *get() const { return ptr.get(); }
    };

    using WindowPtr = ObjectPtr<SDL_Window, SDL_DestroyWindow>;
    using RendererPtr = ObjectPtr<SDL_Renderer, SDL_DestroyRenderer>;
    using SurfacePtr = ObjectPtr<SDL_Surface, SDL_DestroySurface>;
    using TexturePtr = ObjectPtr<SDL_Texture, SDL_DestroyTexture>;

    inline std::pair<WindowPtr, RendererPtr>
    createWindowAndRenderer(const char *title, int w, int h, SDL_WindowFlags window_flags = 0)
    {
        SDL_Window *win = nullptr;
        SDL_Renderer *rend = nullptr;
        if (!SDL_CreateWindowAndRenderer(title, w, h, window_flags, &win, &rend))
            throw Error();
        return {WindowPtr(win), RendererPtr(rend)};
    }

    class ColorGuard {
        SDL_Renderer *renderer;
        Uint8 saved_red, saved_green, saved_blue, saved_alpha;
        SDL_BlendMode saved_blend_mode;
    public:
        ColorGuard(SDL_Renderer *renderer,
        Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha,
        SDL_BlendMode blend_mode = SDL_BLENDMODE_NONE)
        : renderer(renderer)
        {
            if (!SDL_GetRenderDrawBlendMode(renderer, &saved_blend_mode))
                throw Error();
            if (!SDL_GetRenderDrawColor(renderer, &saved_red, &saved_green,
                                       &saved_blue, &saved_alpha))
                throw Error();
            SDL_SetRenderDrawBlendMode(renderer, blend_mode);
            SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
        }
        ~ColorGuard()
        {
            SDL_SetRenderDrawBlendMode(renderer, saved_blend_mode);
            SDL_SetRenderDrawColor(renderer, saved_red, saved_green, saved_blue, saved_alpha);
        }
    };

    using MixAudioPtr = ObjectPtr<MIX_Audio, MIX_DestroyAudio>;

    // Wraps a MIX_Mixer opened on the default playback device, and the handful of
    // mixer-wide operations SoundManager needs (everything in this game is played as a
    // one-shot, fire-and-forget sound via MIX_PlayAudio -- nothing uses explicit tracks).
    class AudioDevice
    {
        MIX_Mixer *mixer;

    public:
        AudioDevice()
        {
            if (!MIX_Init())
                throw Error();
            mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
            if (!mixer)
                throw Error();
        }
        AudioDevice(AudioDevice const &) = delete;
        AudioDevice(AudioDevice &&other) = delete;
        ~AudioDevice() noexcept
        {
            MIX_DestroyMixer(mixer);
            MIX_Quit();
        }
        AudioDevice &operator=(AudioDevice const &) = delete;
        AudioDevice &operator=(AudioDevice &&other) = delete;

        MixAudioPtr loadAudio(char const *path)
        {
            return MixAudioPtr(MIX_LoadAudio(mixer, path, /*predecode=*/true));
        }
        bool play(MIX_Audio *audio)
        {
            auto ok = MIX_PlayAudio(mixer, audio);
            if (!ok) {
                SDL_Log("sdl::AudioDevice::play: failed to play sound: %s", SDL_GetError());
                SDL_ClearError();
            }
            return ok;
        }
        Uint32 getDurationMS(MIX_Audio *audio) const noexcept
        {
            auto frames = MIX_GetAudioDuration(audio);
            if (frames < 0)
                return 0;
            return static_cast<Uint32>(MIX_AudioFramesToMS(audio, frames));
        }
        void pause(bool pause_on) noexcept
        {
            if (pause_on)
                MIX_PauseAllTracks(mixer);
            else
                MIX_ResumeAllTracks(mixer);
        }
        void clearQueuedAudio() noexcept { MIX_StopAllTracks(mixer, 0); }
    };

    using JoystickPtr = ObjectPtr<SDL_Joystick, SDL_CloseJoystick>;
    using GamepadPtr = ObjectPtr<SDL_Gamepad, SDL_CloseGamepad>;
} // namespace sdl
