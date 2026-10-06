add_library(fredcore STATIC
    Geometry.hpp
    Renderer.hpp
    AudioDevice.hpp
    Platform.hpp
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
    GameEvent.hpp
    Controller.hpp Controller.cpp
    Game.cpp Game.hpp
    Menu.cpp Menu.hpp)
