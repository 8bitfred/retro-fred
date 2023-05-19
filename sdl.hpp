#include <stdexcept>
#include <memory>
#include <functional>

#include <SDL2/SDL.h>


namespace sdl {

  class error: public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
    error(): std::runtime_error(SDL_GetError()) {
      SDL_ClearError();
    }
  };


  class app {
  public:
    app(Uint32 flags = SDL_INIT_EVERYTHING) {
      if (SDL_Init(flags) < 0)
        throw error();
    }
    ~app() { SDL_Quit(); }
    app(app const&) = delete;
    app& operator=(app const&) = delete;
  };


  template<typename Type, void(*DestroyFn)(Type *)>
  class object_ptr {
    struct deleter {
      void operator()(Type *p) { if (p) DestroyFn(p); }
    };
    using ptr_type = std::unique_ptr<Type, deleter>;
    ptr_type ptr;
  public:
    explicit object_ptr(Type *p) {
      if (!p)
        throw error();
      ptr = ptr_type(p);
    }
    operator Type*() { return ptr.get(); }
  };

  class window: public object_ptr<SDL_Window, SDL_DestroyWindow> {
  public:
    using object_ptr::object_ptr;
    window(const char *title, int x, int y, int w, int h, Uint32 flags = 0)
      : object_ptr::object_ptr(SDL_CreateWindow(title, x, y, w, h, flags)) { }
  };


  // class window {
  //   struct deleter {
  //     void operator()(SDL_Window *w) { if (w) SDL_DestroyWindow(w); }
  //   };
  //   using ptr = std::unique_ptr<SDL_Window, deleter>;
  //   ptr windowp;
  // public:
  //   window(const char *title, int x, int y, int w, int h, Uint32 flags = 0) {
  //     auto p = SDL_CreateWindow(title, x, y, w, h, flags);
  //     if (!p)
  //       throw error();
  //     windowp = ptr(p, deleter());
  //   }
  //   explicit window(SDL_Window* p): windowp(p, deleter()) {}
  //   operator SDL_Window*() { return windowp.get(); }
  // };


  class renderer {
    struct deleter {
      void operator()(SDL_Renderer *r) { if (r) SDL_DestroyRenderer(r); }
    };
    using ptr = std::unique_ptr<SDL_Renderer, deleter>;
    ptr rendererp;
  public:
    renderer(window w, int index = -1, Uint32 flags = 0) {
      auto p = SDL_CreateRenderer(w, index, flags);
      if (!p)
        throw error();
      rendererp = ptr(p, deleter());
    }
    explicit renderer(SDL_Renderer* p): rendererp(p, deleter()) {}
    operator SDL_Renderer*() { return rendererp.get(); }
  };


  class surface {
    struct deleter {
      void operator()(SDL_Surface *p) { if (p) SDL_FreeSurface(p); }
    };
    using ptr_type = std::unique_ptr<SDL_Surface, deleter>;
    ptr_type ptr;
  public:
    explicit surface(SDL_Surface* p): ptr(p, deleter()) {
      if (!p)
        throw error();
    }
    operator SDL_Surface*() { return ptr.get(); }
  };


  class texture {
    struct deleter {
      void operator()(SDL_Texture *p) { if (p) SDL_DestroyTexture(p); }
    };
    using ptr_type = std::unique_ptr<SDL_Texture, deleter>;
    ptr_type ptr;
  public:
    explicit texture(SDL_Texture* p): ptr(p, deleter()) {
      if (!p)
        throw error();
    }
    operator SDL_Texture*() { return ptr.get(); }
  };


  inline std::pair<window, renderer>
  create_window_and_renderer(int w, int h, Uint32 window_flags = 0) {
    SDL_Window *win = nullptr;
    SDL_Renderer *rend = nullptr;
    if (SDL_CreateWindowAndRenderer(w, h, window_flags, &win, &rend) < 0)
      throw error();
    return { window(win), renderer(rend) };
  }

} // namespace sdl
