cmake_minimum_required(VERSION 3.16)
project("retro-fred" VERSION 1.0.0)

file(CONFIGURE OUTPUT version CONTENT "${CMAKE_PROJECT_VERSION}\n")

if(ANDROID)
    add_subdirectory(android/SDL)
    add_subdirectory(android/SDL_image)
    include_directories(android/SDL/include android/SDL_image/include)
    add_subdirectory(android/SDL_mixer)
    include_directories(android/SDL/include android/SDL_mixer/include)
endif()

add_subdirectory(cpp)
add_subdirectory(assets/sprites sprites)
add_subdirectory(assets/sounds sounds)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    add_custom_target(check_version ALL ${CMAKE_CURRENT_SOURCE_DIR}/tools/check_version.sh)
endif()

set(CPACK_PACKAGE_NAME "Retro-Fred")
set(CPACK_PACKAGE_VENDOR "8bitfred")
set(CPACK_PACKAGE_DESCRIPTION "Modern port of the classic ZX Spectrum videogame Fred")
set(CPACK_STRIP_FILES True)

if(WIN32)
    set(CPACK_PACKAGE_ICON cpp/resources/fred_icon.ico)
endif()

if(APPLE)
    file(COPY "macos/Info.plist" DESTINATION .)
    file(COPY "macos/fred_icon.icns" DESTINATION .)

    set(CPACK_BUNDLE_NAME "Retro-Fred")
    set(CPACK_BUNDLE_PLIST "Info.plist")
    set(CPACK_BUNDLE_ICON "fred_icon.icns")
    if(XCODE)
        set(CPACK_BUNDLE_STARTUP_COMMAND "cpp/Release/retro-fred")
    else()
        set(CPACK_BUNDLE_STARTUP_COMMAND "cpp/retro-fred")
    endif()
endif()

include(CPack)
