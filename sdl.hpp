#pragma once

#include <stdexcept>
#include <memory>
#include <functional>

#include <SDL.h>

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

    inline std::pair<WindowPtr, RendererPtr>
    createWindowAndRenderer(int w, int h, Uint32 window_flags = 0)
    {
        SDL_Window *win = nullptr;
        SDL_Renderer *rend = nullptr;
        if (SDL_CreateWindowAndRenderer(w, h, window_flags, &win, &rend) < 0)
            throw Error();
        return {WindowPtr(win), RendererPtr(rend)};
    }

    class WAVData {
        SDL_AudioSpec spec;
        Uint8* audio_buf;
        Uint32 audio_len;
    public:
        explicit WAVData(const char* file)
        {
            auto result = SDL_LoadWAV(file, &spec, &audio_buf, &audio_len);
            if (!result)
                throw Error();
        }
        WAVData(WAVData const &) = delete;
        WAVData(WAVData &&other) noexcept
            : spec(other.spec), audio_buf(other.audio_buf), audio_len(other.audio_len)
        {
            other.audio_buf = nullptr;
            other.audio_len = 0;
        }
        ~WAVData() noexcept { SDL_FreeWAV(audio_buf); }
        WAVData &operator=(WAVData const &) = delete;
        WAVData &operator=(WAVData&& other) noexcept
        {
            spec = other.spec;
            audio_buf = other.audio_buf;
            audio_len = other.audio_len;
            other.audio_buf = nullptr;
            other.audio_len = 0;
            return *this;
        }

        SDL_AudioSpec const* getSpec() const noexcept { return &spec; }
        Uint8 const* getBuf() const noexcept { return audio_buf; }
        Uint32 getLen() const noexcept { return audio_len; }
    };

    class AudioDevice
    {
        SDL_AudioDeviceID device_id;

    public:
        AudioDevice(char const *device, bool iscapture,
                    SDL_AudioSpec const *desired, SDL_AudioSpec *obtained,
                    int allowed_changes)
        {
            device_id = SDL_OpenAudioDevice(device, iscapture,
                                            desired, obtained, allowed_changes);
            if (device_id == 0)
                throw Error();
        }
        AudioDevice(AudioDevice const &) = delete;
        AudioDevice(AudioDevice &&other) noexcept : device_id(other.device_id) { other.device_id = 0; }
        ~AudioDevice() noexcept
        {
            if (device_id)
                SDL_CloseAudioDevice(device_id);
        }
        AudioDevice &operator=(AudioDevice const &) = delete;
        AudioDevice &operator=(AudioDevice &&other) noexcept
        {
            device_id = other.device_id;
            other.device_id = 0;
            return *this;
        }
        void queueAudio(WAVData const& wav_data)
        {
            auto status = SDL_QueueAudio(device_id, wav_data.getBuf(), wav_data.getLen());
            if (status < 0)
                throw Error();
        }
        void pause(bool pause_on) noexcept { SDL_PauseAudioDevice(device_id, pause_on); }
    };

    class RenderTargetGuard
    {
        SDL_Renderer *renderer;
        SDL_Texture *previous_target;

    public:
        RenderTargetGuard(SDL_Renderer *renderer, SDL_Texture *texture)
        : renderer(renderer)
        , previous_target(SDL_GetRenderTarget(renderer))
        {
            auto status = SDL_SetRenderTarget(renderer, texture);
            if (status < 0)
                throw Error();
        }
        RenderTargetGuard(RenderTargetGuard const &) = delete;
        RenderTargetGuard &operator=(RenderTargetGuard const &) = delete;
        ~RenderTargetGuard()
        {
            SDL_SetRenderTarget(renderer, previous_target);
        }
    };

} // namespace sdl
