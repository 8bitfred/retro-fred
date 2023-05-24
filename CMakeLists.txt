cmake_minimum_required(VERSION 3.16)

project("fred2023")

add_executable(fred main.cpp sdl.hpp
    Config.cpp Config.hpp
    Application.cpp Application.hpp
    GameMap.cpp GameMap.hpp 
    TextureManager.cpp TextureManager.hpp)
target_link_libraries(fred -lSDL2 -lSDL2_image)
target_include_directories(fred PUBLIC /usr/include/SDL2)
set_property(TARGET fred PROPERTY CXX_STANDARD 20)
set_property(TARGET fred PROPERTY CXX_STANDARD_REQUIRED True)
