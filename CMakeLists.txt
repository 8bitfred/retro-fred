cmake_minimum_required(VERSION 3.16)

project("fred2023")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(fred main.cpp sdl.hpp
    Config.cpp Config.hpp
    Application.cpp Application.hpp
    GameMap.cpp GameMap.hpp 
    TextureID.hpp TextureManager.cpp TextureManager.hpp
    Game.cpp Game.hpp
    Frame.cpp Frame.hpp
    Coord.hpp
    Sprite.cpp Sprite.hpp
    Block.cpp Block.hpp
    AcidDrop.hpp AcidDrop.cpp
    Fred.cpp Fred.hpp
    SoundID.hpp SoundManager.cpp SoundManager.hpp)
target_link_libraries(fred ${SDL2_LIBRARIES})
set_property(TARGET fred PROPERTY CXX_STANDARD 20)
set_property(TARGET fred PROPERTY CXX_STANDARD_REQUIRED True)

if (MSVC)
    target_compile_options(fred PRIVATE /W4 /WX)
else()
    target_compile_options(fred PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
