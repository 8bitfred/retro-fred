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
    Application.cpp Application.hpp)

target_include_directories(fredgame PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(fredgame PUBLIC fredcore)
