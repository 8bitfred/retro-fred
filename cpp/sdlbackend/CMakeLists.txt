if(NOT ANDROID)
    # NOTE: VSCode cannot find the include files unless the include_directories() directive
    # appears BEFORE the set(CMAKE_CXX_STANDARD) options.
    find_package(SDL3 REQUIRED)
    find_package(SDL3_image REQUIRED)
    find_package(SDL3_mixer REQUIRED)
    include_directories($<TARGET_PROPERTY:SDL3::SDL3,INTERFACE_INCLUDE_DIRECTORIES>
                        $<TARGET_PROPERTY:SDL3_image::SDL3_image,INTERFACE_INCLUDE_DIRECTORIES>
                        $<TARGET_PROPERTY:SDL3_mixer::SDL3_mixer,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

add_library(sdlbackend STATIC
    sdl_raii.hpp
    SdlMain.hpp
    SdlTexture.hpp
    SdlSurface.hpp
    SdlRenderer.hpp SdlRenderer.cpp
    SdlAudioDevice.hpp
    SdlEventSource.hpp SdlEventSource.cpp
    SdlPlatform.hpp SdlPlatform.cpp)

target_include_directories(sdlbackend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(sdlbackend PUBLIC platform fredcore)

# Only shared SDL3 builds are commonly available (unlike SDL2, which also shipped
# static archives); link dynamically everywhere (on Android the targets are built from
# source by the top-level CMakeLists.txt).
target_link_libraries(sdlbackend PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer)
