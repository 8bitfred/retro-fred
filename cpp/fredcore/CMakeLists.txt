add_library(fredcore STATIC
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
    Controller.hpp Controller.cpp
    Game.cpp Game.hpp
    Menu.cpp Menu.hpp)

target_link_libraries(fredcore PUBLIC platform)
