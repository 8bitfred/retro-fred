#pragma once

#include <stdexcept>
#include <memory>
#include <functional>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

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
        App(Uint32 flags = SDL_INIT_EVERYTHING)
        {
            if (SDL_Init(flags) < 0)
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
        operator bool() const { return ptr; }
        Type *operator->() const { return ptr.get(); }
        Type *get() const { return ptr.get(); }
    };

    class WindowPtr : public ObjectPtr<SDL_Window, SDL_DestroyWindow>
    {
    public:
        using ObjectPtr::ObjectPtr;
        WindowPtr(const char *title, int x, int y, int w, int h, Uint32 flags = 0)
            : ObjectPtr::ObjectPtr(SDL_CreateWindow(title, x, y, w, h, flags)) {}
    };

    class RendererPtr : public ObjectPtr<SDL_Renderer, SDL_DestroyRenderer>
    {
    public:
        using ObjectPtr::ObjectPtr;
        RendererPtr(WindowPtr w, int index = -1, Uint32 flags = 0)
            : ObjectPtr::ObjectPtr(SDL_CreateRenderer(w, index, flags)) {}
    };

    using SurfacePtr = ObjectPtr<SDL_Surface, SDL_FreeSurface>;
    using TexturePtr = ObjectPtr<SDL_Texture, SDL_DestroyTexture>;

    class LockedSurfacePtr : public ObjectPtr<SDL_Surface, SDL_UnlockSurface>
    {
    public:
        LockedSurfacePtr() = default;
        explicit LockedSurfacePtr(SDL_Surface *p): ObjectPtr(p)
        {
            if (SDL_LockSurface(p) < 0)
                throw Error();
        }
    };

    inline std::pair<WindowPtr, RendererPtr>
    createWindowAndRenderer(int w, int h, Uint32 window_flags = 0)
    {
        SDL_Window *win = nullptr;
        SDL_Renderer *rend = nullptr;
        if (SDL_CreateWindowAndRenderer(w, h, window_flags, &win, &rend) < 0)
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
            if (SDL_GetRenderDrawBlendMode(renderer, &saved_blend_mode) < 0)
                throw Error();
            if (SDL_GetRenderDrawColor(renderer, &saved_red, &saved_green,
                                       &saved_blue, &saved_alpha) < 0)
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

    using MixChunkPtr = ObjectPtr<Mix_Chunk, Mix_FreeChunk>;

    class AudioDevice
    {
        int audio_frequency;
        Uint16 audio_format;
        int audio_channels;

    public:
        AudioDevice(int chunksize = 1024,
                    int frequency = MIX_DEFAULT_FREQUENCY,
                    Uint16 format = MIX_DEFAULT_FORMAT,
                    int channels = MIX_DEFAULT_CHANNELS)
        {
            // NOTE: if we actually want to choose a device (instead of the default) we
            // need to call Mix_OpenAudioDevice
            if (Mix_OpenAudio(frequency, format, channels, chunksize) < 0)
                throw Error();
            Mix_QuerySpec(&audio_frequency, &audio_format, &audio_channels);
        }
        AudioDevice(AudioDevice const &) = delete;
        AudioDevice(AudioDevice &&other) = delete;
        ~AudioDevice() noexcept
        {
            Mix_CloseAudio();
        }
        AudioDevice &operator=(AudioDevice const &) = delete;
        AudioDevice &operator=(AudioDevice &&other) = delete;
        int playChannel(Mix_Chunk *chunk, int channel = -1, int loops = 0)
        {
            auto out_channel = Mix_PlayChannel(channel, chunk, loops);
            if (out_channel < 0)
                throw Error();
            return out_channel;
        }
        Uint32 getDuration(Mix_Chunk *chunk) const noexcept
        {
            auto sample_size = SDL_AUDIO_BITSIZE(audio_format) / 8;
            auto sample_count = chunk->alen / (sample_size * audio_channels);
            return 1000 * sample_count / audio_frequency;

        }
        void pause(bool pause_on) noexcept { Mix_PauseAudio(static_cast<int>(pause_on)); }
        void clearQueuedAudio() noexcept { Mix_HaltChannel(-1); }
    };

} // namespace sdl
