cmake_minimum_required(VERSION 3.16)

project("fred2023")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(fred main.cpp sdl.hpp
    Config.cpp Config.hpp
    Application.cpp Application.hpp
    GameMap.cpp GameMap.hpp 
    TextureID.hpp TextureManager.cpp TextureManager.hpp
    Game.cpp Game.hpp
    Window.cpp Window.hpp
    Coord.hpp
    Sprite.cpp Sprite.hpp
    Block.cpp Block.hpp
    AcidDrop.hpp AcidDrop.cpp
    Fred.cpp Fred.hpp
    SoundID.hpp SoundManager.cpp SoundManager.hpp
    Rat.cpp Rat.hpp
    Ghost.cpp Ghost.hpp
    Chameleon.cpp Chameleon.hpp
    Mummy.cpp Mummy.hpp
    MultiDirEnemy.cpp MultiDirEnemy.hpp
    Vampire.cpp Vampire.hpp
    Skeleton.cpp Skeleton.hpp
    Bullet.hpp)
target_link_libraries(fred ${SDL2_LIBRARIES})
if (NOT MSVC)
    target_link_libraries(fred -lSDL2_image)
endif()
set_property(TARGET fred PROPERTY CXX_STANDARD 20)
set_property(TARGET fred PROPERTY CXX_STANDARD_REQUIRED True)
