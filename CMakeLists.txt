cmake_minimum_required(VERSION 3.16)
project("retro-fred")

set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 1)
set(CMAKE_PROJECT_VERSION_PATCH 2)

# SDL sources are in a subfolder named "SDL"
add_subdirectory(SDL)
add_subdirectory(SDL_image)

add_subdirectory(cpp)
add_subdirectory(assets/sprites sprites)
add_subdirectory(assets/sounds sounds)

include(CPack)
set(CPACK_PACKAGE_NAME "Retro-Fred")
set(CPACK_PACKAGE_VENDOR "Chirimoya Soft")
set(CPACK_PACKAGE_DESCRIPTION "Modern port of the classic ZX Spectrum videogame Fred")
set(CPACK_PACKAGE_ICON cpp/resources/fred_icon.ico)
set(CPACK_STRIP_FILES True)
