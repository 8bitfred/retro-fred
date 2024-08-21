
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

# default values
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(build_config.hpp.in build_config.hpp @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(fredcore)

add_library(fredgame STATIC
    GameRunner.cpp GameRunner.hpp
    AcidDrop.hpp AcidDrop.cpp
    Fred.cpp Fred.hpp
    Rat.cpp Rat.hpp
    Ghost.cpp Ghost.hpp
    Chameleon.cpp Chameleon.hpp
    Mummy.cpp Mummy.hpp
    MultiDirEnemy.cpp MultiDirEnemy.hpp
    Vampire.cpp Vampire.hpp
    Skeleton.cpp Skeleton.hpp
    Bullet.cpp Bullet.hpp
    Smoke.cpp Smoke.hpp
    Object.cpp Object.hpp
    Tomb.cpp Tomb.hpp
    Application.cpp Application.hpp
    build_config.hpp.in)

include(InstallRequiredSystemLibraries)

if(WIN32)
    set(RETRO_FRED_SDL_LIBRARIES 
        ${SDL2_LIBRARIES}
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer)
else()
    set(RETRO_FRED_SDL_LIBRARIES
        ${SDL2_STATIC_LIBRARIES}
        SDL2_image::SDL2_image-static
        SDL2_mixer::SDL2_mixer-static)
endif()

if(ANDROID)
    add_library(main main.cpp)
    target_link_libraries(main fredgame fredcore SDL2 SDL2main SDL2_image SDL2_mixer)
else()
    add_executable(retro-fred main.cpp)
    target_link_libraries(retro-fred fredgame fredcore
                          ${RETRO_FRED_SDL_LIBRARIES})

    add_executable(autofred autofred.cpp)
    target_link_libraries(autofred fredgame fredcore
                          ${RETRO_FRED_SDL_LIBRARIES})

    add_executable(sandbox
                   sandbox.cpp
                   fredcore/sdl.hpp
                   Signal.hpp
                   fredcore/GameEvent.hpp fredcore/GameEvent.cpp
                   fredcore/Controller.hpp fredcore/Controller.cpp
                   fredcore/DisplayConfig.hpp fredcore/DisplayConfig.cpp)
    target_link_libraries(sandbox
                          ${RETRO_FRED_SDL_LIBRARIES})

    install(TARGETS
            retro-fred
            RUNTIME DESTINATION .)
endif()

if(WIN32)
    # Configure retro-fred as a win32 executable (not a console application)
    set_property(TARGET retro-fred PROPERTY WIN32_EXECUTABLE True)

    # Add resources with application icon
    target_sources(fredcore PUBLIC resources/retro-fred.rc resources/resource.h)
    target_sources(sandbox PRIVATE resources/retro-fred.rc resources/resource.h)

    # Create a console version of the fred app, to allow command-line interaction
    add_executable(retro-fred-console main.cpp)
    target_link_libraries(retro-fred-console
                          fredgame fredcore
                          ${RETRO_FRED_SDL_LIBRARIES})

    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
    install(TARGETS
            retro-fred-console
            RUNTIME DESTINATION .)
    install(FILES
            $<TARGET_PROPERTY:SDL2::SDL2,IMPORTED_LOCATION>
            $<TARGET_PROPERTY:SDL2_image::SDL2_image,IMPORTED_LOCATION>
            $<TARGET_PROPERTY:SDL2_mixer::SDL2_mixer,IMPORTED_LOCATION>
            DESTINATION .)
endif()
