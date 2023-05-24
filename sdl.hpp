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

} // namespace sdl
