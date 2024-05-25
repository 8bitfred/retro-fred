cmake_minimum_required(VERSION 3.16)
project("retro-fred" VERSION 0.1.2)

file(CONFIGURE OUTPUT version CONTENT "${CMAKE_PROJECT_VERSION}\n")

if(ANDROID)
    add_subdirectory(android/SDL)
    add_subdirectory(android/SDL_image)
    include_directories(android/SDL/include android/SDL_image/include)
endif()

add_subdirectory(cpp)
add_subdirectory(assets/sprites sprites)
add_subdirectory(assets/sounds sounds)

include(CPack)
set(CPACK_PACKAGE_NAME "Retro-Fred")
set(CPACK_PACKAGE_VENDOR "8bitfred")
set(CPACK_PACKAGE_DESCRIPTION "Modern port of the classic ZX Spectrum videogame Fred")
set(CPACK_PACKAGE_ICON cpp/resources/fred_icon.ico)
set(CPACK_STRIP_FILES True)
