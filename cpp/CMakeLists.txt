find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

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
set_property(TARGET fredcore PROPERTY CXX_STANDARD 20)
set_property(TARGET fredcore PROPERTY CXX_STANDARD_REQUIRED True)
target_include_directories(fredcore PUBLIC ${SDL2_INCLUDE_DIRS}
                           $<TARGET_PROPERTY:SDL2_image::SDL2_image,INTERFACE_INCLUDE_DIRECTORIES>)

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
set_property(TARGET fredgame PROPERTY CXX_STANDARD 20)
set_property(TARGET fredgame PROPERTY CXX_STANDARD_REQUIRED True)
target_include_directories(fredgame PUBLIC ${SDL2_INCLUDE_DIRS}
                           $<TARGET_PROPERTY:SDL2_image::SDL2_image,INTERFACE_INCLUDE_DIRECTORIES>)

add_executable(fred main.cpp)
target_link_libraries(fred fredgame fredcore ${SDL2_LIBRARIES} SDL2_image::SDL2_image)
set_property(TARGET fred PROPERTY CXX_STANDARD 20)
set_property(TARGET fred PROPERTY CXX_STANDARD_REQUIRED True)

add_executable(autofred autofred.cpp)
target_link_libraries(autofred fredgame fredcore ${SDL2_LIBRARIES} SDL2_image::SDL2_image)
set_property(TARGET autofred PROPERTY CXX_STANDARD 20)
set_property(TARGET autofred PROPERTY CXX_STANDARD_REQUIRED True)

add_executable(sandbox
               sandbox.cpp
               sdl.hpp
               Signal.hpp
               GameEvent.hpp GameEvent.cpp
               Controller.hpp Controller.cpp)
target_link_libraries(sandbox ${SDL2_LIBRARIES} SDL2_image::SDL2_image)
set_property(TARGET sandbox PROPERTY CXX_STANDARD 20)
set_property(TARGET sandbox PROPERTY CXX_STANDARD_REQUIRED True)
