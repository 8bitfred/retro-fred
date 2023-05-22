#pragma once

#include <stdexcept>
#include <memory>
#include <functional>

#include <SDL2/SDL.h>

namespace sdl
{

    class error : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
        error() : std::runtime_error(SDL_GetError())
        {
            SDL_ClearError();
        }
    };

    class app
    {
    public:
        app(Uint32 flags = SDL_INIT_EVERYTHING)
        {
            if (SDL_Init(flags) < 0)
                throw error();
        }
        ~app() { SDL_Quit(); }
        app(app const &) = delete;
        app &operator=(app const &) = delete;
    };

    template <typename Type, void (*DestroyFn)(Type *)>
    class object_ptr
    {
        struct deleter
        {
            void operator()(Type *p)
            {
                if (p)
                    DestroyFn(p);
            }
        };
        using ptr_type = std::unique_ptr<Type, deleter>;
        ptr_type ptr;

    public:
        explicit object_ptr(Type *p)
        {
            if (!p)
                throw error();
            ptr = ptr_type(p);
        }
        operator Type *() { return ptr.get(); }
    };

    class window : public object_ptr<SDL_Window, SDL_DestroyWindow>
    {
    public:
        using object_ptr::object_ptr;
        window(const char *title, int x, int y, int w, int h, Uint32 flags = 0)
            : object_ptr::object_ptr(SDL_CreateWindow(title, x, y, w, h, flags)) {}
    };

    class renderer : public object_ptr<SDL_Renderer, SDL_DestroyRenderer>
    {
    public:
        using object_ptr::object_ptr;
        renderer(window w, int index = -1, Uint32 flags = 0)
            : object_ptr::object_ptr(SDL_CreateRenderer(w, index, flags)) {}
    };

    using surface = object_ptr<SDL_Surface, SDL_FreeSurface>;
    using texture = object_ptr<SDL_Texture, SDL_DestroyTexture>;

    inline std::pair<window, renderer>
    create_window_and_renderer(int w, int h, Uint32 window_flags = 0)
    {
        SDL_Window *win = nullptr;
        SDL_Renderer *rend = nullptr;
        if (SDL_CreateWindowAndRenderer(w, h, window_flags, &win, &rend) < 0)
            throw error();
        return {window(win), renderer(rend)};
    }

} // namespace sdl
