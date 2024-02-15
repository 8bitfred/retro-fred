find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS}
                    $<TARGET_PROPERTY:SDL2_image::SDL2_image,INTERFACE_INCLUDE_DIRECTORIES>)

# default values
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_library(fredcore STATIC
    sdl.hpp
    Config.cpp Config.hpp
    TextureID.hpp TextureManager.cpp TextureManager.hpp
    SoundID.hpp SoundManager.cpp SoundManager.hpp
    Label.hpp LabelTable.hpp LabelTable.cpp
    DisplayConfig.cpp DisplayConfig.hpp
    Coord.hpp
    GameMap.cpp GameMap.hpp
    GameWindow.hpp
    Window.cpp Window.hpp
    Sprite.cpp Sprite.hpp
    GameEvent.hpp GameEvent.cpp
    Game.cpp Game.hpp)

add_library(fredgame STATIC
    Controller.cpp Controller.hpp
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
    Application.cpp Application.hpp)

add_executable(retro-fred main.cpp)
target_link_libraries(retro-fred fredgame fredcore ${SDL2_LIBRARIES} SDL2_image::SDL2_image)

add_executable(autofred autofred.cpp)
target_link_libraries(autofred fredgame fredcore ${SDL2_LIBRARIES} SDL2_image::SDL2_image)

add_executable(sandbox
               sandbox.cpp
               sdl.hpp
               Signal.hpp
               GameEvent.hpp GameEvent.cpp
               Controller.hpp Controller.cpp)
target_link_libraries(sandbox ${SDL2_LIBRARIES} SDL2_image::SDL2_image)

if (WIN32)
    set_property(TARGET retro-fred PROPERTY WIN32_EXECUTABLE True)

    # Create a console version of the fred app, to allow command-line interaction
    add_executable(retro-fred-console main.cpp)
    target_link_libraries(retro-fred-console
                          fredgame fredcore ${SDL2_LIBRARIES} SDL2_image::SDL2_image)

    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
    include(InstallRequiredSystemLibraries)
    install(TARGETS
            retro-fred retro-fred-console autofred
            RUNTIME DESTINATION .)
    install(FILES
            $<TARGET_PROPERTY:SDL2::SDL2,IMPORTED_LOCATION>
            $<TARGET_PROPERTY:SDL2_image::SDL2_image,IMPORTED_LOCATION> DESTINATION .)
endif()

