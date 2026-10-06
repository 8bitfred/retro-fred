if(NOT ANDROID)
    # NOTE: VSCode cannot find the include files unless the include_directories() directive
    # appears BEFORE the set(CMAKE_CXX_STANDARD) options.
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS}
                        $<TARGET_PROPERTY:SDL2_image::SDL2_image,INTERFACE_INCLUDE_DIRECTORIES>
                        $<TARGET_PROPERTY:SDL2_mixer::SDL2_mixer,INTERFACE_INCLUDE_DIRECTORIES>)
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
target_link_libraries(sdlbackend PUBLIC fredcore)

if(WIN32)
    target_link_libraries(sdlbackend PUBLIC
        ${SDL2_LIBRARIES}
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer)
elseif(ANDROID)
    target_link_libraries(sdlbackend PUBLIC SDL2 SDL2main SDL2_image SDL2_mixer)
else()
    target_link_libraries(sdlbackend PUBLIC
        ${SDL2_STATIC_LIBRARIES}
        SDL2_image::SDL2_image-static
        SDL2_mixer::SDL2_mixer-static)
endif()
