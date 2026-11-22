#pragma once

// On Android/iOS, SDL needs the application's main() renamed to SDL_main (its Java/UIKit
// glue calls SDL_main directly). Include this header from the composition root (main.cpp)
// before defining main() -- it is the one unavoidable, purely mechanical SDL touch outside
// of sdlbackend itself.
#include <SDL3/SDL_main.h>
