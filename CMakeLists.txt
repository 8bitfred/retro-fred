cmake_minimum_required(VERSION 3.16)
project("retro-fred")

add_subdirectory(cpp)
add_subdirectory(assets/sprites sprites)
add_subdirectory(assets/sounds sounds)

include(CPack)
