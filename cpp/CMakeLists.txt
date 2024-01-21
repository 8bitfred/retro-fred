find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(fred main.cpp sdl.hpp
    GameEvent.hpp GameEvent.cpp
    Config.cpp Config.hpp
    DisplayConfig.cpp DisplayConfig.hpp
    Controller.cpp Controller.hpp
    Application.cpp Application.hpp
    GameMap.cpp GameMap.hpp 
    TextureID.hpp TextureManager.cpp TextureManager.hpp
    Game.cpp Game.hpp
    Window.cpp Window.hpp
    Coord.hpp
    Sprite.cpp Sprite.hpp
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
    Bullet.cpp Bullet.hpp
    Smoke.cpp Smoke.hpp
    Object.cpp Object.hpp
    Tomb.cpp Tomb.hpp)
target_link_libraries(fred ${SDL2_LIBRARIES} SDL2_image::SDL2_image)
set_property(TARGET fred PROPERTY CXX_STANDARD 20)
set_property(TARGET fred PROPERTY CXX_STANDARD_REQUIRED True)

add_executable(sandbox
               sandbox.cpp
               sdl.hpp
               Signal.hpp
               GameEvent.hpp GameEvent.cpp
               Controller.hpp Controller.cpp)
target_link_libraries(sandbox ${SDL2_LIBRARIES} SDL2_image::SDL2_image)
set_property(TARGET sandbox PROPERTY CXX_STANDARD 20)
set_property(TARGET sandbox PROPERTY CXX_STANDARD_REQUIRED True)
