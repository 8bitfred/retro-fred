cmake_minimum_required(VERSION 3.16)

project("fred2023")

add_executable(fred hello.cpp sdl.hpp)
target_link_libraries(fred -lSDL2 -lSDL2_image)

set_property(TARGET fred PROPERTY CXX_STANDARD 20)
set_property(TARGET fred PROPERTY CXX_STANDARD_REQUIRED True)
