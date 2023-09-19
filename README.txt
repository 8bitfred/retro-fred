

Building in Visual Studio 20222 in Windows:

  -Select clone a repository, make a clone of:
  
      git@github.com:migcat22/fred2023.git

  -CMake will fail if it cannot find the SDL libraries.
   Select "Open CMake Settings Editor" and edit the SDL_DIR variable.
   Make it point to the location of the sdl2-config.cmake file. Then
   save the CMakeSettings.json file. Contents of the sdl2-config.cmake file:

       #
       # See:
       #    https://trenki2.github.io/blog/2017/06/02/using-sdl2-with-cmake/
       #    https://trenki2.github.io/blog/2017/07/04/using-sdl2-image-with-cmake/
       #
       
       set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
       
       # Support both 32 and 64 bit builds
       if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
         set(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2main.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2_image.lib")
       else ()
         set(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2main.lib;;${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_image.lib")
       endif ()
       
       string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

  -Copy SDL.dll and SDL_Image.dll to the output directory: out\build\x64-Debug. TODO: this
   is a hack that is needed because I don't know how to have the executable know the location of
   the .dll files.
