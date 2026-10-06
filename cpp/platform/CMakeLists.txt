# Abstract platform contract: value types and pure-virtual interfaces that a backend
# (e.g. sdlbackend) implements. Header-only, with no dependencies of its own -- this is
# what makes it safe for fredcore to depend on it without any risk of a dependency cycle.
add_library(platform INTERFACE
    Geometry.hpp
    Renderer.hpp
    AudioDevice.hpp
    GameEvent.hpp
    Platform.hpp)

target_include_directories(platform INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
